
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Methods for the class DAS - a class used to parse the dataset attribute
// structure.
//
// jhrg 7/25/94

// $Log: DAS.cc,v $
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

static char rcsid[] __unused__ ={"$Id: DAS.cc,v 1.20 1997/05/13 23:32:14 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <assert.h>

#include <iostream.h>
#include <Pix.h>
#include <String.h>

#include "DAS.h"		// follows pragma since DAS.h is interface
#include "Error.h"
#include "parser.h"
#include "debug.h"

extern void dasrestart(FILE *yyin);
extern int dasparse(void *arg); // defined in das.tab.c

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

String
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
    // `.' separates hierarchies in the DAS.
    if (name.contains(".")) {
	String n = (String)name; // cast away const
	String container = n.before(".");
	String field = n.after(".");

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

	AttrTable *at = map[container];
	AttrTable *at2 = (at) ? at->get_attr_table(field) : 0;
	return (at) ? ((at2) ? at2 : at) : 0;
    }
    else
	return map[name];
}

// This function is necessary because (char *) arguments will be converted to
// Pixs (and not Strings). Without this mfunc get_table(name) needs a cast;
// it seems tough to believe folks will always remember that.

AttrTable *
DAS::get_table(const char *name)
{
    return get_table((String)name);
}

AttrTable *
DAS::add_table(const String &name, AttrTable *at)
{
    return map[name] = at;
}

#if 0
AttrTable *
DAS::add_table(const char *name, AttrTable *at)
{
//    DBG2(cerr << "In DAS::add_table(const char *, AttrTable *" << endl);
    return add_table((String)name, at);
}
#endif

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

    for(Pix p = map.first(); p; map.next(p)) {
	os << "    " << map.key(p) << " {" << endl;
	// map.contents(p) is an (AttrTable *)
	map.contents(p)->print(os, "        "); 
	os << "    }" << endl;
    }

    os << "}" << endl;

    return true;
}
