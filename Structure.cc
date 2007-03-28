
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the class Structure
//
// jhrg 9/14/94


#include "config.h"

#include <stdlib.h>

#include "Structure.h"
#include "util.h"
#include "debug.h"
#include "InternalErr.h"
#include "escaping.h"

using std::cerr;
using std::endl;

// Jose Garcia 1/26/2000
// Note: all asserts of nature
// for (Pix p = _vars.first(); p; _vars.next(p)) {
//  assert(_vars(p));
// had been commented out, later when we get sure
// we do not need then we can remove them all.

void
Structure::_duplicate(const Structure &s)
{
    Structure &cs = const_cast<Structure &>(s);

    DBG(cerr << "Copying strucutre: " << name() << endl);

    for (Vars_iter i = cs._vars.begin(); i != cs._vars.end(); i++) {
        DBG(cerr << "Copying field: " << cs.name() << endl);
        // Jose Garcia
        // I think this assert here is part of a debugging
        // process since it is going along with a DBG call
        // I leave it here since it can be remove by defining NDEBUG.
        // assert(*i);
        BaseType *btp = (*i)->ptr_duplicate();
        btp->set_parent(this);
        _vars.push_back(btp);
    }
}

/** The Structure constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications.

    @param n A string containing the name of the variable to be
    created.
*/
Structure::Structure(const string &n) : Constructor(n, dods_structure_c)
{}

/** The Structure copy constructor. */
Structure::Structure(const Structure &rhs) : Constructor(rhs)
{
    _duplicate(rhs);
}

Structure::~Structure()
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        BaseType *btp = *i ;
        delete btp ;  btp = 0;
    }
}

BaseType *
Structure::ptr_duplicate()
{
    return new Structure(*this);
}

Structure &
Structure::operator=(const Structure &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<Constructor &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

int
Structure::element_count(bool leaves)
{
    if (!leaves)
        return _vars.size();
    else {
        int i = 0;
        for (Vars_iter j = _vars.begin(); j != _vars.end(); j++) {
            j += (*j)->element_count(leaves);
        }
        return i;
    }
}

bool
Structure::is_linear()
{
    bool linear = true;
    for (Vars_iter i = _vars.begin(); linear && i != _vars.end(); i++) {
        if ((*i)->type() == dods_structure_c)
            linear = linear && dynamic_cast<Structure*>((*i))->is_linear();
        else
            linear = linear && (*i)->is_simple_type();
    }

    return linear;
}

void
Structure::set_send_p(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        //assert(*i);
        (*i)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Structure::set_read_p(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        //assert(*i);
        (*i)->set_read_p(state);
    }

    BaseType::set_read_p(state);
}

/** Set the \e in_selection property for this variable and all of its
    children.

    @brief Set the \e in_selection property.
    @param state Set the property value to \e state. */
void
Structure::set_in_selection(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        (*i)->set_in_selection(state);
    }

    BaseType::set_in_selection(state);
}

/** @brief Traverse Structure, set Sequence leaf nodes. */
void
Structure::set_leaf_sequence(int level)
{
    for (Vars_iter i = var_begin(); i != var_end(); i++) {
        if ((*i)->type() == dods_sequence_c)
            dynamic_cast<Sequence&>(**i).set_leaf_sequence(++level);
        else if ((*i)->type() == dods_structure_c)
            dynamic_cast<Structure&>(**i).set_leaf_sequence(level);
    }
}

// NB: Part defaults to nil for this class

/** Adds an element to a Structure.

    @param bt A pointer to the DAP2 type variable to add to this Structure.
    @param part defaults to nil */
void
Structure::add_var(BaseType *bt, Part)
{
    // Jose Garcia
    // Passing and invalid pointer to an object is a developer's error.
    if (!bt)
        throw InternalErr(__FILE__, __LINE__,
                          "The BaseType parameter cannot be null.");

    // Jose Garcia
    // Now we add a copy of bt so the external user is able to destroy bt as
    // he/she whishes. The policy is: "If it is allocated outside, it is
    // deallocated outside, if it is allocated inside, it is deallocated
    // inside"
    BaseType *btp = bt->ptr_duplicate();
    btp->set_parent(this);
    _vars.push_back(btp);
}

unsigned int
Structure::width()
{
    unsigned int sz = 0;

    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        sz += (*i)->width();
    }

    return sz;
}

void
Structure::transfer_data(const string & dataset,
                         ConstraintEvaluator & eval, DDS & dds)
{
    if (!read_p())
        read(dataset);          // read() throws Error and InternalErr

    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        if ((*i)->send_p()) {
            switch ((*i)->type()) {
            case dods_sequence_c:
                dynamic_cast <Sequence & >(**i).transfer_data(dataset,
                        eval, dds);
                break;

            case dods_structure_c:
                dynamic_cast <Structure & >(**i).transfer_data(dataset,
                        eval, dds);
                break;

            default:
                (*i)->read(dataset);
                break;
            }
        }
    }
}

// Returns: false if an error was detected, true otherwise.
// NB: this means that serialize() returns true when the CE evaluates to
// false. This bug might be fixed using exceptions.

bool
Structure::serialize(const string &dataset, ConstraintEvaluator &eval, DDS &dds,
                     XDR *sink, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
        read(dataset);  // read() throws Error and InternalErr

#if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset))
        return true;
#endif
    dds.timeout_off();

    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        if ((*i)->send_p()) {
            (*i)->serialize(dataset, eval, dds, sink, false);
        }
    }

    return true;
}

bool
Structure::deserialize(XDR *source, DDS *dds, bool reuse)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        (*i)->deserialize(source, dds, reuse);
    }

    return false;
}

/**  This function assumes that val contains values for all the elements of the
     structure in the order those elements are declared.
     @return Returns the size of the structure. */
unsigned int
Structure::val2buf(void *, bool)
{
    return sizeof(Structure);
}

/** Returns the size of the structure. */
unsigned int
Structure::buf2val(void **)
{
    return sizeof(Structure);
}

// If EXACT is true, then use breadth-first search and assume that NAME is
// the path to a variable where a dot (.) separates the ctor variable(s) from
// the variable to be found. If S is not null, push the path to NAME on the
// statck.
BaseType *
Structure::var(const string &name, bool exact_match, btp_stack *s)
{
    string n = www2id(name);

    if (exact_match)
        return m_exact_match(n, s);
    else
        return m_leaf_match(n, s);
}

// Get rid of this method ASAP.
// A depth-first search for leaf nodes matching NAME.
BaseType *
Structure::var(const string &n, btp_stack &s)
{
    string name = www2id(n);

    BaseType *btp = m_exact_match(name, &s);
    if (btp)
        return btp;

    return m_leaf_match(name, &s);
}

// If S is not null, push the path of the depth-first search for a
// leaf-node called NAME onto S.
BaseType *
Structure::m_leaf_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
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

/** Breadth-first search for NAME. If NAME contains one or more dots (.) */
BaseType *
Structure::m_exact_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        DBG(cerr << "Looking at " << (*i)->name() << " in: " << *i
            << endl);
        if ((*i)->name() == name) {
            DBG(cerr << "Found " << (*i)->name() << " in: "
                << *i << endl);
            if (s) {
                DBG(cerr << "Pushing " << this->name() << endl);
                s->push(static_cast<BaseType *>(this));
            }
            return *i;
        }
    }

    string::size_type dot_pos = name.find("."); // zero-based index of `.'
    if (dot_pos != string::npos) {
        string aggregate = name.substr(0, dot_pos);
        string field = name.substr(dot_pos + 1);

        BaseType *agg_ptr = var(aggregate);
        if (agg_ptr) {
            DBG(cerr << "Descending into " << agg_ptr->name() << endl);
            if (s) {
                DBG(cerr << "Pushing " << this->name() << endl);
                s->push(static_cast<BaseType *>(this));
            }
            return agg_ptr->var(field, true, s); // recurse
        }
        else
            return 0;  // qualified names must be *fully* qualified
    }

    return 0;
}

void
Structure::print_val(FILE *out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
        fprintf(out, " = ") ;
    }

    fprintf(out, "{ ") ;
    for (Vars_citer i = _vars.begin(); i != _vars.end();
         i++, (void)(i != _vars.end() && fprintf(out, ", "))) {
        (*i)->print_val(out, "", false);
    }

    fprintf(out, " }") ;

    if (print_decl_p)
        fprintf(out, ";\n") ;
}

void
Structure::print_all_vals(FILE *out, XDR *, DDS *, string space, bool print_decl_p)
{
    print_val(out, space, print_decl_p);
}

bool
Structure::check_semantics(string &msg, bool all)
{
    if (!BaseType::check_semantics(msg))
        return false;

    bool status = true;

    if (!unique_names(_vars, name(), type_name(), msg))
        return false;

    if (all) {
        for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
            //assert(*i);
            if (!(*i)->check_semantics(msg, true)) {
                status = false;
                goto exit;
            }
        }
    }

exit:
    return status;
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
Structure::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Structure::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    Constructor::dump(strm) ;
    DapIndent::UnIndent() ;
}

