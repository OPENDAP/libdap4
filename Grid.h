
// -*- C++ -*-

// Interface to the Grid ctor class. Grids contain a single array (the `main'
// array) of dimension N and N single dimension arrays (map arrays). For any
// dimension n of the main array, the size of the nth map array must match
// the size of the main array's nth dimension. Grids are used to map
// non-integer scales to multidimensional point data.
//
// jhrg 9/15/94

/* $Log: Grid.h,v $
/* Revision 1.8  1995/01/19 21:59:17  jimg
/* Added read_val from dummy_read.cc to the sample set of sub-class
/* implementations.
/* Changed the declaration of readVal in BaseType so that it names the
/* mfunc read_val (to be consistant with the other mfunc names).
/* Removed the unnecessary duplicate declaration of the abstract virtual
/* mfuncs read and (now) read_val from the classes Byte, ... Grid. The
/* declaration in BaseType is sufficient along with the decl and definition
/* in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
/*
 * Revision 1.7  1995/01/18  18:40:08  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.6  1995/01/11  15:54:48  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.5  1994/12/15  21:25:45  dan
 * Added print_val() member function.
 *
 * Revision 1.4  1994/11/22  14:05:57  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
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
#include "BaseType.h"

#include "config.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

class Grid: public BaseType {
private:
    BaseType *array_var_;
    SLList<BaseTypePtr> map_vars;

    void duplicate(const Grid &s);

public:
    Grid(const String &n = (char *)0);
    Grid(const Grid &rhs);
    virtual ~Grid();
    
    const Grid &operator=(const Grid &rhs);
    virtual BaseType *ptr_duplicate() = 0;

    virtual BaseType *var(const String &name);
    virtual void add_var(BaseType *bt, Part part);

    BaseType *array_var();

    Pix first_map_var();
    void next_map_var(Pix &p);
    BaseType *map_var(Pix p);

    virtual unsigned int size();

#ifdef NEVER
    virtual bool read(String dataset, String var_name, String constraint) = 0;
    virtual bool readVal(void *stuff);
#endif

    virtual bool serialize(bool flush, unsigned int num = 0);
    virtual unsigned int deserialize();

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true);
    virtual void print_val(ostream &os, String space = "");
    virtual bool check_semantics(bool all = false);
};

#endif

