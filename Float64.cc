
// Implementation for Float64.
//
// jhrg 9/7/94

// $Log: Float64.cc,v $
// Revision 1.5  1994/12/09 21:35:58  jimg
// Used the XDR_INT32 and XDR_FLOAT64 symbols defined in config.h.
//
// Revision 1.4  1994/11/29  20:10:39  jimg
// Added functions for data transmission.
// Added boolean parameter to serialize which, when true, causes the output
// buffer to be flushed. The default value is false.
// Added FILE *in and *out parameters to the ctor. The default values are
// stdin/out.
// Removed the `type' parameter from the ctor.
//
// Revision 1.3  1994/11/22  14:05:46  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.2  1994/09/23  14:36:10  jimg
// Fixed errors in comments.
//
// Revision 1.1  1994/09/15  21:08:40  jimg
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

#include <rpc/xdr.h>

#include "Float64.h"

Float64::Float64(const String &n, FILE *in, FILE *out) 
    : BaseType(n, "Float64", XDR_FLOAT64, in, out)
{
}

BaseType *
Float64::ptr_duplicate()
{
    return new Float64(*this);	// Copy ctor calls duplicate to do the work
}
 
unsigned int
Float64::size()
{
    return sizeof(buf);
}

// Serialize the contents of member BUF and write the result to stdout. NUM
// defaults to zero -- it is used by descendents of CtorType. If FLUSH is
// true, write the contents of the output buffer to the kernel. FLUSH is
// false by default.
//
// NB: See the comment in BaseType re: why we don't use XDR_CODER here

bool
Float64::serialize(bool flush, unsigned int num)
{
    bool stat = (bool)xdr_double(xdrout, &buf);

    if (stat && flush)
	 stat = expunge();

    return stat;
}

// deserialize the double on stdin and put the result in BUF.

unsigned int
Float64::deserialize()
{
    unsigned int num = xdr_double(xdrin, &buf);

    return num;
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Float64::print_val(ostream &os, String space)
{
    print_decl(os, "", false);
    os << " = " << buf << ";" << endl;
}

