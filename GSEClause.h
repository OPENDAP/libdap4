
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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
 
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

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
    dods_equal_op,
    dods_not_equal_op
};

/** Holds the results of parsing one of the Grid Selection Expression
    clauses. The Grid selection function takes a set of clauses as arguments
    and must create one instance of this class for each of those clauses. The
    GridSelectionExpr class holds N instances of this class.

    @author James Gallagher
    @see GridSelectionExpr */

class GSEClause {
private:
    Array *d_map;
    // _value1, 2 and _op1, 2 hold the first and second operators and
    // operands. For a clause like `var op value' only _op1 and _value1 have
    // valid information. For a clause like `value op var op value' the
    // second operator and operand are on _op2 and _value2. 1/19/99 jhrg
    double d_value1, d_value2;
    relop d_op1, d_op2;
    int d_start;
    int d_stop;

    string d_map_min_value, d_map_max_value;

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
    template<class T> 
    T 
    set_start_stop(T *t=0)
    {
	// Read the byte array, scan, set start and stop.
	T *vals = 0;
	d_map->buf2val((void **)&vals);

	// Set the map's max and min values for use in error messages (it's a
	// lot easier to do here, now, than later... 9/20/2001 jhrg)
	set_map_min_max_value(vals[d_start], vals[d_stop]);

	int i = d_start;
	int end = d_stop;
	while(i <= end && !compare<T>(vals[i], d_op1, d_value1))
	    i++;

	d_start = i;

	i = end;
	while(i >= 0 && !compare<T>(vals[i], d_op1, d_value1))
	    i--;

	d_stop = i;

	// Every clause must have one operator but the second is optional
	// since the more complex for of a clause is optional.
	if (d_op2 != dods_nop_op) {
	    int i = d_start;
	    int end = d_stop;
	    while(i <= end && !compare<T>(vals[i], d_op2, d_value2))
		i++;

	    d_start = i;

	    i = end;
	    while(i >= 0 && !compare<T>(vals[i], d_op2, d_value2))
		i--;

	    d_stop = i;
	}

	return 0;
    };

	//  See above comment regarding win32.  That also applies here
	//  except that we don't have to force in a dummy arg because
	//  we already use parameterized types in the args for this method.
	template<class T>
	T
	set_map_min_max_value(T min, T max)
	{
    DBG(cerr << "Inside set map min max value " << min << ", " << max << endl);
    std::ostrstream oss1;
    oss1 << min << std::ends;
    d_map_min_value = oss1.str();
    oss1.freeze(0);

    std::ostrstream oss2;
    oss2 << max << std::ends;
    d_map_max_value = oss2.str();
    oss2.freeze(0);

	return 0;
	}
#else
    template<class T> void set_start_stop();
    template<class T> void set_map_min_max_value(T min, T max);
#endif // WIN32

    void compute_indices();

public:
  /** @name Constructors */
  //@{
  GSEClause(Grid *grid, const string &map, const double value,
	    const relop op);

  GSEClause(Grid *grid, const string &map, const double value1,
	    const relop op1, const double value2, const relop op2);

  GSEClause(Grid *grid, const string &expr);

  GSEClause(Grid *grid, char *expr);
  //@}
    
  bool OK() const;

  /** @name Accessors */
  //@{
  Array *get_map() const;

  string get_map_name() const;

  int get_start() const;

  int get_stop() const;

  string get_map_min_value() const;

  string get_map_max_value() const;
  //@}

  /** @name Mutators */
  //@{
  void set_map(Array *map);

  void set_start(int start);

  void set_stop(int stop);
  //@}
};

// $Log: GSEClause.h,v $
// Revision 1.9  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.8  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.7  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.5.4.2  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.6  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.5.4.1  2001/09/25 20:32:16  jimg
// Changes/Fixes associated with fixing grid() (see ce_functions.cc).
//
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

