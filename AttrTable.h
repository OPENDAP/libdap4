// This may look like C code, but it is really -*- C++ -*-

// An AttrTable is a table of attributes (name-value pairs). The class
// AttrTable inherits from AttrVHMap. 
//
// NB: static String empty was addded to the AttrTable class so that the
// AttrVHMap constructors could be passed a reference to a string without
// having an object of type String be created every time you wanted a new
// AttrVHMap (or one of its decendents). Using "" as the initialized creates
// a temporary object according to g++'s warnings.

/* $Log: AttrTable.h,v $
/* Revision 1.4  1994/09/15 21:08:54  jimg
/* Added many classes to the BaseType hierarchy - the complete set of types
/* described in the DODS API design documet is not represented.
/* The parser can parse DDS files.
/* Fixed many small problems with BaseType.
/* Added CtorType.
/*
 * Revision 1.3  1994/09/09  15:26:41  jimg
 * Removed operator<< and added print() since I have no good way to define
 * operator>>. It seems best to define all operators from a set (like <<, >>)
 * or none at all. Since parse() is the input mfunc, it seems that output
 * should be a mfunc too.
 *
 * Revision 1.2  1994/08/02  19:17:39  jimg
 * Fixed `$Log: AttrTable.h,v $
 * Fixed `Revision 1.4  1994/09/15 21:08:54  jimg
 * Fixed `Added many classes to the BaseType hierarchy - the complete set of types
 * Fixed `described in the DODS API design documet is not represented.
 * Fixed `The parser can parse DDS files.
 * Fixed `Fixed many small problems with BaseType.
 * Fixed `Added CtorType.
 * Fixed `
 * Revision 1.3  1994/09/09  15:26:41  jimg
 * Removed operator<< and added print() since I have no good way to define
 * operator>>. It seems best to define all operators from a set (like <<, >>)
 * or none at all. Since parse() is the input mfunc, it seems that output
 * should be a mfunc too.
 *' comments and rcsid[] variables (syntax errors due to //
 * comments caused compilation failures.
 * das.tab.c and .h are commited now as well.
 *
 * Revision 1.1  1994/08/02  18:30:26  jimg
 * Class which inherits from AttrVHMap. This is the class that is contained by
 * the DAS mapping class. In addition to the methods defined by the AttrVHMap
 * class, this class defines operator<<. It also declares a static class
 * variable String empty which is defined in AttrTable.cc to be "".
 * Also in this file is a typedef for a pointer to an AttrTable (that is the
 * type actually held by the DAS container class).
 */

#ifndef _AttrTable_h
#ifdef __GNUG__
#pragma interface
#endif
#define _AttrTable_h 1

#include "AttrVHMap.h"

class AttrTable : public AttrVHMap {
private:
    static String empty;	// defined in AttrTable.cc

protected:
    
public:
    AttrTable(String& dflt=empty, unsigned int sz=DEFAULT_INITIAL_CAPACITY);
    AttrTable(AttrTable& a);
    ~AttrTable();

    void print(ostream &os);
};

typedef AttrTable * AttrTablePtr;

AttrTable::AttrTable(String& dflt, unsigned int sz) : AttrVHMap(dflt, sz)
{
}

inline AttrTable::AttrTable(AttrTable& a) : AttrVHMap(a)
{
}

inline AttrTable::~AttrTable()
{
}

#endif
