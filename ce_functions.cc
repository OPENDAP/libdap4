
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


// These functions are used by the CE evaluator
//
// 1/15/99 jhrg

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: ce_functions.cc,v 1.17 2003/12/08 18:02:30 edavis Exp $"};

#include <iostream>
#include <vector>

#include "BaseType.h"
#include "Array.h"
#include "Sequence.h"
#include "Grid.h"
#include "Error.h"
#include "GSEClause.h"

#include "parser.h"
#include "gse.tab.h"
#include "debug.h"
#include "util.h"

#ifdef WIN32
using std::vector<GSEClause *>;
#else
using std::vector;
#endif

int gse_parse(void *arg);
void gse_restart(FILE *in);

// Glue routines declared in gse.lex
void gse_switch_to_buffer(void *new_buffer);
void gse_delete_buffer(void * buffer);
void *gse_string(const char *yy_str);

string
extract_string_argument(BaseType *arg)
{
    if (arg->type() != dods_str_c)
	throw Error(malformed_expr, 
		    "The function requires a DODS string-type argument.");
    
    string *sp = 0;
    arg->buf2val((void **)&sp);
    string s = *sp;
    delete sp; sp = 0;

    DBG(cerr << "s: " << s << endl);

    return s;
}

BaseType *
func_length(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 1) {
	throw Error("Wrong number of arguments to length().");
    }
    
    switch (argv[0]->type()) {

      case dods_sequence_c: {
	  Sequence *var = dynamic_cast<Sequence *>(argv[0]);
	  if (!var)
	      throw Error("Expected a Sequence variable in length()");
	  dods_int32 result = var->length();
    
	  BaseType *ret = (BaseType *)NewInt32("constant");
	  ret->val2buf(&result);
	  ret->set_read_p(true);
	  ret->set_send_p(true);
	  dds.append_constant(ret); 
    
	  return ret;
      }

      default:
	throw Error("Wrong type argument to length()");
    }
}

void
parse_gse_expression(gse_arg *arg, BaseType *expr)
{
    gse_restart(0);		// Restart the scanner.
    void *cls = gse_string(extract_string_argument(expr).c_str());
    // gse_switch_to_buffer(cls); // Get set to scan the string.
    bool status = gse_parse((void *)arg) == 0;
    gse_delete_buffer(cls);
    if (!status)
	throw Error(malformed_expr, "Error parsing grid selection.");
}

// Assume the following arguments are sent to func_grid_select:
// Grid name, 0 or more strings which contain relational expressions.

// This function has the type 'proj_func' (it is a projection function). It
// must be used in the projection part of a CE. However, it performs a mix of
// operations, some that are really projection (marking variables to be
// returned) and some that are selection (choosing what to return based on
// the values of variables, not just their structure).
void 
func_grid_select(int argc, BaseType *argv[], DDS &dds)
{
    DBG(cerr << "Entering func_grid_select..." << endl);

    if (argc < 1)
	throw Error(unknown_error, "Wrong number of arguments to grid()");

    Grid *grid = dynamic_cast<Grid*>(argv[0]);
    if (!grid)
	throw Error("The first argument to grid() must be a Grid variable!");

    // Mark this grid as part of the current projection.
    if (!dds.mark(grid->name(), true))
	throw Error("Could not find the variable: " + grid->name());

    // argv[1..n] holds strings; each are little expressions to be parsed.
    vector<GSEClause *> clauses;
    gse_arg *arg = new gse_arg(grid);
    for (int i = 1; i < argc; ++i) {
	parse_gse_expression(arg, argv[i]);
	clauses.push_back(arg->get_gsec());
    }
    delete arg;

    // In this loop we have to iterate over the map vectors and the grid
    // dimensions at the same time and set the grid's array's constraint to
    // match that of the map vectors. Maybe we need an interface in Grid to
    // do this? 9/21/2001 jhrg
    Array *grid_array = dynamic_cast<Array *>(grid->array_var());

    Grid::Map_iter piter = grid->map_begin() ;
    Array::Dim_iter grid_dim = grid_array->dim_begin() ;
    for (; piter != grid->map_end(); piter++, grid_dim++)
    {
	Array *map = dynamic_cast<Array *>((*piter)) ;
	string map_name = map->name();

	// a valid Grid Map is a vector.
	Array::Dim_iter qiter = map->dim_begin();

	int start = map->dimension_start(qiter);
	int stop = map->dimension_stop(qiter);

	vector<GSEClause*>::iterator cs_iter;
	for (cs_iter = clauses.begin(); cs_iter != clauses.end(); cs_iter++) {
	    GSEClause *gsec = *cs_iter;
	    if (gsec->get_map_name() == map_name) {
		if (gsec->get_start() >= start)
		    start = gsec->get_start();
		else
		    throw InternalErr(__FILE__, __LINE__,
			      "Improper starting Grid selection value; the value preceeds the starting index of the map vector.");

		if (gsec->get_stop() <= stop)
		    stop = gsec->get_stop();
		else
		    throw InternalErr(__FILE__, __LINE__,
			       "Improper ending Grid selection value; the index overran the end of the map vector");

		if (start > stop) {
		    string msg = "The selection range given does not correspond to any values of ";
		    msg += gsec->get_map_name()
			+ (string)".\nThe vector's values range from "
			+ gsec->get_map_min_value()
			+ (string)" to "
			+ gsec->get_map_max_value()
			+ (string)".";
		    throw Error(unknown_error, msg);
		}
		// This map is constrained, set read_p so that during
		// serialization new values will be read according to the
		// constraint set here. 9/21/2001 jhrg
		map->set_read_p(false);
	    }
	}		
	DBG(cerr << "Setting constraint on " << map->name() \
	    << "[" << start << ":" << stop << "]" << endl);

	// Stride is always one.
	Array::Dim_iter fd = map->dim_begin() ;
	map->add_constraint(fd, start, 1, stop);
	grid_array->add_constraint(grid_dim, start, 1, stop);
    }
    
    // Make sure we reread the grid's array, too. 9/24/2001 jhrg
    grid_array->set_read_p(false);

    DBG(cerr << "Exiting func_grid_select." << endl);
}

// $Log: ce_functions.cc,v $
// Revision 1.17  2003/12/08 18:02:30  edavis
// Merge release-3-4 into trunk
//
// Revision 1.15.2.1  2003/09/06 22:58:59  jimg
// New Comments.
//
// Revision 1.16  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.15  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.13.2.2  2003/04/18 03:33:58  jimg
// Added check to grid(); throws Error if the named Grid does not exist.
//
// Revision 1.14  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.13.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.13  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.12  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.8.4.9  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.8.4.8  2002/09/12 22:49:58  pwest
// Corrected signature changes made with Pix to IteratorAdapter changes. Rather
// than taking a reference to a Pix, taking a Pix value.
//
// Revision 1.8.4.7  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.8.4.6  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.11  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.8.4.5  2002/03/01 21:03:08  jimg
// Significant changes to the var(...) methods. These now take a btp_stack
// pointer and are used by DDS::mark(...). The exact_match methods have also
// been updated so that leaf variables which contain dots in their names
// will be found. Note that constructor variables with dots in their names
// will break the lookup routines unless the ctor is the last field in the
// constraint expression. These changes were made to fix bug 330.
//
// Revision 1.8.4.4  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.10  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.8.4.3  2001/09/25 20:26:53  jimg
// Massive fixes to the grid() server side function.
//
// Revision 1.9  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.8.4.2  2001/08/18 00:13:03  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.8.4.1  2001/07/28 01:10:42  jimg
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
// Revision 1.8  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.7  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.6  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.5  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.4.20.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
//
// Revision 1.4.14.1  2000/02/17 05:03:16  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.4  1999/04/29 02:29:34  jimg
// Merge of no-gnu branch
//
// Revision 1.3  1999/04/22 22:30:52  jimg
// Uses dynamic_cast
//
// Revision 1.2  1999/01/21 02:52:52  jimg
// Added extract_string_argument function.
// Added grid_selection projection function.
//
// Revision 1.1  1999/01/15 22:06:44  jimg
// Moved code from util.cc
//

