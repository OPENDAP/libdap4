
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// The Grid Selection Expression Clause class.

// $Log: GSEClause.h,v $
// Revision 1.3  1999/04/29 02:29:30  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/03/24 23:37:15  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.1  1999/01/21 02:07:44  jimg
// Created
//

#ifndef _gse_clause_h
#define _gse_clause_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <string>

#include "BaseType.h"
#include "Array.h"
#include "Grid.h"

enum relop {
    dods_nop_op,
    dods_greater_op,
    dods_greater_equal_op,
    dods_less_op,
    dods_less_equal_op,
    dods_equal_op
};

/** Holds the results of parsing one of the Grid Selection Expression
    clauses. The Grid selection function takes a set of clauses as arguments
    and must create one instance of this class for each of those clauses. The
    GridSelectionExpr class holds N instances of this class.

    @author James Gallagher
    @see GridSelectionExpr */

class GSEClause {
private:
    Array *_map;
    // _value1, 2 and _op1, 2 hold the first and second operators and
    // operands. For a clause like `var op value' only _op1 and _value1 have
    // valid information. For a clause like `value op var op value' the
    // second operator and operand are on _op2 and _value2. 1/19/99 jhrg
    double _value1, _value2;
    relop _op1, _op2;
    int _start;
    int _stop;
    string _expression;		// Original expression before parsing

    GSEClause();		// Hidden default constructor.

    template<class T> void set_start_stop();
    void compute_indices();

public:
    /** @name Constructors */
    //@{
    /** Create an instance using discrete parameters. */
    GSEClause(Grid *grid, const string &map, const double value,
	      const relop op);

    /** Create an instance using discrete parameters. */
    GSEClause(Grid *grid, const string &map, const double value1,
	      const relop op1, const double value2, const relop op2);
    //@}
    
    /** Class invariant. 
	@return True if the object is valid, otherwise False. */
    bool OK() const;

    /** @name Access */
    //@{
    /** Get a pointer to the map variable constrained by this clause.
	@return The Array object. */
    Array *get_map() const;

    /** Get the name of the map variable constrained by this clause.
	@return The Array object's name. */
    string get_map_name() const;

    /** Get the starting index of the clause's map variable as constrained by
	this clause.
	@return The start index. */
    int get_start() const;

    /** Get the stopping index of the clause's map variable as constrained by
	this clause.
	@return The stop index. */
    int get_stop() const;
    //@}
};

#endif // _grid_selection_expr_h

