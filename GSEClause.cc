
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

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"

static char id[] not_used = {"$Id: GSEClause.cc,v 1.15 2003/12/10 21:11:57 jimg Exp $"};

#include <iostream>
#include <sstream>

#include "dods-datatypes.h"
#include "Error.h"
#include "InternalErr.h"

#include "debug.h"
#include "GSEClause.h"
#include "parser.h"
#include "gse.tab.h"

using namespace std;

int gse_parse(void *arg);
void gse_restart(FILE *in);

// Glue routines declared in gse.lex
void gse_switch_to_buffer(void *new_buffer);
void gse_delete_buffer(void * buffer);
void *gse_string(const char *yy_str);


// Private methods

GSEClause::GSEClause()
{
  throw InternalErr(__FILE__, __LINE__, "default ctor called for GSEClause");
}

GSEClause::GSEClause(const GSEClause &param)
{
  throw InternalErr(__FILE__, __LINE__, "copy ctor called for GSEClause");
}

GSEClause &GSEClause::operator=(GSEClause &rhs)
{
  throw InternalErr(__FILE__, __LINE__, "assigment called for GSEClause");
}

template<class T>
static bool
compare(T elem, relop op, double value)
{
    switch (op) {
      case dods_greater_op:
	return elem > value;
      case dods_greater_equal_op:
	return elem >= value;
      case dods_less_op:
	return elem < value;
      case dods_less_equal_op:
	return elem <= value;
      case dods_equal_op:
	return elem == value;
      case dods_not_equal_op:
	return elem != value;
      case dods_nop_op:
	throw Error(malformed_expr, "Attempt to use NOP in Grid selection.");
      default:
	throw Error(malformed_expr, "Unknown relational operator in Grid selection.");
    }
}


#ifndef WIN32
template<class T>
void
GSEClause::set_map_min_max_value(T min, T max)
{
    DBG(cerr << "Inside set map min max value " << min << ", " << max << endl);
    std::ostringstream oss1;
    oss1 << min;
    d_map_min_value = oss1.str();

    std::ostringstream oss2;
    oss2 << max;
    d_map_max_value = oss2.str();
}

#endif

#ifndef WIN32
template<class T>
void
GSEClause::set_start_stop()
{
    // Read the byte array, scan, set start and stop.
    T *vals = 0;
    d_map->buf2val((void **)&vals);

    // Set the map's max and min values for use in error messages (it's a lot
    // easier to do here, now, than later... 9/20/2001 jhrg)
    set_map_min_max_value<T>(vals[d_start], vals[d_stop]);

    int i = d_start;
    int end = d_stop;
    while(i <= end && !compare<T>(vals[i], d_op1, d_value1))
	i++;

    d_start = i;

    i = end;
    while(i >= 0 && !compare<T>(vals[i], d_op1, d_value1))
	i--;
    d_stop = i;

    // Every clause must have one operator but the second is optional since
    // the more complex for of a clause is optional.
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
}
#endif

void
GSEClause::compute_indices()
{
#ifdef WIN32
    //  Allows us to get around short-comming with MS Visual C++ 6.0
    //  templates
    char dummy;

    switch (d_map->var()->type()) {
      case dods_byte_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_int16_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_uint16_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_int32_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_uint32_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_float32_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_float64_c:
	set_start_stop((char *)(&dummy));
	break;
    default:
	throw Error(malformed_expr, 
             "Grid selection using non-numeric map vectors is not supported");
    }
#else
    switch (d_map->var()->type()) {
      case dods_byte_c:
	set_start_stop<dods_byte>();
	break;
      case dods_int16_c:
	set_start_stop<dods_int16>();
	break;
      case dods_uint16_c:
	set_start_stop<dods_uint16>();
	break;
      case dods_int32_c:
	set_start_stop<dods_int32>();
	break;
      case dods_uint32_c:
	set_start_stop<dods_uint32>();
	break;
      case dods_float32_c:
	set_start_stop<dods_float32>();
	break;
      case dods_float64_c:
	set_start_stop<dods_float64>();
	break;
    default:
	throw Error(malformed_expr, 
             "Grid selection using non-numeric map vectors is not supported");
    }
#endif // WIN32

}

// Public methods

/** @brief Create an instance using discrete parameters. */
GSEClause::GSEClause(Grid *grid, const string &map, const double value,
		     const relop op) 
    : d_map(0),
      d_value1(value), d_value2(0), d_op1(op), d_op2(dods_nop_op),
      d_map_min_value(""), d_map_max_value("")
{
    d_map = dynamic_cast<Array *>(grid->var(map));
    if (!d_map)
	throw Error(string("The variable '") + map 
		    + string("' does not exist in the grid '")
		    + grid->name() + string("'."));

    DBG(cerr << d_map->toString());

    // Initialize the start and stop indices.
    Array::Dim_iter iter = d_map->dim_begin();
    d_start = d_map->dimension_start(iter);
    d_stop = d_map->dimension_stop(iter);

    compute_indices();
}

/** @brief Create an instance using discrete parameters. */
GSEClause::GSEClause(Grid *grid, const string &map, const double value1,
		     const relop op1, const double value2, const relop op2) 
    : d_map(0),
      d_value1(value1), d_value2(value2), d_op1(op1), d_op2(op2),
      d_map_min_value(""), d_map_max_value("")
{
    d_map = dynamic_cast<Array *>(grid->var(map));
    if (!d_map)
	throw Error(string("The variable '") + map 
		    + string("' does not exist in the grid '")
		    + grid->name() + string("'."));

    DBG(cerr << d_map->toString());

    // Initialize the start and stop indices.
    Array::Dim_iter iter = d_map->dim_begin();
    d_start = d_map->dimension_start(iter);
    d_stop = d_map->dimension_stop(iter);

    compute_indices();
}

/** @brief Create an instance using a grid and an expression. */
GSEClause::GSEClause(Grid *grid, const string &expr)
{
}

/** @brief Create an instance using a grid and an expression. */
GSEClause::GSEClause(Grid *grid, char *expr)
{
}

/** Class invariant. 
    @return True if the object is valid, otherwise False. */
bool
GSEClause::OK() const
{
    if (!d_map)
	return false;
    
    // More ...

    return true;
}

/** @brief Get a pointer to the map variable constrained by this clause.
    @return The Array object. */
Array *
GSEClause::get_map() const
{
    return d_map;
}

/** @brief Set the pointer to the map vector contrained by this clause.

    Note that this method also sets the name of the map vector.
    @return void */
void
GSEClause::set_map(Array *map)
{
    d_map = map;
}

/** @brief Get the name of the map variable constrained by this clause.
    @return The Array object's name. */
string
GSEClause::get_map_name() const
{
    return d_map->name();
}

/** @brief Get the starting index of the clause's map variable as
    constrained by this clause.
    @return The start index. */
int
GSEClause::get_start() const
{
    return d_start;
}

/** @brief Set the starting index.
    @return void */
void
GSEClause::set_start(int start)
{
    d_start = start;
}

/** @brief Get the stopping index of the clause's map variable as
    constrained by this clause.
    @return The stop index. */
int
GSEClause::get_stop() const
{
    DBG(cerr << "Returning stop index value of: " << d_stop << endl);
    return d_stop;
}

/** @brief Set the stopping index.
    @return void */
void
GSEClause::set_stop(int stop)
{
    d_stop = stop;
}

/** @brief Get the minimum map vector value. 

    Useful in messages back to users.
    @return The minimum map vetor value. */
string
GSEClause::get_map_min_value() const
{
    return d_map_min_value;
}

/** @brief Get the maximum map vector value. 

    Useful in messages back to users.
    @return The maximum map vetor value. */
string
GSEClause::get_map_max_value() const
{
    return d_map_max_value;
}

// $Log: GSEClause.cc,v $
// Revision 1.15  2003/12/10 21:11:57  jimg
// Merge with 3.4. Some of the files contains erros (some tests fail). See
// the ChangeLog for information about fixes.
//
// Revision 1.14  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.12.2.1  2003/06/05 20:15:26  jimg
// Removed many uses of strstream and replaced them with stringstream.
//
// Revision 1.13  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.12  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.10.2.2  2003/04/18 03:12:19  jimg
// Added a check to make sure that the map vector used to build an instance
// actually exists. If not, throw an error.
//
// Revision 1.11  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.10.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.10  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.9  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.5.4.6  2002/12/01 14:37:52  rmorris
// Smalling changes for the win32 porting and maintenance work.
//
// Revision 1.5.4.5  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.5.4.4  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.5.4.3  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
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
// Revision 1.5.4.1  2001/09/25 20:33:02  jimg
// Changes/Fixes associated with fixes to grid() (see ce_functions.cc).
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
// Revision 1.2  1999/03/24 23:37:14  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.1  1999/01/21 02:07:43  jimg
// Created
//

