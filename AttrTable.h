
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
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

/* 
 * $Log: AttrTable.h,v $
 * Revision 1.26  1999/04/29 02:29:26  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.25  1999/03/24 23:37:13  jimg
 * Added support for the Int16, UInt16 and Float32 types
 *
 * Revision 1.24  1999/01/21 20:42:00  tom
 * Fixed comment formatting problems for doc++
 *
 * Revision 1.23  1998/12/28 21:39:39  jimg
 * Fixed comments for get_attr().
 *
 * Revision 1.22.2.1  1999/02/02 19:43:39  jimg
 * StringSPlex Removed
 *
 * Revision 1.22  1998/08/06 16:07:39  jimg
 * Field added so that aliases may be displayed as such and not by displaying
 * the aliased information.
 *
 * Revision 1.21  1998/07/13 20:20:41  jimg
 * Fixes from the final test of the new build process
 *
 * Revision 1.20  1998/01/12 14:27:55  tom
 * Second pass at class documentation.
 *
 * Revision 1.19  1997/10/09 22:19:09  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.18  1997/08/11 18:19:11  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.17  1997/08/09 21:20:06  jimg
 * Changed the names of AttrTable_to_String and String_to_AttrTable.
 *
 * Revision 1.16  1997/06/06 03:11:34  jimg
 * Added mfuncs which have char * parameters for those mfuncs which accept both
 * Pix and String parameters. This avoids a conflict between promotion of
 * char actuals to Pix instead of String (while the latter makes more sense,
 * the former actually happens).
 *
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
#include <string>
#include <vector>
#include <DLList.h>

#include "config_dap.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

/** {\bf AttrType} identifies the data types which may appear in an
    attribute table object. 

    \begin{verbatim}
    enum AttrType {
	Attr_unknown,
	Attr_container,
	Attr_byte,
	Attr_int16,
	Attr_uint16,
	Attr_int32,
	Attr_uint32,
	Attr_float32,
	Attr_float64,
	Attr_string,
	Attr_url
    };
    \end{verbatim}

    @memo Identifies attribute table data types.
    @see AttrTable
    */
enum AttrType {
    Attr_unknown,
    Attr_container,
    Attr_byte,
    Attr_int16,
    Attr_uint16,
    Attr_int32,
    Attr_uint32,
    Attr_float32,
    Attr_float64,
    Attr_string,
    Attr_url
};

/** An AttrTable (``Attribute Table'') stores a set of names and, for
    each name, either a type and a value, or another attribute table.
    The attribute value can be a vector containing many values of the
    same type.  The attributes can have any of the types listed in the
    #AttrType# list.  However, all attribute types are stored as
    string data, except for the container type, which is stored as a
    pointer to another attribute table.

    Each element in the attribute table can itself be an attribute
    table.  The table can also contain ``alias'' attributes whose
    value is given by the value of another attribute to which it is
    linked.

    The attribute tables have a standard printed representation.
    There is a member function #print()# for writing this form.  Use
    the #DAS::parse()# function to read the printed form.

    An attribute table might look something like this:

    \begin{verbatim}
    string long_name "Weekly Means of Sea Surface Temperature";
    actual_range {
        Float64 min -1.8;
        Float64 max 35.09;
    }
    string units "degC";
    conversion_data {
        Float64 add_offset 0.;
        Float64 scale_factor 0.0099999998;
    }
    Int32 missing_value 32767;
    \end{verbatim}

    Here, #long_name#, #units#, and #missing_value# are simple
    attributes, and #actual_range# and #conversion_data# are container
    attributes containing other attribute tables.

    @memo Holds a table of Attributes.  
    @see DAS
    @see AttrType */
class AttrTable {
private:
    struct entry {
	string name;
	AttrType type;
	
	bool is_alias;
        string aliased_to;

	// If type == Attr_container, use attributes to read the contained
	// table, otherwise use attr to read the vector of values.
	union {
	    AttrTable *attributes;
	    vector<string> *attr;	// a vector of values. jhrg 12/5/94
	} value;
    };

    DLList<entry> attr_map;
    
    Pix find(const string &target, bool cont_only = false);
    Pix simple_find(const string &target);
    string AttrType_to_String(const AttrType at);
    AttrType String_to_AttrType(const string &s);

public:
    AttrTable();
    ~AttrTable();

  /** Returns a pointer to the first element of this attribute table. */
    Pix first_attr();

  /** Advance the pointer to the next element of this attribute
    table. */
    void next_attr(Pix &p);

  /** Returns the name of the attribute. */
    string get_name(Pix p);

  /** Returns true if the attribute is a container. */
    bool is_container(Pix p);

  /** Returns a pointer to the attribute table for the attribute referenced
      by {\it p} or named {\it name} only if the attribute is a container
      attribute.  If there is no such attribute table, or if the table is
      not a container, the function returns null. 

      @memo Returns an attribute sub-table. 
      @name get\_attr\_table()
      */  
  //@{
  ///
    AttrTable *get_attr_table(Pix p);
  ///
    AttrTable *get_attr_table(const string &name);
  ///
    AttrTable *get_attr_table(const char *name);
  //@}

  /** Returns the type of an attribute in a string.

      @name get\_type()
      */
  //@{
  ///
    string get_type(Pix p);
  ///
    string get_type(const string &name);
  ///
    string get_type(const char *name);
  //@}

  /** Returns the type of an attribute using AttrType.

      @name get\_attr\_type()
      @see AttrType
      */
  //@{
  ///
    AttrType get_attr_type(Pix p);
  ///
    AttrType get_attr_type(const string &name);
  ///
    AttrType get_attr_type(const char *name);
  //@}

  /** If the indicated attribute is a container attribute, this
      function returns the number of attributes in {\it its} attribute
      table.  If the indicated attribute is not a container, the
      function returns zero.

      @name get\_attr\_num()
      @memo Returns the number of entries in a container attribute.
      */
  //@{
  ///
    unsigned int get_attr_num(Pix p);
  ///
    unsigned int get_attr_num(const string &name);
  ///
    unsigned int get_attr_num(const char *name);
  //@}

  /** Returns the value of an attribute.  If the attribute has a vector
      value, you can indicate which is the desired value with the
      index argument, {\it i}.  If the argument is omitted, the first
      value is returned.  If the attribute has only a single value,
      the index argument is ignored.  If {\it i} is greater than the
      number of elements in the attribute, an error is produced.

      Note that all values in an attribute table are stored as string
      data.  They may be converted to a more appropriate internal
      format by the calling program.

      @name get\_attr()
      @return If the indicated attribute is a container, this function
      returns the string ``None''. If using a name to refer to the attribute
      and the named attribute does not exist, return the empty string. */
  //@{
  ///
    string get_attr(Pix p, unsigned int i = 0);
  ///
    string get_attr(const string &name, unsigned int i = 0);
  ///
    string get_attr(const char *name, unsigned int i = 0);
  //@}
    
  /** Returns a pointer to the vector of values associated with the
      attribute referenced by Pix {\it p} or named {\it name}. 

      Note that all values in an attribute table are stored as string
      data.  They may be converted to a more appropriate internal
      format by the calling program.

      @memo Returns a vector attribute.
      @name get\_attr\_vector()
      @return If the indicated attribute is a container, this function
      returns the null pointer.  Otherwise returns a pointer to the
      the attribute vector value.
      */
  //@{
  ///
    vector<string> *get_attr_vector(Pix p);
  ///
    vector<string> *get_attr_vector(const string &name);
  ///
    vector<string> *get_attr_vector(const char *name);
  //@}

  /** Adds an attribute to the table.  If the given name already
      refers to an attribute, and the attribute has a vector value,
      the given value is appended to the attribute vector.  Calling
      this function repeatedly is the way to create an attribute
      vector.

      The function returns an error condition if the attribute is a
      container, or if the type of the input value does not match the
      existing attribute's type.  Use #append_container()# to add
      container attributes.

      @name append\_attr() 
      @return Returns the length of the added attribute value, or zero
      for failure.
      @param name The name of the attribute to add or modify.
      @param type The type of the attribute to add or modify.
      @param value The value to add to the attribute table.
  */
  //@{
  ///
    unsigned int append_attr(const string &name, const string &type, 
			     const string &value);
  ///
    unsigned int append_attr(const char *name, const char *type, 
			     const char *value);
  //@}

  /** Create and append an attribute container to the AttrTable
      object.  A container is an attribute that contains other
      attributes.  That is, it is another attribute table.

      @return A pointer to the new AttrTable object. 
      @memo Create a new container attribute.
      */
    AttrTable *append_container(const string &name);

    /** Adds an alias to the set of attributes.  Once an alias is
	inserted into an attribute table, reading the attributes for
	{\it alias} will return those stored for {\it name}. 

	Two forms for this function exist: one searches for {\it name}
	in the AttrTable referenced by {\it at} while the other uses
	#this#. You can use #DAS::get_attr_table()# to get the attribute
	table for an arbitrary name.

	@name attr\_alias()
	@see get_attr_table */
    //@{

    /** @param alias The alias to insert into the attribute table.
	@param name The name of the already-existing attribute to which
	the alias will refer.
	@param at An attribute table in which to insert the alias. */
    bool attr_alias(const string &alias, AttrTable *at, const string &name);

    /** @param alias The alias to insert into the attribute table.
	@param name The name of the already-existing attribute to which
	the alias will refer. */
    bool attr_alias(const string &alias, const string &name);
    //@}

  /** Delete the attribute named {\it name}. If {\it i} is given, and
      the attribute has a vector value, delete the i-th element of the
      vector.

      You can use this function to delete container attributes,
      although the {\it i} parameter has no meaning for that
      operation.

      @memo Deletes an attribute. 
      @param name The name of the attribute to delete.  This can be an
      attribute of any type, including containers.
      @param i If the named attribute is a vector, and {\it i} is
      non-negative, the i-th entry in the vector is deleted, and the
      array is repacked.  If {\it i} equals -1 (the default), the
      entire attribute is deleted.
      */
    void del_attr(const string &name, int i = -1);

  /** Prints an ASCII representation of the attribute table to the
      indicated output stream.  The {\it pad} argument is prefixed to
      each line of the output to provide control of indentation.

      @memo Prints the attribute table.  */
    void print(ostream &os, string pad = "    ");
};

typedef AttrTable * AttrTablePtr;

#endif


