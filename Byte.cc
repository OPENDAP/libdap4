
// Implementation for Byte.
//
// jhrg 9/7/94

// $Log: Byte.cc,v $
// Revision 1.6  1995/02/10 02:22:41  jimg
// Added DBMALLOC includes and switch to code which uses malloc/free.
// Private and protected symbols now start with `_'.
// Added new accessors for name and type fields of BaseType; the old ones
// will be removed in a future release.
// Added the store_val() mfunc. It stores the given value in the object's
// internal buffer.
// Made both List and Str handle their values via pointers to memory.
// Fixed read_val().
// Made serialize/deserialize handle all malloc/free calls (even in those
// cases where xdr initiates the allocation).
// Fixed print_val().
//
// Revision 1.5  1995/01/19  20:05:16  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.4  1995/01/11  15:54:26  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.3  1994/11/29  20:06:32  jimg
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

#include <assert.h>

#include "Byte.h"

// NB: This ctor sets _xdr_coder to xdr_bytes(). That is the function that
// should be used to en/decode arrays or lists of bytes. However, for a
// single byte, we *must* use xdr_char() (which puts a single byte in 4 (yes
// four) bytes). See serialize() and deserialize().

Byte::Byte(const String &n) : BaseType(n, "Byte", xdr_bytes)
{
}

unsigned int
Byte::size()
{
    return sizeof(byte);
}

// Serialize the contents of member BUF and write the result to stdout. NUM
// defaults to zero -- it is used by descendents of CtorType. If FLUSH is
// true, write the contents of the output buffer to the kernel. FLUSH is
// false by default.
//
// NB: See the comment in BaseType re: why we don't use XDR_CODER here

bool
Byte::serialize(bool flush)
{
    bool stat = (bool)xdr_char(_xdrout, &_buf);
    if (stat && flush)
	stat = expunge();

    return stat;
}

// deserialize the char on stdin and put the result in BUF.

unsigned int
Byte::deserialize(bool reuse)
{
    unsigned int num = xdr_char(_xdrin, &_buf);

    return num;
}

// Store the value referenced by VAL in the object's internal buffer. REUSE
// has no effect because this class does not dynamically allocate storage for
// the internal buffer.
//
// Returns: size in bytes of the value's representation.

unsigned int
Byte::store_val(void *val, bool reuse)
{
    assert(val);

    _buf = *(byte *)val;

    return size();
}

unsigned int
Byte::read_val(void **val)
{
    assert(_buf && val);

    if (!*val)
	*val = new byte;

    *(byte *)val = _buf;

    return size();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Byte::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << (unsigned int)_buf << ";" << endl;
    }
    else 
	os << (unsigned int)_buf;
}

