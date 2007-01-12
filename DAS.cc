
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

// Methods for the class DAS - a class used to parse the dataset attribute
// structure.
//
// jhrg 7/25/94

#include "config.h"

static char rcsid[] not_used ={"$Id$"};


#include <stdio.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <string>

#include "DAS.h"		// follows pragma since DAS.h is interface
#include "Error.h"
#include "InternalErr.h"
#include "parser.h"
#include "escaping.h"
#include "debug.h"

using std::cerr;
using std::endl;

// Glue routines declared in das.lex
extern void das_switch_to_buffer(void *new_buffer);
extern void das_delete_buffer(void * buffer);
extern void *das_buffer(FILE *fp);

extern void dasrestart(FILE *yyin);
extern int dasparse(void *arg); // defined in das.tab.c

AttrTable *
DAS::das_find(string name)
{
    return find_container(name); // Only containers at the top level.
}

/** Create a DAS from a single attribute table.  

    \note In an older version of this class, <tt>dflt</tt> and <tt>sz</tt>
    initialized a hash table. That is no longer used and these
    params should no longer matter. Note that this constructor is
    effectively the empty constructor. 11/23/98 jhrg

    @param dflt A pointer to a valid attribute table.
    @param sz The number of entries in the table. This is unused. It
    was part of the ctor when DAS used the old GNU VHMap class. I
    switched from that to a SLList of struct toplevel_entry objects
    because the VHMap class had bugs I didn't want to fix. 11/23/98
    jhrg  */
DAS::DAS(AttrTable *, unsigned int)
{
}

/** Create a DAS object with one attribute table. Use <tt>append_attr()</tt>
    to add additional attributes.

    @see append_attr()
    @param attr The initial AttrTable. 
    @param name The name of the DAS parent structure.
*/

DAS::DAS(AttrTable *attr, string name)
{
    append_container(attr, www2id(name));
}

// The class DASVHMap knows that it contains pointers to things and correctly
// makes copies of those things when its copy ctor is called, so DAS can do a
// simple member-wise copy. Similarly, we don't need to define our own op=.

// This deletes the pointers to AttrTables allocated during the parse (and at 
// other times). jhrg 7/29/94

DAS::~DAS()
{
}

AttrTable::Attr_iter
DAS::var_begin()
{
    return AttrTable::attr_begin() ;
}

AttrTable::Attr_iter
DAS::var_end()
{
    return AttrTable::attr_end() ;
}


string
DAS::get_name(Attr_iter &i)
{
    return AttrTable::get_name(i);
}

/** @brief Returns the attribute table. */
AttrTable *
DAS::get_table(Attr_iter &i)
{
    return AttrTable::get_attr_table(i);
}

/** @brief Returns the attribute table with the given name. 
*/
AttrTable *
DAS::get_table(const string &name)
{
    return AttrTable::get_attr_table(name);
}

//@}

/** @brief Adds an attribute table to the DAS.
    @name add_table()
*/
  //@{

/** @brief Adds an attribute table to the DAS. */
AttrTable *
DAS::add_table(const string &name, AttrTable *at)
{
    DBG(cerr << "Adding table: " << name << "(" << at << ")" << endl);
    return AttrTable::append_container(at, name);
}

//@}

/** @brief Reads a DAS in from an external source. 

    @name parse()
*/
    //@{


/** @brief Reads a DAS from the named file. 

    Read attributes from a file. Returns false if unable to open
    the file, otherwise returns the result of the mfunc parse. */
void
DAS::parse(string fname)
{
    FILE *in = fopen(fname.c_str(), "r");

    if (!in) {
      throw Error(can_not_read_file, "Could not open: " + fname);
    }

    parse(in);

    int res = fclose(in);
    if( res ) {
	DBG(cerr << "DAS::parse - Failed to close file " << (void *)in << endl ;) ;
    }
}

/** @brief Read attributes from a file descriptor. 

    If the file descriptor cannot be fdopen'd, return false, otherwise
    return the status of the mfunc parse. 

    \note Added call to dup() within fdopen so that once the FILE * is
    closed the decriptor fd will not also be closed (instead the
    duplicate descriptor will be closed). Thus further information can
    be read from the descriptor fd. 
*/
void
DAS::parse(int fd)
{
#ifdef WIN32
    FILE *in = fdopen(_dup(fd), "r");
#else
    FILE *in = fdopen(dup(fd), "r");
#endif

    if (!in) {
	throw InternalErr(__FILE__, __LINE__, "Could not access file.");
    }

    parse(in);

    int res = fclose(in);
    if( res ) {
	DBG(cerr << "DAS::parse(fd) - Failed to close " << (void *)in << endl ;) ;
    }
}

    

/** @brief Reads a DAS from an open file descriptor. 

    Read attributes from in (which defaults to stdin). If
    dasrestart() fails, return false, otherwise return the status
    of dasparse(). 
*/
void
DAS::parse(FILE *in)
{
    if (!in) {
	throw InternalErr(__FILE__, __LINE__, "Null input stream.");
    }

    void *buffer = das_buffer(in);
    das_switch_to_buffer(buffer);

    parser_arg arg(this);

    bool status = dasparse((void *)&arg) == 0;

    das_delete_buffer(buffer);

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
	if (arg.error())
	  throw *arg.error();
    }
}

//@}

/** Creates an ASCII representation of a DAS on the given output
    stream.

    When an identifier contains a character that contains
    characters that cannot be present in a URL (e.g., a space)
    AttrTable::print replaces those characters with WWW
    escape codes. 7/13/2001 jhrg 

    @param out output FILE on which to print the DAS
    @param dereference If true, follow aliases. Default is false.
*/

void
DAS::print(FILE *out, bool dereference)
{
    fprintf( out, "Attributes {\n" ) ;

    AttrTable::print(out, "    ", dereference);

    fprintf( out, "}\n" ) ;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and then calls parent dump
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
DAS::dump( ostream &strm ) const
{
    strm << DapIndent::LMarg << "DAS::dump - ("
			      << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    AttrTable::dump( strm ) ;
    DapIndent::UnIndent() ;
}

