
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Test the DAS class.
// Read attributes from one or more files, printing the resulting table to
// stdout. If a file is named `-' read from stdin for that file. The option
// `-d' causes new/delete run-time debugging to be turned on.
//
// jhrg 7/25/94

#include "config.h"

#include <cstdlib>
#include <string>
#include <GetOpt.h>

#define YYSTYPE char *

#include "DAS.h"
#include "das.tab.hh"
#include "Error.h"

using namespace libdap ;

void plain_driver(DAS &das, bool deref_alias);
void load_attr_table(AttrTable at);
void load_attr_table_ptr(AttrTable *atp);
void parser_driver(DAS &das, bool deref_alias, bool as_xml);
void test_scanner();

int daslex();

extern int dasdebug;
const char *prompt = "das-test: ";
const char *version = "version 1.19";

using namespace std;

void
usage(string name)
{
    fprintf( stderr, "usage: %s %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n",
		     name.c_str(),
		     "[-v] [-s] [-d] [-c] [-p] [rx] {< in-file > out-file}",
		     "s: Test the DAS scanner.",
		     "p: Scan and parse from <in-file>; print to <out-file>.",
		     "c: Test building the DAS from C++ code.",
		     "v: Print the version of das-test and exit.",
		     "d: Print parser debugging information.",
		     "r: Print the DAS with aliases deReferenced.",
		     "x: Print as xml.") ;
}

int main(int argc, char *argv[])
{

    GetOpt getopt (argc, argv, "scpvdrx");
    int option_char;
    bool parser_test = false;
    bool scanner_test = false;
    bool code_test = false;
    bool deref_alias = false;
    bool as_xml = false;
    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'p':
	      parser_test = true;
	      break;
	    case 's':
	      scanner_test = true;
	      break;
	    case 'c':
	      code_test = true;
	      break;
	    case 'v':
	      fprintf( stderr, "%s: %s\n", argv[0], version ) ;
	      return 0;
	    case 'd':
	      dasdebug = 1;
	      break;
	    case 'r':
	      deref_alias = true;
	      break;
	    case 'x':
	        as_xml = true;
	        break;
	    case '?':
	    default:
	      usage(argv[0]);
              return 1;
	  }

    DAS das;

    if (!parser_test && !scanner_test && !code_test) {
	usage(argv[0]);
	return 1;
    }

    try {
      if (parser_test)
	parser_driver(das, deref_alias, as_xml);

      if (scanner_test)
	test_scanner();

      if (code_test)
	plain_driver(das, deref_alias);
    }
    catch (Error &e) {
	cerr << "Caught Error object:" << endl;
	cerr << e.get_error_message() << endl;
	return 1;
    }

    return 0;
}

void
test_scanner()
{
    int tok;

    fprintf( stdout, "%s", prompt ) ; // first prompt
    fflush( stdout ) ;
    while ((tok = daslex())) {
	switch (tok) {
	  case SCAN_ATTR:
	    fprintf( stdout, "ATTR\n" ) ;
	    break;
	  case SCAN_ALIAS:
	    fprintf( stdout, "ALIAS\n" ) ;
	    break;
	  case SCAN_WORD:
	    fprintf( stdout, "WORD=%s\n", daslval ) ;
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
            fprintf( stdout, "URL\n" ) ;
            break;

          case SCAN_XML:
            fprintf( stdout, "OtherXML\n" ) ;
            break;

	  case '{':
	    fprintf( stdout, "Left Brace\n" ) ;
	    break;
	  case '}':
	    fprintf( stdout, "Right Brace\n" ) ;
	    break;
	  case ';':
	    fprintf( stdout, "Semicolon\n" ) ;
	    break;
	  case ',':
	    fprintf( stdout, "Comma\n" ) ;
	    break;

	  default:
	    fprintf( stdout, "Error: Unrecognized input\n" ) ;
	    break;
	}
	fprintf( stdout, "%s", prompt ) ; // print prompt after output
	fflush( stdout ) ;
    }
}


void
parser_driver(DAS &das, bool deref_alias, bool as_xml)
{
    das.parse();

    if (as_xml) {
        das.get_top_level_attributes()->print_xml(stdout, "    ");
    }
    else
        das.print(stdout, deref_alias);
}

// Given a DAS, add some stuff to it.

void
plain_driver(DAS &das, bool deref_alias)
{
    AttrTable *atp;
    AttrTable *dummy;

    string name = "test";
    atp = new AttrTable;
    load_attr_table_ptr(atp);
    dummy = das.get_table(name);
    das.add_table(name, atp);

    name = "test2";
    atp = new AttrTable;
    load_attr_table_ptr(atp);
    das.add_table(name, atp);

    das.print(stdout, deref_alias);
}

// stuff an AttrTable full of values. Also, print it out.

void
load_attr_table(AttrTable at)
{
    at.append_attr("month", "String", "Feb");
    at.append_attr("month", "String", "Feb");

    at.append_attr("month_a", "String", "Jan");
    at.append_attr("month_a", "String", "Feb");
    at.append_attr("month_a", "String", "Mar");

    at.append_attr("Date", "Int32", "12345");
    at.append_attr("day", "Int32", "01");
    at.append_attr("Time", "Float64", "3.1415");

    fprintf( stdout, "Using the iterator:\n" ) ;
    for (AttrTable::Attr_iter p2 = at.attr_begin(); p2 != at.attr_end(); p2++)
    {
		fprintf( stdout, "%s %s ", at.get_name(p2).c_str(),
			at.get_type(p2).c_str() ) ;
		for (unsigned i = 0; i < at.get_attr_num(p2); ++i)
			fprintf( stdout, "%s ", at.get_attr(p2, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    string name = "month";
    fprintf( stdout, "Using String: %s %s %s\n",
		     at.get_type(name).c_str(),
		     at.get_attr(name, 0).c_str(),
		     at.get_attr(name, 1).c_str()) ;
    fprintf( stdout, "Using char *: %s %s %s\n",
		     at.get_type("month").c_str(),
		     at.get_attr("month", 0).c_str(),
		     at.get_attr("month", 1).c_str() ) ;

    at.del_attr("month");

    fprintf( stdout, "After deletion:\n" ) ;
    for (AttrTable::Attr_iter p3 = at.attr_begin(); p3 != at.attr_end(); p3++)
    {
		fprintf( stdout, "%s %s ", at.get_name(p3).c_str(),
			at.get_type(p3).c_str() ) ;
		for (unsigned i = 0; i < at.get_attr_num(p3); ++i)
			fprintf( stdout, "%s ", at.get_attr(p3, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    at.print(stdout);

    fprintf( stdout, "After print:\n" ) ;
    for (AttrTable::Attr_iter p4 = at.attr_begin(); p4 != at.attr_end(); p4++)
    {
	fprintf( stdout, "%s %s ", at.get_name(p4).c_str(),
		at.get_type(p4).c_str() ) ;
	for (unsigned i = 0; i < at.get_attr_num(p4); ++i)
	     fprintf( stdout, "%s ", at.get_attr(p4, i).c_str() ) ;
	fprintf( stdout, "\n" ) ;
    }
}

// OK, now try it with a dymanic AttrTable

void
load_attr_table_ptr(AttrTable *at)
{
    at->append_attr("month", "String", "Feb");
    at->append_attr("month", "String", "Feb");

    at->append_attr("month_a", "String", "Jan");
    at->append_attr("month_a", "String", "Feb");
    at->append_attr("month_a", "String", "Mar");

    at->append_attr("Date", "Int32", "12345");
    at->append_attr("day", "Int32", "01");
    at->append_attr("Time", "Float64", "3.1415");

    fprintf( stdout, "Using the iterator:\n" ) ;
    for (AttrTable::Attr_iter p2 = at->attr_begin(); p2 != at->attr_end(); p2++)
    {
		fprintf( stdout, "%s %s ", at->get_name(p2).c_str(),
			at->get_type(p2).c_str() ) ;
		for (unsigned i = 0; i < at->get_attr_num(p2); ++i)
			fprintf( stdout, "%s ", at->get_attr(p2, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    string name = "month";
    fprintf( stdout, "Using String: %s %s %s\n",
		     at->get_type(name).c_str(),
		     at->get_attr(name, 0).c_str(),
		     at->get_attr(name, 1).c_str() ) ;
    fprintf( stdout, "Using char *: %s %s %s\n",
		     at->get_type("month").c_str(),
		     at->get_attr("month", 0).c_str(),
		     at->get_attr("month", 1).c_str() ) ;

    at->del_attr("month");

    fprintf( stdout, "After deletion:\n" ) ;
    for (AttrTable::Attr_iter p3 = at->attr_begin(); p3 != at->attr_end(); p3++)
    {
		fprintf( stdout, "%s %s ", at->get_name(p3).c_str(),
			at->get_type(p3).c_str() ) ;
		for (unsigned i = 0; i < at->get_attr_num(p3); ++i)
			fprintf( stdout, "%s ", at->get_attr(p3, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    at->print(stdout);

    fprintf( stdout, "After print:\n" ) ;
    for (AttrTable::Attr_iter p4 = at->attr_begin(); p4 !=at->attr_end(); p4++)
    {
		fprintf( stdout, "%s %s ", at->get_name(p4).c_str(),
			at->get_type(p4).c_str() ) ;
		for (unsigned i = 0; i < at->get_attr_num(p4); ++i)
			fprintf( stdout, "%s ", at->get_attr(p4, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }
}

