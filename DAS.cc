
// Methods for the class DAS - a class used to parse the dataset attribute
// structure.
//
// jhrg 7/25/94

// $Log: DAS.cc,v $
// Revision 1.8  1994/10/13 16:42:59  jimg
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

static char rcsid[]="$Id: DAS.cc,v 1.8 1994/10/13 16:42:59 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include <iostream.h>
#include <stdiostream.h>
#include <Pix.h>
#include <String.h>

#include "debug.h"

#include "DAS.h"		// follows pragma since DAS.h is interface

void dasrestart(FILE *yyin);
int dasparse(DAS &table);	// defined in das.tab.c

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
// Pixs (and not Strings). Thus, get_table(name) needs a cast; it seems tough
// to believe folks will always remember that.

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

/*
  Read attributes from a file. Returns false if unable to open the file,
  otherwise returns the result of the mfunc parse.
*/

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

/*
  Read attributes from a file descriptor. If the file descriptor cannot be
  fdopen'd, return false, otherwise return the status of the mfunc parse.

  NB: Added call to dup() within fdopen so that once the FILE * is closed the
  decriptor fd will not also be closed (instead the duplicate descriptor will
  be closed). Thus futeher information can be read from the descriptor fd.
*/

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

    
/*
  Read attributes from in (which defaults to stdin). If dasrestart() fails,
  return false, otherwise return the status of dasparse().
*/

bool
DAS::parse(FILE *in)
{
    dasrestart(in);

    return dasparse(*this) == 0;
}

#ifdef NEVER
/*
  Write attributes from internal tables to a file. If the file cannot be
  opened for writing, return false, otherwise return the status of mfunc
  print. 
*/

bool
DAS::print(String fname)
{
    FILE *out = fopen(fname, "w");

    if (!out) {
	cerr << "Could not open: " << fname << endl;
	return false;
    }

    bool status = print(out);

    fclose(out);
    
    return status;
}

/*
  Write attributes from internal tables to a file descriptor. If the file
  descriptor cannot be fdopen'd, return false, otherwise return the status of
  the mfunc print.

  NB: See note for DAS::parse(int fd) about dup().
*/

bool
DAS::print(int fd)
{
    FILE *out = fdopen(dup(fd), "w");

    if (!out) {
	cerr << "Could not access the file descriptor for writing" << endl;
	return false;
    }

    bool status = print(out);

    fclose(out);
    
    return status;
}
#endif

/*
  Write attributes from tables to `out' (which defaults to stdout). Return
  true. 
*/

bool
DAS::print(ostream &os)
{
//    ostdiostream os(out);

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
