
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Methods for the class DAS - a class used to parse the dataset attribute
// structure.
//
// jhrg 7/25/94

// $Log: DAS.cc,v $
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

#include "config_dap.h"

static char rcsid[] not_used ={"$Id: DAS.cc,v 1.30 2000/07/09 22:05:35 rmorris Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <assert.h>

#include <iostream>
#include <Pix.h>
#include <string>

#include "debug.h"
#include "Error.h"
#include "parser.h"
#include "debug.h"

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

#include "DAS.h"		// follows pragma since DAS.h is interface

extern void dasrestart(FILE *yyin);
extern int dasparse(void *arg); // defined in das.tab.c

AttrTable *
DAS::das_find(string name)
{
    for (Pix p = entries.first(); p; entries.next(p))
	if (entries(p).name == name)
	    return entries(p).attr_table;
    return 0;
}

// sz is unused. It was part of the ctor when DAS used the old GNU VHMap
// class. I switched from that to a SLList of struct toplevel_entry objects
// because the VHMap class had bugs I didn't want to fix. 11/23/98 jhrg

DAS::DAS(AttrTable *, unsigned int)
{
}

DAS::DAS(AttrTable *attr, string name)
{
    toplevel_entry tle;
    tle.name = name;
    tle.attr_table = attr;
    entries.append(tle);
}
// The class DASVHMap knows that it contains pointers to things and correctly
// makes copies of those things when its copy ctor is called, so DAS can do a
// simple member-wise copy. Similarly, we don't need to define our own op=.

// This deletes the pointers to AttrTables allocated during the parse (and at 
// other times). jhrg 7/29/94

DAS::~DAS()
{
    for(Pix p = entries.first(); p; entries.next(p)) {
	DBG(cerr << "entries(p) = " << entries(p).name << "(" 
	    << entries(p).attr_table << ")" << endl);
	delete entries(p).attr_table;
    }
}

Pix
DAS::first_var()
{
    return entries.first();
}

void
DAS::next_var(Pix &p)
{
    entries.next(p);
}

string
DAS::get_name(Pix p)
{
    return entries(p).name;
}

AttrTable *
DAS::get_table(Pix p)
{
    return entries(p).attr_table;
}

AttrTable *
DAS::get_table(const string &name)
{
    // `.' separates hierarchies in the DAS.
    string::size_type dotpos = name.find('.');
    if (dotpos != name.npos) {
	string container = name.substr(0, dotpos);
	string field = name.substr(dotpos+1);

	// The following strangeness is due to the weird implmentation of
	// DAS/AttrTable objects. For a name like cont1.cont2.var1.a1,
	// look first in the DAS object for `cont1', if found, look in the
	// attribute table (AttrTable) for `cont1' for `cont2.var1'. Note
	// that AttrTable::get_attr_table() takes `cont2.var1.a1' as its
	// argument but *returns the AttrTable for `cont2.var1'* because `a1'
	// is the name of the actual attribute. In order to access the data
	// for `a1', you need *both the AttrTable and the name or Pix of the
	// attribute*. 

	// The DAS is a simple one-level data structure (names and
	// AttrTables) while AttrTables are recursive.

	AttrTable *at = das_find(container.c_str());
	AttrTable *at2 = (at) ? at->get_attr_table(field) : 0;
	return (at) ? ((at2) ? at2 : at) : 0;
    }
    else
	return das_find(name.c_str());
}

// This function is necessary because (char *) arguments will be converted to
// Pixs (and not strings). Without this mfunc get_table(name) needs a cast;
// it seems tough to believe folks will always remember that.

AttrTable *
DAS::get_table(const char *name)
{
    return get_table((string)name);
}

AttrTable *
DAS::add_table(const string &name, AttrTable *at)
{
    DBG(cerr << "Adding table: " << name << "(" << at << ")" << endl);
    toplevel_entry tle;
    tle.name = name;
    tle.attr_table = at;
    entries.append(tle);
    return at;
}

AttrTable *
DAS::add_table(const char *name, AttrTable *at)
{
    return add_table((string)name, at);
}

// Read attributes from a file. Returns false if unable to open the file,
// otherwise returns the result of the mfunc parse.

bool
DAS::parse(string fname)
{
    FILE *in = fopen(fname.c_str(), "r");

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
#ifdef WIN32
    FILE *in = fdopen(_dup(fd), "r");
#else
    FILE *in = fdopen(dup(fd), "r");
#endif

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
    if (!in) {
	cerr << "DAS::parse: Null input stream" << endl;
	return false;
    }

    dasrestart(in);

    parser_arg arg(this);

    bool status = dasparse((void *)&arg) == 0;

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
	if (arg.error())
	    arg.error()->display_message();
	return false;
    }
    else
	return true;
}

// Write attributes from tables to `out' (which defaults to stdout). Return
// true. 

bool
DAS::print(ostream &os)
{
    os << "Attributes {" << endl;

    for(Pix p = entries.first(); p; entries.next(p)) {
	os << "    " << entries(p).name << " {" << endl;
	entries(p).attr_table->print(os, "        "); 
	os << "    }" << endl;
    }

    os << "}" << endl;

    return true;
}
