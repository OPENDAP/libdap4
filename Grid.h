
// -*- C++ -*-

// Interface to the Grid ctor class. Grids contain a single array (the `main'
// array) of dimension N and N single dimension arrays (map arrays). For any
// dimension n of the main array, the size of the nth map array must match
// the size of the main array's nth dimension. Grids are used to map
// non-integer scales multidimensional point data.
//
// jhrg 9/15/94

#ifndef _Grid_h
#define _Grid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <SLList.h>
#include "CtorType.h"

class Grid: public CtorType {
private:
    BaseType *array_var_;
    SLList<BaseTypePtr> map_vars_;

public:
    Grid(const String &n = (char *)0, const String &t = "Grid");
    virtual ~Grid() {}
    // use the default copy ctor and op=

    virtual BaseType *var(const String &name);
    virtual void add_var(BaseType *bt, Part part);

    BaseType *array_var();

    Pix first_map_var();
    void next_map_var(Pix &p);
    BaseType *map_var(Pix p);

    virtual void print_decl(bool print_semi = true);
};

#endif

