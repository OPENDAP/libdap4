// This may look like C code, but it is really -*- C++ -*-

// Using the DDSVHMap class, build a parser for the DDS and add functions
// that provide access to the variable's type information.
//
// jhrg 9/8/94

// $Log: DDS.h,v $
// Revision 1.1  1994/09/08 21:09:42  jimg
// First version of the Dataset descriptor class.
//

#ifndef _DDS_h
#define _DDS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

#include <String.h>
#include <Pix.h>

#include "DDSVHMap.h"

class DDS : public DDSVHMap {
private:
    String name;		// the dataset name

protected:

public:
    DDS(const BaseTypePtr dflt=(BaseType *)NULL, 
	const unsigned int sz=DEFAULT_INITIAL_CAPACITY,
	const String &n = (char *)0);
    DDS(DDS& a);
    virtual ~DDS();

    String &get_name() { return name; }

    bool parse(FILE *in=stdin);

    bool print(FILE *out=stdout);
};

DDS::DDS(const BaseTypePtr dflt, const unsigned int sz, const String &n) 
    : DDSVHMap(dflt, sz), name(n)
{
}

inline DDS::DDS(DDS &dds) : DDSVHMap(dds)
{
}

// This deletes the pointers to BaseType allocated during the parse (and at 
// other times?). I could step through the protected member `cont[]' and
// delete all the non empty stuff, but I used the iterator member functions
// instead. jhrg 7/29/94

DDS::~DDS()
{
    for(Pix p = this->first(); p; this->next(p))
	delete this->contents(p);
}

#endif
