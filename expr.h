
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Types for the expr parser.
//
// 11/4/95 jhrg

// $Log: expr.h,v $
// Revision 1.10  1998/10/21 16:43:36  jimg
// Added typedef for projection functions (proj_func).
// Removed the structs rvalue and func_rvalue - these have been merged into a
// single class RValue which is defined in Rvalue.cc/h.
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

#ifndef _expr_h
#define _expr_h

#include <String.h>
#include <SLList.h>

#include "config_dap.h"

#include "BaseType.h"

// VALUE is used to return constant values from the scanner to the parser.
// Constants are packaged in BaseType *s for evaluation by the parser.

typedef struct {
    Type type;			// Type is an enum defined in BaseType.h
    union {
	dods_int32 i;
	dods_float64 f;
	String *s;
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

typedef SLList<int> int_list;
typedef SLList<int_list *> int_list_list;

#endif /* _expr_h */
