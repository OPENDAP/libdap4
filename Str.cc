
// Implementation for Str.
//
// jhrg 9/7/94

// $Log: Str.cc,v $
// Revision 1.6  1994/12/22 04:32:23  reza
// Changed the default type to String (from Str) to match DDS parser.
//
// Revision 1.5  1994/12/14  19:18:00  jimg
// Added mfunc len(). Replaced size() with a mfunc that returns the size of
// a pointer to a string (this simplifies Structure, ...).
//
// Revision 1.4  1994/12/14  18:04:33  jimg
// Changed definition of size() so that it returns the number of bytes in
// the string.
//
// Revision 1.3  1994/11/29  20:16:32  jimg
// Added mfunc for data transmission.
// Uses special xdr function for serialization and xdr_coder.
// Removed `type' parameter from ctor.
// Added FILE *in and *out to ctor parameter list.
//
// Revision 1.2  1994/09/23  14:36:12  jimg
// Fixed errors in comments.
//
// Revision 1.1  1994/09/15  21:08:48  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/09  15:38:45  jimg
// Child class of BaseType -- used in the future to hold specific serialization
// information for integers. Should this be a class that uses BaseType?
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "Str.h"
#include "util.h"

Str::Str(const String &n, FILE *in, FILE *out)
    : BaseType(n, "String", xdr_str, in, out)
{
    buf = 0;			// read() frees if buf != 0
}

BaseType *
Str::ptr_duplicate()
{
    return new Str(*this);
}

// Return: the number of bytes needed to store the string's value or 0 if no
// storage for the string has been allocated.

unsigned int
Str::len()
{
    return buf ? strlen(buf): 0;
}

// Return: The number of bytes needed to store a string (which is represented
// as a pointer to a char).

unsigned int
Str::size()
{
    return sizeof(char *);
}

bool
Str::serialize(bool flush, unsigned int num)
{
    bool stat = (bool)xdr_str(xdrout, &buf);
    if (stat && flush)
	stat = expunge();

    return stat;
}

// deserialize the double on stdin and put the result in BUF.

unsigned int
Str::deserialize()
{
    unsigned int num = xdr_str(xdrin, &buf);

    return num;
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Str::print_val(ostream &os, String space)
{
    print_decl(os, "", false);
    os << " = " << buf << ";" << endl;
}
