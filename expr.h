
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

// VALUE is used to return constant values from the scanner to the parser.
// Constants are packaged in BaseType *s for evaluation by the parser.

typedef struct {
    Type type;			// Type is an enum defined in BaseType.h
    union {
	unsigned int ui;
	int i;
	double f;
	string *s;
    } v;
} value;

// Syntactic sugar for `pointer to function returning boolean' (bool_func)
// and `pointer to function returning BaseType *' (btp_func). Both function
// types take three arguments, an integer (argc), a vector of BaseType *s
// (argv) and the DDS for the dataset for which these function is being
// evaluated (analogous to the ENVP in UNIX). ARGC is the length of ARGV.

// Try to make a single `selection function' type.

typedef bool (*bool_func)(int argc, BaseType *argv[], DDS &dds);
typedef BaseType *(*btp_func)(int argc, BaseType *argv[], DDS &dds);
typedef void (*proj_func)(int argc, BaseType *argv[], DDS &dds);

// INT_LIST and INT_LIST_LIST are used by the parser to store the array
// indices.

typedef std::vector<int> int_list;
typedef std::vector<int>::const_iterator int_citer ;
typedef std::vector<int>::iterator int_iter ;
typedef std::vector<int_list *> int_list_list;
typedef std::vector<int_list *>::const_iterator int_list_citer ;
typedef std::vector<int_list *>::iterator int_list_iter ;

// $Log: expr.h,v $
// Revision 1.19  2003/12/08 18:02:30  edavis
// Merge release-3-4 into trunk
//
// Revision 1.18  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.17  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.16.2.1  2003/02/21 00:10:08  jimg
// Repaired copyright.
//
// Revision 1.16  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.15  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.13.4.3  2002/09/22 14:22:02  rmorris
// Changed 'vector' to 'std::vector' as the 'using' directive is no longer
// cutting it in this case.
//
// Revision 1.13.4.2  2002/09/05 22:52:55  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.14  2002/06/03 22:21:16  jimg
// Merged with release-3-2-9
//
// Revision 1.13.4.1  2002/02/20 19:16:27  jimg
// Changed the expression parser so that variable names may contain only
// digits.
//
// Revision 1.13  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.12  2000/08/02 22:46:50  jimg
// Merged 3.1.8
//
// Revision 1.11.6.1  2000/08/02 21:10:08  jimg
// Removed the header config_dap.h. If this file uses the dods typedefs for
// cardinal datatypes, then it gets those definitions from the header
// dods-datatypes.h.
//
// Revision 1.11  1999/04/29 02:29:36  jimg
// Merge of no-gnu branch
//
// Revision 1.10  1998/10/21 16:43:36  jimg
// Added typedef for projection functions (proj_func).
// Removed the structs rvalue and func_rvalue - these have been merged into a
// single class RValue which is defined in Rvalue.cc/h.
//
// Revision 1.9.14.1  1999/02/02 21:57:07  jimg
// String to string version
//
// Revision 1.9  1996/11/27 22:40:24  jimg
// Added DDS as third parameter to function in the CE evaluator
//
// Revision 1.8  1996/05/31 23:31:01  jimg
// Updated copyright notice.
//
// Revision 1.7  1996/05/29 22:08:56  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.6  1996/05/22 18:05:37  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.5  1996/05/14 15:38:58  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.4  1996/04/05 00:22:18  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.3  1996/03/02 01:33:25  jimg
// Added to the value typedef to support the complete STL syntax.
// Added support for the rvalue and btp_func_value classes.
//
// Revision 1.2  1996/02/01 17:43:15  jimg
// Added support for lists as operands in constraint expressions.
//
// Revision 1.1  1996/01/31 18:55:15  jimg
// Added to repository.

#endif /* _expr_h */
