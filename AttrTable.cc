
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// jhrg 7/29/94

// $Log: AttrTable.cc,v $
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
//

static char rcsid[]="$Id: AttrTable.cc,v 1.13 1996/05/31 23:29:21 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include <ostream.h>

#include "AttrTable.h"

AttrTable::AttrTable()
{
}

Pix 
AttrTable::first_attr()
{
    return map.first();
}

void
AttrTable::next_attr(Pix &p)
{
    map.next(p);
}

String
AttrTable::get_name(Pix p)
{
    return map(p).name;
}

String
AttrTable::get_type(Pix p)
{
    return map(p).type;
}

// Returns: The number of elements in the vector of attribute values.
//
// NB: *Assumes* that the vector is never longer than the actual number of
// elements. 

unsigned int 
AttrTable::get_attr_num(Pix p)
{
    return map(p).attr.length();
}
unsigned int 
AttrTable::get_attr_num(const String &name)
{
    Pix p = find(name);
    if (p)
        return map(p).attr.length();
    else
        return 0;
}
unsigned int 
AttrTable::get_attr_num(const char *name)
{
    Pix p = find((String)name);
    if (p)
        return map(p).attr.length();
    else
        return 0;
}

// Returns: The string which contains the I(th) attribute value for list
// element referred to by Pix P.
//
// NB: I defaults to the zero(th) element of the vector of values

String
AttrTable::get_attr(Pix p, unsigned int i)
{
    return map(p).attr[i];
}

// Private mfunc that finds the entry with name == target. If TARGET is not
// found, returns null.

Pix
AttrTable::find(const String &target)
{
    for (Pix p = map.first(); p; map.next(p))
	if (target == map(p).name)
	    return p;
    return 0;
}

String
AttrTable::get_attr(const String &name, unsigned int i)
{
    Pix p = find(name);
    if (p)
	return map(p).attr[i];
    else
	return (char *)0;
}

String
AttrTable::get_attr(const char *name, unsigned int i)
{
    Pix p = find((String)name);
    if (p)
	return map(p).attr[i];
    else
	return (char *)0;
}

String
AttrTable::get_type(const String &name)
{
    Pix p = find(name);
    if (p)
	return map(p).type;
    else
	return (char *)0;
}

String
AttrTable::get_type(const char *name)
{
    Pix p = find((String)name);
    if (p)
	return map(p).type;
    else
	return (char *)0;
}

// Add the attribute ATTR to variable NAME with type TYPE. If there exists a
// variable with that name, append the attribute to the PLex of attributes,
// otherwise creat an entry for the new variable and append it to the list of
// vars. 
//
// Returns: The length of the attribute PLex (array) for the named variable.
// If an error is detected, returns 0.

unsigned int
AttrTable::append_attr(const String &name, const String &type, 
		       const String &attr)
{
    Pix p = find(name);
    if (p && get_type(p) != type)
	return 0;		// error:same name but diff type
    else if (p)
	return map(p).attr.add_high(attr) + 1; // new variable
    else {
	entry e;

	e.name = name;
	e.type = type;
	unsigned int len = e.attr.add_high(attr) + 1;

	map.append(e);
    
	return len;		// return the length of the attr XPlex
    }
}

// (char *) interface to above mfunc.

unsigned int
AttrTable::append_attr(const char *name, const char *type, const char *attr)
{
    return append_attr((String)name, (String)type, (String)attr);
}

// Delete the attribute NAME. If NAME is an array of attributes, delete the
// I(th) element if I is >= 0. If I is -1 (the default), remove the entire
// attribute even f it is an array of values.
//
// Returns: void

void
AttrTable::del_attr(const String &name, int i)
{
    Pix p = find(name);
    if (p) {
	if (i == -1) {		// Delete the whole attribute
	    map.prev(p);	// p now points to the previous element
	    map.del_after(p);	// ... delete the following element
	}
	else {			// Delete one element from attribute array
	    StringXPlex &sxp = map(p).attr;
		
	    assert(i >= 0 && i < sxp.fence());
	    for (int j = i+1; j < sxp.fence(); ++j)
		sxp[j-1] = sxp[j]; // mv array elements to empty space
	    (void) sxp.del_high(); // rm extra element
	}
    }
}
	
	    
void
AttrTable::print(ostream &os, String pad)
{
    for(Pix p = map.first(); p; map.next(p)) {
	os << pad << map(p).type << " " << map(p).name << " " ;

	StringXPlex &sxp = map(p).attr;

	for (int i = 0; i < sxp.high(); ++i)
	    os << sxp[i] << ", ";

	os << sxp[sxp.high()] << ";" << endl;
    }
}

