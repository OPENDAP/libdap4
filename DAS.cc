
/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*///      The DODS Data Access Protocol (DAP) Library

//      Copyright (C) 1994, 1995 The University of Rhode Island, Graduate
//      School of Oceanography. 
//     
//      This library is free software; you can redistribute it and/or
//      modify it under the terms of the GNU Library General Public
//      License as published by the Free Software Foundation; either
//      version 2 of the License, or (at your option) any later version.
//     
//      This library is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//      Library General Public License for more details.
//     
//      You should have received a copy of the GNU Library General Public
//      License along with this library; if not, write to the
//      Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
//      MA 02139, USA.
//
//      Authors: James Gallagher (jimg@dcz.gso.uri.edu), 
//               Dan Holloway (dan@hollywood.gso.uri.edu), 
//               Reza Nekovei (reza@hadaf.gso.uri.edu)
//
//               The University of Rhode Island
//               South Ferry Rd.
//               Narragansett, RI. 02882
//               U.S.A.
//
// Methods for the class DAS - a class used to parse the dataset attribute
// structure.
//
// jhrg 7/25/94

// $Log: DAS.cc,v $
// Revision 1.15  1995/07/09 21:28:54  jimg
// Added copyright notice.
//
// Revision 1.14  1995/07/08  18:31:11  jimg
// Added extern keyword for the external declarations of dasparse and
// dasrestart.
//
// Revision 1.13  1995/05/10  13:45:11  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.12  1995/02/10  02:29:37  jimg
// Added second parameter to dasparse. It is a switch that indicates that
// the parse went OK. Now the parser will return true even if erros were
// detected so long as they were recoverable errors.
//
// Revision 1.11  1995/01/19  21:58:49  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.10  1994/11/22  14:05:36  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.9  1994/10/17  23:39:49  jimg
// Removed unnecessary print functions.
//
// Revision 1.8  1994/10/13  16:42:59  jimg
// dasrestart was incorrectly declared as void dasrestart(...) in DAS.cc.
// This caused the alpha to say `Could not read from file' whenever
// dasrestart was called (which happens whenever a new file is read). Fixed
// the declaration and removed the test on the (phantom) return value.
//
// Revision 1.7  1994/10/13  15:46:57  jimg
// Added compile-time switched instrumentation.
// Removed the three definitions of DAS::print().
// Added DAS::print(ostream &os = cout) -- this is the only function for
// printing the in-memory DAS.
//
// Revision 1.6  1994/10/05  16:34:12  jimg
// Fixed bug in the parse function(s): the bison generated parser returns
// 1 on error, 0 on success, but parse() was not checking for this.
// Instead it returned the value of bison's parser function.
// Changed types of `status' in print and parser functions from int to bool.
//
// Revision 1.5  1994/09/27  22:46:29  jimg
// Changed the implementation of the class DAS from one which inherited
// from DASVHMap to one which contains an instance of DASVHMap.
// Added mfuncs to set/access the new instance variable.
//
// Revision 1.4  1994/09/09  15:33:38  jimg
// Changed the base name of this class's parents from `Var' to DAS.
// Added print() and removed operator<< (see the comments in AttrTable).
// Added overloaded versions of print() and parse(). They can be called
// using nothing (which defaults to std{in,out}), with a file descriptor,
// with a FILE *, or with a String givin a file name.
//
// Revision 1.3  1994/08/02  20:11:25  jimg
// Changes operator<< so that it writes a parsable version of the
// attribute table.
//
// Revision 1.2  1994/08/02  19:17:40  jimg
// Fixed log comments and rcsid[] variables (syntax errors due to //
// comments caused compilation failures).
// das.tab.c and .h are commited now as well.
//
// Revision 1.1  1994/08/02  18:40:09  jimg
// Implemetation of the DAS class. This class is a container that maps
// String objects which name variables to AttrTablePtr objects.
//

static char rcsid[]="$Id: DAS.cc,v 1.15 1995/07/09 21:28:54 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>

#include <iostream.h>
#include <Pix.h>
#include <String.h>

#include "config_dap.h"
#include "debug.h"

#include "DAS.h"		// follows pragma since DAS.h is interface

extern void dasrestart(FILE *yyin);
extern int dasparse(DAS &table, int &parse_ok); // defined in das.tab.c

DAS::DAS(AttrTablePtr dflt, unsigned int sz) : map(dflt, sz)
{
}

// The class DASVHMap knows that it contains pointers to things and correctly
// makes copies of those things when its copy ctor is called, so DAS can do a
// simple member-wise copy. Similarly, we don't need to define our own op=.

// This deletes the pointers to AttrTables allocated during the parse (and at 
// other times). jhrg 7/29/94

DAS::~DAS()
{
    for(Pix p = map.first(); p; map.next(p)) {
	DBG(cerr << "map.contents() = " << map.contents(p) << endl);
	delete map.contents(p);
    }
}

Pix
DAS::first_var()
{
    return map.first();
}

void
DAS::next_var(Pix &p)
{
    map.next(p);
}

String &
DAS::get_name(Pix p)
{
    return map.key(p);
}

AttrTable *
DAS::get_table(Pix p)
{
    return map.contents(p);
}

AttrTable *
DAS::get_table(const String &name)
{
    return map[name];
}

// This function is necessary because (char *) arguments will be converted to
// Pixs (and not Strings). Without this mfunc get_table(name) needs a cast;
// it seems tough to believe folks will always remember that.

AttrTable *
DAS::get_table(const char *name)
{
    return map[name];
}

AttrTable *
DAS::add_table(const String &name, AttrTable *at)
{
    return map[name] = at;
}

AttrTable *
DAS::add_table(const char *name, AttrTable *at)
{
//    DBG2(cerr << "In DAS::add_table(const char *, AttrTable *" << endl);
    return add_table((String)name, at);
}

// Read attributes from a file. Returns false if unable to open the file,
// otherwise returns the result of the mfunc parse.

bool
DAS::parse(String fname)
{
    FILE *in = fopen(fname, "r");

    if (!in) {
	cerr << "Could not open: " << fname << endl;
	return false;
    }

    bool status = parse(in);

    fclose(in);

    return status;
}

// Read attributes from a file descriptor. If the file descriptor cannot be
// fdopen'd, return false, otherwise return the status of the mfunc parse.
// 
// NB: Added call to dup() within fdopen so that once the FILE * is closed the
// decriptor fd will not also be closed (instead the duplicate descriptor will
// be closed). Thus further information can be read from the descriptor fd.

bool
DAS::parse(int fd)
{
    FILE *in = fdopen(dup(fd), "r");

    if (!in) {
	cerr << "Could not access file" << endl;
	return false;
    }

    bool status = parse(in);

    fclose(in);

    return status;
}

    
// Read attributes from in (which defaults to stdin). If dasrestart() fails,
// return false, otherwise return the status of dasparse().

bool
DAS::parse(FILE *in)
{
    int parse_ok;
    dasrestart(in);

    return dasparse(*this, parse_ok) == 0 && parse_ok;
}

// Write attributes from tables to `out' (which defaults to stdout). Return
// true. 

bool
DAS::print(ostream &os)
{
    os << "Attributes {" << endl;

    for(Pix p = map.first(); p; map.next(p)) {
	os << "    " << map.key(p) << " {" << endl;
	// map.contents(p) is an (AttrTable *)
	map.contents(p)->print(os, "        "); 
	os << "    }" << endl;
    }

    os << "}" << endl;

    return true;
}
