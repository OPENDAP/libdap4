
// -*- C++ -*-

// Interface for the class Structure. A structure contains a single set of
// variables, all at the same lexical level. Of course, a structure may
// contain other structures... The variables contained in a structure are
// stored by instances of this class in a SLList of BaseType pointers.
//
// jhrg 9/14/94

/* $Log: Structure.h,v $
/* Revision 1.5  1994/12/16 15:17:15  dan
/* Removed inheritance from class CtorType, now directly inherits
/* from class BaseType.
/*
 * Revision 1.4  1994/11/22  14:06:13  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/10/17  23:34:48  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:45:27  jimg
 * Added mfunc check_semantics().
 * Added sanity checking on the variable list (is it empty?).
 */

#ifndef _Structure_h
#define _Structure_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <SLList.h>
#include "BaseType.h"

#include "config.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

class Structure: public BaseType {
private:
    SLList<BaseTypePtr> vars;
    
    void duplicate(const Structure &s);

public:
    Structure(const String &n = (char *)0, FILE *in = stdin, FILE *out = stdout);
    Structure(const Structure &rhs);
    virtual ~Structure();

    const Structure &operator=(const Structure &rhs);
    virtual BaseType *ptr_duplicate();

    virtual unsigned int size();
    virtual bool read(String dataset, String var_name, String constraint);

    virtual bool serialize(bool flush, unsigned int num = 0);
    virtual unsigned int deserialize();

    virtual BaseType *var(const String &name);
    virtual void add_var(BaseType *, Part p = nil);

    Pix first_var();
    void next_var(Pix &p);
    BaseType *var(Pix p);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true);
    virtual void print_val(ostream &os, String space = "");
    virtual bool check_semantics(bool all = false);
};

#endif
