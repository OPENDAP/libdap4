
// -*- C++ -*-

// Interface for the class Structure. A structure contains a single set of
// variables, all at the same lexical level. Of course, a structure may
// contain other structures... The variables contained in a structure are
// stored by instances of this class in a SLList of BaseType pointers.
//
// jhrg 9/14/94

/* $Log: Structure.h,v $
/* Revision 1.2  1994/09/23 14:45:27  jimg
/* Added mfunc check_semantics().
/* Added sanity checking on the variable list (is it empty?).
/*
 */

#ifndef _Structure_h
#define _Structure_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <SLList.h>
#include "CtorType.h"

class Structure: public CtorType {
private:
    SLList<BaseTypePtr> vars;

public:
    Structure(const String &n = (char *)0, const String &t = "Structure");
    // use the default copy ctor, and op=
    virtual ~Structure() {}

    virtual BaseType *var(const String &name);
    virtual void add_var(BaseType *, Part p = nil);

    Pix first_var();
    void next_var(Pix &p);
    BaseType *var(Pix p);

    virtual void print_decl(bool print_semi = true);
    virtual bool check_semantics(bool all = false);
};

#endif
