
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// An AttrTable is a table of attributes (type-name-value tuples).

#ifndef _attrtable_h
#define _attrtable_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <string>
#include <vector>

#include <Pix.h>
#include <DLList.h>

#ifndef _error_h
#include "Error.h"
#endif

using std::string;
using std::vector;
using std::ostream;

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
    @see AttrTable */
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
	AttrTable *attributes;
	vector<string> *attr;	// a vector of values. jhrg 12/5/94

	entry(): name(""), type(Attr_unknown), is_alias(false),
		 aliased_to("") {
	    attributes = 0;
	    attr = 0;
	}

	entry(const entry &rhs) {
	    clone(rhs);
	}

	void delete_entry() {
	    if (is_alias)	// alias copies the pointers.
		return;
	    if (type == Attr_container) {
		delete attributes; attributes = 0;
	    }
	    else {
		delete attr; attr = 0;
	    }
	}

	virtual ~entry() {
	    delete_entry();
	}
	
	void clone(const entry &rhs) {
	    name = rhs.name;
	    type = rhs.type;
	    is_alias = rhs.is_alias;
	    aliased_to = rhs.aliased_to;
	    switch (rhs.type) {
	      case Attr_unknown:
		break;
	      case Attr_container: {
		  AttrTable *src_atp = rhs.attributes;
		  AttrTable *dest_atp = new AttrTable(*src_atp);
		  attributes = dest_atp;
		  break;
	      }
	      default: {
		  vector<string> *src_attr = rhs.attr;
		  vector<string> *dest_attr = new vector<string>(*src_attr);
		  attr = dest_attr;
		  break;
	      }
	    }
	}

	entry &operator=(const entry &rhs) {
	    if (this != &rhs) {
		delete_entry();
		clone(rhs);
	    }
	    return *this;
	}
    };

    string d_name;
    DLList<entry *> attr_map;
    
    Pix simple_find(const string &target);
    AttrTable *simple_find_container(const string &target);

    string AttrType_to_String(const AttrType at);
    AttrType String_to_AttrType(const string &s);

    void delete_attr_table();

protected:
    /** Clone the given attribute table in #this#. */
    void clone(const AttrTable &at);
    /** A simple printer that does nothing fancy with aliases. */
    void simple_print(ostream &os, string pad, Pix p, bool dereference);

public:
    /** @name Instance management */
    //@{
    ///
    AttrTable();
    ///
    AttrTable(const AttrTable &rhs);
    ///
    virtual ~AttrTable();
    ///
    AttrTable & operator=(const AttrTable &rhs);
    //@}

    /** @name Accessors */
    //@{
    /** Get the number of entries in this attribute table. Attributes
	that are containers count one attribute, as do attributes with both
	scalar and vector values. 
	@return The number of entries. */
    unsigned int get_size() const;

    /** Get the name of this attribute table. 
	@return A string containing the name. */
    string get_name();

    /** Look in this attribute table for an attribute container named
	#target#. The search starts at this attribute table; #target# should
	use the dot notation to name containers held within children of this
	attribute table. 

	To search the entire DAS object, make sure to invoke this method from
	that object.

	@param target The attribute container to find.
	@return A pointer to the attribute table or null if the container
	cannot be found. */
    AttrTable *find_container(const string &target);

    /** Look for an attribute or an attribute container. If used to search
	for an attribute container, this method returns the container's {\it
	parent} using the value-result parameter #at# and a reference to the
	container using the Pix return value. If used to search for an
	attribute, the attribute's container is returned using #at#; the
	attribute itself can be accessed using the Pix return value.

	@param target The name (using dot notation) of the attribute or
	container to find.
	@param at A value-result used to return the attribute container in
	which #target# was found. Null if #target# was not found.
	@return A Pix which can be used to access #target# from within #at#.
	Null if the attribute or container does not exist. */
    Pix find(const string &target, AttrTable **at);

    /** Each of the following accessors get information using the name of an
	attribute. They perform a simple search for the name in this
	attribute table only; sub-tables are not searched and the dot
	notation is not recognized.

	@name get information using attribute names */
    //@{

    /** @name get\_attr\_table */
    //@{
    /// Get an attribute container. 
    AttrTable *get_attr_table(const string &name);
    /// 
    AttrTable *get_attr_table(const char *name);
    //@}

    /** @name get\_type */
    //@{
    /** Get the type name of an attribute within this attribute table. */
    string get_type(const string &name);
    ///
    string get_type(const char *name);
    //@}

    /** @name get\_attr\_type */
    //@{
    /** Get the type of an attribute using AttrType. */
    AttrType get_attr_type(const string &name);
    /// 
    AttrType get_attr_type(const char *name);
    //@}

    /** @name get\_attr\_num */
    //@{
    /** If the indicated attribute is a container attribute, this function
	returns the number of attributes in {\it its} attribute table. If the
	indicated attribute is not a container, the method returns the number
	of values for the attribute (1 for a scalar attribute, N for a vector
	attribute value). */
    unsigned int get_attr_num(const string &name);
    ///
    unsigned int get_attr_num(const char *name);
    //@} 

    /** @name get\_attr */
    //@{
    /** Get the value of an attribute. If the attribute has a vector value,
	you can indicate which is the desired value with the index argument,
	#i#. If the argument is omitted, the first value is returned. If the
	attribute has only a single value, the index argument is ignored. If
	#i# is greater than the number of elements in the attribute, an error
	is produced.

	All values in an attribute table are stored as string data. They may
	be converted to a more appropriate internal format by the calling
	program. */
    string get_attr(const string &name, unsigned int i = 0);
    ///
    string get_attr(const char *name, unsigned int i = 0);
    //@}

    /** @name get\_attr\_vector */
    //@{
    /** Get a pointer to the vector of values associated with the attribute
	referenced by Pix #p# or named #name#.

	Note that all values in an attribute table are stored as string data.
	They may be converted to a more appropriate internal format by the
	calling program.

	@return If the indicated attribute is a container, this function
	returns the null pointer.  Otherwise returns a pointer to the
	the attribute vector value. */
    vector<string> *get_attr_vector(const string &name);
    ///
    vector<string> *get_attr_vector(const char *name);
    //@}
    //@} Accessors that use names

    /** @name get information using a Pix */
    //@{
    /** Get a reference to the first entry in this attribute table. 
	@return Pix; returns null if there's nothing in the list. */
    Pix first_attr();

    /** Advance to the next element of this attribute table. Set to null when
	there are no more elements in the list.
	@param p Pix to advance. */
    void next_attr(Pix &p);

    /** Returns the name of the attribute table. */
    string get_name(Pix p);

    /** Returns true if the attribute is a container. */
    bool is_container(Pix p);

    /** Get the attribute container referenced by #p#. If no
	such container exists, then return null. 
	@param p Reference to a table contained by this object.
	@return The child attribute table. */
    AttrTable *get_attr_table(Pix p);

    /** Get the type name of an attribute.
	@param p
	@return A string with the name of this attribute datatype. */
    string get_type(Pix p);

    /** Get the type of an attribute.
	@param p
	@return The datatype of this attribute in an instance of AttrType. */
    AttrType get_attr_type(Pix p);

    /** If the indicated attribute is a container attribute, this function
	returns the number of attributes in {\it its} attribute table. If the
	indicated attribute is not a container, the method returns the number
	of values for the attribute (1 for a scalar attribute, N for a vector
	attribute value).
	@param p
	@return The number of elements in the attribute. */
    unsigned int get_attr_num(Pix p);

    /** Returns the value of an attribute. If the attribute has a vector
	value, you can indicate which is the desired value with the index
	argument, #i#. If the argument is omitted, the first value is
	returned. If the attribute has only a single value, the index
	argument is ignored. If #i# is greater than the number of
	elements in the attribute, an error is produced.

	All values in an attribute table are stored as string data. They may
	be converted to a more appropriate internal format by the calling
	program.

	@param p
	@param i The attribute value index, zero-based.
	@return If the indicated attribute is a container, this function
	returns the string ``None''. If using a name to refer to the attribute
	and the named attribute does not exist, return the empty string. */
    string get_attr(Pix p, unsigned int i = 0);
    
    /** Returns a pointer to the vector of values associated with the
	attribute referenced by Pix #p# or named #name#. 

	Note that all values in an attribute table are stored as string data.
	They may be converted to a more appropriate internal format by the
	calling program.

	@param p
	@return If the indicated attribute is a container, this function
	returns the null pointer.  Otherwise returns a pointer to the
	the attribute vector value. */
    vector<string> *get_attr_vector(Pix p);
    //@} Accessors that use a pix

    //@} Accessors

    /** @name Mutators */
    //@{
    /** Set the name of this attribute table.
	@param n The new name of the attribute table. */
    void set_name(const string &n);

    /** Adds an attribute to the table. If the given name already refers to
	an attribute, and the attribute has a value, the given value is
	appended to the attribute vector. Calling this function repeatedly is
	the way to append to an attribute vector.

	The function returns an error condition if the attribute is a
	container, or if the type of the input value does not match the
	existing attribute's type. Use #append_container()# to add container
	attributes.

	This method performs a simple search for #name# in this attribute
	table only; sub-tables are not searched and the dot notation is not
	recognized.

	@name append\_attr() 
	@return Returns the length of the added attribute value.
	@param name The name of the attribute to add or modify.
	@param type The type of the attribute to add or modify.
	@param value The value to add to the attribute table. */
    //@{
    ///
    unsigned int append_attr(const string &name, const string &type, 
			     const string &value) throw (Error);
    ///
    unsigned int append_attr(const char *name, const char *type, 
			     const char *value) throw (Error);
    //@}

    /** Create and append an attribute container to this AttrTable. If this
	attribute table already contains an attribute container called #name#
	an exception is thrown.

	@return A pointer to the new AttrTable object. 
	@memo Create a new container attribute. */
    AttrTable *append_container(const string &name) throw (Error);

    /** Append a new attribute container to this attribute table. The new
	container is #at# and its name is set to #name#. If this attribute
	table already contains an attribute container called #name# an
	exception is thrown.

	@return A pointer to the new AttrTable object. 
	@memo Create a new container attribute. */
    AttrTable *append_container(AttrTable *at, const string &name) 
	throw (Error);

    /** Add an alias to a container held by this attribute table. 
	@param name The name of the alias. May {\it not} use dot notation.
	@param src The existing attribute container to alias.
	@exception Error if an attribute, container or alias called #name#
	already exists in this attribute table. */
    void add_container_alias(const string &name, AttrTable *src) 
	throw (Error);

    /** Add an alias to an attribute held by this attribute table.
	@param name The name of the alias. May {\it not} use dot notation.
	@param source The name of the attribute to alias. May use dot
	notation. 
	@exception Error if the attribute table already contains an
	attribute, container or alias called #name# or if an attribute called
	#source# does not exist. */
    void add_value_alias(AttrTable *das, const string &name, 
			 const string &source) throw (Error);

    /** Adds an alias to the set of attributes.  Once an alias is
	inserted into an attribute table, reading the attributes for
	{\it alias} will return those stored for {\it name}. 

	Two forms for this function exist: one searches for {\it name}
	in the AttrTable referenced by {\it at} while the other uses
	#this#. You can use #DAS::get_attr_table()# to get the attribute
	table for an arbitrary name.

	@name attr\_alias()
	@see get_attr_table
	@deprecated */
    //@{

    /** @param alias The alias to insert into the attribute table.
	@param name The name of the already-existing attribute to which
	the alias will refer.
	@deprecated
	@param at An attribute table in which to insert the alias. */
    bool attr_alias(const string &alias, AttrTable *at, const string &name);

    /** @deprecated
	@param alias The alias to insert into the attribute table.
	@param name The name of the already-existing attribute to which
	the alias will refer. */
    bool attr_alias(const string &alias, const string &name);
    //@}

    /** Delete the attribute named #name#. If #i# is given, and
	the attribute has a vector value, delete the #i#$^th$ element of the
	vector.

	You can use this function to delete container attributes, although
	the #i# parameter has no meaning for that operation.

	@memo Deletes an attribute. 
	@param name The name of the attribute to delete.  This can be an
	attribute of any type, including containers. However, this method
	looks only in this attribute table and does not recognize the dot
	notation. 
	@param i If the named attribute is a vector, and #i# is
	non-negative, the i-th entry in the vector is deleted, and the
	array is repacked.  If #i# equals -1 (the default), the
	entire attribute is deleted. */
    void del_attr(const string &name, int i = -1);

    /** Prints an ASCII representation of the attribute table to the
	indicated output stream. The #pad# argument is prefixed to each
	line of the output to provide control of indentation.

	@memo Prints the attribute table.
	@param os Print to the given output stream.
	@param pad Indent elements of a table using this string of spaces. By
	default this is a string of four spaces
	@param dereference If true, follow aliases. Default is false. */
    void print(ostream &os, string pad = "    ", bool dereference = false);
    //@}
};

/* 
 * $Log: AttrTable.h,v $
 * Revision 1.34  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.32.4.3  2001/07/28 01:10:41  jimg
 * Some of the numeric type classes did not have copy ctors or operator=.
 * I added those where they were needed.
 * In every place where delete (or delete []) was called, I set the pointer
 * just deleted to zero. Thus if for some reason delete is called again
 * before new memory is allocated there won't be a mysterious crash. This is
 * just good form when using delete.
 * I added calls to www2id and id2www where appropriate. The DAP now handles
 * making sure that names are escaped and unescaped as needed. Connect is
 * set to handle CEs that contain names as they are in the dataset (see the
 * comments/Log there). Servers should not handle escaping or unescaping
 * characters on their own.
 *
 * Revision 1.33  2001/01/26 19:48:09  jimg
 * Merged with release-3-2-3.
 *
 * Revision 1.32.4.2  2000/11/30 05:24:46  jimg
 * Significant changes and improvements to the AttrTable and DAS classes. DAS
 * now is a child of AttrTable, which makes attributes behave uniformly at
 * all levels of the DAS object. Alias now work. I've added unit tests for
 * several methods in AttrTable and some of the functions in parser-util.cc.
 * In addition, all of the DAS tests now work.
 *
 * Revision 1.32.4.1  2000/11/22 21:47:42  jimg
 * Changed the implementation of DAS; it now inherits from AttrTable
 *
 * Revision 1.32  2000/09/22 02:17:18  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.31  2000/08/02 22:46:48  jimg
 * Merged 3.1.8
 *
 * Revision 1.27.6.4  2000/08/02 21:10:07  jimg
 * Removed the header config_dap.h. If this file uses the dods typedefs for
 * cardinal datatypes, then it gets those definitions from the header
 * dods-datatypes.h.
 *
 * Revision 1.27.6.3  2000/08/01 21:09:35  jimg
 * Destructor is now virtual
 *
 * Revision 1.30  2000/07/09 21:57:09  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.29  2000/06/07 19:33:21  jimg
 * Merged with verson 3.1.6
 *
 * Revision 1.27.6.2  2000/05/18 17:47:21  jimg
 * Fixed a bug in the AttrTable. Container attributes below the top level were
 * broken in the latest changes to the DAS code.
 *
 * Revision 1.27.6.1  2000/05/12 18:55:00  jimg
 * Moved the log to the end of the file - let's see if it is an improvement.
 * Changed the get_attr_num() method so that it returns a sensible value for
 * both containers and `plain' attributes.
 * Added to struct entry so that it is more a complete class.
 * Added methods so that operator= and the copy-ctor really work.
 *
 * Revision 1.27  1999/05/04 19:47:20  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
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

#endif // _attrtable_h
