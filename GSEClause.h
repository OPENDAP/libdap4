
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// The Grid Selection Expression Clause class.

#ifndef _gseclause_h
#define _gseclause_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <string>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _array_h
#include "Array.h"
#endif

#ifndef _grid_h
#include "Grid.h"
#endif

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

  GSEClause(const GSEClause &param); // Hide
  GSEClause &operator=(GSEClause &rhs); // Hide

#ifdef WIN32
  //  MS Visual C++ 6.0 forces us to declare template member functions
  //  this way and forces us to inline them due to short-comings in their
  //  implementation.  In addition, the use of the arg is a bug work-around
  //  that lets it be known what the type of T is.  There exists an non-
  //  inline version of this function also - if you edit one, you should
  //  probably edit the other also.
  template<class T> T set_start_stop(T *t=0)
  {
    // Read the byte array, scan, set start and stop.
    T *vals = 0;
    _map->buf2val((void **)&vals);

    int i = _start;
    int end = _stop;
    while(i <= end && !compare<T>(vals[i], _op1, _value1))
      i++;
    _start = i;

    i = end;
    while(i >= 0 && !compare<T>(vals[i], _op1, _value1))
      i--;
    _stop = i;

    // Every clause must have one operator but the second is optional since
    // the more complex for of a clause is optional.
    if (_op2 != dods_nop_op) {
      int i = _start;
      int end = _stop;
      while(i <= end && !compare<T>(vals[i], _op2, _value2))
	i++;
      _start = i;

      i = end;
      while(i >= 0 && !compare<T>(vals[i], _op2, _value2))
	i--;
      _stop = i;
    }

    return 0;
  };
#else
  template<class T> void set_start_stop();
#endif

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

// $Log: GSEClause.h,v $
// Revision 1.5  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.4  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.3.20.1  2000/06/02 18:21:27  rmorris
// Mod's for port to Win32.
//
// Revision 1.3  1999/04/29 02:29:30  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/03/24 23:37:15  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.1  1999/01/21 02:07:44  jimg
// Created
//

#endif // _gseclause_h

