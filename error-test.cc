
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

// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Test the Error object scanner, parser and class.
//
// jhrg 4/25/96

#include "config.h"

static char rcsid[] not_used =
    {"$Id$"
    };

#include <GetOpt.h>

#include "Error.h"
#include "parser.h"
#include "Error.tab.hh"

void test_scanner();
void test_parser(Error &err);
#ifdef GUI
void test_object(Error &err);
#endif
void usage();

int Errorlex();
int Errorparse(void *);

extern YYSTYPE Errorlval;
extern int Errordebug;
const char *prompt = "error-test: ";

#ifdef WIN32
void
#else
int
#endif
main(int argc, char *argv[])
{
#ifdef WIN32
    GetOpt getopt(argc, argv, "spd");
#else
    GetOpt getopt(argc, argv, "spdo");
#endif
    int option_char;
    bool scanner_test = false, parser_test = false, object_test = false;

    // process options

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            Errordebug = 1;
            break;
        case 's':
            scanner_test = true;
            break;
        case 'p':
            parser_test = true;
            break;
#ifndef WIN32
        case 'o':
            parser_test = object_test = true;
            break;
#endif
        case '?':
        default:
            usage();
        }

#ifdef WIN32
    if (!(scanner_test || parser_test))
#else
    if (!(scanner_test || parser_test || object_test))
#endif
        usage();

    if (scanner_test)
        test_scanner();

    Error err;
    if (parser_test)
        test_parser(err);

#ifdef GUI
    if (object_test)
        test_object(err);
#endif

#ifdef WIN32
    exit(0);  //  Cygwin/Dejagu test suites require this to succeed.
    return;   //  Visual C++ requires this.
#endif
}

void
usage()
{
#ifdef WIN32
    fprintf(stderr, "usage: error-test: [d][sp] <  filename ...\n") ;
#else
    fprintf(stderr, "usage: error-test: [d][spo] <  filename ...\n") ;
#endif
    fprintf(stderr, "       d: extra parser debugging information\n") ;
    fprintf(stderr, "       s: run the scanner\n") ;
    fprintf(stderr, "       p: run the parser\n") ;
#ifdef WIN32
    fprintf(stderr, "       o: evaluate the object, runs the parser\n") ;
#endif
}

void
test_scanner()
{
    int tok;

    fprintf(stdout, "%s", prompt) ;   // first prompt
    fflush(stdout) ;
    while ((tok = Errorlex())) {
        switch (tok) {
        case SCAN_ERROR:
            fprintf(stdout, "ERROR\n") ;
            break;
        case SCAN_CODE:
            fprintf(stdout, "CODE\n") ;
            break;
        case SCAN_PTYPE:
            fprintf(stdout, "PTYPE\n") ;
            break;
        case SCAN_MSG:
            fprintf(stdout, "MSG\n") ;
            break;
        case SCAN_PROGRAM:
            fprintf(stdout, "PROGRAM\n") ;
            break;
        case SCAN_STR:
            fprintf(stdout, "%s\n", Errorlval.string) ;
            break;
        case SCAN_INT:
            fprintf(stdout, "%d\n", Errorlval.integer) ;
            break;
        case '{':
            fprintf(stdout, "Left Brace\n") ;
            break;
        case '}':
            fprintf(stdout, "Right Brace\n") ;
            break;
        case ';':
            fprintf(stdout, "Semicolon\n") ;
            break;
        case '=':
            fprintf(stdout, "Assignment\n") ;
            break;
        default:
            fprintf(stdout, "Error: Unrecognized input\n") ;
        }
        fprintf(stdout, "%s", prompt) ;   // print prompt after output
        fflush(stdout) ;
    }
}

void
test_parser(Error &err)
{
    int status = err.parse(stdin);
    fprintf(stdout, "Status from parser: %d\n", status) ;

    if (err.OK())
        fprintf(stdout, "Error passed OK check\n") ;
    else
        fprintf(stdout, "Error failed OK check\n") ;

    err.print(stdout);
}
