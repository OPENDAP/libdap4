// -*- C++ -*-

// Interface for Int32 type. 
//
// jhrg 9/7/94

/* $Log: Int32.h,v $
/* Revision 1.4  1994/11/22 14:05:59  jimg
/* Added code for data transmission to parts of the type hierarchy. Not
/* complete yet.
/* Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
/*
 * Revision 1.3  1994/09/23  14:36:09  jimg
 * Fixed errors in comments.
 *
 * Revision 1.2  1994/09/15  21:09:05  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Int32_h
#define _Int32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "BaseType.h"

class Int32: public BaseType {
private:

public:
    Int32(const String &n = (char *)0, const String &t = "Int32");
    virtual ~Int32() {}
};

typedef Int32 * Int32Ptr;

#endif

