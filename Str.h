// -*- C++ -*-

// Interface for Str type.
//
// jhrg 9/7/94

/* $Log: Str.h,v $
/* Revision 1.5  1994/11/29 20:16:33  jimg
/* Added mfunc for data transmission.
/* Uses special xdr function for serialization and xdr_coder.
/* Removed `type' parameter from ctor.
/* Added FILE *in and *out to ctor parameter list.
/*
 * Revision 1.4  1994/11/22  14:06:08  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/11/03  04:43:07  reza
 * Changed the default type to match the dds parser.
 *
 * Revision 1.2  1994/09/23  14:36:14  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:09:08  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Str_h
#define _Str_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <limits.h>

#include "BaseType.h"

const unsigned int max_str_len = UINT_MAX-1;

class Str: public BaseType {
private:
    char *buf;

public:
    Str(const String &n = (char *)0, FILE *in = stdin, FILE *out = stdout);
    virtual ~Str() {}

    virtual BaseType *ptr_duplicate();
    
    virtual unsigned int size();

    //defined in <API>_read.cc
    virtual bool read(String dataset, String var_name, String constraint);

    virtual bool serialize(bool flush = false, unsigned int num = 0);
    virtual unsigned int deserialize();

    virtual void print_val(ostream &os, String space = "");
};

typedef Str * StrPtr;

#endif

