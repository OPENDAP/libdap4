
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// jhrg 7/29/94

#include "config_dap.h"

static char rcsid[] not_used ="$Id: AttrTable.cc,v 1.30 2001/08/24 17:46:22 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include <iostream>

#include "Error.h"
#include "debug.h"
#include "util.h"
#include "AttrTable.h"
#include "escaping.h"

using std::string;
#if 0
using std::vector<string>;
#endif
using std::vector;
using std::endl;

// Find the attribute #target#. To reference an arbitrary attribute, a user
// needs an AttrTable and a Pix pointing to an attribute tuple within that
// table. 
Pix 
AttrTable::find(const string &target, AttrTable **at)
{
    string::size_type dotpos = target.rfind('.');
    if (dotpos != string::npos) {
	string container = target.substr(0, dotpos);
	string field = target.substr(dotpos+1);
	
	*at = find_container(container);
	return (*at) ? (*at)->simple_find(field) : 0;
    }
    else {
	*at = this;
	return simple_find(target);
    }
}

Pix
AttrTable::simple_find(const string &target)
{
    for (Pix p = attr_map.first(); p; attr_map.next(p))
	if (target == attr_map(p)->name)
	    return p;
    return 0;
}

// Find the attribute container which holds #target#. Use #this# as the
// root of the tables to search. If a table called #target# does not exist,
// return null. The string target may contain dots in which case this
// function will parse #target#.
AttrTable *
AttrTable::find_container(const string &target)
{
    string::size_type dotpos = target.find('.');
    if (dotpos != string::npos) {
	string container = target.substr(0, dotpos);
	string field = target.substr(dotpos+1);
	
	AttrTable *at= simple_find_container(container);
	return (at) ? at->find_container(field) : 0;
    }
    else {
	return simple_find_container(target);
    }
}

AttrTable *
AttrTable::simple_find_container(const string &target)
{
    if (get_name() == target)
	return this;

    for (Pix p = attr_map.first(); p; attr_map.next(p))
	if (is_container(p) && target == attr_map(p)->name)
	    return attr_map(p)->attributes;

    return 0;
}

string 
AttrTable::AttrType_to_String(const AttrType at)
{
    switch (at) {
      case Attr_container: return "Container";
      case Attr_byte: return "Byte";
      case Attr_int16: return "Int16";
      case Attr_uint16: return "Uint16";
      case Attr_int32: return "Int32";
      case Attr_uint32: return "Uint32";
      case Attr_float32: return "Float32";
      case Attr_float64: return "Float64";
      case Attr_string: return "String";
      case Attr_url: return "Url";
      default: return "";
    }
}

AttrType
AttrTable::String_to_AttrType(const string &s)
{
    string s2 = s;
    downcase(s2);

    if (s2 == "container")
	return Attr_container;
    else if (s2 == "byte")
	return Attr_byte;
    else if (s2 == "int16")
	return Attr_int16;
    else if (s2 == "uint16")
	return Attr_uint16;
    else if (s2 == "int32")
	return Attr_int32;
    else if (s2 == "uint32")
	return Attr_uint32;
    else if (s2 == "float32")
	return Attr_float32;
    else if (s2 == "float64")
	return Attr_float64;
    else if (s2 == "string")
	return Attr_string;
    else if (s2 == "url")
	return Attr_url;
    else 
	return Attr_unknown;
}

// Protected member functions

void
AttrTable::clone(const AttrTable &at)
{
    for (Pix p = at.attr_map.first(); p; at.attr_map.next(p)) {
	entry *e = new entry(*at.attr_map(p));
	attr_map.append(e);
    }
}

// Public member functions

AttrTable::AttrTable()
{
}

AttrTable::AttrTable(const AttrTable &rhs)
{
    clone(rhs);
}

void
AttrTable::delete_attr_table() 
{
    for (Pix p = attr_map.first(); p; attr_map.next(p)) {
	delete attr_map(p);
	attr_map(p) = 0;
    }
}

AttrTable::~AttrTable()
{
    DBG(cerr << "Entering ~AttrTable (" << this << ")" << endl);
    delete_attr_table();
    DBG(cerr << "Exiting ~AttrTable" << endl);
}

AttrTable &
AttrTable::operator=(const AttrTable &rhs)
{
    if (this != &rhs) {
	delete_attr_table();
	clone(rhs);
    }

    return *this;
}	    

unsigned int
AttrTable::get_size() const
{
    return attr_map.length();
}

string
AttrTable::get_name()
{
    return d_name;
}

void
AttrTable::set_name(const string &n)
{
    d_name = www2id(n);
}

Pix 
AttrTable::first_attr()
{
    return attr_map.first();
}

void
AttrTable::next_attr(Pix &p)
{
    attr_map.next(p);
}

string
AttrTable::get_name(Pix p)
{
    assert(p);
    return attr_map(p)->name;
}

bool
AttrTable::is_container(Pix p)
{
    assert(p);
    return attr_map(p)->type == Attr_container;
}

AttrTable *
AttrTable::get_attr_table(Pix p)
{
    assert(p);
    return attr_map(p)->type == Attr_container ? attr_map(p)->attributes : 0;
}

AttrTable *
AttrTable::get_attr_table(const string &name)
{
    return find_container(name);
}

AttrTable *
AttrTable::get_attr_table(const char *name)
{
    return get_attr_table((string)name);
}

string
AttrTable::get_type(Pix p)
{
    assert(p);
    return AttrType_to_String(attr_map(p)->type);
}

string
AttrTable::get_type(const string &name)
{
    Pix p = simple_find(name);
    return (p) ?  get_type(p) : (string)"";
}

string
AttrTable::get_type(const char *name)
{
    return get_type((string)name);
}

AttrType
AttrTable::get_attr_type(Pix p)
{
    assert(p);
    return attr_map(p)->type;
}

AttrType
AttrTable::get_attr_type(const string &name)
{
    Pix p = simple_find(name);
    return (p) ?  get_attr_type(p) : Attr_unknown;
}

AttrType
AttrTable::get_attr_type(const char *name)
{
    return get_attr_type((string)name);
}

unsigned int 
AttrTable::get_attr_num(Pix p)
{
    assert(p);
    return (attr_map(p)->type == Attr_container)
	? attr_map(p)->attributes->get_size() 
	: attr_map(p)->attr->size();
}

unsigned int 
AttrTable::get_attr_num(const string &name)
{
    Pix p = simple_find(name);
    return (p) ?  get_attr_num(p) : 0;
}

unsigned int 
AttrTable::get_attr_num(const char *name)
{
    return get_attr_num((string)name);
}

string
AttrTable::get_attr(Pix p, unsigned int i)
{
    assert(p);
    return attr_map(p)->type == Attr_container ? (string)"None" : (*attr_map(p)->attr)[i];
}

string
AttrTable::get_attr(const string &name, unsigned int i)
{
    Pix p = simple_find(name);
    return (p) ? get_attr(p, i) : (string)"";
}

string
AttrTable::get_attr(const char *name, unsigned int i)
{
    return get_attr((string)name, i);
}

vector<string> *
AttrTable::get_attr_vector(Pix p)
{
    assert(p);
    return attr_map(p)->type != Attr_container ? attr_map(p)->attr : 0;
}

vector<string> *
AttrTable::get_attr_vector(const string &name)
{
    Pix p = simple_find(name);
    return (p) ?  get_attr_vector(p) : 0;
}

vector<string> *
AttrTable::get_attr_vector(const char *name)
{
    return get_attr_vector((string)name);
}

unsigned int
AttrTable::append_attr(const string &name, const string &type, 
		       const string &attr) throw (Error)
{
    string lname = www2id(name);

    Pix p = simple_find(lname);

    // If the types don't match OR this attribute is a container, calling
    // this mfunc is an error!
    if (p && (attr_map(p)->type != String_to_AttrType(type)))
	throw Error(string("An attribute called `") + name 
		    + string("' already exists but is of a different type"));
    if (p && (get_type(p) == "Container"))
	throw Error(string("An attribute called `") + name 
		    + string("' already exists but is a container."));

    if (p) {			// Must be a new attribute value; add it.
        attr_map(p)->attr->push_back(attr);
	return attr_map(p)->attr->size();
    } else {			// Must be a completely new attribute; add it
	entry *e = new entry;

	e->name = lname;
	e->is_alias = false;
	e->type = String_to_AttrType(type); // Record type using standard names.
	e->attr = new vector<string>;
	e->attr->push_back(attr);

	attr_map.append(e);
    
	return e->attr->size();	// return the length of the attr vector
    }
}

unsigned int
AttrTable::append_attr(const char *name, const char *type, const char *attr)
    throw (Error)
{
    return append_attr((string)name, (string)type, (string)attr);
}

AttrTable *
AttrTable::append_container(const string &name) throw (Error)
{
    return append_container(new AttrTable, name);
}

AttrTable *
AttrTable::append_container(AttrTable *at, const string &name) throw (Error)
{
    string lname = www2id(name);

    if (simple_find(name))
	throw Error(string("There already exists a container called `")
		    + name + string("in this attribute table."));

    at->set_name(lname);

    entry *e = new entry;
    e->name = lname;
    e->is_alias = false;
    e->type = Attr_container;
    e->attributes = at;

    attr_map.append(e);

    return e->attributes;
}

// Alias an attribute table. The alias should be added to this object.
void
AttrTable::add_container_alias(const string &name, AttrTable *src) 
    throw (Error)
{
    string lname = www2id(name);

    if (simple_find(lname))
	throw Error(string("There already exists a container called `")
		    + name + string("in this attribute table."));

    entry *e = new entry;
    e->name = lname;
    e->is_alias = true;
    e->aliased_to = src->get_name();
    e->type = Attr_container;

    e->attributes = src;

    attr_map.append(e);
}

// Assume #source# names an attribute value in some container. Add an alias
// #name# for that value in this object.
void
AttrTable::add_value_alias(AttrTable *das, const string &name, 
			   const string &source) throw (Error)
{
    string lname = www2id(name);
    string lsource = www2id(source);

    // find the container that holds #source# and then find #source#'s Pix
    // within that container. Search at the uppermost level of the attribtue
    // object to find values defined `above' the current container.
    AttrTable *at;
    Pix p = das->find(lsource, &at);

    // If #source# is not found by looking at the topmost level, look in the
    // current table (i.e., alias z x where x is in the current container
    // won't be found by looking for `x' at the top level). See test case 26
    // in das-testsuite.
    if (!(at && p)) {
	p = find(lsource, &at);
	if (!(at && p))
	    throw Error(string("Could not find the attribute `")
			+ source + string("' in the attribute object."));
    }

    // If we've got a value to alias and it's being added at the top level of
    // the DAS, that's an error.
    if (!at->is_container(p) && this == das)
	throw Error(string("A value cannot be aliased to the top level of the\
 DAS;\nOnly containers may be present at that level of the DAS."));

    if (simple_find(lname))
	throw Error(string("There already exists a container called `")
		    + name + string("in this attribute table."));

    entry *e = new entry;
    e->name = lname;
    e->is_alias = true;
    e->aliased_to = lsource;
    e->type = at->attr_map(p)->type;
    if (e->type == Attr_container)
	e->attributes = at->get_attr_table(p);
    else
	e->attr = at->attr_map(p)->attr;

    attr_map.append(e);
}

// Deprecated
bool
AttrTable::attr_alias(const string &alias, AttrTable *at, const string &name)
{
    add_value_alias(at, alias, name);
    return true;
}

bool
AttrTable::attr_alias(const string &alias, const string &name)
{
    return attr_alias(alias, this, name);
}

// Delete the attribute NAME. If NAME is an attribute vector, delete the
// I(th) element if I is >= 0. If I is -1 (the default), remove the entire
// attribute even if it is an array of values.
//
// Returns: void

void
AttrTable::del_attr(const string &name, int i)
{
    string lname = www2id(name);

    Pix p = simple_find(lname);
    if (p) {
	if (i == -1) {		// Delete the whole attribute
	    attr_map.prev(p);	// p now points to the previous element
	    attr_map.del_after(p);	// ... delete the following element
	}
	else {			// Delete one element from attribute array
	    // Don't try to delete elements from the vector of values if the
	    // map is a container!
	    if (attr_map(p)->type == Attr_container) 
		return;

	    vector<string> *sxp = attr_map(p)->attr;
		
	    assert(i >= 0 && i < (int)sxp->size());
	    sxp->erase(sxp->begin() + i); // rm the element
	}
    }
}

// This is protected.	
void
AttrTable::simple_print(ostream &os, string pad, Pix p, bool dereference)
{
    switch (attr_map(p)->type) {
      case Attr_container:
	os << pad << id2www(get_name(p)) << " {" << endl;

	attr_map(p)->attributes->print(os, pad + "    ", dereference);

	os << pad << "}" << endl;
	break;

      default: {
	    os << pad << get_type(p) << " " << id2www(get_name(p)) << " " ;

	    vector<string> *sxp = attr_map(p)->attr;
	    
	    vector<string>::iterator last = sxp->end()-1;
	    for (vector<string>::iterator i = sxp->begin(); i != last; ++i)
		os << *i << ", ";
  
	    os << *(sxp->end()-1) << ";" << endl;
	}
	break;
    }
}
	    
void
AttrTable::print(ostream &os, string pad, bool dereference)
{
    for(Pix p = attr_map.first(); p; attr_map.next(p)) {
	if (attr_map(p)->is_alias) {
	    if (dereference) {
		simple_print(os, pad, p, dereference);
	    }
	    else {
		os << pad << "Alias " << id2www(get_name(p)) << " " 
		   << id2www(attr_map(p)->aliased_to) << ";" << endl;
	    }
	} 
	else {
	    simple_print(os, pad, p, dereference);
	}
    }
}

// $Log: AttrTable.cc,v $
// Revision 1.30  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.28.4.4  2001/08/18 01:48:53  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.28.4.3  2001/07/28 01:10:41  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.29  2001/01/26 19:48:09  jimg
// Merged with release-3-2-3.
//
// Revision 1.28.4.2  2000/11/30 05:24:46  jimg
// Significant changes and improvements to the AttrTable and DAS classes. DAS
// now is a child of AttrTable, which makes attributes behave uniformly at
// all levels of the DAS object. Alias now work. I've added unit tests for
// several methods in AttrTable and some of the functions in parser-util.cc.
// In addition, all of the DAS tests now work.
//
// Revision 1.28.4.1  2000/11/22 21:47:42  jimg
// Changed the implementation of DAS; it now inherits from AttrTable
//
// Revision 1.28  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.27  2000/06/16 18:14:59  jimg
// Merged with 3.1.7
//
// Revision 1.23.6.4  2000/06/14 16:59:00  jimg
// Added instrumentation for the dtor.
//
// Revision 1.26  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.23.6.3  2000/05/18 17:47:21  jimg
// Fixed a bug in the AttrTable. Container attributes below the top level were
// broken in the latest changes to the DAS code.
//
// Revision 1.23.6.2  2000/05/12 18:55:54  jimg
// See comments in AttrTable.h.
//
// Revision 1.23.6.1  2000/01/26 23:55:02  jimg
// Fixed the return type of string::find.
//
// Revision 1.23  1999/04/29 02:29:26  jimg
// Merge of no-gnu branch
//
// Revision 1.22  1999/03/24 23:37:13  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.21  1998/11/24 06:50:07  jimg
// Added instrumentation. Used while I was removing DASVHMap.
//
// Revision 1.20.2.2  1999/02/05 09:32:10  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.20.2.1  1999/02/02 19:43:13  jimg
// StringSPlex Removed
//
// Revision 1.20  1998/08/06 16:06:42  jimg
// Now prints aliases as such rather than replicating the aliased entry.
//
// Revision 1.19  1997/08/09 21:18:41  jimg
// Changed/fixed the type comparison in attr_append so that code which calls
// unsigned int `UInt32' won't break (the type is called Uint32, but the das
// parser and scanner accept UInt32 as well).
//
// Revision 1.18  1997/07/15 21:58:04  jimg
// Formatting.
//
// Revision 1.17  1997/06/06 03:12:44  jimg
// Added mfuncs with char * parameters for some of the calls. See AttrTable.h
//
// Revision 1.16  1997/05/13 23:32:11  jimg
// Added changes to handle the new Alias and lexical scoping rules.
//
// Revision 1.15  1997/01/13 16:56:03  jimg
// Changed the name of the private member `map' to `attr_map' to avoid a name
// collision with the STL'd map class.
//
// Revision 1.14  1996/08/13 20:49:30  jimg
// Added not_used to definition of char rcsid[].
//
// Revision 1.13  1996/05/31 23:29:21  jimg
// Updated copyright notice.
//
// Revision 1.12  1996/04/05 00:21:18  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.11  1995/07/09  21:28:50  jimg
// Added copyright notice.
//
// Revision 1.10  1995/02/10  02:27:53  jimg
// Fixed an error where two attributes could be declared with the same name
// (for the same variable) if they hade different types.
//
// Revision 1.9  1994/12/21  03:03:37  reza
// Added overloading functions for get_attr_num().
//
// Revision 1.8  1994/12/07  21:09:24  jimg
// Added support for vectors of attributes (using XPlex from libg++).
//
// Revision 1.7  1994/10/13  15:43:29  jimg
// Added a new version of append_attr that takes (const char *)s and modified
// the version that takes strings to take (const String &).
//
// Revision 1.6  1994/10/05  16:38:17  jimg
// Changed internal representation of the attribute table from a Map
// to a DLList<>.
//
// Revision 1.5  1994/09/27  22:42:44  jimg
// Changed definition of the class AttrTable; it no longer inherits from
// AttrVHMap, instead it uses that class (contains a member that is an instance
// of AttrVHMap).
// Added mfuncs to AttrTable so that the new member could be set/accessed.
//
// Revision 1.4  1994/09/09  15:26:39  jimg
// Removed operator<< and added print() since I have no good way to define
// operator>>. It seems best to define all operators from a set (like <<, >>)
// or none at all. Since parse() is the input mfunc, it seems that output
// should be a mfunc too.
//
// Revision 1.3  1994/08/02  20:11:27  jimg
// Changes operator<< so that it writes a parsable version of the
// attribute table.
//' comments and rcsid[] variables (syntax errors due to //
// comments caused compilation failures.
// das.tab.c and .h are commited now as well.
//
// Revision 1.1  1994/08/02  18:32:04  jimg
// The implementation of AttrTable. This file defined ostream &operator<< and
// a static class variable String empty (it is initialized to "").


