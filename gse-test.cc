
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

// Test the gse scanner and parser.
//
// 1/17/99 jhrg

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: gse-test.cc,v 1.9 2003/04/22 19:40:28 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <string>

#include <GetOpt.h>

#include "dods-datatypes.h"
#include "BaseType.h"
#include "Grid.h"
#include "DDS.h"
#include "GSEClause.h"
#include "parser.h"
#include "gse.tab.h"
#include "debug.h"

#define YY_BUFFER_STATE (void *)

void test_gse_scanner(const char *str);
void test_gse_scanner(bool show_prompt);
void test_parser(const string &dds_file);

int gse_lex();			// gse_lex() uses the global gse_exprlval
int gse_parse(void *arg);
int gse_restart(FILE *in);

// Glue routines declared in expr.lex
void gse_switch_to_buffer(void *new_buffer);
void gse_delete_buffer(void * buffer);
void *gse_string(const char *yy_str);

extern int gse_debug;


const string version = "$Revision: 1.9 $";
const string prompt = "gse-test: ";
const string options = "sS:p:dv";
const string usage = "gse-test [-s [-S string] -d -v [-p dds file]\n\
Test the grid selections expression evaluation software.\n\
Options:\n\
	-s: Feed the input stream directly into the expression scanner, does\n\
	    not parse.\n\
        -S: <string> Scan the string as if it was standard input.\n\
        -p: <dds file> parse stdin using the grid defined in the DDS file.\n\
	-d: Turn on expression parser debugging.\n\
        -v: Print the version of expr-test";

int
main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, options.c_str());
    int option_char;
    bool scan_gse = false, scan_gse_string = false;
    bool test_parse = false;
    string constraint = "";
    string dds_file;
    // process options

    while ((option_char = getopt()) != EOF)
	switch (option_char) {
	  case 'd': 
	    gse_debug = true;
	    break;
	  case 's':
	    scan_gse = true;
	    break;
	  case 'S':
	    scan_gse_string = true;
	    scan_gse = true;
	    constraint = getopt.optarg;
	    break;
	  case 'p':
	    test_parse = true;
	    dds_file = getopt.optarg;
	    break;
	  case 'v':
	    fprintf( stderr, "%s: %s\n", argv[0], version.c_str() ) ;
	    exit(0);
	  case '?': 
	  default:
	    fprintf( stderr, "%s\n", usage.c_str() ) ;
	    exit(1);
	    break;
	}

    if (!scan_gse && !test_parse) {
	fprintf( stderr, "%s\n", usage.c_str() ) ;
	exit(1);
    }

    // run selected tests

    if (scan_gse) {
	if (scan_gse_string)
	    test_gse_scanner(constraint.c_str());
	else
	    test_gse_scanner(true);
	exit(0);
    }

    if (test_parse) {
	test_parser(dds_file);
	exit(0);
    }
}

// Instead of reading the tokens from stdin, read them from a string.

void
test_gse_scanner(const char *str)
{
    gse_restart(0);
    void *buffer = gse_string(str);
    gse_switch_to_buffer(buffer);

    test_gse_scanner(false);

    gse_delete_buffer(buffer);
}

void
test_gse_scanner(bool show_prompt)
{
    if (show_prompt) 
	fprintf( stdout, "%s", prompt.c_str() ) ;// first prompt

    int tok;
    while ((tok = gse_lex())) {
	switch (tok) {
	  case SCAN_WORD:
	    fprintf( stdout, "WORD: %s\n", gse_lval.id ) ;
	    break;
	  case SCAN_INT:
	    fprintf( stdout, "INT: %d\n", (int)gse_lval.val ) ;
	    break;
	  case SCAN_FLOAT:
	    fprintf( stdout, "FLOAT: %f\n", gse_lval.val ) ;
	    break;
	  case SCAN_EQUAL:
	    fprintf( stdout, "EQUAL: %d\n", gse_lval.op ) ;
	    break;
	  case SCAN_NOT_EQUAL:
	    fprintf( stdout, "NOT_EQUAL: %d\n", gse_lval.op ) ;
	    break;
	  case SCAN_GREATER:
	    fprintf( stdout, "GREATER: %d\n", gse_lval.op ) ;
	    break;
	  case SCAN_GREATER_EQL:
	    fprintf( stdout, "GREATER_EQL: %d\n", gse_lval.op ) ;
	    break;
	  case SCAN_LESS:
	    fprintf( stdout, "LESS: %d\n", gse_lval.op ) ;
	    break;
	  case SCAN_LESS_EQL:
	    fprintf( stdout, "LESS_EQL: %d\n", gse_lval.op ) ;
	    break;
	  default:
	    fprintf( stdout, "Error: Unrecognized input\n" ) ;
	}
	fprintf( stdout, "%s", prompt.c_str() ) ; // print prompt after output
    }
}

// Create and return a map vector for use in testing the GSE parser. The map
// is monotonically increasing in T.

template<class T>
static T*
new_map(int size)
{
    T *t = new T[size];
    srand(time(0));
    double offset = rand()/(double)((1<<15)-1);
    double space = rand()/(double)((1<<15)-1);

    offset *= 10.0;
    space *= 10.0;

    for (int i = 0; i < size; ++i)
	t[i] = (int)(i * space  + offset);

    return t;
}

void
test_parser(const string &dds_file)
{
    // Read the grid

    Grid *grid = 0;
    DDS dds;
    dds.parse(dds_file);
    for (DDS::Vars_iter p = dds.var_begin(); p != dds.var_end(); p++)
    {
	if ((*p)->type() == dods_grid_c)
	    grid = dynamic_cast<Grid *>((*p));
    }
    if (!grid) {
	fprintf( stderr,
		 "Could not find a grid variable in the DDS, exiting.\n" ) ; 
	exit(1);
    }
    
    // Load the grid map indices. The GSEClause object does not process the
    // Array values, but does need valid map data. Assume that the maps are
    // all either Float64 or Int32 maps.

    for (Grid::Map_iter p = grid->map_begin(); p != grid->map_end(); p++)
    {
	Array *map = dynamic_cast<Array *>((*p));
	// Can safely assume that maps are one-dimensional Arrays.
	Array::Dim_iter diter = map->dim_begin() ;
	int size = map->dimension_size(diter);
	switch(map->var()->type()) {
	  case dods_int16_c: {
	    dods_int16 *vec = new_map<dods_int16>(size);
	    map->val2buf(vec);
	    break;
	  }
	  case dods_int32_c: {
	    dods_int32 *vec = new_map<dods_int32>(size);
	    map->val2buf(vec);
	    break;
	  }
	  case dods_float32_c: {
	    dods_float32 *vec = new_map<dods_float32>(size);
	    map->val2buf(vec);
	    break;
	  }
	  case dods_float64_c: {
	    dods_float64 *vec = new_map<dods_float64>(size);
	    map->val2buf(vec);
	    break;
	  }
	  default:
	    fprintf( stderr, "Invalid map vector type in grid, exiting.\n" ) ;
	    exit(1);
	}
    }

    dds.print(stdout);
    for (Grid::Map_iter p = grid->map_begin(); p != grid->map_end(); p++)
	(*p)->print_val(stdout);

    // Parse the GSE and mark the selection in the Grid.

    gse_restart(stdin);

    fprintf( stdout, "%s", prompt.c_str() ) ;
    fflush( stdout ) ;
    
    gse_arg *arg = new gse_arg(grid);
    bool status = false ;
    try {
	status = gse_parse((void *)arg) == 0;
    }
    catch (Error &e) {
	e.display_message();
	exit(1);
    }

    if (status) {
	fprintf( stdout, "Input parsed\n" ) ;
	GSEClause *gsec = arg->get_gsec();
	fprintf( stdout, "Start: %d Stop: %d\n", gsec->get_start(),
						 gsec->get_stop() ) ;
    }
    else
	fprintf( stdout, "Input did not parse\n" ) ;
}

// $Log: gse-test.cc,v $
// Revision 1.9  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.8  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.7.2.1  2003/02/21 00:10:08  jimg
// Repaired copyright.
//
// Revision 1.7  2003/01/23 00:22:25  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.6  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.4.4.4  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.4.4.3  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.4.4.2  2002/09/05 22:52:55  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.5  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.4.4.1  2001/09/25 20:21:47  jimg
// Added scanner cases for EQUAL and BOT EQUAL.
//
// Revision 1.4  2000/09/22 02:17:23  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.3  1999/04/29 02:29:36  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/03/24 23:30:07  jimg
// Added minimal support for the new Int16, UInt16 and Float32 types.
//
// Revision 1.1  1999/01/21 02:07:44  jimg
// Created
//

