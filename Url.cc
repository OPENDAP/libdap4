
// Implementation for Url.
//
// jhrg 9/7/94

// $Log: Url.cc,v $
// Revision 1.4  1995/01/11 15:54:37  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.3  1994/11/29  20:16:35  jimg
// Added mfunc for data transmission.
// Uses special xdr function for serialization and xdr_coder.
// Removed `type' parameter from ctor.
// Added FILE *in and *out to ctor parameter list.
//
// Revision 1.2  1994/09/23  14:36:15  jimg
// Fixed errors in comments.
//
// Revision 1.1  1994/09/15  21:08:50  jimg
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

#include "Url.h"
#include "util.h"

Url::Url(const String &n) : BaseType(n, "Url", (xdrproc_t)xdr_url)
{
    _buf = 0;
}

BaseType *
Url::ptr_duplicate()
{
    return new Url(*this);
}

unsigned int
Url::size()
{
    return sizeof(_buf);
}

bool
Url::serialize(bool flush, unsigned int num)
{
    bool stat = (bool)xdr_url(_xdrout, &_buf);
    if (stat && flush)
	stat = expunge();

    return stat;
}

// deserialize the double on stdin and put the result in BUF.

unsigned int
Url::deserialize()
{
    unsigned int num = xdr_url(_xdrin, &_buf);

    return num;
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Url::print_val(ostream &os, String space)
{
    print_decl(os, "", false);
    os << " = " << _buf << ";" << endl;
}
