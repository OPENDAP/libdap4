
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

// (c) COPYRIGHT URI/MIT 1996-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// This file contains mfuncs defined for struct rvalue (see expr.h) that
// *cannot* be included in that struct's declaration because their
// definitions must follow *both* rvalue's and func_rvalue's declarations.
// jhrg 3/4/96

#include "config.h"

static char rcsid[] not_used =
    {"$Id$"
    };

#include <cassert>
#include <iostream>

#include "BaseType.h"
#include "expr.h"
#include "RValue.h"
#include "DDS.h"
#include "dods-limits.h"
#include "util.h"

using namespace std;

namespace libdap {

rvalue_list *
make_rvalue_list(rvalue *rv)
{
    assert(rv);

    rvalue_list *rvals = new rvalue_list;

    return append_rvalue_list(rvals, rv);
}

// Given a rvalue_list pointer RVALS and a value pointer VAL, make a variable
// to hold VAL and append that variable to the list RVALS.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
append_rvalue_list(rvalue_list *rvals, rvalue *rv)
{
    rvals->push_back(rv);

    return rvals;
}


/** Build an argument list suitable for calling a \c btp_func,
    \c bool_func, and so on. Since this takes an rvalue_list and
    not an rvalue, it is a function rather than a class
    member.

    This function performs a common task but does not fit within the RValue
    class well. It is used by Clause and ce_expr.y.

    @param args A list of RValue objects
    @param dds Use this DDS when evaluating functions */
BaseType **
build_btp_args(rvalue_list *args, DDS &dds)
{
    int argc = 0;

    if (args)
        argc = args->size();

    // Sanitize allocation size
    if (!size_ok(sizeof(BaseType*), argc + 1))
        throw Error(malformed_expr, string("Malformed argument list (")
                + long_to_string(argc) + string(")."));

    // Add space for a null terminator
    BaseType **argv = new BaseType*[argc + 1];

    int index = 0;
    if (argv && argc) {
        for (rvalue::Args_iter i = args->begin(); i != args->end() && index
                < argc + 1; ++i)
            argv[index++] = (*i)->bvalue(dds);

        argv[index] = 0; // Add the null terminator.
    }

    if (index != argc) {
        delete[] argv;
        throw InternalErr(__FILE__, __LINE__, "index out of range.");
    }
#if 0
    argv[index] = 0; // Add the null terminator.
#endif
    return argv;
}

rvalue::rvalue(BaseType *bt): d_value(bt), d_func(0), d_args(0)
{}

rvalue::rvalue(btp_func f, vector<rvalue *> *a) : d_value(0), d_func(f), d_args(a)
{}

rvalue::rvalue(): d_value(0), d_func(0), d_args(0)
{}

rvalue::~rvalue()
{
    // Deleting the BaseType pointers in value and args is a bad idea since
    // those might be variables in the dataset. The DDS dtor will take care
    // of deleting them. The constants wrapped in BaseType objects should be
    // pushed on the list of CE-allocated temp objects which the CE frees.
}

string
rvalue::value_name()
{
    assert(d_value);

    return d_value->name();
}

/** Return the BaseType * for a given rvalue. If
    the rvalue is a func_rvalue, evaluates the func_rvalue and returns the
    result. The functions referenced by func_rvalues must encapsulate their
    return values in BaseType *s.

    @param dds The dds to pass to a function.
*/
BaseType *
rvalue::bvalue(DDS &dds)
{
    if (d_value) {        // i.e., if this RValue is a BaseType
        return d_value;
    }
    else if (d_func) {
        // If func is true, then args must be set. See the constructor.
        // 12/23/04 jhrg
        BaseType **argv = build_btp_args(d_args, dds);
        BaseType *ret_val;
        (*d_func)(d_args->size(), argv, dds, &ret_val);
        delete[] argv;
        return ret_val;
    }
    else {
        return 0;
    }
}

} // namespace libdap

