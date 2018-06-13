/*
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
*/

/*
  This header defines macros which enable compile-time program
  instrumentation. These macros work for both C and C++. Enclose the entire
  statement to be debugged within the DBG() macro *and* put the semicolon
  after the macro. (e.g., DBG(cerr << "Bad program" << endl); ). Statements
  should not span lines unless they include `\'s to escape the newlines.

  jhrg 10/13/94
*/

#ifndef _debug_h
#define _debug_h

#ifdef __cplusplus

#include <iostream>
using std::cerr;
using std::string;
using std::endl;
#define FILE_N_LINE std::cerr << __FILE__ << ":" << __LINE__ << ": "

#else

#define FILE_N_LINE fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);

#endif /* cplusplus */

#ifdef DODS_DEBUG
#define DBG(x) FILE_N_LINE, x
#define DBGN(x) x
#else
#define DBG(x) /* x */
#define DBGN(x) /* x */
#endif

/** The purpose of DODS_DEBUG1 is to look at only a handful of the DBG()
    macros by changing them to DBG1() macros and defining DODS_DEBUG1. */
#ifdef DODS_DEBUG1
#define DBG1(x) FILE_N_LINE, x
#else
#define DBG1(x) /* x */
#endif

#ifdef DODS_DEBUG2
#define DBG2(x) FILE_N_LINE, x
#define DBG2N(x) x
#else
#define DBG2(x) /* x */
#define DBG2n(x) /* x */
#endif

#ifdef DODS_PERF
#error "Deprecated macro!"
#endif

#endif /* _debug_h */
