// -*- C++ -*-

// Interface for Int32 type. This class may be extended to include code that
// can transport Int32's over the network (either a scalar or an array of
// Int32's). 
//
// jhrg 9/7/94

/* $Log: Int32.h,v $
/* Revision 1.3  1994/09/23 14:36:09  jimg
/* Fixed errors in comments.
/*
 * Revision 1.2  1994/09/15  21:09:05  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
 * Revision 1.1  1994/09/09  15:38:46  jimg
 * Child class of BaseType -- used in the future to hold specific serialization
 * information for integers. Should this be a class that uses BaseType?
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

