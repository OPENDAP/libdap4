// -*- C++ -*-

// Interface for Str type. This class may be extended to include code that
// can transport Str's over the network (either a scalar or an array of
// Str's). 
//
// jhrg 9/7/94

/* $Log: Str.h,v $
/* Revision 1.3  1994/11/03 04:43:07  reza
/* Changed the default type to match the dds parser.
/*
 * Revision 1.2  1994/09/23  14:36:14  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:09:08  jimg
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

#ifndef _Str_h
#define _Str_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "BaseType.h"

class Str: public BaseType {
private:

public:
    Str(const String &n = (char *)0, const String &t = "String");
    virtual ~Str() {}
};

typedef Str * StrPtr;

#endif

