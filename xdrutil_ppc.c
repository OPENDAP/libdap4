/*  The the old-style C function prototypes such as those found in the XDR */
/*  structure in <rpc/xdr.h> under OS X are not usable from C++.  This,    */
/*  along with xdr_destroy() being a macro that expands into that old      */
/*  style ("()" used to mean any number of unspecified args) means that    */
/*  we cannot use delete_xdrstdio from C++ under OS X.  The other two      */
/*  functions were brought along for uniformity.                           */
/*  ROM - 6/22/2003                                                        */

#ifdef __POWERPC__
#include <stdio.h>
#include <rpc/types.h>
#include <rpc/xdr.h>

XDR *
new_xdrstdio(FILE *stream, enum xdr_op xop)
{
    XDR *xdr = (XDR *)malloc(sizeof(XDR));
    
    xdrstdio_create(xdr, stream, xop);
    
    return xdr;
}

XDR *
set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop)
{
    xdrstdio_create(xdr, stream, xop);
    
    return xdr;
}

// Delete an XDR pointer allocated using the above function. Do not close the
// associated FILE pointer.
void
delete_xdrstdio(XDR *xdr)
{
    xdr_destroy(xdr);

    free(xdr);xdr = 0;
}
#endif  /*  __POWERPC__  */
