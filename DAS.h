// This may look like C code, but it is really -*- C++ -*-

// Using the DASVHMap class, build a parser for the DAS and add functions
// that provide access to the variables, their attributes and values.
//
// jhrg 7/25/94

// $Log: DAS.h,v $
// Revision 1.4  1994/09/15 21:08:59  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is not represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.3  1994/09/09  15:33:40  jimg
// Changed the base name of this class's parents from `Var' to DAS.
// Added print() and removed operator<< (see the comments in AttrTable).
// Added overloaded versions of print() and parse(). They can be called
// using nothing (which defaults to std{in,out}), with a file descriptor,
// with a FILE *, or with a String givin a file name.
//
// Revision 1.2  1994/08/02  19:17:41  jimg
// Fixed log comments and rcsid[] variables (syntax errors due to //
// comments caused compilation failures).
// das.tab.c and .h are commited now as well.
//
// Revision 1.1  1994/08/02  18:39:00  jimg
// This Class is a container that maps Strings onto AttrTable pointers.
// It inherits from DASVHMap.


#ifndef _DAS_h
#define _DAS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

#include <String.h>
#include <Pix.h>

#include "DASVHMap.h"

class DAS : public DASVHMap {
private:

protected:

public:
    DAS(AttrTablePtr dflt=(void *)NULL, 
	unsigned int sz=DEFAULT_INITIAL_CAPACITY);
    DAS(DAS& a);
    ~DAS();

    bool parse(String fname);
    bool parse(int fd);
    bool parse(FILE *in=stdin);

    bool print(String fname);
    bool print(int fd);
    bool print(FILE *out=stdout);
};

DAS::DAS(AttrTablePtr dflt, unsigned int sz) : DASVHMap(dflt, sz)
{
}

inline DAS::DAS(DAS &das) : DASVHMap(das)
{
}

// This deletes the pointers to AttrTables allocated during the parse (and at 
// other times?). I could step through the protected member `cont[]' and
// delete all the non empty stuff, but I used the iterator member functions
// instead. jhrg 7/29/94

DAS::~DAS()
{
    for(Pix p = this->first(); p; this->next(p))
	delete this->contents(p);
}

#endif
