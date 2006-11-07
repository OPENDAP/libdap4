
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Test the DDS scanner, parser and DDS class.
//
// jhrg 8/29/94

#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <GetOpt.h>

#include "parser.h"
#include "dds.tab.h"
#include "BaseType.h"
#include "Int32.h"
#include "DDS.h"
#include "util.h"
#include "Error.h"

void test_scanner();
void test_parser();
void test_class();

int ddslex();
int ddsparse(DDS &);

extern YYSTYPE ddslval;
extern int ddsdebug;
const char *prompt = "dds-test: ";

void
usage(string name)
{
    fprintf( stderr, "usage: %s %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n",
		     name.c_str(),
		     "[s] [pd] [c]",
		     "s: Test the scanner.",
		     "p: Test the parser; reads from stdin and prints the",
		     "   internal structure to stdout.",
		     "d: Turn on parser debugging. (only for the hard core.)",
		     "c: Test the C++ code for manipulating DDS objects.",
		     "   Reads from stdin, parses and writes the modified DDS",
		     "   to stdout." ) ;
}

int
main(int argc, char *argv[])
{
    GetOpt getopt (argc, argv, "spdc");
    int option_char;
    int scanner_test = 0, parser_test = 0, class_test = 0;

    // process options

    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'd': 
	      ddsdebug = 1;
	      break;
	    case 's':
	      scanner_test = 1;
	      break;
	    case 'p':
	      parser_test = 1;
	      break;
	    case 'c':
	      class_test = 1;
	      break;
	    case '?': 
	    default:
	      usage(argv[0]);
	      exit(1);
	  }

    if (!scanner_test && !parser_test && !class_test) {
	usage(argv[0]);
	exit(1);
    }

    try {
      if (scanner_test) {
	test_scanner();
      }

      if (parser_test) {
	test_parser();
      }

      if (class_test) {
	test_class();
      }
    }
    catch (Error &e) {
      cerr << e.get_error_message() << endl;
    }
}

void
test_scanner(void)
{
    int tok;

    fprintf( stdout, "%s", prompt ) ; // first prompt
    fflush( stdout ) ;
    while ((tok = ddslex())) {
	switch (tok) {
	  case SCAN_DATASET:
	    fprintf( stdout, "DATASET\n" ) ;
	    break;
	  case SCAN_LIST:
	    fprintf( stdout, "LIST\n" ) ;
	    break;
	  case SCAN_SEQUENCE:
	    fprintf( stdout, "SEQUENCE\n" ) ;
	    break;
	  case SCAN_STRUCTURE:
	    fprintf( stdout, "STRUCTURE\n" ) ;
	    break;
	  case SCAN_FUNCTION:
	    fprintf( stdout, "FUNCTION\n" ) ;
	    break;
	  case SCAN_GRID:
	    fprintf( stdout, "GRID\n" ) ;
	    break;
	  case SCAN_BYTE:
	    fprintf( stdout, "BYTE\n" ) ;
	    break;
	  case SCAN_INT16:
	    fprintf( stdout, "INT16\n" ) ;
	    break;
	  case SCAN_UINT16:
	    fprintf( stdout, "UINT16\n" ) ;
	    break;
	  case SCAN_INT32:
	    fprintf( stdout, "INT32\n" ) ;
	    break;
	  case SCAN_UINT32:
	    fprintf( stdout, "UINT32\n" ) ;
	    break;
	  case SCAN_FLOAT32:
	    fprintf( stdout, "FLOAT32\n" ) ;
	    break;
	  case SCAN_FLOAT64:
	    fprintf( stdout, "FLOAT64\n" ) ;
	    break;
	  case SCAN_STRING:
	    fprintf( stdout, "STRING\n" ) ;
	    break;
	  case SCAN_URL:
	    fprintf( stdout, "Url\n" ) ;
	    break;
	  case SCAN_WORD:
	    fprintf( stdout, "WORD: %s\n", ddslval.word ) ;
	    break;
	  case '{':
	    fprintf( stdout, "Left Brace\n" ) ;
	    break;
	  case '}':
	    fprintf( stdout, "Right Brace\n" ) ;
	    break;
	  case '[':
	    fprintf( stdout, "Left Bracket\n" ) ;
	    break;
	  case ']':
	    fprintf( stdout, "Right Bracket\n" ) ;
	    break;
	  case ';':
	    fprintf( stdout, "Semicolon\n" ) ;
	    break;
	  case ':':
	    fprintf( stdout, "Colon\n" ) ;
	    break;
	  case '=':
	    fprintf( stdout, "Assignment\n" ) ;
	    break;
	  default:
	    fprintf( stdout, "Error: Unrecognized input\n" ) ;
	}
	fprintf( stdout, "%s", prompt ) ; // print prompt after output
	fflush( stdout ) ;
    }
}

void
test_parser(void)
{
    BaseTypeFactory *factory = new BaseTypeFactory;
    DDS table(factory);
    table.parse();
    
    if (table.check_semantics())
	fprintf( stdout, "DDS past semantic check\n" ) ;
    else 
	fprintf( stdout, "DDS failed semantic check\n" ) ;

    if (table.check_semantics(true))
	fprintf( stdout, "DDS past full semantic check\n" ) ;
    else 
	fprintf( stdout, "DDS failed full semantic check\n" ) ;

    table.print( stdout );

    delete factory; factory = 0;
}

void
test_class(void)
{
    BaseTypeFactory *factory = new BaseTypeFactory;
    DDS table(factory);
    table.parse();
    
    if (table.check_semantics())
	fprintf( stdout, "DDS past semantic check\n" ) ;
    else 
	fprintf( stdout, "DDS filed semantic check\n" ) ;

    if (table.check_semantics(true))
	fprintf( stdout, "DDS past full semantic check\n" ) ;
    else 
	fprintf( stdout, "DDS filed full semantic check\n" ) ;

    table.print( stdout );

    DDS table2 = table;		// test copy ctor;
    table2.print( stdout );

    BaseTypeFactory *factory2 = new BaseTypeFactory;
    DDS table3(factory2);
    table3 = table;		// test operator=

    fprintf( stdout, "Dataset name: %s\n", table.get_dataset_name().c_str()) ;

    string name = "goofy";
    table.add_var(table.get_factory()->NewInt32(name)); // table dtor should delete this object

    table.print( stdout );

    BaseType *btp = table.var(name);

    btp->print_decl(stdout, "", true); // print out goofy w/semicolon

    table.del_var(name);

    table.print( stdout );

    table.add_var(table.get_factory()->NewInt32("goofy"));

    table.print( stdout );

    btp = table.var("goofy");

    btp->print_decl(stdout, "", true); // print out goofy w/semicolon

    table.del_var("goofy");

    table.print( stdout );

    for (DDS::Vars_iter p = table.var_begin(); p != table.var_end(); p++)
	(*p)->print_decl(stdout, "", true);	// print them all w/semicolons

    delete factory; factory = 0;
    delete factory2; factory2 = 0;
}

