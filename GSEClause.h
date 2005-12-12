
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
 
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// The Grid Selection Expression Clause class.

#ifndef _gseclause_h
#define _gseclause_h 1


#include <string>
#include <sstream>

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

    template<class T> void set_start_stop();
    template<class T> void set_map_min_max_value(T min, T max);

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
// Revision 1.13  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.11.2.3  2004/07/02 20:41:52  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.12  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.11.2.2  2003/09/06 22:32:35  jimg
// Now uses stringstream instead of strstream.
//
// Revision 1.11.2.1  2003/06/14 00:54:30  rmorris
// Added a header VC++ was looking for - <strstream>.
//
// Revision 1.11  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.10  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.9.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
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

