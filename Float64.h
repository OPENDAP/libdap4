// -*- C++ -*-

// Interface for Float64 type.
//
// jhrg 9/7/94

/* $Log: Float64.h,v $
/* Revision 1.3  1994/11/22 14:05:50  jimg
/* Added code for data transmission to parts of the type hierarchy. Not
/* complete yet.
/* Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
/*
 * Revision 1.2  1994/09/23  14:36:11  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:09:01  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Float64_h
#define _Float64_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "BaseType.h"

class Float64: public BaseType {
private:
    double buf;

public:
    Float64(const String &n = (char *)0, const String &t = "Float64");
    virtual ~Float64() {}

    virtual BaseType *ptr_duplicate();
    
    virtual unsigned int size();

    // defined in <API>_read.c (e.g., nc_read.cc, ...)
    virtual bool read(String dataset, String var_name, String constraint);

    virtual bool serialize(unsigned int num = 0);
    virtual unsigned int deserialize();

    virtual void print_val(ostream &os, String space = "");
};

typedef Float64 * Float64Ptr;

#endif

