
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
// Please first read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
// jhrg,jimg James Gallagher <jgallagher@gso.uri.edu>

// Interface for the CE Clause class.

#ifndef _clause_h
#define _clause_h


#ifndef _expr_h
#include "expr.h"
#endif

#ifndef _rvalue_h
#include "RValue.h"
#endif

namespace libdap
{

/** The selection part of a a DAP constraint expression may contain one or
    more clauses, separated by ampersands (\&). This is modeled in the DDS
    class structure as a singly-linked list of Clause objects. In addition, a
    constraint expression may be a single function call, also represented in
    the DDS using an instance of Clause.

    Each clause object can contain a representation of one of three
    possible forms:

    <ol>

    <li> A relational clause, where an operator tests the relation
    between two operands.  This kind of clause evaluates to a boolean
    value. For example: <tt>a > b</tt>.

    <li> A boolean function, where some function operates on
    arguments in the clause to return a boolean value.  For example,
    consider a scalar A and a list L.  The clause <tt>find(A,L)</tt> might
    return TRUE if A is a member of L (if the <tt>find()</tt> function is
    defined).

    <li> A clause that returns a pointer to a DAP BaseType value.
    This is a clause that evaluates to some data value (be it scalar
    or vector).  For example, <tt>sig0()</tt> might be included in the
    constraint expression parser to calculate density from pressure,
    temperature, and salinity.  In this case, <tt>sig0(p,t,s)</tt> would be a
    clause that evaluates to a data value.

    </ol>

    This might be a bit confusing; in the first, and by far more common, form
    of constraint expressions (CEs) only the first two types of clauses may
    appear. In the second form of the CE only the last type of clause may
    occur. The Clause class, however, can store them all.

    The Clause object holds the constraint expression <i>after</i> it
    has been parsed.  The parser renders the relational operator into
    an integer, and the functions into pointers.

    @brief Holds a fragment of a constraint expression.
    @see DDS::parse_constraint */
struct Clause
{

private:
    /** The relational operator, if any. */
    int _op;
    /** A pointer to a valid boolean function. */
    bool_func _b_func;

    /** A pointer to a valid function that returns a pointer to a
    BaseType. */
    btp_func _bt_func;

    int _argc;   // arg count
    rvalue *_arg1;  // only for operator
    rvalue_list *_args;  // vector arg

    Clause(const Clause &);
    Clause &operator=(const Clause &);

public:
    Clause(const int oper, rvalue *a1, rvalue_list *rv);
    Clause(bool_func func, rvalue_list *rv);
    Clause(btp_func func, rvalue_list *rv);
    Clause();

    virtual ~Clause();

    bool OK();

    bool boolean_clause();

    bool value_clause();

    bool value(DDS &dds);

    bool value(DDS &dds, BaseType **value);
};

} // namespace libdap

#endif // _clause_h
