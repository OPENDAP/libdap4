
// -*- C++ -*-

// Interface to the TestGrid ctor class. See TestByte.h
//
// jhrg 1/13/95

/* $Log: TestGrid.h,v $
/* Revision 1.1  1995/01/19 20:20:46  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
 */

#ifndef _TestGrid_h
#define _TestGrid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Grid.h"

class TestGrid: public Grid {
public:
    TestGrid(const String &n = (char *)0);
    virtual ~TestGrid();
    
    virtual BaseType *ptr_duplicate();

    virtual bool read(String dataset, String var_name, String constraint);
};

#endif

