// This may look like C code, but it is really -*- C++ -*-

// Using the DDSVHMap class, build a parser for the DDS.
//
// jhrg 9/8/94

/* $Log: DDS.h,v $
/* Revision 1.2  1994/09/15 21:09:00  jimg
/* Added many classes to the BaseType hierarchy - the complete set of types
/* described in the DODS API design documet is not represented.
/* The parser can parse DDS files.
/* Fixed many small problems with BaseType.
/* Added CtorType.
/*
 * Revision 1.1  1994/09/08  21:09:42  jimg
 * First version of the Dataset descriptor class.
 */

#ifndef _DDS_h
#define _DDS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

#include <String.h>
#include <Pix.h>
#include <SLList.h>

#include "BaseType.h"

class DDS {
private:
    String name;		// the dataset name
    SLList<BaseTypePtr> vars;	// variables at the top level 

public:
    DDS(const String &n = (char *)0);
    // Use the default copy ctor and op=
    virtual ~DDS();

    String get_dataset_name();
    void set_dataset_name(const String &n);

    void add_var(BaseType *bt);
    void del_var(const String &n);
    BaseType *var(const String &n);

    Pix first_var();
    void next_var(Pix &p);
    BaseType *var(Pix p);

    // Interface to the parser
    bool parse(FILE *in=stdin);

    bool print(FILE *out=stdout);
};

#endif
