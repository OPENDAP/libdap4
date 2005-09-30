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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
#define FILE_N_LINE cerr << __FILE__ << ": " << __LINE__ << ":"

#else

#define FILE_N_LINE fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);

#endif /* cplusplus */

#ifdef DODS_DEBUG
#define DBG(x) FILE_N_LINE; x
#define DBGN(x) x
#else
#define DBG(x) /* x */
#define DBGN(x) /* x */
#endif

/** The purpose of DODS_DEBUG1 is to look at only a handful of the DBG()
    macros by changing them to DBG1() macros and definging DODS_DEBUG1. */
#ifdef DODS_DEBUG1
#define DBG1(x) FILE_N_LINE; x
#else
#define DBG1(x) /* x */
#endif

#ifdef DODS_DEBUG2
#define DBG2(x) FILE_N_LINE; x
#else
#define DBG2(x) /* x */
#endif

#ifdef DODS_PERF
#error "Deprecated macro!"
#endif

/* 
 * $Log: debug.h,v $
 * Revision 1.14  2004/02/19 19:42:53  jimg
 * Merged with release-3-4-2FCS and resolved conflicts.
 *
 * Revision 1.12.2.2  2004/02/10 20:43:37  jimg
 * Added DBG1 macro.
 *
 * Revision 1.12.2.1  2004/01/23 00:16:24  jimg
 * Added Frank Warmerdam's suggestion about <iostream> and std decls.
 *
 * Revision 1.13  2003/12/08 18:02:30  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.12  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.11  2003/03/04 17:16:46  jimg
 * Added DBGN macro which does not print the file and line number information.
 *
 * Revision 1.10  2003/02/21 00:14:25  jimg
 * Repaired copyright.
 *
 * Revision 1.9.2.1  2003/02/21 00:10:08  jimg
 * Repaired copyright.
 *
 * Revision 1.9  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.8  2000/09/22 02:17:22  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.7  1998/09/08 21:34:41  jimg
 * Fixed up this file for use with C as well as C++.
 * Removed the old PERF macro since it was based on a non-POSIX time function
 * (systime()) and was never used!
 *
 * Revision 1.6  1997/08/11 18:19:35  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.5  1996/11/25 03:43:08  jimg
 * Added DODS_PERF macro.
 *
 * Revision 1.4  1996/08/09 18:20:46  jimg
 * Changed DEBUG and DEBUG2 to DODS_DEBUG and DODS_DEBUG2 to avoid conflicts
 * with the WWW libraries use of that name (which it always defines...).
 *
 * Revision 1.3  1996/06/18 23:50:13  jimg
 * Added file and line number information to the macros.
 *
 * Revision 1.2  1996/03/05 00:59:47  jimg
 * Added _DEBUG_H to prevent multiple definition messages.
 *
 * Revision 1.1  1995/09/13 21:58:05  jimg
 * Added to src/dap in keeping with the plan to store all include files in src
 * dirs and `build' the include directory during `make depend' from those
 * inlcudes.
 */

#endif /* _debug_h */
