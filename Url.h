// This may look like C code, but it is really -*- C++ -*-

// Interface for Url type. This class may be extended to include code that
// can transport Url's over the network (either a scalar or an array of
// Url's). 
//
// jhrg 9/7/94

/* $Log: Url.h,v $
/* Revision 1.1  1994/09/15 21:09:10  jimg
/* Added many classes to the BaseType hierarchy - the complete set of types
/* described in the DODS API design documet is not represented.
/* The parser can parse DDS files.
/* Fixed many small problems with BaseType.
/* Added CtorType.
/*
 * Revision 1.1  1994/09/09  15:38:46  jimg
 * Child class of BaseType -- used in the future to hold specific serialization
 * information for integers. Should this be a class that uses BaseType?
 */

#ifndef _Url_h
#define _Url_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "BaseType.h"

class Url: public BaseType {
private:

public:
    Url(const String &n = (char *)0, const String &t = "Url");
};

typedef Url * UrlPtr;

#endif

