// -*- C++ -*-

// Interface for Url type.
//
// jhrg 9/7/94

/* $Log: Url.h,v $
/* Revision 1.3  1994/11/22 14:06:18  jimg
/* Added code for data transmission to parts of the type hierarchy. Not
/* complete yet.
/* Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
/*
 * Revision 1.2  1994/09/23  14:36:16  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:09:10  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
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
    virtual ~Url() {}
};

typedef Url * UrlPtr;

#endif

