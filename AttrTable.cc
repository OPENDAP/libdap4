
// Implementation of the attribute table (AttrTable) class.
//
// jhrg 7/29/94

// $Log: AttrTable.cc,v $
// Revision 1.4  1994/09/09 15:26:39  jimg
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
// Fixed `Revision 1.4  1994/09/09 15:26:39  jimg
// Fixed `Removed operator<< and added print() since I have no good way to define
// Fixed `operator>>. It seems best to define all operators from a set (like <<, >>)
// Fixed `or none at all. Since parse() is the input mfunc, it seems that output
// Fixed `should be a mfunc too.
// Fixed `
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

static char rcsid[]="$Id: AttrTable.cc,v 1.4 1994/09/09 15:26:39 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif
#include "AttrTable.h"

#include <ostream.h>
#include <Pix.h>

String AttrTable::empty = "";	// this must only be defined once.

void
AttrTable::print(ostream &os)
{
    for(Pix p = first(); p; next(p))
	os << key(p) << " " << contents(p) << ";" << endl;
}
