
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// An AttrTable is a table of attributes (type-name-value tuples).

#ifndef _attrtable_h
#define _attrtable_h 1

#ifndef __POWERPC__
#ifdef __GNUG__
#pragma interface
#endif
#endif

#include <string>
#include <vector>

#include "Pix.h"

#ifndef _error_h
#include "Error.h"
#endif

using std::vector;
using std::string;
using std::ostream;
using std::vector;

/** <b>AttrType</b> identifies the data types which may appear in an
    attribute table object. 

    \code
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
    \endcode

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

string AttrType_to_String(const AttrType at);
AttrType String_to_AttrType(const string &s);

/** An AttrTable (``Attribute Table'') stores a set of names and, for
    each name, either a type and a value, or another attribute table.
    The attribute value can be a vector containing many values of the
    same type.  The attributes can have any of the types listed in the
    <tt>AttrType</tt> list.  However, all attribute types are stored as
    string data, except for the container type, which is stored as a
    pointer to another attribute table.

    Each element in the attribute table can itself be an attribute
    table.  The table can also contain ``alias'' attributes whose
    value is given by the value of another attribute to which it is
    linked.

    The attribute tables have a standard printed representation.
    There is a member function <tt>print()</tt> for writing this form.  Use
    the <tt>DAS::parse()</tt> function to read the printed form.

    An attribute table might look something like this:

    \verbatim
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
    \endverbatim

    Here, <tt>long_name</tt>, <tt>units</tt>, and
    <tt>missing_value</tt> are simple 
    attributes, and <tt>actual_range</tt> and <tt>conversion_data</tt>
    are container attributes containing other attribute tables.

    @todo Look at refactoring this by splitting it into three classes. Move
    the struct entry into its own calls (maybe called Attribute?), make
    AttrTable a child of that class and then make alises a separate class,
    also a child of Attribute. Look at the design of the Java code.

    @brief Contains the attributes for a dataset.
    @see DAS
    @see AttrType */
class AttrTable {
    // entry needs to be made public to make up for issues with this class'
    // design. It should probably be moved to it's own class. 05/22/03 jhrg
public:
    /** Each AttrTable has zero or more entries. Instead of access this
	struct's members directly, use AttrTable methods.

	This struct is public because its type is used in public typedefs. */
    struct entry {
	string name;
	AttrType type;
	
	bool is_alias;
        string aliased_to;

	// If type == Attr_container, use attributes to read the contained
	// table, otherwise use attr to read the vector of values.
	AttrTable *attributes;
	std::vector<string> *attr;	// a vector of values. jhrg 12/5/94

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
		  std::vector<string> *src_attr = rhs.attr;
		  std::vector<string> *dest_attr = new std::vector<string>(*src_attr);
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

    typedef std::vector<entry *>::const_iterator Attr_citer ;
    typedef std::vector<entry *>::iterator Attr_iter ;

private:
    string d_name;
    std::vector<entry *> attr_map;

    Pix simple_find(const string &target);

    Attr_iter simple_find( const string &target, bool unused );
    AttrTable *simple_find_container(const string &target);

    void delete_attr_table();

    friend class AttrTableTest;

protected:
    void clone(const AttrTable &at);

    void simple_print(ostream &os, string pad, Attr_iter &i,
		      bool dereference);

    void simple_print(FILE *out, string pad, Attr_iter &i,
		      bool dereference);

public:
    AttrTable();
    AttrTable(const AttrTable &rhs);
    virtual ~AttrTable();
    AttrTable & operator=(const AttrTable &rhs);

    void erase();

    unsigned int get_size() const;
    string get_name() const;
    void set_name(const string &n);

    unsigned int append_attr(const string &name, const string &type, 
			     const string &value) throw (Error);
    unsigned int append_attr(const char *name, const char *type, 
			     const char *value) throw (Error);

    AttrTable *append_container(const string &name) throw (Error);
    AttrTable *append_container(AttrTable *at, const string &name) 
	throw (Error);

    void find(const string &target, AttrTable **at, Attr_iter &iter);
    AttrTable *find_container(const string &target);

    AttrTable *get_attr_table(const string &name);
    AttrTable *get_attr_table(const char *name);

    string get_type(const string &name);
    string get_type(const char *name);

    AttrType get_attr_type(const string &name);
    AttrType get_attr_type(const char *name);

    unsigned int get_attr_num(const string &name);
    unsigned int get_attr_num(const char *name);

    string get_attr(const string &name, unsigned int i = 0);
    string get_attr(const char *name, unsigned int i = 0);

    vector<string> *get_attr_vector(const string &name);
    vector<string> *get_attr_vector(const char *name);

    void del_attr(const string &name, int i = -1);

    Pix first_attr();
    void next_attr(Pix p);
    AttrTable::entry *attr(Pix p);
    string get_name(Pix p);
    bool is_container(Pix p);
    AttrTable *get_attr_table(Pix p);
    string get_type(Pix p);
    AttrType get_attr_type(Pix p);
    unsigned int get_attr_num(Pix p);
    string get_attr(Pix p, unsigned int i = 0);
    vector<string> *get_attr_vector(Pix p);
    Pix find(const string &target, AttrTable **at);

    Attr_iter attr_begin();
    Attr_iter attr_end();
    Attr_iter get_attr_iter(int i);
    string get_name(Attr_iter &iter);
    bool is_container(Attr_iter &iter);
    AttrTable *get_attr_table(Attr_iter &iter);
    string get_type(Attr_iter &iter);
    AttrType get_attr_type(Attr_iter &iter);
    unsigned int get_attr_num(Attr_iter &iter);
    string get_attr(Attr_iter &iter, unsigned int i = 0);
    std::vector<string> *get_attr_vector(Attr_iter &iter);

    void add_container_alias(const string &name, AttrTable *src) 
	throw (Error);
    void add_value_alias(AttrTable *das, const string &name, 
			 const string &source) throw (Error);
    bool attr_alias(const string &alias, AttrTable *at, const string &name);
    bool attr_alias(const string &alias, const string &name);

    void print(ostream &os, string pad = "    ", bool dereference = false);

    void print(FILE *out, string pad = "    ", bool dereference = false);

    void print_xml(FILE *out, string pad = "    ", bool constrained = false);
};

/* 
 * $Log: AttrTable.h,v $
 * Revision 1.47  2003/12/08 18:02:29  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.44.2.2  2003/09/06 22:37:50  jimg
 * Updated the documentation.
 *
 * Revision 1.44.2.1  2003/06/23 11:49:18  rmorris
 * The #pragma interface directive to GCC makes the dynamic typing functionality
 * go completely haywire under OS X on the PowerPC.  We can't use that directive
 * on that platform and it was ifdef'd out for that case.
 *
 * Revision 1.46  2003/05/30 16:53:42  jimg
 * Made get_name() a const method.
 *
 * Revision 1.45  2003/05/23 03:24:56  jimg
 * Changes that add support for the DDX response. I've based this on Nathan
 * Potter's work in the Java DAP software. At this point the code can
 * produce a DDX from a DDS and it can merge attributes from a DAS into a
 * DDS to produce a DDX fully loaded with attributes. Attribute aliases
 * are not supported yet. I've also removed all traces of strstream in
 * favor of stringstream. This code should no longer generate warnings
 * about the use of deprecated headers.
 *
 * Revision 1.44  2003/04/22 19:40:27  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.41.2.2  2003/04/15 00:46:16  jimg
 * Added get_attr_iter().
 * Rearranged the methods and moved documentation into the .cc file.
 *
 * Revision 1.43  2003/02/25 23:27:50  jimg
 * Added erase() method.
 *
 * Revision 1.42  2003/02/21 00:14:24  jimg
 * Repaired copyright.
 *
 * Revision 1.41.2.1  2003/02/21 00:10:06  jimg
 * Repaired copyright.
 *
 * Revision 1.41  2003/01/23 00:22:23  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.40  2003/01/22 01:05:39  jimg
 * Added using std::vector.
 *
 * Revision 1.39  2003/01/15 19:24:39  pwest
 * Removing IteratorAdapterT and replacing with non-templated versions.
 *
 * Revision 1.38  2003/01/10 19:46:39  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.32.4.11  2002/12/17 22:35:02  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.32.4.10  2002/11/18 18:51:59  jimg
 * Changed the include of Pix.h from #include <Pix.h> to "Pix.h" to fix
 * a problem with the dependencies (see today's check in of Makefile.in).
 *
 * Revision 1.32.4.9  2002/10/28 21:17:43  pwest
 * Converted all return values and method parameters to use non-const iterator.
 * Added operator== and operator!= methods to IteratorAdapter to handle Pix
 * problems.
 *
 * Revision 1.32.4.8  2002/09/22 14:15:42  rmorris
 * Changed the use of vector to std::vector.  The 'using' directive for VC++
 * no longer cut it in this case.
 *
 * Revision 1.32.4.7  2002/09/12 22:49:57  pwest
 * Corrected signature changes made with Pix to IteratorAdapter changes. Rather
 * than taking a reference to a Pix, taking a Pix value.
 *
 * Revision 1.32.4.6  2002/09/05 22:52:54  pwest
 * Replaced the GNU data structures SLList and DLList with the STL container
 * class vector<>. To maintain use of Pix, changed the Pix.h header file to
 * redefine Pix to be an IteratorAdapter. Usage remains the same and all code
 * outside of the DAP should compile and link with no problems. Added methods
 * to the different classes where Pix is used to include methods to use STL
 * iterators. Replaced the use of Pix within the DAP to use iterators instead.
 * Updated comments for documentation, updated the test suites, and added some
 * unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
 *
 * Revision 1.37  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.36  2002/06/03 22:21:15  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.35  2002/05/23 15:22:39  tom
 * modified for doxygen
 *
 * Revision 1.32.4.5  2002/03/29 18:29:00  jimg
 * Fixed a comment about escaping backslashes.
 *
 * Revision 1.32.4.4  2002/01/30 19:06:21  jimg
 * Fixed some of the doc++ comment about append_attr().
 *
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
