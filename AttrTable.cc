
// Implementation of the attribute table (AttrTable) class.
//
// jhrg 7/29/94

// $Log: AttrTable.cc,v $
// Revision 1.6  1994/10/05 16:38:17  jimg
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
//
// Revision 1.2  1994/08/02  19:17:37  jimg
// Fixed `$Log: AttrTable.cc,v $
// Fixed `Revision 1.6  1994/10/05 16:38:17  jimg
// Fixed `Changed internal representation of the attribute table from a Map
// Fixed `to a DLList<>.
// Fixed `
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

static char rcsid[]="$Id: AttrTable.cc,v 1.6 1994/10/05 16:38:17 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

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

String
AttrTable::get_attr(Pix p)
{
    return map(p).attr;
}

// private mfunc that finds the entry with name == target

Pix
AttrTable::find(const String &target)
{
    for (Pix p = map.first(); p; map.next(p))
	if (target == map(p).name)
	    return p;
}

String
AttrTable::get_attr(const String &name)
{
    Pix p = find(name);
    if (p)
	return map(p).attr;
    else
	return (char *)0;
}

String
AttrTable::get_attr(const char *name)
{
    Pix p = find((String)name);
    if (p)
	return map(p).attr;
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

void
AttrTable::append_attr(const String &name, String type, String attr)
{
    entry e;

    e.name = name;
    e.type = type;
    e.attr = attr;

    map.append(e);
}

void
AttrTable::del_attr(const String &name)
{
    Pix p = find(name);
    if (p) {
	map.prev(p);
	map.del_after(p);
    }
}
	
	    
void
AttrTable::print(ostream &os, String pad)
{
    for(Pix p = map.first(); p; map.next(p))
	os << pad << map(p).type << " " << map(p).name << " " << map(p).attr
	   << ";" << endl;
}

