// XDRUtils.h

#ifndef S_XDRUtils_h
#define S_XDRUtils_h 1

#include <stdio.h>
#include "xdr-datatypes.h"
#include "BaseType.h"

#define xdr_proc_t int *

//  These func's moved to xdrutil_ppc.* under the PPC as explained there
#ifdef __POWERPC__
extern "C" XDR *new_xdrstdio(FILE *stream, enum xdr_op xop);
extern "C" XDR *set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop);
extern "C" void delete_xdrstdio(XDR *xdr);
#else
XDR *new_xdrstdio(FILE *stream, enum xdr_op xop);
XDR *set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop);
void delete_xdrstdio(XDR *xdr);
#endif

extern "C" bool_t xdr_str(XDR *xdrs, string &buf);

class XDRUtils
{
private:			XDRUtils() {}
public:
    // xdr_coder is used as an argument to xdr procedures that encode groups
    // of things (e.g., xdr_array()). Each leaf class's constructor must set
    // this.
    static xdrproc_t		xdr_coder( const Type &t ) ;
} ;

#endif // S_XDRUtils_h

