// -*- C++ -*-

// Provide access to the DDS. This class is used to parse DDS text files, to
// produce a printed representation of the in-memory variable table, and to
// update the table on a per-variable basis.
//
// jhrg 9/8/94

/* $Log: DDS.h,v $
/* Revision 1.4  1994/10/18 00:20:47  jimg
/* Added copy ctor, dtor, duplicate, operator=.
/* Added var() for const cahr * (to avoid confusion between char * and
/* Pix (which is void *)).
/* Switched to errmsg library.
/* Added formatting to print().
/*
 * Revision 1.3  1994/09/23  14:42:23  jimg
 * Added mfunc check_semantics().
 * Replaced print mfunc stub with real code.
 * Fixed some errors in comments.
 *
 * Revision 1.2  1994/09/15  21:09:00  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
 * Revision 1.1  1994/09/08  21:09:42  jimg
 * First version of the Dataset descriptor class.
 */

#ifndef _DDS_h
#define _DDS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

#include <iostream.h>
#include <String.h>
#include <Pix.h>
#include <SLList.h>

#include "BaseType.h"

class DDS {
private:
    String name;		// the dataset name
    SLList<BaseTypePtr> vars;	// variables at the top level 
    
    void duplicate(DDS &dds);

public:
    DDS(const String &n = (char *)0);
    DDS(DDS &dds);
    ~DDS();

    DDS & operator=(DDS &rhs);

    String get_dataset_name();
    void set_dataset_name(const String &n);

    void add_var(BaseType *bt);
    void del_var(const String &n);
    BaseType *var(const String &n);
    BaseType *var(const char *n); // to avoid cast of char * to Pix.

    Pix first_var();
    void next_var(Pix &p);
    BaseType *var(Pix p);

    // Interface to the parser
    bool parse(FILE *in=stdin);

    bool print(ostream &os = cout);
    bool check_semantics(bool all = false);
};

#endif
