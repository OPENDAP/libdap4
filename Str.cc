
// Implementation for Str.
//
// jhrg 9/7/94

// $Log: Str.cc,v $
// Revision 1.9  1995/02/10 02:22:49  jimg
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
// Revision 1.8  1995/01/19  20:05:19  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.7  1995/01/11  15:54:33  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.6  1994/12/22  04:32:23  reza
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

#include <assert.h>
#include <string.h>
#ifdef DBMALLOC
#include <stdlib.h>
#include <dbmalloc.h>
#endif

#include "Str.h"
#include "util.h"

Str::Str(const String &n) : BaseType(n, "String", (xdrproc_t)xdr_str)
{
    _buf = 0;			// read() frees if buf != 0
}

// Return: the number of bytes needed to store the string's value or 0 if no
// storage for the string has been allocated.

unsigned int
Str::len()
{
    return _buf ? strlen(_buf): 0;
}

// Return: The number of bytes needed to store a Str (which is represented
// as a pointer to a char). This is the number of bytes which must be
// allocated to hold the value of _BUF. The mfunc len() contains the number
// of bytes needed to store the string pointed to by _BUF.

unsigned int
Str::size()
{
    return sizeof(char *);
}

// serialize and deserialize manage memory using malloc and free since, in
// some cases, they must let the xdr library (bundled with the CPU) do the
// allocation and that library will always use malloc/free.

bool
Str::serialize(bool flush)
{
    bool stat = (bool)xdr_str(_xdrout, &_buf);
    if (stat && flush)
	stat = expunge();

    return stat;
}

// deserialize the double on stdin and put the result in BUF.

unsigned int
Str::deserialize(bool reuse)
{
    if (_buf && !reuse) {
	free(_buf);
	_buf = 0;
    }

    unsigned int num = xdr_str(_xdrin, &_buf);

    return num;
}

// Copy information in the object's internal buffers into the memory pointed
// to by VAL. If *VAL is null, then allocate memory for the value (a string
// in this case). 
//
// NB: return the size of the thing val points to (sizeof val), not the
// length of the string. Thus if there is an array of of strings (char *)s,
// then the return value of this mfunc can be used to advance to the next
// char * in that array.

unsigned int
Str::read_val(void **val)
{
    assert(_buf && val);

    if (!*val) 
	*val = new char[strlen((char *)_buf) + 1];

    strcpy(*(char **)val, (char *)_buf);

    return size();		// the same as sizeof (char *)
}

// Copy data in VAL to _BUF.
//
// Returns the number of bytes needed for _BUF (which is a pointer in this
// case). 

unsigned int
Str::store_val(void *val, bool reuse)
{
    assert(val);

    if (_buf && !reuse) {
	free(_buf);
	_buf = 0;
    }

    if (!_buf) {
	_buf = strdup((char *)val); // allocate new memory
    }
    else {
	unsigned int len = strlen(_buf); // _buf might be bigger...
	strncpy(_buf, (char *)val, len);
	*(_buf + len) = '\0';	// strlen won't supply a \0 above
    }

    return size();
}

void 
Str::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}
