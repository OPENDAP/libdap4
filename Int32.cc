
// Implementation for Int32.
//
// jhrg 9/7/94

// $Log: Int32.cc,v $
// Revision 1.11  1995/03/16 17:26:40  jimg
// Moved include of config.h to top of includes.
// Added TRACE_NEW switched dbnew debugging includes.
//
// Revision 1.10  1995/03/04  14:34:46  jimg
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
// Revision 1.9  1995/02/10  02:22:45  jimg
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
// Revision 1.8  1995/01/19  20:05:17  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.7  1995/01/11  15:54:29  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.6  1994/12/09  21:35:59  jimg
// Used the XDR_INT32 and XDR_FLOAT64 symbols defined in config.h.
//
// Revision 1.5  1994/12/07  21:23:18  jimg
// Changed from xdr_long to XDR_INT32 (defined in config.h by configure)
//
// Revision 1.4  1994/11/29  20:10:36  jimg
// Added functions for data transmission.
// Added boolean parameter to serialize which, when true, causes the output
// buffer to be flushed. The default value is false.
// Added FILE *in and *out parameters to the ctor. The default values are
// stdin/out.
// Removed the `type' parameter from the ctor.
//
// Revision 1.3  1994/09/23  14:36:08  jimg
// Fixed errors in comments.
//
// Revision 1.2  1994/09/15  21:08:44  jimg
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

#include "config.h"

#include <assert.h>

#include "Int32.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

Int32::Int32(const String &n) : BaseType(n, "Int32", XDR_INT32)
{
}

bool
Int32::card()
{
    return true;
}

// deprecated

unsigned int
Int32::size()
{
    return width();
}

unsigned int
Int32::width()
{
    return sizeof(int32);
}

bool
Int32::serialize(bool flush)
{
    bool stat = (bool)xdr_long(_xdrout, &_buf);
    if (stat && flush)
	stat = expunge();

    return stat;
}

bool
Int32::deserialize(bool reuse)
{
    unsigned int num = xdr_long(_xdrin, &_buf);

    return num;
}

unsigned int
Int32::store_val(void *val, bool reuse)
{
    assert(val);

    _buf = *(int32 *)val;

    return size();
}

unsigned int
Int32::read_val(void **val)
{
    assert(_buf && val);

    if (!*val)
	*val = new int32;

    *(int32 *)*val =_buf;

    return size();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Int32::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}
