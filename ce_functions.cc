
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

#include "config.h"

static char rcsid[] not_used = {"$Id$"};

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

#if defined(_MSC_VER) && (_MSC_VER == 1200)  //  VC++ 6.0 only
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
		    "The function requires a DAP string-type argument.");
    
    string *sp = 0;
    arg->buf2val((void **)&sp);
    string s = *sp;
    delete sp; sp = 0;

    DBG(cerr << "s: " << s << endl);

    return s;
}

// In reality no server imlements this; it _should_ be removed. 03/28/05 jhrg
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
    
	  BaseType *ret = dds.get_factory()->NewInt32("constant");
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
    delete arg; arg = 0;

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

