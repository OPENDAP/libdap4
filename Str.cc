
// Implementation for Str.
//
// jhrg 9/7/94

// $Log: Str.cc,v $
// Revision 1.13  1995/05/10 15:34:05  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.12  1995/05/10  13:45:30  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.11  1995/03/16  17:26:41  jimg
// Moved include of config_dap.h to top of includes.
// Added TRACE_NEW switched dbnew debugging includes.
//
// Revision 1.10  1995/03/04  14:34:50  jimg
// Major modifications to the transmission and representation of values:
// 	Added card() virtual function which is true for classes that
// 	contain cardinal types (byte, int float, string).
// 	Changed the representation of Str from the C rep to a C++
// 	class represenation.
// 	Chnaged read_val and store_val so that they take and return
// 	types that are stored by the object (e.g., inthe case of Str
// 	an URL, read_val returns a C++ String object).
// 	Modified Array representations so that arrays of card()
// 	objects are just that - no more storing strings, ... as
// 	C would store them.
// 	Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// 	an array of a structure is represented as an array of Structure
// 	objects).
//
// Revision 1.9  1995/02/10  02:22:49  jimg
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

#include "config_dap.h"

#include <assert.h>
#include <string.h>

#include "Str.h"
#include "util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

Str::Str(const String &n) 
    : BaseType(n, "String", (xdrproc_t)xdr_str_array), _buf("")
{
}

// Return: the number of bytes needed to store the string's value or 0 if no
// storage for the string has been allocated.

unsigned int
Str::len()			// deprecated
{
    return length();
}

unsigned int
Str::length()
{
    return _buf.length();
}

bool
Str::card()
{
    return true;
}

unsigned int
Str::size()			// deprecated
{
    return width();
}

// return the number of bytes that the value of a Str object occupies when
// that value is accessed using read_val().

unsigned int
Str::width()
{
    return sizeof(String);
}

// serialize and deserialize manage memory using malloc and free since, in
// some cases, they must let the xdr library (bundled with the CPU) do the
// allocation and that library will always use malloc/free.

bool
Str::serialize(bool flush)
{
    String *tmp = &_buf;	// kluge for xdr_str

    bool stat = (bool)xdr_str(_xdrout, &tmp);
    if (stat && flush)
	stat = expunge();

    return stat;
}

// deserialize the double on stdin and put the result in BUF.

bool
Str::deserialize(bool reuse)
{
    String *tmp = &_buf;

    return (bool)xdr_str(_xdrin, &tmp);
}

// FIXME old comments
// Copy information in the object's internal buffers into the memory pointed
// to by VAL. If *VAL is null, then allocate memory for the value (a string
// in this case).
//
// NB: return the size of the thing val points to (sizeof val), not the
// length of the string. Thus if there is an array of of strings (i.e., (char
// *)s), then the return value of this mfunc can be used to advance to the
// next char * in that array. This weirdness is needed because C programs
// which will need to interface to libraries built using this toolkit will
// not know about g++ Strings and will need to use the C representation for
// strings, but here in the toolkit I use the String class to cut down on
// memory management problems.

unsigned int
Str::read_val(void **val)
{
    assert(val);

    if (!*val) 
	(String *)*val = new String(_buf);
    else
	*(String *)*val = _buf;

    return sizeof(String);
}

// Copy data in VAL to _BUF.
//
// Returns the number of bytes needed for _BUF (which is a pointer in this
// case). 

unsigned int
Str::store_val(void *val, bool reuse)
{
    assert(val);

    _buf = *(String *)val;

    return sizeof(String);
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
