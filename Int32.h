// -*- C++ -*-

// Interface for Int32 type. 
//
// jhrg 9/7/94

/* $Log: Int32.h,v $
/* Revision 1.6  1995/01/11 15:54:30  jimg
/* Added modifications necessary for BaseType's static XDR pointers. This
/* was mostly a name change from xdrin/out to _xdrin/out.
/* Removed the two FILE pointers from ctors, since those are now set with
/* functions which are friends of BaseType.
/*
 * Revision 1.5  1994/11/29  20:10:37  jimg
 * Added functions for data transmission.
 * Added boolean parameter to serialize which, when true, causes the output
 * buffer to be flushed. The default value is false.
 * Added FILE *in and *out parameters to the ctor. The default values are
 * stdin/out.
 * Removed the `type' parameter from the ctor.
 *
 * Revision 1.4  1994/11/22  14:05:59  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
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
    int32 buf;

public:
    Int32(const String &n = (char *)0);
    virtual ~Int32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual unsigned int size();

    // defined in <API>_read.cc
    virtual bool read(String dataset, String var_name, String constraint);

    virtual bool serialize(bool flush = false, unsigned int num = 0);
    virtual unsigned int deserialize();

    virtual void print_val(ostream &os, String space = "");
};

typedef Int32 * Int32Ptr;

#endif

