
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This file contains mfuncs defined for struct rvalue (see expr.h) that
// *cannot* be included in that struct's declaration because their
// definitions must follow *both* rvalue's and func_rvalue's declarations.
// jhrg 3/4/96

// $Log: RValue.cc,v $
// Revision 1.3  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/01/21 02:54:27  jimg
// Fixed dataset; this variable should be the filename of the dataset, not the
// value returned by get_data_name() which is the DODS name of the dataset. This
// value is not set by the dds.filename() method.
//
// Revision 1.1  1998/10/21 16:14:16  jimg
// Added. Based on code that used to be in expr.h/cc
//
// Revision 1.9  1998/09/17 16:59:09  jimg
// Added a test in bvalue() to prevent non-existent variables/fields from being
// accessed. Instead the member function returns null.
//
// Revision 1.8  1997/02/10 02:32:45  jimg
// Added assert statements for pointers
//
// Revision 1.7  1996/11/27 22:40:22  jimg
// Added DDS as third parameter to function in the CE evaluator
//
// Revision 1.6  1996/08/13 18:55:48  jimg
// Added __unused__ to definition of char rcsid[].
//
// Revision 1.5  1996/05/31 23:31:00  jimg
// Updated copyright notice.
//
// Revision 1.4  1996/05/29 22:08:54  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.3  1996/05/22 18:05:36  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.2  1996/03/05 00:54:35  jimg
// Added dtor for rvalue.
// Modified ctor to work with new btp_rvalue class.
//
// Revision 1.1  1996/02/23 17:30:21  jimg
// Created.
//

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: RValue.cc,v 1.3 1999/04/29 02:29:31 jimg Exp $"};

#include <assert.h>

#include "BaseType.h"
#include "expr.h"
#include "RValue.h"
#include "DDS.h"

rvalue::rvalue(BaseType *bt): value(bt), func(0), args(0)
{
}

rvalue::rvalue(btp_func f, rvalue_list *a) : value(0), func(f), args(a)
{
}

rvalue::rvalue(): value(0), func(0), args(0)
{
}

rvalue::~rvalue() 
{
    // Deleting the BaseType pointers in value and args is a bad idea since
    // those might be variables in the dataset. The DDS dtor will take care
    // of deleting them. The constants wrapped in BaseType objects should be
    // pushed on the list of CE-allocated temp objects which the DDS also
    // frees. 
}

string
rvalue::value_name()
{
    assert(value);
#if 0
    if (!value)
	throw not_a_basetype;
#endif

    return value->name();
}

// Return the BaseType * to a value for an rvalue.
// NB: this must be defined after the struct func_rvalue (since it uses
// func_rvalue's bvalue() mfunc. 

BaseType *
rvalue::bvalue(const string &dataset, DDS &dds) 
{
    int error = 0;

    if (value) {
	if (!value->read_p())
	    value->read(dataset, error);
	return value;
    }
    else if (func) {
	int argc = args->length();
	BaseType *argv[argc + 1]; // Add space for null terminator

	int i = 0;
	for (Pix p = args->first(); p; args->next(p)) {
	    assert((*args)(p));
	    argv[i++] = (*args)(p)->bvalue(dataset, dds);
	}

	argv[i] = 0;		// Add null terminator
	return (*func)(argc, argv, dds);
    }
    else {
	return 0;
    }
}

/* This function performs a common task but does not fit within the RValue
   class well. It is used by Clause and expr.y. */

BaseType **
build_btp_args(rvalue_list *args, DDS &dds)
{
    int argc = 0;

    if (args)
	argc = args->length();

    // Add space for a null terminator
    BaseType **argv = new (BaseType *)[argc + 1]; 
    string dataset = dds.filename();
		
    int i = 0;
    if (argc) {
	for (Pix p = args->first(); p; args->next(p)) {
	    argv[i++] = (*args)(p)->bvalue(dataset, dds);
	}
    }

    argv[i] = 0;		// Add the null terminator.

    return argv;
}
