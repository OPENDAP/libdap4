// -*- C++ -*-

// (c) COPRIGHT URI/MIT 1995-1999
// Please first read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// Interface for the CE Clause class.

#ifndef _clause_h
#define _clause_h

#ifdef __GNUG__
#pragma interface
#endif

#ifndef _expr_h
#include "expr.h"
#endif

#ifndef _rvalue_h
#include "RValue.h"
#endif

/** The selection part of a a DODS constraint expression may contain one or
    more clauses, separated by ampersands (\&). This is modeled in the DDS
    class structure as a singly-linked list of Clause objects. In addition, a
    constraint expression may be a single function call, also represented in
    the DDS using an instance of Clause.

    Each clause object can contain a representation of one of three
    possible forms:

    \begin{enumerate}

    \item A relational clause, where an operator tests the relation
    between two operands.  This kind of clause evaluates to a boolean
    value. For example: #a > b#.

    \item A boolean function, where some function operates on
    arguments in the clause to return a boolean value.  For example,
    consider a scalar A and a list L.  The clause #find(A,L)# might
    return TRUE if A is a member of L (if the #find()# function is
    defined). 

    \item A clause that returns a pointer to a DODS BaseType value.
    This is a clause that evaluates to some data value (be it scalar
    or vector).  For example, #sig0()# might be included in the
    constraint expression parser to calculate density from pressure,
    temperature, and salinity.  In this case, #sig0(p,t,s)# would be a
    clause that evaluates to a data value.

    \end{enumerate}

    This might be a bit confusing; in the first, and by far more common, form
    of constraint expressions (CEs) only the first two types of clauses may
    appear. In the second form of the CE only the last type of clause may
    occur. The Clause class, however, can store them all.

    The Clause object holds the constraint expression \emph{after} it
    has been parsed.  The parser renders the relational operator into
    an integer, and the functions into pointers.

    @memo Holds a section of a constraint expression.
    @see DDS::parse_constraint */
struct Clause {

private:
    /** The relational operator, if any. */
    int _op;
    /** A pointer to a valid boolean function. */
    bool_func _b_func;
    /** A pointer to a valid function that returns a pointer to a
	BaseType. */
    btp_func _bt_func;

    int _argc;			// arg count
    rvalue *_arg1;		// only for operator
    rvalue_list *_args;		// vector arg

public:
    Clause(const int oper, rvalue *a1, rvalue_list *rv);
    Clause(bool_func func, rvalue_list *rv);
    Clause(btp_func func, rvalue_list *rv);
    Clause();

    virtual ~Clause();

    /// Checks the "representation invariant" of a clause.
    bool OK();

    /// Return true if the clause returns a boolean value.
    bool boolean_clause();

    /// Return true if the clause returns a value in a BaseType pointer.
    bool value_clause();

    /// Evaluate a clause which returns a boolean value
    bool value(const string &dataset, DDS &dds);

    /// Evaluate a clause that returns a value via a BaseType pointer.
    bool value(const string &dataset, DDS &dds, BaseType **value);
};
	
// $Log: Clause.h,v $
// Revision 1.8  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.7  2000/08/02 22:46:48  jimg
// Merged 3.1.8
//
// Revision 1.6.6.1  2000/08/01 21:09:35  jimg
// Destructor is now virtual
//
// Revision 1.6  1999/04/29 02:29:27  jimg
// Merge of no-gnu branch
//
// Revision 1.5  1998/10/21 16:35:22  jimg
// Fixed doc++ comments.
// Changed member names so they start with an underscore - makes the member
// functions easier to read.
//
// Revision 1.4.6.1  1999/02/02 21:56:56  jimg
// String to string version
//
// Revision 1.4  1998/01/12 14:27:56  tom
// Second pass at class documentation.
//
// Revision 1.3  1997/12/18 15:06:10  tom
// First draft of class documentation, entered in doc++ format,
// in the comments
//
// Revision 1.2  1996/11/27 22:40:18  jimg
// Added DDS as third parameter to function in the CE evaluator
//
// Revision 1.1  1996/05/31 22:45:03  jimg
// Added.
//

#endif // _clause_h
