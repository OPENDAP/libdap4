
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

// Interface for the class Structure. A structure contains a single set of
// variables, all at the same lexical level. Of course, a structure may
// contain other structures... The variables contained in a structure are
// stored by instances of this class in a SLList of BaseType pointers.
//
// jhrg 9/14/94

#ifndef _structure_h
#define _structure_h 1

#ifndef __POWERPC__
#endif

#include <vector>

#include "Constructor.h"

namespace libdap
{

class DDS;
class ConstraintEvaluator;

/** This data type is used to hold a collection of related data types,
    in a manner roughly corresponding to a C structure.  The member
    types can be simple or compound types, and can include other
    Structures.

    The DAP2 structure is defined as a singly-linked list.  This means
    that Structure elements can be accessed either by name, with the
    <tt>var()</tt> function, or by their position in the list, either with
    the overloaded version of <tt>var()</tt>, or the combination of the
    <tt>first_var()</tt> and <tt>next_var()</tt> functions.

    The <tt>val2buf()</tt> and <tt>buf2val()</tt> functions only
    return the size of
    the structure.  To read parts of a DAP2 Structure into an
    application program, use the <tt>buf2val()</tt> function of the element
    of the Structure in question.

    Note that the predicate-setting functions <tt>set_send_p()</tt> and
    <tt>set_read_p()</tt> set their flags for the Structure as well as for
    each of the Structure's member elements.

    Similar to C, you can refer to members of Structure elements
    with a ``.'' notation.  For example, if the Structure has a member
    Structure called ``Tom'' and Tom has a member Float32 called
    ``shoe_size'', then you can refer to Tom's shoe size as
    ``Tom.shoe_size''.

    @brief Holds a structure (aggregate) type.
*/

class Structure: public Constructor
{
private:
#if 0
    BaseType *m_leaf_match(const string &name, btp_stack *s = 0);
    BaseType *m_exact_match(const string &name, btp_stack *s = 0);
#endif

protected:
#if 0
    void m_duplicate(const Structure &s);
#endif

public:
    Structure(const string &n);
    Structure(const string &n, const string &d);

    Structure(const Structure &rhs);
    virtual ~Structure();

    Structure &operator=(const Structure &rhs);
    virtual BaseType *ptr_duplicate();

#if 0
    virtual int element_count(bool leaves = false);
#endif
    virtual bool is_linear();

#if 0
    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);
#endif
#if 0
    virtual void set_in_selection(bool state);
#endif
    virtual void set_leaf_sequence(int level = 1);
#if 0
    virtual unsigned int width(bool constrained = false);
    virtual unsigned int width(bool constrained);
#endif
#if 0
    virtual void intern_data(ConstraintEvaluator &eval, DDS &dds);
    virtual bool serialize(ConstraintEvaluator &eval, DDS &dds,
			   Marshaller &m, bool ce_eval = true);
    virtual bool deserialize(UnMarshaller &um, DDS *dds, bool reuse = false);
#endif
#if 0
    // Do not store values in memory as for C; force users to work with the
    // C++ objects as defined by the DAP.

    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int buf2val(void **val);
#endif

#if 0
    virtual BaseType *var(const string &name, bool exact_match = true, btp_stack *s = 0);
    virtual BaseType *var(const string &n, btp_stack &s);
#endif
#if 0
    virtual void add_var(BaseType *bt, Part part = nil);
    virtual void add_var_nocopy(BaseType *bt, Part part = nil);

    virtual void del_var(const string &name);
#endif
#if 0
    virtual bool read() ;
#endif
#if 0
    virtual void print_val(FILE *out, string space = "",
                           bool print_decl_p = true);
    virtual void print_val(ostream &out, string space = "",
                           bool print_decl_p = true);
#endif
#if 0
    virtual bool check_semantics(string &msg, bool all = false);
#endif
    virtual void dump(ostream &strm) const ;
};

} // namespace libdap

#endif // _structure_h
