
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// An AttrTable is a table of attributes (name-value pairs). The class
// AttrTable inherits from AttrVHMap. 
//
// NB: static String empty was addded to the AttrTable class so that the
// AttrVHMap constructors could be passed a reference to a string without
// having an object of type String be created every time you wanted a new
// AttrVHMap (or one of its decendents). Using "" as the initialized creates
// a temporary object according to g++'s warnings.

/* $Log: AttrTable.h,v $
/* Revision 1.16  1997/06/06 03:11:34  jimg
/* Added mfuncs which have char * parameters for those mfuncs which accept both
/* Pix and String parameters. This avoids a conflict between promotion of char *
/* actuals to Pix instead of String (while the latter makes more sense, the
/* former actually happens).
/*
 * Revision 1.15  1997/05/13 23:32:13  jimg
 * Added changes to handle the new Alias and lexical scoping rules.
 *
 * Revision 1.14  1997/01/13 16:56:21  jimg
 * Changed the name of the private member `map' to `attr_map' to avoid a name
 * collision with the STL'd map class.
 *
 * Revision 1.13  1996/05/31 23:29:22  jimg
 * Updated copyright notice.
 *
 * Revision 1.12  1996/04/05 00:21:20  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.11  1995/05/10  13:45:05  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.10  1995/03/04  14:34:55  jimg
 * Major modifications to the transmission and representation of values:
 * Added card() virtual function which is true for classes that
 * contain cardinal types (byte, int float, string).
 * Changed the representation of Str from the C rep to a C++
 * class represenation.
 * Chnaged read_val and store_val so that they take and return
 * types that are stored by the object (e.g., inthe case of Str
 * an URL, read_val returns a C++ String object).
 * Modified Array representations so that arrays of card()
 * objects are just that - no more storing strings, ... as
 * C would store them.
 * Arrays of non cardinal types are arrays of the DODS objects (e.g.,
 * an array of a structure is represented as an array of Structure
 * objects).
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

/// Types which may appear in a Attribute object.
enum AttrType {
    Attr_unknown,
    Attr_container,
    Attr_byte,
    Attr_int32,
    Attr_uint32,
    Attr_float64,
    Attr_string,
    Attr_url
};

/** An AttrTable stores a set of names and, for each name, a group of
    attribute type-name-value tuples and/or other AttrTables. Because
    each AttrTable can contain other AttrTable objects, complex hierarchies
    of attributes maybe built. */
class AttrTable {
private:
    struct entry {
	String name;
	AttrType type;
	
	bool is_alias;

	// If type == Attr_container, use attributes to read the contained
	// table, otherwise use attr to read the vector of values.
	union {
	    AttrTable *attributes;
	    StringXPlex *attr;	// a vector of values. jhrg 12/5/94
	} value;
    };

    DLList<entry> attr_map;
    
    Pix find(const String &target, bool cont_only = false);
    Pix simple_find(const String &target);
    String AttrTypeToString(const AttrType at);
    AttrType StringToAttrType(const String &s);

public:
    AttrTable();
    ~AttrTable();

    /// Return the Pix to the first element of this attribute table.
    Pix first_attr();

    /// Advance Pix #p# to the next element of this attribute table.
    void next_attr(Pix &p);

    /// Return the name of the attribute.
    String get_name(Pix p);

    /// Is the attribute a container?
    bool is_container(Pix p);

    /** Return a pointer to the attribute table for the attribute referenced
        by Pix #p# or named #name# *if the attribute is a container
	attribute*. If there is no such attribute table, or it the table is
	not a container, return False. */  
    AttrTable *get_attr_table(Pix p);
    AttrTable *get_attr_table(const String &name);
    AttrTable *get_attr_table(const char *name);

    /// Return the type of the attribute in a String.
    String get_type(Pix p);
    String get_type(const String &name);
    String get_type(const char *name);

    /// Return the type of an attribute using AttrType.
    AttrType get_attr_type(Pix p);
    AttrType get_attr_type(const String &name);
    AttrType get_attr_type(const char *name);

    /// Returns the number of values for the attribute.
    unsigned int get_attr_num(Pix p);
    unsigned int get_attr_num(const String &name);
    unsigned int get_attr_num(const char *name);

    /** By default, return the first value in the vector of values. If the
        attribute has only one value you're done. If the value is a vector
	then use #i# to request the 0 to N-1 values of the N-element
	value-vector. It is an error to request an element beyond N-1. */
    String get_attr(Pix p, unsigned int i = 0);
    String get_attr(const String &name, unsigned int i = 0);
    String get_attr(const char *name, unsigned int i = 0);
    
    /** Return a pointer to the vector of values associated with the
        attribute referenced by Pix #p# or named #name#. */
    StringXPlex *get_attr_vector(Pix p);
    StringXPlex *get_attr_vector(const String &name);
    StringXPlex *get_attr_vector(const char *name);

    /** If #name# is not an attribute in the current attribute table, create
        a new entry in the current table for #name# and bind the type #type#
	and value #value# to that attribute. If the value of the attribute is
	a vector, call this mfunc repeatedly, once for each of the vector
	elements. They will be added in the order of calls made to this
	function. */
    unsigned int append_attr(const String &name, const String &type, 
			     const String &value);
    unsigned int append_attr(const char *name, const char *type, 
			     const char *value);

    /** Create and append the attribute container #name# to the AttrTable
        object. Return a pointer to the new AttrTable object. */
    AttrTable *append_container(const String &name);

    /** Add an alias to the set of attributes. Reading the attributes for
        #alais#  returns those stored for #name#. NB: Two forms for this call
	exist; one searches for #name# in the AttrTable referenced by #at#
	while the other uses #this#. You can use DAS::get_attr_table() to get
	the attribute table for an arbitrary name. */
    bool attr_alias(const String &alias, AttrTable *at, const String &name);
    bool attr_alias(const String &alias, const String &name);

    /** Delete the attribute named #name#. If #i# is given, delete the ith
        element of the attribute value vector. This mfunc works for container
	attributes also. */
    void del_attr(const String &name, int i = -1);

    /// Print the DAS object on stdout.
    void print(ostream &os, String pad = "    ");
};

typedef AttrTable * AttrTablePtr;

#endif


