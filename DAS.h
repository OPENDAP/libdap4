// This may look like C code, but it is really -*- C++ -*-

// Using the VarVHMap class, build a parser for the DAS and add functions
// that provide access to the variables, their attributes and values.
//
// jhrg 7/25/94

/* $Log: DAS.h,v $
/* Revision 1.2  1994/08/02 19:17:41  jimg
/* Fixed `$Log$' comments and rcsid[] variables (syntax errors due to //
/* comments caused compilation failures.
/* das.tab.c and .h are commited now as well.
/*
 * Revision 1.1  1994/08/02  18:39:00  jimg
 * This Class is a container that maps Strings onto AttrTable pointers.
 * It inherits from VarVHMap.
 */

#ifndef _DAS_h
#define _DAS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <ostream.h>
#include <String.h>
#include <Pix.h>

#include "VarVHMap.h"

class DAS : public VarVHMap {
private:

protected:

public:
    DAS(AttrTablePtr dflt=(void *)NULL, unsigned int sz=DEFAULT_INITIAL_CAPACITY);
    DAS(DAS& a);
    ~DAS();

    parse(void);
    parse(String fname);

    friend ostream& operator<< (ostream &os, DAS &das);
};

inline DAS::DAS(AttrTablePtr dflt, unsigned int sz) : VarVHMap(dflt, sz)
{
}

inline DAS::DAS(DAS &das) : VarVHMap(das)
{
}

// This deletes the pointers to AttrTables allocated during the parse (and at 
// other times?). I could step through the protected member `cont[]' and
// delete all the non empty stuff, but I used the iterator member functions
// instead. jhrg 7/29/94

inline DAS::~DAS()
{
    for(Pix p = this->first(); p; this->next(p))
	delete this->contents(p);
}

#endif
