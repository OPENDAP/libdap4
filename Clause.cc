
// (c) COPRIGHT URI/MIT 1996
// Please first read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the CE Clause class.

// $Log: Clause.cc,v $
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

#include "Clause.h"

Clause::Clause(const int oper, rvalue *a1, rvalue_list *rv)
    : op(oper), b_func(0), bt_func(0), arg1(a1), args(rv) 
{
}

Clause::Clause(bool_func func, rvalue_list *rv)
    : op(0), b_func(func), bt_func(0), arg1(0), args(rv)
{
}

Clause::Clause(btp_func func, rvalue_list *rv)
    : op(0), b_func(0), bt_func(func), arg1(0), args(rv)
{
}

Clause::Clause() : op(0), b_func(0), bt_func(0), arg1(0), args(0)
{
}

Clause::~Clause() 
{
#if 0
	if (arg1)
	    delete arg1;

	delete args;
#endif
}

bool
Clause::OK()
{
    // Each clause object can contain one of: a relational clause, a boolean
    // function clause or a BaseType pointer function clause. It must have a
    // valid argument list.
    bool relational = (op && !b_func && !bt_func);
    bool boolean = (!op && b_func && !bt_func);
    bool basetype = (!op && !b_func && bt_func);

    return (relational || boolean || basetype) && args;
}

bool 
Clause::boolean_clause()
{
    assert(OK());

    return op || b_func;
}

bool
Clause::value_clause()
{
    assert(OK());

    return (bool)bt_func;
}

bool 
Clause::value(const String &dataset) 
{
    assert(OK());
    assert(op || b_func);

    if (op) {			// Is it a relational clause?
	// rvalue::bvalue(...) returns the rvalue encapsulated in a
	// BaseType *.
	BaseType *btp = arg1->bvalue(dataset);
	// The list of rvalues is an implicit logical OR, so assume
	// FALSE and return TRUE for the first TRUE subclause.
	bool result = false;
	for (Pix p = args->first(); p && !result; args->next(p))
	    result = result || btp->ops(*(*args)(p)->bvalue(dataset), op);

	return result;
    }
    else if (b_func) {		// ...A bool function?
	int argc = args->length();
	BaseType *argv[argc];
		
	int i = 0;
	for (Pix p = args->first(); p; args->next(p)) {
	    argv[i++] = (*args)(p)->bvalue(dataset);
	}

	bool result = (*b_func)(argc, argv);
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
Clause::value(const String &dataset, BaseType **value) 
{
    assert(OK());
    assert(bt_func);

    if (bt_func) {
	int argc = args->length();
	BaseType *argv[argc];
		
	int i = 0;
	for (Pix p = args->first(); p; args->next(p)) {
	    argv[i++] = (*args)(p)->bvalue(dataset);
	}

	*value = (*bt_func)(argc, argv);
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
