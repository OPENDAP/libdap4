
// Methods for the class DAS - a class used to parse the dataset attribute
// structure.
//
// jhrg 7/25/94

// $Log: DAS.cc,v $
// Revision 1.4  1994/09/09 15:33:38  jimg
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

static char rcsid[]="$Id: DAS.cc,v 1.4 1994/09/09 15:33:38 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>

#include <iostream.h>
#include <stdiostream.h>
#include <String.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include "DAS.h"		// follows pragma since DAS.h is interface

int dasrestart(FILE *yyin);
int dasparse(DASVHMap &table);	// defined in das.tab.c

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

    int status = parse(in);

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

    int status = parse(in);

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
    if (!dasrestart(in)) {
	cerr << "Could not read from input source" << endl;
	return false;
    }

    return dasparse(*this);
}

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

    int status = print(out);

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

    int status = print(out);

    fclose(out);
    
    return status;
}


/*
  Write attributes from tables to `out' (which defaults to stdout). Return
  true. 
*/

bool
DAS::print(FILE *out)
{
    ostdiostream os(out);

    os << "Attributes {" << endl;

    for(Pix p = this->first(); p; this->next(p)) {
	os << this->key(p) << "{" << endl;
	this->contents(p)->print(os); // this->contents(p) is an (AttrTable *)
	os << "}" << endl;
    }

    os << "}" << endl;

    return true;
}
