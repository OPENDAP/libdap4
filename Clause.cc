
// (c) COPRIGHT URI/MIT 1996
// Please first read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the CE Clause class.

// $Log: Clause.cc,v $
// Revision 1.5  1998/10/21 16:34:03  jimg
// Made modifications that allow null argument lists.
// Replaced repeated code (to build arg lists) with a function call (it's a
// function call because it needs to be called from inside the expr parser,
// too).
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

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "expr.h"
#include "DDS.h"
#include "Clause.h"

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
	_argc = _args->length();
    else
	_argc = 0;
}

Clause::Clause(btp_func func, rvalue_list *rv)
    : _op(0), _b_func(0), _bt_func(func), _arg1(0), _args(rv)
{
    assert(OK());

    if (_args)
	_argc = _args->length();
    else
	_argc = 0;
}

Clause::Clause() : _op(0), _b_func(0), _bt_func(0), _arg1(0), _args(0)
{
}

Clause::~Clause() 
{
}

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

bool 
Clause::boolean_clause()
{
    assert(OK());

    return _op || _b_func;
}

bool
Clause::value_clause()
{
    assert(OK());

    return (bool)_bt_func;
}

bool 
Clause::value(const String &dataset, DDS &dds) 
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
	for (Pix p = _args->first(); p && !result; _args->next(p))
	    result = result || btp->ops(*(*_args)(p)->bvalue(dataset, dds),
					_op, dataset);

	return result;
    }
    else if (_b_func) {		// ...A bool function?
	BaseType **argv = build_btp_args(_args, dds);

	bool result = (*_b_func)(_argc, argv, dds);
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

bool 
Clause::value(const String &dataset, DDS &dds, BaseType **value) 
{
    assert(OK());
    assert(_bt_func);

    if (_bt_func) {
	BaseType **argv = build_btp_args(_args, dds);

	*value = (*_bt_func)(_argc, argv, dds);
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
