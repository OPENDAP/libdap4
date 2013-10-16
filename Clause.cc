
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

// (c) COPYRIGHT URI/MIT 1996,1998,1999
// Please first read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
// jhrg,jimg James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the CE Clause class.


#include "config.h"

#include <cassert>
#include <algorithm>

#include "expr.h"
#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "DDS.h"
#include "Clause.h"

using std::cerr;
using std::endl;

namespace libdap {

Clause::Clause(const int oper, rvalue *a1, rvalue_list *rv)
        : _op(oper), _b_func(0), _bt_func(0), _argc(0), _arg1(a1), _args(rv)
{
    assert(OK());
}
#if 1
Clause::Clause(bool_func func, rvalue_list *rv)
        : _op(0), _b_func(func), _bt_func(0), _argc(0), _arg1(0), _args(rv)
{
    assert(OK());

    if (_args)   // account for null arg list
        _argc = _args->size();
    else
        _argc = 0;
}
#endif
Clause::Clause(btp_func func, rvalue_list *rv)
        : _op(0), _b_func(0), _bt_func(func), _argc(0), _arg1(0), _args(rv)
{
    assert(OK());

    if (_args)
        _argc = _args->size();
    else
        _argc = 0;
}

Clause::Clause() : _op(0), _b_func(0), _bt_func(0), _argc(0), _arg1(0), _args(0)
{}

static inline void
delete_rvalue(rvalue *rv)
{
    delete rv; rv = 0;
}

Clause::~Clause()
{
    if (_arg1) {
        delete _arg1; _arg1 = 0;
    }

    if (_args) {
        // _args is a pointer to a vector<rvalue*> and we must must delete
        // each rvalue pointer here explicitly. 02/03/04 jhrg
        for_each(_args->begin(), _args->end(), delete_rvalue);
        delete _args; _args = 0;
    }
}

/** @brief Checks the "representation invariant" of a clause. */
bool
Clause::OK()
{
    // Each clause object can contain one of: a relational clause, a boolean
    // function clause or a BaseType pointer function clause. It must have a
    // valid argument list.
    //
    // But, a valid arg list might contain zero arguments! 10/16/98 jhrg
    bool relational = (_op && !_b_func && !_bt_func);
#if 1
    bool boolean = (!_op && _b_func && !_bt_func);
#endif
    bool basetype = (!_op && !_b_func && _bt_func);

    if (relational)
        return _arg1 && _args;
    else if (boolean || basetype)
        return true;  // Until we check arguments...10/16/98 jhrg
    else
        return false;
}

/** @brief Return true if the clause returns a boolean value. */
bool
Clause::boolean_clause()
{
    assert(OK());

    return _op || _b_func;
}

/** @brief Return true if the clause returns a value in a BaseType pointer. */
bool
Clause::value_clause()
{
    assert(OK());

    return (_bt_func != 0);
}

/** @brief Evaluate a clause which returns a boolean value
    This method must only be evaluated for clauses with relational
    expressions or boolean functions.

    @param dds Use variables from this DDS when evaluating the
    expression

    @return True if the clause is true, false otherwise.
    @exception InternalErr if called for a clause that returns a
    BaseType pointer. */
bool
Clause::value(DDS &dds)
{
    assert(OK());
    assert(_op || _b_func);

    if (_op) {   // Is it a relational clause?
        // rvalue::bvalue(...) returns the rvalue encapsulated in a
        // BaseType *.
        BaseType *btp = _arg1->bvalue(dds);
        // The list of rvalues is an implicit logical OR, so assume
        // FALSE and return TRUE for the first TRUE subclause.
        bool result = false;
        for (rvalue_list_iter i = _args->begin();
             i != _args->end() && !result;
             i++) {
            result = result || btp->ops((*i)->bvalue(dds), _op);
        }

        return result;
    }
    else if (_b_func) {  // ...A bool function?
        BaseType **argv = build_btp_args(_args, dds);

        bool result = false;
        (*_b_func)(_argc, argv, dds, &result);
        delete[] argv;  // Cache me!
        argv = 0;

        return result;
    }
    else {
        throw InternalErr(__FILE__, __LINE__,
                          "A selection expression must contain only boolean clauses.");
    }
}

/** @brief Evaluate a clause that returns a value via a BaseType
    pointer.
    This method should be called only for those clauses that return values.

    @param dds Use variables from this DDS when evaluating the
    expression
    @param value A value-result parameter

    @return True if the the BaseType pointer is not null, false otherwise.
    @exception InternalErr if called for a clause that returns a
    boolean value. Not that this method itself \e does return a
    boolean value. */
bool
Clause::value(DDS &dds, BaseType **value)
{
    assert(OK());
    assert(_bt_func);

    if (_bt_func) {
        // build_btp_args() is a function defined in RValue.cc. It no longer
        // reads the values as it builds the arguments, that is now left up
        // to the functions themselves. 9/25/06 jhrg
        BaseType **argv = build_btp_args(_args, dds);

        (*_bt_func)(_argc, argv, dds, value);

        delete[] argv;  // Cache me!
        argv = 0;

        if (*value) {
            // FIXME This comment is likely wrong... 10/19/12
            // This call to set_send_p was removed because new logic used
            // in ResponseBuilder will handle it. See send_data(), ...
            // When the second part of the CE is parsed, if it is null,
            // then all the variables in the DDS that holds the function
            // result variables will be sent. If there's a projection in
            // that second CE, it will denote what is to be sent. Setting
            // set_send_p(true) here had the affect of overriding that
            // second CE. Note, however, that the code in send_data() clears
            // all of the send_p properties for variables in the DDS, so
            // removing the call here is just removing something that will
            // actually have no affect. jhrg 10/19/12
            (*value)->set_send_p(true);
            (*value)->set_read_p(true);
            return true;
        }
        else {
            return false;
        }
    }
    else {
        throw InternalErr(__FILE__, __LINE__,
                          "Clause::value() was called in a context expecting a BaseType pointer return, but the Clause was boolean-valued instead.");
    }
}

} // namespace libdap
