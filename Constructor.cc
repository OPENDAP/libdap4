
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

#include "config.h"

#include <string>
#include <sstream>

#include "crc.h"

#include "Constructor.h"
#include "Grid.h"

#include "DMR.h"
#include "XMLWriter.h"
#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"
#include "D4Group.h"

#include "D4Attributes.h"

#include "escaping.h"
#include "util.h"
#include "InternalErr.h"
#include "DapIndent.h"

#include "debug.h"

using namespace std;

namespace libdap {

// Private member functions

void
Constructor::m_duplicate(const Constructor &c)
{
	// Clear out any spurious vars in Constructor::d_vars
	// Moved from Grid::m_duplicate. jhrg 4/3/13
	d_vars.clear(); // [mjohnson 10 Sep 2009]

    for (auto var: c.d_vars) {
        BaseType *btp = var->ptr_duplicate();
        btp->set_parent(this);
        d_vars.push_back(btp);
    }
}

// Public member functions

// A public method, but just barely...
// TODO Understand what this method does. What is dest? Is it the parent-to-be
//  of the variables in this Constructor? jhrg 4/25/22
void
Constructor::transform_to_dap4(D4Group *root, Constructor *dest)
{
    for (Constructor::Vars_citer i = var_begin(), e = var_end(); i != e; ++i) {

        BaseType *d4_var = dest->var((*i)->name());
        // Don't add duplicate variables. We have to make this check
        // because some child variables may add arrays
        // to the root object. For example, this happens in
        // Grid with the Map Arrays - ndp - 05/08/17
        if (!d4_var) {
            (*i)->transform_to_dap4(root /*group*/, dest /*container*/);
        }
    }
    dest->attributes()->transform_to_dap4(get_attr_table());
    dest->set_is_dap4(true);
}

string
Constructor::FQN() const
{
	if (get_parent() == 0)
		return name();
	else if (get_parent()->type() == dods_group_c)
		return get_parent()->FQN() + name();
	else if (get_parent()->type() == dods_array_c)
		return get_parent()->FQN();
	else
		return get_parent()->FQN() + "." + name();
}

int
Constructor::element_count(bool leaves)
{
    if (!leaves)
        return d_vars.size();
    else {
        int i = 0;
        for (auto var: d_vars) {
            i += var->element_count(leaves);
        }
        return i;
    }
}

void
Constructor::set_send_p(bool state)
{
    for (auto var: d_vars) {
        var->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Constructor::set_read_p(bool state)
{
    for (auto var: d_vars) {
        var->set_read_p(state);
    }

    BaseType::set_read_p(state);
}

/** This version of width simply returns the same thing as width() for simple
    types and Arrays. For Structure it returns the total size if constrained
    is false, or the size of the elements in the current projection if true.

    @param constrained If true, return the size after applying a constraint.
    @return  The number of bytes used by the variable.
 */
unsigned int
Constructor::width(bool constrained) const
{
    unsigned int sz = 0;

    for (auto var: d_vars) {
        if (constrained) {
            if (var->send_p())
                sz += var->width(constrained);
        }
        else {
            sz += var->width(constrained);
        }
    }

    return sz;
}

BaseType *
Constructor::var(const string &name, bool exact_match, btp_stack *s)
{
    string n = www2id(name);

    if (exact_match)
        return m_exact_match(n, s);
    else
        return m_leaf_match(n, s);
}

/** @deprecated See comment in BaseType */
BaseType *
Constructor::var(const string &n, btp_stack &s)
{
	// This should probably be removed. The BES code should remove web encoding
	// with the possible exception of spaces. jhrg 11/25/13
    string name = www2id(n);

    BaseType *btp = m_exact_match(name, &s);
    if (btp)
        return btp;

    return m_leaf_match(name, &s);
}

// Protected method
BaseType *
Constructor::m_leaf_match(const string &name, btp_stack *s)
{
    for (auto var: d_vars) {
        if (var->name() == name) {
            if (s) {
                s->push(static_cast<BaseType *>(this));
            }
            return var;
        }
        if (var->is_constructor_type()) {
            BaseType *btp = var->var(name, false, s);
            if (btp) {
                if (s) {
                    s->push(static_cast<BaseType *>(this));
                }
                return btp;
            }
        }
    }

    return nullptr;
}

// Protected method
BaseType *
Constructor::m_exact_match(const string &name, btp_stack *s)
{
    // Look for name at the top level first.
    for (auto var: d_vars) {
        if (var->name() == name) {
            if (s)
                s->push(static_cast<BaseType *>(this));

            return var;
        }
    }

    // If it was not found using the simple search, look for a dot and
    // search the hierarchy.
    string::size_type dot_pos = name.find("."); // zero-based index of `.'
    if (dot_pos != string::npos) {
        string aggregate = name.substr(0, dot_pos);
        string field = name.substr(dot_pos + 1);

        BaseType *agg_ptr = var(aggregate);
        if (agg_ptr) {
            if (s)
                s->push(static_cast<BaseType *>(this));

            return agg_ptr->var(field, true, s); // recurse
        }
        else
            return nullptr;  // qualified names must be *fully* qualified
    }

    return nullptr;
}

/** Returns an iterator referencing the first structure element. */
Constructor::Vars_iter
Constructor::var_begin()
{
    return d_vars.begin();
}

/** Returns an iterator referencing the end of the list of structure
    elements. Does not reference the last structure element. */
Constructor::Vars_iter
Constructor::var_end()
{
    return d_vars.end();
}

/** Return a reverse iterator that references the last element. */
Constructor::Vars_riter
Constructor::var_rbegin()
{
    return d_vars.rbegin();
}

/** Return a reverse iterator that references a point 'before' the first
    element. */
Constructor::Vars_riter
Constructor::var_rend()
{
    return d_vars.rend();
}

/** Return the iterator for the \e ith variable.
    @param i the index
    @return The corresponding  Vars_iter */
Constructor::Vars_iter
Constructor::get_vars_iter(int i)
{
    return d_vars.begin() + i;
}

/** Return the BaseType pointer for the \e ith variable.
    @param i This index
    @return The corresponding BaseType*. */
BaseType *
Constructor::get_var_index(int i)
{
    return *(d_vars.begin() + i);
}

/** Adds an element to a Constructor.

    @param bt A pointer to the variable to add to this Constructor.
    @param part Not used by this class, defaults to nil */
void
Constructor::add_var(BaseType *bt, Part)
{
    // Jose Garcia
    // Passing and invalid pointer to an object is a developer's error.
    if (!bt)
        throw InternalErr(__FILE__, __LINE__, "The BaseType parameter cannot be null.");

    // Jose Garcia
    // Now we add a copy of bt so the external user is able to destroy bt as
    // he/she wishes. The policy is: "If it is allocated outside, it is
    // deallocated outside, if it is allocated inside, it is deallocated
    // inside"
    BaseType *btp = bt->ptr_duplicate();
    btp->set_parent(this);
    d_vars.push_back(btp);
}

/** Adds an element to a Constructor.

    @param bt A pointer to thee variable to add to this Constructor.
    @param part Not used by this class, defaults to nil */
void
Constructor::add_var_nocopy(BaseType *bt, Part)
{
    if (!bt)
        throw InternalErr(__FILE__, __LINE__, "The BaseType parameter cannot be null.");

    bt->set_parent(this);
    d_vars.push_back(bt);
}

/**
 * @brief Remove an element from a Constructor.
 * @note New version. There is a subtle change in that this version will
 * remove all variables in this Constructor with name 'n' while the old
 * version would just remove the first variable.
 * @param n name of the variable to remove
 */
void
Constructor::del_var(const string &n)
{
    for (auto i = d_vars.begin(); i != d_vars.end(); i++) {
        if ((*i)->name() == n) {
            delete *i;
            d_vars.erase(i);
            return;
        }
    }
}

/**
 * @brief Delete the BaseType* and erase the iterator .
 * @note It is OK to call this with an iterator that points to nullptr.
 * @param i The iterator that points to the BaseType.
 */
void
Constructor::del_var(Vars_iter i)
{
    delete *i;
    d_vars.erase(i);
}

/** @brief simple implementation of read that iterates through vars
 *  and calls read on them
 *
 * @return returns false to signify all has been read
 */
bool Constructor::read()
{
    if (!read_p()) {
        for (auto var: d_vars) {
            var->read();
        }
        set_read_p(true);
    }

    return false;
}

void
Constructor::intern_data(ConstraintEvaluator & eval, DDS & dds)
{
    if (!read_p())
        read();          // read() throws Error and InternalErr

    for (auto var: d_vars) {
        if (var->send_p()) {
            var->intern_data(eval, dds);
        }
    }
}

bool
Constructor::serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval)
{
    if (!read_p())
        read();  // read() throws Error and InternalErr

    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;

    for (auto var: d_vars) {
        if (var->send_p()) {
#ifdef CHECKSUMS
            XDRStreamMarshaller *sm = dynamic_cast<XDRStreamMarshaller*>(&m);
            if (sm && sm->checksums() && var->type() != dods_structure_c && var->type() != dods_grid_c)
                sm->reset_checksum();

            var->serialize(eval, dds, m, false);

            if (sm && sm->checksums() && var->type() != dods_structure_c && var->type() != dods_grid_c)
                sm->get_checksum();
#else
            // (*i)->serialize(eval, dds, m, false);
            // Only Sequence and Vector run the evaluator.
            var->serialize(eval, dds, m, true);
#endif
        }
    }

    return true;
}

bool
Constructor::deserialize(UnMarshaller &um, DDS *dds, bool reuse)
{
    for (auto var: d_vars) {
        var->deserialize(um, dds, reuse);
    }

    return false;
}

void
Constructor::compute_checksum(Crc32 &)
{
	throw InternalErr(__FILE__, __LINE__, "Computing a checksum alone is not supported for Constructor types.");
}

void
Constructor::intern_data(/*Crc32 &checksum, DMR &dmr, ConstraintEvaluator & eval*/)
{
    for (auto var: d_vars) {
        if (var->send_p()) {
            var->intern_data(/*checksum, dmr, eval*/);
        }
    }
}

/**
 * @brief Serialize a Constructor
 *
 * @todo See notebook for 8/21/14
 *
 * @param m
 * @param dmr Unused
 * @param eval Unused
 * @param filter Unused
 * @exception Error is thrown if the value needs to be read and that operation fails.
 */
void
Constructor::serialize(D4StreamMarshaller &m, DMR &dmr, bool filter)
{
	// Not used for the same reason the equivalent code in D4Group::serialize()
	// is not used. Fail for D4Sequence and general issues with memory use.
	//
	// Revisit this - I had to uncomment this to get the netcdf_handler code
	// to work - it relies on having NCStructure::read() called. The D4Sequence
	// ::serialize() method calls read_next_instance(). What seems to be happening
	// is that this call to read gets the first set of values, but does not store
	// them; the call to serialize then runs the D4Sequence::serialize() method that
	// _does_ read all the sequence data and then serialize it. However, the first
	// sequence instance is missing...
    if (!read_p())
        read();  // read() throws Error

#if 0
    // place holder for now. There may be no need for this; only Array and Seq?
    // jhrg 9/6/13
    if (filter && !eval.eval_selection(dmr, dataset()))
        return true;
#endif

    for (auto var: d_vars) {
        if (var->send_p()) {
            var->serialize(m, dmr, filter);
        }
    }
}

void
Constructor::deserialize(D4StreamUnMarshaller &um, DMR &dmr)
{
    for (auto var: d_vars) {
        var->deserialize(um, dmr);
    }
}

void
Constructor::print_decl(FILE *out, string space, bool print_semi,
                        bool constraint_info, bool constrained)
{
    ostringstream oss;
    print_decl(oss, space, print_semi, constraint_info, constrained);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void
Constructor::print_decl(ostream &out, string space, bool print_semi,
                        bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
        return;

    out << space << type_name() << " {\n" ;
    for (auto var: d_vars) {
        var->print_decl(out, space + "    ", true, constraint_info, constrained);
    }
    out << space << "} " << id2www(name()) ;

    if (constraint_info) { // Used by test drivers only.
        if (send_p())
            out << ": Send True";
        else
            out << ": Send False";
    }

    if (print_semi)
        out << ";\n" ;
}

void
Constructor::print_val(FILE *out, string space, bool print_decl_p)
{
    ostringstream oss;
    print_val(oss, space, print_decl_p);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void
Constructor::print_val(ostream &out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
        out << " = " ;
    }

    out << "{ " ;
    for (Vars_citer i = d_vars.begin(), e = d_vars.end(); i != e; i++, (void)(i != e && out << ", ")) {
        (*i)->print_val(out, "", false);
    }

    out << " }" ;

    if (print_decl_p)
        out << ";\n" ;
}

/**
 * @deprecated
 */
void
Constructor::print_xml(FILE *out, string space, bool constrained)
{
    XMLWriter xml(space);
    print_xml_writer(xml, constrained);
    fwrite(xml.get_doc(), sizeof(char), xml.get_doc_size(), out);
}

/**
 * @deprecated
 */
void
Constructor::print_xml(ostream &out, string space, bool constrained)
{
    XMLWriter xml(space);
    print_xml_writer(xml, constrained);
    out << xml.get_doc();
}

void
Constructor::print_xml_writer(XMLWriter &xml, bool constrained)
{
    if (constrained && !send_p())
        return;

    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)type_name().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write " + type_name() + " element");

    if (!name().empty())
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    // DAP2 prints attributes first. For some reason we decided that DAP4 should
    // print them second. No idea why... jhrg 8/15/14
    if (!is_dap4() && get_attr_table().get_size() > 0)
        get_attr_table().print_xml_writer(xml);

    if (!d_vars.empty())
        for_each(d_vars.begin(), d_vars.end(),
                 [&xml, constrained](BaseType *btp) { btp->print_xml_writer(xml, constrained); });

    if (is_dap4())
        attributes()->print_dap4(xml);

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end " + type_name() + " element");
}

void
Constructor::print_dap4(XMLWriter &xml, bool constrained)
{
    if (constrained && !send_p())
        return;

    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)type_name().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write " + type_name() + " element");

    if (!name().empty())
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    if (!d_vars.empty())
        for_each(d_vars.begin(), d_vars.end(),
                 [&xml, constrained](BaseType *btp) { btp->print_dap4(xml, constrained); });

    attributes()->print_dap4(xml);

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end " + type_name() + " element");
}

bool
Constructor::check_semantics(string &msg, bool all)
{
    if (!BaseType::check_semantics(msg))
        return false;

    if (!unique_names(d_vars, name(), type_name(), msg))
        return false;

    if (all) {
        for (auto var: d_vars) {
            if (!var->check_semantics(msg, true)) {
                return false;
            }
        }
    }

    return true;
}

/** True if the instance can be flattened and printed as a single table
    of values. For Arrays and Grids this is always false. For Structures
    and Sequences the conditions are more complex. The implementation
    provided by this class always returns false. Other classes should
    override this implementation.

    @todo Change the name to is_flattenable or something like that. 05/16/03
    jhrg

    @brief Check to see whether this variable can be printed simply.
    @return True if the instance can be printed as a single table of
    values, false otherwise. */
bool
Constructor::is_linear()
{
    return false;
}

/** Set the \e in_selection property for this variable and all of its
    children.

    @brief Set the \e in_selection property.
    @param state Set the property value to \e state. */
void
Constructor::set_in_selection(bool state)
{
    for (auto var: d_vars) {
        var->set_in_selection(state);
    }

    BaseType::set_in_selection(state);
}

void Constructor::transfer_attributes(AttrTable *at_container)
{
    AttrTable *at = at_container->get_attr_table(name());

    if (at) {
        BaseType::transfer_attributes(at_container);
        for (auto var: d_vars) {
            var->transfer_attributes(at);
        }
    }
}

AttrTable *
Constructor::make_dropped_vars_attr_table(vector<BaseType *> *dropped_vars)
{
    AttrTable *dv_table = nullptr;
    if (!dropped_vars->empty()) {
        dv_table = new AttrTable;
        dv_table->set_name("dap4:dropped_members");

        vector<BaseType *>::iterator dvIter = dropped_vars->begin();
        vector<BaseType *>::iterator dvEnd = dropped_vars->end();
        unsigned int i = 0;
        for (; dvIter != dvEnd; dvIter++, i++) {
            BaseType *bt = (*dvIter);

            AttrTable *bt_attr_table = new AttrTable(bt->get_attr_table());
            bt_attr_table->set_name(bt->name());
            string type_name = bt->type_name();

            if (bt->is_vector_type()) {
                Array *array = dynamic_cast <Array *>(bt);
                if (array) {    // This is always true - only an Array is_vector_type(). jhrg 4/25/22
                    type_name = array->prototype()->type_name();
                    Array::Dim_iter d_iter = array->dim_begin();
                    Array::Dim_iter end = array->dim_end();
                    for (; d_iter < end; d_iter++) {

                        ostringstream dim_size;
                        dim_size << (*d_iter).size;
                        bt_attr_table->append_attr("array_dimensions", AttrType_to_String(Attr_uint32), dim_size.str());
                    }
                }
            }

            bt_attr_table->append_attr("dap4:type", "String", type_name);
            dv_table->append_container(bt_attr_table, bt_attr_table->get_name());
            // Clear entry now that we're done.
            (*dvIter) = 0;
        }
    }

    return dv_table;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and information about this
 * instance.
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
Constructor::dump(ostream &strm) const {
    strm << DapIndent::LMarg << "Constructor::dump - (" << (void *) this << ")" << endl;
    DapIndent::Indent();
    BaseType::dump(strm);
    strm << DapIndent::LMarg << "vars: " << endl;
    DapIndent::Indent();

    for (auto var: d_vars) {
        var->dump(strm);
    }

    DapIndent::UnIndent();
    DapIndent::UnIndent();
}

} // namespace libdap

