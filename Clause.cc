
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
 
// (c) COPRIGHT URI/MIT 1996,1998,1999
// Please first read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//	jhrg,jimg	James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the CE Clause class.

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

#include <assert.h>

#include "expr.h"
#include "DDS.h"
#include "Clause.h"

using std::cerr;
using std::endl;

Clause::Clause(const int oper, rvalue *a1, rvalue_list *rv)
    : _op(oper), _b_func(0), _bt_func(0), _arg1(a1), _args(rv) 
{
    assert(OK());
}

Clause::Clause(bool_func func, rvalue_list *rv)
    : _op(0), _b_func(func), _bt_func(0), _arg1(0), _args(rv)
{
    assert(OK());

    if (_args)			// account for null arg list
	_argc = _args->size();
    else
	_argc = 0;
}

Clause::Clause(btp_func func, rvalue_list *rv)
    : _op(0), _b_func(0), _bt_func(func), _arg1(0), _args(rv)
{
    assert(OK());

    if (_args)
	_argc = _args->size();
    else
	_argc = 0;
}

Clause::Clause() : _op(0), _b_func(0), _bt_func(0), _arg1(0), _args(0)
{
}

Clause::~Clause() 
{
    if (_arg1) {
	delete _arg1;
	_arg1 = 0;
    }
    
    if (_args) {
	delete _args;
	_args = 0;
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
    bool boolean = (!_op && _b_func && !_bt_func);
    bool basetype = (!_op && !_b_func && _bt_func);

    if (relational)
	return _arg1 && _args;
    else if (boolean || basetype)
	return true;		// Until we check arguments...10/16/98 jhrg
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

/** @brief Evaluate a clause which returns a boolean value */
bool 
Clause::value(const string &dataset, DDS &dds) 
{
    assert(OK());
    assert(_op || _b_func);

    if (_op) {			// Is it a relational clause?
	// rvalue::bvalue(...) returns the rvalue encapsulated in a
	// BaseType *.
	BaseType *btp = _arg1->bvalue(dataset, dds);
	// The list of rvalues is an implicit logical OR, so assume
	// FALSE and return TRUE for the first TRUE subclause.
	bool result = false;
	for (rvalue_list_iter i = _args->begin();
	     i != _args->end() && !result;
	     i++)
	{
	    result = result || btp->ops((*i)->bvalue(dataset, dds),
					_op, dataset);
	}

	return result;
    }
    else if (_b_func) {		// ...A bool function?
	BaseType **argv = build_btp_args(_args, dds);

	bool result = (*_b_func)(_argc, argv, dds);
	delete[] argv;		// Cache me!
	argv = 0;

	return result;
    }
    else {
	cerr << "Internal error: " << endl
	     << "The constraint expression parser built an invalid clause."
	     << endl
	     << "Please report this error." << endl;
	return false;
    }
}

/** @brief Evaluate a clause that returns a value via a BaseType pointer. */
bool 
Clause::value(const string &dataset, DDS &dds, BaseType **value) 
{
    assert(OK());
    assert(_bt_func);

    if (_bt_func) {
	BaseType **argv = build_btp_args(_args, dds);

	*value = (*_bt_func)(_argc, argv, dds);
	delete[] argv;		// Cache me!
	argv = 0;

	if (*value) {
	    (*value)->set_read_p(true);
	    (*value)->set_send_p(true);
	    return true;
	}
	else {
	    return false;
	}
    }
    else {
	cerr << "Internal error:" << endl
	    << "The constraint expression parser built an invalid clause."
	    << endl
	    << "Please report this error." << endl;
	return false;
    }
}

// $Log: Clause.cc,v $
// Revision 1.19  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.18  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.17  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.16.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.16  2003/01/23 00:22:23  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.15  2003/01/10 19:46:39  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.12.4.4  2002/10/28 21:17:43  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.12.4.3  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.14  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.13  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.12.4.2  2001/08/18 00:18:57  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.12.4.1  2001/07/28 01:10:41  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.12  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.11  2000/08/02 22:46:48  jimg
// Merged 3.1.8
//
// Revision 1.8.6.1  2000/08/02 20:58:26  jimg
// Included the header config_dap.h in this file. config_dap.h has been
// removed from all of the DODS header files.
//
// Revision 1.10  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.9  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.8.20.1  2000/06/02 18:14:42  rmorris
// Mod for port to win32.
//
// Revision 1.8  1999/04/29 02:29:27  jimg
// Merge of no-gnu branch
//
// Revision 1.7  1999/03/24 23:37:14  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.6  1998/11/10 01:09:47  jimg
// Added to the dtors to make sure the various Clause objects are deleted.
//
// Revision 1.5  1998/10/21 16:34:03  jimg
// Made modifications that allow null argument lists.
// Replaced repeated code (to build arg lists) with a function call (it's a
// function call because it needs to be called from inside the expr parser,
// too).
//
// Revision 1.4.14.1  1999/02/02 21:56:56  jimg
// String to string version
//
// Revision 1.4  1996/12/02 23:10:08  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.3  1996/11/27 22:40:16  jimg
// Added DDS as third parameter to function in the CE evaluator
//
// Revision 1.2  1996/08/13 17:49:58  jimg
// Fixed a bug in the value() member function where non-existent functions
// were `evaluated' (producing a core dump).
//
// Revision 1.1  1996/05/31 23:18:55  jimg
// Added.
//

