// This may look like C code, but it is really -*- C++ -*-

// Using the DASVHMap class, build a parser for the DAS and add functions
// that provide access to the variables, their attributes and values.
//
// jhrg 7/25/94

/* $Log: DAS.h,v $
/* Revision 1.7  1994/10/05 16:44:27  jimg
/* Changed from Map to DLList for representation of the attribute table.
/* Added TYPE to the attribute table.
/*
 * Revision 1.6  1994/09/27  22:46:31  jimg
 * Changed the implementation of the class DAS from one which inherited
 * from DASVHMap to one which contains an instance of DASVHMap.
 * Added mfuncs to set/access the new instance variable.
 *
 * Revision 1.5  1994/09/23  14:38:03  jimg
 * Fixed broken header. Agian.
 *
 * Revision 1.4  1994/09/15  21:08:59  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is not represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
 * Revision 1.3  1994/09/09  15:33:40  jimg
 * Changed the base name of this class's parents from `Var' to DAS.
 * Added print() and removed operator<< (see the comments in AttrTable).
 * Added overloaded versions of print() and parse(). They can be called
 * using nothing (which defaults to std{in,out}), with a file descriptor,
 * with a FILE *, or with a String givin a file name.
 *
 * Revision 1.2  1994/08/02  19:17:41  jimg
 * Fixed log comments and rcsid[] variables (syntax errors due to //
 * comments caused compilation failures).
 * das.tab.c and .h are commited now as well.
 *
 * Revision 1.1  1994/08/02  18:39:00  jimg
 * This Class is a container that maps Strings onto AttrTable pointers.
 * It inherits from DASVHMap.
 */

#ifndef _DAS_h
#define _DAS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

#include <String.h>
#include <Pix.h>

#include "DASVHMap.h"

class DAS {
private:
    DASVHMap map;

protected:

public:
    DAS(AttrTablePtr dflt=(void *)NULL, 
	unsigned int sz=DEFAULT_INITIAL_CAPACITY);
    ~DAS();

    Pix first_var();
    void next_var(Pix &p);
    String &get_name(Pix p);
    AttrTable *get_table(Pix p);

    AttrTable *get_table(const String &name);
    AttrTable *get_table(const char *name); // avoid converting char * to Pix
    AttrTable *add_table(const String &name, AttrTable *at);
    AttrTable *add_table(const char *name, AttrTable *at);

    bool parse(String fname);
    bool parse(int fd);
    bool parse(FILE *in=stdin);

    bool print(String fname);
    bool print(int fd);
    bool print(FILE *out=stdout);
};

#endif
