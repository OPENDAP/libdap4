
// -*- C++ -*-

// Interface to the Grid ctor class. Grids contain a single array (the `main'
// array) of dimension N and N single dimension arrays (map arrays). For any
// dimension n of the main array, the size of the nth map array must match
// the size of the main array's nth dimension. Grids are used to map
// non-integer scales to multidimensional point data.
//
// jhrg 9/15/94

/* $Log: Grid.h,v $
/* Revision 1.4  1994/11/22 14:05:57  jimg
/* Added code for data transmission to parts of the type hierarchy. Not
/* complete yet.
/* Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
/*
 * Revision 1.3  1994/10/17  23:34:55  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:45:29  jimg
 * Added mfunc check_semantics().
 * Added sanity checking on the variable list (is it empty?).
 */

#ifndef _Grid_h
#define _Grid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <SLList.h>
#include "CtorType.h"

#include "config.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

class Grid: public CtorType {
private:
    BaseType *array_var_;
    SLList<BaseTypePtr> map_vars;

    void duplicate(const Grid &s);

public:
    Grid(const String &n = (char *)0, const String &t = "Grid");
    Grid(const Grid &rhs);
    virtual ~Grid();
    
    const Grid &operator=(const Grid &rhs);
    virtual BaseType *ptr_duplicate();

    virtual BaseType *var(const String &name);
    virtual void add_var(BaseType *bt, Part part);

    BaseType *array_var();

    Pix first_map_var();
    void next_map_var(Pix &p);
    BaseType *map_var(Pix p);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true);
    virtual bool check_semantics(bool all = false);
};

#endif

