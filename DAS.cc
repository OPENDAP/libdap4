
// Methods for the class DAS - a class used to parse the dataset attribute
// structure.
//
// jhrg 7/25/94

// $Log: DAS.cc,v $
// Revision 1.1  1994/08/02 18:40:09  jimg
// Implemetation of the DAS class. This class is a container that maps
// String objects which name variables to AttrTablePtr objects.
//

static rcsid[]={"$Id: DAS.cc,v 1.1 1994/08/02 18:40:09 jimg Exp $"};

#include <iostream.h>
#include <String.h>

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>

#include "DAS.h"

int dasrestart(FILE *yyin);
int dasparse(VarVHMap &table);	// defined in das.tab.c

/*
  Read attributes from a file.
*/

DAS::parse(String fname)
{
    FILE *in = fopen(fname, "r");

    dasrestart(in);
    dasparse(*this);

    fclose(in);
}

/*
  Read attributes from stdin.
*/

DAS::parse(void)
{
    dasrestart(stdin);
    dasparse(*this);
}

ostream& operator<< (ostream &os, DAS &das)
{
    for(Pix p = das.first(); p; das.next(p))
	os << das.key(p) << endl << *(das.contents(p)) << endl;
    
    return os;
}
