

// -*- c++ -*-

// (c) COPRIGHT URI/MIT 1995-1996
// Please first read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// Interface for the CE Clause class.

// $Log: Clause.h,v $
// Revision 1.1  1996/05/31 22:45:03  jimg
// Added.
//

#ifndef clause_h
#define clause_h

#ifdef __GNUG__
#pragma interface
#endif

#include "expr.h"

/// A constraint expression is comprised of N instances of clause.
struct Clause {
    int op;			// Relational operator
    bool_func b_func;		// Boolean function pointer
    btp_func bt_func;		// BaseType * function pointer

    rvalue *arg1;		// only for operator
    rvalue_list *args;		// vector arg

    Clause(const int oper, rvalue *a1, rvalue_list *rv);
    Clause(bool_func func, rvalue_list *rv);
    Clause(btp_func func, rvalue_list *rv);
    Clause();

    ~Clause();

    /// Checks the "representation invariant" of a clause.
    bool OK();

    /// Return true if the clause returns a boolean value.
    bool boolean_clause();

    /// Return true if the clause returns a value in a BaseType pointer.
    bool value_clause();

    /// Evaluate a clause which returns a boolean value
    bool value(const String &dataset);

    /// Evaluate a clause that returns a value via a BaseType pointer.
    bool value(const String &dataset, BaseType **value);
};
	
#endif // clause_h
