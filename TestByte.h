
// -*- C++ -*-

// TestByte is a demonstration of subclassing a class in the hierarchy of
// DODS data types. It does not do much of anything - a real subclass would
// add specifics for a API or format (e.g., a read mfunc for netcdf, HDF,
// ...).  The class is used by some of the test code (hence the name) as well
// as serving as a template for others who need to subclass the hierarchy. 
//
// NB: since the class Byte is an abstract class (as are all the other
// `variable type' classes), the hierarchy *must* be subclassed in order to
// be used.
//
// jhrg 1/12/95

/* $Log: TestByte.h,v $
/* Revision 1.1  1995/01/19 20:20:38  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
 */

#ifndef _TestByte_h
#define _TestByte_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Byte.h"

class TestByte: public Byte {
public:
    TestByte(const String &n = (char *)0);
    virtual ~TestByte() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(String dataset, String var_name, String constraint);
};

typedef TestByte * TestBytePtr;

#endif

