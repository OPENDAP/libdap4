// -*- C++ -*-

// An AttrTable is a table of attributes (name-value pairs). The class
// AttrTable inherits from AttrVHMap. 
//
// NB: static String empty was addded to the AttrTable class so that the
// AttrVHMap constructors could be passed a reference to a string without
// having an object of type String be created every time you wanted a new
// AttrVHMap (or one of its decendents). Using "" as the initialized creates
// a temporary object according to g++'s warnings.

/* $Log: AttrTable.h,v $
/* Revision 1.11  1995/05/10 13:45:05  jimg
/* Changed the name of the configuration header file from `config.h' to
/* `config_dap.h' so that other libraries could have header files which were
/* installed in the DODS include directory without overwriting this one. Each
/* config header should follow the convention config_<name>.h.
/*
 * Revision 1.10  1995/03/04  14:34:55  jimg
 * Major modifications to the transmission and representation of values:
 * 	Added card() virtual function which is true for classes that
 * 	contain cardinal types (byte, int float, string).
 * 	Changed the representation of Str from the C rep to a C++
 * 	class represenation.
 * 	Chnaged read_val and store_val so that they take and return
 * 	types that are stored by the object (e.g., inthe case of Str
 * 	an URL, read_val returns a C++ String object).
 * 	Modified Array representations so that arrays of card()
 * 	objects are just that - no more storing strings, ... as
 * 	C would store them.
 * 	Arrays of non cardinal types are arrays of the DODS objects (e.g.,
 * 	an array of a structure is represented as an array of Structure
 * 	objects).
 *
 * Revision 1.9  1994/12/07  21:09:25  jimg
 * Added support for vectors of attributes (using XPlex from libg++).
 *
 * Revision 1.8  1994/11/22  14:05:24  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.7  1994/10/13  15:44:36  jimg
 * Added a new version of append_attr (it takes (const char *)s) and
 * changed the types of the old version to (const String &).
 *
 * Revision 1.6  1994/10/05  16:38:15  jimg
 * Changed internal representation of the attribute table from a Map
 * to a DLList<>.
 *
 * Revision 1.5  1994/09/27  22:42:45  jimg
 * Changed definition of the class AttrTable; it no longer inherits from
 * AttrVHMap, instead it uses that class (contains a member that is an instance
 * of AttrVHMap).
 * Added mfuncs to AttrTable so that the new member could be set/accessed.
 *
 * Revision 1.4  1994/09/15  21:08:54  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is not represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
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
#define _AttrTable_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <Pix.h>
#include <String.h>
#include <DLList.h>

#include "config_dap.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#include "String.XPlex.h"

class AttrTable {
private:
    struct entry {
	String name;
	String type;
	StringXPlex attr;	// a vector of values. jhrg 12/5/94
    };

    DLList<entry> map;
    
    Pix find(const String &target);

public:
    AttrTable();

    Pix first_attr();
    void next_attr(Pix &p);

    String get_name(Pix p);
    String get_type(Pix p);
    unsigned int get_attr_num(Pix p); // returns the length of the attr vec
    String get_attr(Pix p, unsigned int i = 0);

    unsigned int get_attr_num(const String &name); // ret attr vec len
    unsigned int get_attr_num(const char *name);
    String get_attr(const String &name, unsigned int i = 0);
    String get_attr(const char *name, unsigned int i = 0);
    String get_type(const String &name);
    String get_type(const char *name);

    unsigned int append_attr(const String &name, const String &type, 
		     const String &value);
    unsigned int append_attr(const char *name, const char *type, 
			      const char *value);
    // by default delete the last element of the attribute vector
    void del_attr(const String &name, int i = -1);

    void print(ostream &os, String pad = "    ");
};

typedef AttrTable * AttrTablePtr;

#endif


