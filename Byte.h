// -*- C++ -*-

// Interface for Byte type. 
//
// jhrg 9/7/94

/* $Log: Byte.h,v $
/* Revision 1.3  1994/11/22 14:05:32  jimg
/* Added code for data transmission to parts of the type hierarchy. Not
/* complete yet.
/* Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
/*
 * Revision 1.2  1994/09/23  14:36:07  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:08:57  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Byte_h
#define _Byte_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "BaseType.h"

class Byte: public BaseType {
private:

public:
    Byte(const String &n = (char *)0, const String &t = "Byte");
    virtual ~Byte() {}
};

typedef Byte * BytePtr;

#endif

