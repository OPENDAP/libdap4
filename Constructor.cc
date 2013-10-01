
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
#include <algorithm>
#include <functional>

//#define DODS_DEBUG

#include "Constructor.h"
#include "Grid.h"

#include "debug.h"
#include "escaping.h"
#include "util.h"
#include "Error.h"
#include "InternalErr.h"


using namespace std;

namespace libdap {

// Private member functions

void
Constructor::m_duplicate(const Constructor &c)
{
    Constructor &cs = const_cast<Constructor &>(c);

    for (Vars_iter i = cs.d_vars.begin(); i != cs.d_vars.end(); i++) {
        BaseType *btp = (*i)->ptr_duplicate();
        btp->set_parent(this);
        d_vars.push_back(btp);
    }
}

// Public member functions

Constructor::Constructor(const string &n, const Type &t, bool is_dap4)
        : BaseType(n, t, is_dap4)
{}

/** Server-side constructor that takes the name of the variable to be
 * created, the dataset name from which this variable is being created, and
 * the type of data being stored in the Constructor. This is a protected
 * constructor, available only to derived classes of Constructor
 *
 * @param n string containing the name of the variable to be created
 * @param d string containing the name of the dataset from which this
 * variable is being created
 * @param t type of data being stored
 */
Constructor::Constructor(const string &n, const string &d, const Type &t, bool is_dap4)
        : BaseType(n, d, t, is_dap4)
{}

Constructor::Constructor(const Constructor &rhs) : BaseType(rhs), d_vars(0)
{}

Constructor::~Constructor()
{}

Constructor &
Constructor::operator=(const Constructor &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<BaseType &>(*this) = rhs; // run BaseType=

    m_duplicate(rhs);

    return *this;
}

int
Constructor::element_count(bool leaves)
{
    if (!leaves)
        return d_vars.size();
    else {
        int i = 0;
        for (Vars_iter j = d_vars.begin(); j != d_vars.end(); j++) {
            i += (*j)->element_count(leaves);
        }
        return i;
    }
}

void
Constructor::set_send_p(bool state)
{
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        (*i)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Constructor::set_read_p(bool state)
{
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        (*i)->set_read_p(state);
    }

    BaseType::set_read_p(state);
}

#if 0
// TODO Recode to use width(bool). Bur see comments in BaseType.h
unsigned int
Constructor::width()
{
    unsigned int sz = 0;

    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        sz += (*i)->width();
    }

    return sz;
}
#endif
/** This version of width simply returns the same thing as width() for simple
    types and Arrays. For Structure it returns the total size if constrained
    is false, or the size of the elements in the current projection if true.

    @param constrained If true, return the size after applying a constraint.
    @return  The number of bytes used by the variable.
 */
unsigned int
Constructor::width(bool constrained)
{
    unsigned int sz = 0;

    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        if (constrained) {
            if ((*i)->send_p())
                sz += (*i)->width(constrained);
        }
        else {
            sz += (*i)->width(constrained);
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
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        if ((*i)->name() == name) {
            if (s) {
                DBG(cerr << "Pushing " << this->name() << endl);
                s->push(static_cast<BaseType *>(this));
            }
            return *i;
        }
        if ((*i)->is_constructor_type()) {
            BaseType *btp = (*i)->var(name, false, s);
            if (btp) {
                if (s) {
                    DBG(cerr << "Pushing " << this->name() << endl);
                    s->push(static_cast<BaseType *>(this));
                }
                return btp;
            }
        }
    }

    return 0;
}

// Protected method
BaseType *
Constructor::m_exact_match(const string &name, btp_stack *s)
{
    // Look for name at the top level first.
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        if ((*i)->name() == name) {
            if (s)
                s->push(static_cast<BaseType *>(this));

            return *i;
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
            return 0;  // qualified names must be *fully* qualified
    }

    return 0;
}

/** Returns an iterator referencing the first structure element. */
Constructor::Vars_iter
Constructor::var_begin()
{
    return d_vars.begin() ;
}

/** Returns an iterator referencing the end of the list of structure
    elements. Does not reference the last structure element. */
Constructor::Vars_iter
Constructor::var_end()
{
    return d_vars.end() ;
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
#if 0
    if (bt->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 Structure.");
#endif
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
#if 0
    if (bt->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 Structure.");
#endif
    bt->set_parent(this);
    d_vars.push_back(bt);
}

/** Remove an element from a Constructor.

    @param n name of the variable to remove */
void
Constructor::del_var(const string &n)
{
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        if ((*i)->name() == n) {
            BaseType *bt = *i ;
            d_vars.erase(i) ;
            delete bt ; bt = 0;
            return;
        }
    }
}

void
Constructor::del_var(Vars_iter i)
{
    if (*i != 0) {
        BaseType *bt = *i;
        d_vars.erase(i);
        delete bt;
    }
}

/** @brief simple implementation of read that iterates through vars
 *  and calls read on them
 *
 * @return returns false to signify all has been read
 */
bool Constructor::read()
{
    if (!read_p()) {
        for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
            (*i)->read();
        }
        set_read_p(true);
    }

    return false;
}

void
Constructor::intern_data(ConstraintEvaluator & eval, DDS & dds)
{
    DBG(cerr << "Structure::intern_data: " << name() << endl);
    if (!read_p())
        read();          // read() throws Error and InternalErr

    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        if ((*i)->send_p()) {
            (*i)->intern_data(eval, dds);
        }
    }
}

bool
Constructor::serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
        read();  // read() throws Error and InternalErr

#if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#endif

    dds.timeout_off();

    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        if ((*i)->send_p()) {
#ifdef CHECKSUMS
            XDRStreamMarshaller *sm = dynamic_cast<XDRStreamMarshaller*>(&m);
            if (sm && sm->checksums() && (*i)->type() != dods_structure_c && (*i)->type() != dods_grid_c)
                sm->reset_checksum();

            (*i)->serialize(eval, dds, m, false);

            if (sm && sm->checksums() && (*i)->type() != dods_structure_c && (*i)->type() != dods_grid_c)
                sm->get_checksum();
#else
            (*i)->serialize(eval, dds, m, false);
#endif
        }
    }

    return true;
}

bool
Constructor::deserialize(UnMarshaller &um, DDS *dds, bool reuse)
{
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        (*i)->deserialize(um, dds, reuse);
    }

    return false;
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
    for (Vars_citer i = d_vars.begin(); i != d_vars.end(); i++) {
        (*i)->print_decl(out, space + "    ", true, constraint_info, constrained);
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
    for (Vars_citer i = d_vars.begin(); i != d_vars.end();
         i++, (void)(i != d_vars.end() && out << ", ")) {
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

class PrintFieldXMLWriter : public unary_function<BaseType *, void>
{
    XMLWriter &d_xml;
    bool d_constrained;
public:
    PrintFieldXMLWriter(XMLWriter &x, bool c)
            : d_xml(x), d_constrained(c)
    {}

    void operator()(BaseType *btp)
    {
        btp->print_xml_writer(d_xml, d_constrained);
    }
};

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

    bool has_attributes = get_attr_table().get_size() > 0;
    bool has_variables = (var_begin() != var_end());
    if (has_attributes)
        get_attr_table().print_xml_writer(xml);
    if (has_variables)
        for_each(var_begin(), var_end(), PrintFieldXMLWriter(xml, constrained));

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

    if (all)
        for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
            if (!(*i)->check_semantics(msg, true)) {
                return false;
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
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        (*i)->set_in_selection(state);
    }

    BaseType::set_in_selection(state);
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
Constructor::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Constructor::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    BaseType::dump(strm) ;
    strm << DapIndent::LMarg << "vars: " << endl ;
    DapIndent::Indent() ;
    Vars_citer i = d_vars.begin() ;
    Vars_citer ie = d_vars.end() ;
    for (; i != ie; i++) {
        (*i)->dump(strm) ;
    }
    DapIndent::UnIndent() ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

