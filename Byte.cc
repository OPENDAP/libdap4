
// Implementation for Byte.
//
// jhrg 9/7/94

// $Log: Byte.cc,v $
// Revision 1.3  1994/11/29 20:06:32  jimg
// Added mfuncs for data transmission.
// Made the xdr_coder function pointer xdr_bytes() while (de)serialize() uses
// xdr_char().
// Removed `type' from ctor parameter list.
// Added FILE *in and *out to parameter list (they default to stdin/out).
//
// Revision 1.2  1994/09/23  14:36:06  jimg
// Fixed errors in comments.
//
// Revision 1.1  1994/09/15  21:08:38  jimg
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

#include "Byte.h"

// NB: This ctor sets _xdr_coder to xdr_bytes(). That is the functionthat
// should be used to en/decode arrays or lists of bytes. However, for a
// single byte, we *must* use xdr_char() (which puts a single byte in 4 (yes
// four) bytes). See serialize() and deserialize().

Byte::Byte(const String &n, FILE *in, FILE *out) 
    : BaseType(n, "Byte", xdr_bytes, in, out)
{
}

BaseType *
Byte::ptr_duplicate()
{
    return new Byte(*this);
}

unsigned int
Byte::size()
{
    return sizeof(buf);
}

bool
Byte::serialize(bool flush, unsigned int num)
{
    bool stat = (bool)xdr_char(xdrout, &buf);
    if (stat && flush)
	stat = expunge();

    return stat;
}

// deserialize the double on stdin and put the result in BUF.

unsigned int
Byte::deserialize()
{
    unsigned int num = xdr_char(xdrin, &buf);

    return num;
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Byte::print_val(ostream &os, String space)
{
    print_decl(os, "", false);
    os << " = " << (unsigned int)buf << ";" << endl;
}

