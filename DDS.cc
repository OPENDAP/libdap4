
// Methods for the class DDS - a class used to parse the dataset descriptor
// structure.
//
// jhrg 7/25/94

// $Log: DDS.cc,v $
// Revision 1.1  1994/09/08 21:09:40  jimg
// First version of the Dataset descriptor class.
// 

static char rcsid[]="$Id: DDS.cc,v 1.1 1994/09/08 21:09:40 jimg Exp $";

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

#include "DDS.h"

int ddsrestart(FILE *yyin);
int ddsparse(DDS &table);	// defined in dds.tab.c

/*
  Read structure from in (which defaults to stdin). If ddsrestart() fails,
  return false, otherwise return the status of ddsparse().
*/

bool
DDS::parse(FILE *in)
{
    if (!ddsrestart(in)) {
	cerr << "Could not read from input source" << endl;
	return false;
    }

    return ddsparse(*this);
}

/*
  Write strucutre from tables to `out' (which defaults to stdout). Return
  true. 
*/

bool
DDS::print(FILE *out)
{
#ifdef NEVER
    ostdiostream os(out);

    os << "Attributes {" << endl;

    for(Pix p = this->first(); p; this->next(p)) {
	os << this->key(p) << "{" << endl;
	this->contents(p)->print(os); // this->contents(p) is an (AttrTable *)
	os << "}" << endl;
    }

#endif
    return true;
}
