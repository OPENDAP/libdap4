
// Implementation for Float64.
//
// jhrg 9/7/94

// $Log: Float64.cc,v $
// Revision 1.3  1994/11/22 14:05:46  jimg
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

Float64::Float64(const String &n, const String &t) 
    : BaseType(n, t, xdr_double)
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
// defaults to zero -- it is used by descendents of CtorType.
// NB: See the comment in BaseType re: why we don't use XDR_CODER here

bool
Float64::serialize(unsigned int num)
{
    return (bool)xdr_double(xdrout, &buf);
}

// deserialize the double on stdin and put the result in BUF.

unsigned int
Float64::deserialize()
{
    bool status = xdr_double(xdrin, &buf);

    return status ? size(): 0;
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Float64::print_val(ostream &os, String space)
{
    print_decl(os, "", false);
    os << " = " << buf << ";" << endl;
}

