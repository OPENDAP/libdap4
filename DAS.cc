
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

#include "config_dap.h"

static char rcsid[] not_used ={"$Id: DAS.cc,v 1.41 2003/12/08 18:02:29 edavis Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

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

/** @brief Returns a pointer to the first attribute table. */
Pix
DAS::first_var()
{
    return AttrTable::first_attr();
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

/** @brief Increments an attribute table pointer to indicate the next table
    in the series. */
void
DAS::next_var(Pix p)
{
    AttrTable::next_attr(p);
}

/** @brief Returns the name of the indicated attribute table. */
string
DAS::get_name(Pix p)
{
    return AttrTable::get_name(p);
}

string
DAS::get_name(Attr_iter &i)
{
    return AttrTable::get_name(i);
}

/** @brief Returns the attribute table with the given name. 
    @name get_table()
*/
  
  //@{
/** @brief Returns the indicated attribute table. */
AttrTable *
DAS::get_table(Pix p)
{
    return AttrTable::get_attr_table(p);
}

/** @brief Returns the attribute table with the given name string. */
AttrTable *
DAS::get_table(Attr_iter &i)
{
    return AttrTable::get_attr_table(i);
}

AttrTable *
DAS::get_table(const string &name)
{
    return AttrTable::get_attr_table(name);
}

/** @brief Returns the attribute table with the given name. 

    This function is necessary because (char *) arguments will be
    converted to Pixs (and not strings). Without this member
    function get_table(name) needs a cast, and it seems tough to
    believe folks will always remember that.   
*/
AttrTable *
DAS::get_table(const char *name)
{
    return get_table((string)name);
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

/** @brief Adds an attribute table to the DAS. */
AttrTable *
DAS::add_table(const char *name, AttrTable *at)
{
    return add_table((string)name, at);
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

    dasrestart(in);

    parser_arg arg(this);

    bool status = dasparse((void *)&arg) == 0;

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
	if (arg.error())
	  throw *arg.error();
    }
}

//@}

/** @brief Creates an ASCII representation of a DAS on the given output
    stream. 

    Write attributes from tables to `out' (which defaults to
    stdout). Return true. 

    When an identifier contains a character that contains
    characters that cannot be present in a URL (e.g., a space)
    AttrTable::print replaces those characters with WWW
    escape codes. 7/13/2001 jhrg 

    @deprecated Using the C++ iostream class is deprecated.
*/

void
DAS::print(ostream &os, bool dereference)
{
    os << "Attributes {" << endl;

    AttrTable::print(os, "    ", dereference);

    os << "}" << endl;

}

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

// $Log: DAS.cc,v $
// Revision 1.41  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.40.2.2  2003/09/06 22:37:50  jimg
// Updated the documentation.
//
// Revision 1.40.2.1  2003/06/06 08:28:28  reza
// Error code changes in the error object.
//
// Revision 1.40  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.39  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.38.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.38  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.37  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.32.4.10  2002/12/17 22:35:02  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.32.4.9  2002/11/21 21:24:17  pwest
// memory leak cleanup and file descriptor cleanup
//
// Revision 1.32.4.8  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.32.4.7  2002/09/12 22:49:57  pwest
// Corrected signature changes made with Pix to IteratorAdapter changes. Rather
// than taking a reference to a Pix, taking a Pix value.
//
// Revision 1.32.4.6  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.32.4.5  2002/08/08 06:54:56  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.36  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.35  2001/08/27 16:39:42  jimg
// Fixed up cruft from merge.
//
// Revision 1.34  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.32.4.4  2001/07/28 01:10:42  jimg
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
// Revision 1.33  2001/01/26 19:48:09  jimg
// Merged with release-3-2-3.
//
// Revision 1.32.4.3  2000/11/30 05:24:46  jimg
// Significant changes and improvements to the AttrTable and DAS classes. DAS
// now is a child of AttrTable, which makes attributes behave uniformly at
// all levels of the DAS object. Alias now work. I've added unit tests for
// several methods in AttrTable and some of the functions in parser-util.cc.
// In addition, all of the DAS tests now work.
//
// Revision 1.32.4.2  2000/11/22 21:47:42  jimg
// Changed the implementation of DAS; it now inherits from AttrTable
//
// Revision 1.32.4.1  2000/11/22 01:31:30  jimg
// Made this class a child of AttrTable. Most methods are now simply calls to
// methods of AttrTable.
//
// Revision 1.32  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.31  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.30  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.29  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.28  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.27.6.1  2000/06/02 18:16:47  rmorris
// Mod's for port to Win32.
//
// Revision 1.26.6.2  2000/05/12 18:46:17  jimg
// Minor changes in the dtor.
//
// Revision 1.26.14.2  2000/02/17 05:03:12  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.26.14.1  2000/02/07 21:11:35  jgarcia
// modified prototypes and implementations to use exceeption handling
//
// Revision 1.27  2000/01/27 06:29:56  jimg
// Resolved conflicts from merge with release-3-1-4
//
// Revision 1.26.6.1  2000/01/26 23:56:22  jimg
// Fixed the return type of string::find.
//
// Revision 1.26  1999/04/29 02:29:28  jimg
// Merge of no-gnu branch
//

// Revision 1.25  1999/03/24 23:37:14  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.24  1998/11/24 06:52:42  jimg
// Fixed copyright.
//
// Revision 1.23  1998/11/24 06:46:08  jimg
// Ripped out the DASVHMap class and replaced it with an SLList of structs.
// See DAS.h for the (private) struct definition. There are no changes to the
// class interface. I did add a default ctor (DAS()), but that conflicted
// with another (old) ctor that had defaults for its two parameters, so I
// bagged that for now.
//
// Revision 1.22  1998/08/13 22:11:24  jimg
// Added include of unistd.h for dup(2).
//
// Revision 1.21.14.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.21.14.1  1999/02/02 21:56:57  jimg
// String to string version
//
// Revision 1.21  1997/06/06 00:44:02  jimg
// Removed add_table(char *, ...).
//
// Revision 1.20  1997/05/13 23:32:14  jimg
// Added changes to handle the new Alias and lexical scoping rules.
//
// Revision 1.19  1996/08/13 18:00:32  jimg
// Switched to use of parse_arg object for passing values to and from the
// bison generated parser.
// Added test for error objects on return from the parser and call the
// display_message member function when one is detected.
//
// Revision 1.18  1996/05/31 23:29:34  jimg
// Updated copyright notice.
//
// Revision 1.17  1996/04/05 00:21:25  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.16  1995/08/23  00:05:40  jimg
// Added copyright notice.
//
// Revision 1.15  1995/07/09  21:28:54  jimg
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

