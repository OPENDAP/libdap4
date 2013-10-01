
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

// Types for the expr parser.
//
// 11/4/95 jhrg

#ifndef _expr_h
#define _expr_h

#include <string>
#include <vector>

#ifndef _basetype_h
#include "BaseType.h"
#endif

namespace libdap
{

// VALUE is used to return constant values from the scanner to the parser.
// Constants are packaged in BaseType *s for evaluation by the parser.

typedef struct
{
    Type type;   // Type is an enum defined in BaseType.h
    union {
        unsigned int ui;
        int i;
        double f;
        string *s;
    } v;
} value;

// Syntactic sugar for `pointer to function returning boolean' (bool_func)
// and `pointer to function returning BaseType *' (btp_func). Both function
// types take four arguments, an integer (argc), a vector of BaseType *s
// (argv), the DDS for the dataset for which these function is being
// evaluated (analogous to the ENVP in UNIX) and a pointer for the function's
// return value. ARGC is the length of ARGV.

/** @todo A better design would wrap these in a class and record at minimum
    their names so that error messages could be a bit more informative. jhrg
 */
typedef void(*bool_func)(int argc, BaseType *argv[], DDS &dds, bool *result);
typedef void(*btp_func)(int argc, BaseType *argv[], DDS &dds, BaseType **btpp);

// Projection function: A function that appears in the projection part of the
// CE and is executed for its side-effect. Usually adds a new variable to
// the DDS. These are run _during the parse_ so their side-effects can be used
// by subsequent parts of the CE.

typedef void(*proj_func)(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);

// INT_LIST and INT_LIST_LIST are used by the parser to store the array
// indexes.

// To add the new feature of 'to the end' in an array projection (denoted using
// start), I used the value -1 for an index. This makes do difference here. jhrg
// 12/20/12

typedef std::vector<int> int_list;
typedef std::vector<int>::const_iterator int_citer ;
typedef std::vector<int>::iterator int_iter ;
typedef std::vector<int_list *> int_list_list;
typedef std::vector<int_list *>::const_iterator int_list_citer ;
typedef std::vector<int_list *>::iterator int_list_iter ;

} // namespace libdap

#endif /* _expr_h */
