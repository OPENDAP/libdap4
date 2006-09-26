
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
 
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Declarations for CE functions.
//
// 1/15/99 jhrg

#ifndef _ce_functions_h
#define _ce_functions_h

#include "BaseType.h"
#include "Array.h"
#include "Error.h"
#include "ConstraintEvaluator.h"

namespace libdap {
    string extract_string_argument(BaseType *arg) throw(Error);
    double extract_double_value(BaseType *arg) throw(Error);
    double *extract_double_array(Array *a) throw(Error);
    void set_array_using_double(Array *dest, double *src, int src_len) throw(Error);
#if 0
    void projection_function_grid(int argc, BaseType *argv[], DDS &dds,
                                  ConstraintEvaluator &) throw(Error);
#endif                                  
    BaseType *func_one(int argc, BaseType *argv[], DDS &dds, const string &dataset) throw(Error);                             
    BaseType *function_grid(int argc, BaseType *argv[], DDS &dds, const string &dataset) throw(Error);
    BaseType *function_geogrid(int argc, BaseType *argv[], DDS &dds, const string &dataset) throw(Error);
    
    void register_functions(ConstraintEvaluator &ce);
}

#if 0
// These are out of date. jhrg 8/31/06
BaseType *func_length(int argc, BaseType *argv[], DDS &dds);
#endif

#endif // _ce_functions_h
