
// Methods for the class DDS - a class used to parse the dataset descriptor
// structure.
//
// jhrg 9/7/94

// $Log: DDS.cc,v $
// Revision 1.2  1994/09/15 21:08:39  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is not represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/08  21:09:40  jimg
// First version of the Dataset descriptor class.
// 

static char rcsid[]="$Id: DDS.cc,v 1.2 1994/09/15 21:08:39 jimg Exp $";

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

DDS::DDS(const String &n) : name(n)
{
}

DDS::~DDS()
{
}

String 
DDS::get_dataset_name() 
{ 
    return name; 
}

void
DDS::set_dataset_name(const String &n) 
{ 
    name = n; 
}

void
DDS::add_var(BaseType *bt)
{ 
    cout << "Adding: " << bt->get_var_name() << " @ " << bt << endl; 
    vars.append(bt); 
}

void 
DDS::del_var(const String &n)
{ 
    Pix pp = 0;			// previous Pix

    for (Pix p = vars.first(); p; vars.next(p))
	if (vars(p)->get_var_name() == n) {
	    vars.del_after(pp);	// pp points to the pos before p
	    return;
	}
	else
	    pp = p;
}

BaseType *
DDS::var(const String &n)
{ 
    for (Pix p = vars.first(); p; vars.next(p))
	if (vars(p)->get_var_name() == n)
	    return vars(p);
}

Pix 
DDS::first_var() 
{ 
    return vars.first(); 
}

void 
DDS::next_var(Pix &p) 
{ 
    vars.next(p); 
}

BaseType *
DDS::var(Pix p) 
{ 
    return vars(p); 
}

// Read structure from IN (which defaults to stdin). If ddsrestart() fails,
// return false, otherwise return the status of ddsparse().

bool
DDS::parse(FILE *in)
{
    if (!ddsrestart(in)) {
	cerr << "Could not read from input source" << endl;
	return false;
    }

    return ddsparse(*this);
}

// Write strucutre from tables to OUT (which defaults to stdout). Return
// true. 

bool
DDS::print(FILE *out)
{
    return true;
}
