
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Test the gse scanner and parser.
//
// 1/17/99 jhrg

// $Log: gse-test.cc,v $
// Revision 1.3  1999/04/29 02:29:36  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/03/24 23:30:07  jimg
// Added minimal support for the new Int16, UInt16 and Float32 types.
//
// Revision 1.1  1999/01/21 02:07:44  jimg
// Created
//

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: gse-test.cc,v 1.3 1999/04/29 02:29:36 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <streambuf.h>
#include <iostream.h>
#include <stdiostream.h>
#include <string>

#include <GetOpt.h>

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

const string version = "$Revision: 1.3 $";
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
	    cerr << argv[0] << ": " << version << endl;
	    exit(0);
	  case '?': 
	  default:
	    cerr << usage << endl; 
	    exit(1);
	    break;
	}

    if (!scan_gse && !test_parse) {
	cerr << usage << endl;
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

// Instead of reading the tokens from srdin, read them from a string.

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
	cout << prompt;		// first prompt

    int tok;
    while ((tok = gse_lex())) {
	switch (tok) {
	  case ID:
	    cout << "ID: " << gse_lval.id << endl;
	    break;
	  case INT:
	    cout << "INT: " << gse_lval.val << endl;
	    break;
	  case FLOAT:
	    cout << "FLOAT: " << gse_lval.val << endl;
	    break;
	  case GREATER:
	    cout << "GREATER: " << gse_lval.op << endl;
	    break;
	  case GREATER_EQL:
	    cout << "GREATER_EQL: " << gse_lval.op << endl;
	    break;
	  case LESS:
	    cout << "LESS: " << gse_lval.op << endl;
	    break;
	  case LESS_EQL:
	    cout << "LESS_EQL: " << gse_lval.op << endl;
	    break;
	  default:
	    cout << "Error: Unrecognized input" << endl;
	}
	cout << prompt;		// print prompt after output
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
    for (Pix p = dds.first_var(); p; dds.next_var(p)) {
	if (dds.var(p)->type() == dods_grid_c)
	    grid = dynamic_cast<Grid *>(dds.var(p));
    }
    if (!grid) {
	cerr << "Could not find a grid variable in the DDS, exiting." 
	     << endl;
	exit(1);
    }
    
    // Load the grid map indices. The GSEClause object does not process the
    // Array values, but does need valid map data. Assume that the maps are
    // all either Float64 or Int32 maps.

    for (Pix p = grid->first_map_var(); p; grid->next_map_var(p)) {
	Array *map = dynamic_cast<Array *>(grid->map_var(p));
	// Can safely assume that maps are one-dimensional Arrays.
	int size = map->dimension_size((Pix)map->first_dim());
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
	    cerr << "Invalid map vector type in grid, exiting." << endl;
	    exit(1);
	}
    }

    dds.print();
    for (Pix p = grid->first_map_var(); p; grid->next_map_var(p))
	grid->map_var(p)->print_val(cout);

    // Parse the GSE and mark the selection in the Grid.

    gse_restart(stdin);

    cout << prompt << flush;
    
    gse_arg *arg = new gse_arg(grid);
    bool status;
    try {
	status = gse_parse((void *)arg) == 0;
    }
    catch (Error &e) {
	e.display_message();
	exit(1);
    }

    if (status) {
	cout << "Input parsed" << endl;
	GSEClause *gsec = arg->get_gsec();
	cout << "Start: " << gsec->get_start() 
	     << " Stop: " << gsec->get_stop()
	     << endl;
    }
    else
	cout << "Input did not parse" << endl;
}
