
// Implementation of the attribute table (AttrTable) class.
//
// jhrg 7/29/94

// $Log: AttrTable.cc,v $
// Revision 1.5  1994/09/27 22:42:44  jimg
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
// Fixed `Revision 1.5  1994/09/27 22:42:44  jimg
// Fixed `Changed definition of the class AttrTable; it no longer inherits from
// Fixed `AttrVHMap, instead it uses that class (contains a member that is an instance
// Fixed `of AttrVHMap).
// Fixed `Added mfuncs to AttrTable so that the new member could be set/accessed.
// Fixed `
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

static char rcsid[]="$Id: AttrTable.cc,v 1.5 1994/09/27 22:42:44 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <ostream.h>
#include <Pix.h>

#include "AttrTable.h"

AttrTable::AttrTable(String& dflt, unsigned int sz) : map(dflt, sz)
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

String &
AttrTable::get_name(Pix p)
{
    return map.key(p);
}

String &
AttrTable::get_attr(Pix p)
{
    return map.contents(p);
}

String &
AttrTable::get_attr(const String &name)
{
    return map[name];
}

String &
AttrTable::get_attr(const char *name)
{
    return map[name];
}

void
AttrTable::set_attr(const String &name, String value)
{
    map[name] = value;
}

void
AttrTable::print(ostream &os, String pad)
{
    for(Pix p = map.first(); p; map.next(p))
	os << pad << map.key(p) << " " << map.contents(p) << ";" << endl;
}

