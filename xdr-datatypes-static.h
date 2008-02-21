/* dods-datatypes.h.  Generated from dods-datatypes.h.in by configure.  */

/*
  Determine at compile-time the sizes of various datatypes. This uses symbols
  defined by configure (See configure.in).
  jhrg 10/24/94

  This header is included by all of the DODS DAP library header files which
  make use of the dods_* typedefs. C or C++ files can either include
  config_dap.h, use their own configure script which defines SIZEOF_LONG,
  _INT, _CHAR and _DOUBLE or set these preprocessor symbols themselves in a
  Makefile, etc.

  This used to be part of the config_dap.h header, but including that in the
  DAP library headers introduced problems when the DAP was used in conjunction
  with other libraries. 8/1/2000 jhrg
*/

#ifndef __XDR_DATATYPES__
#define __XDR_DATATYPES__

#ifdef WIN32
#include <rpc.h>
#include <winsock2.h>
#include <xdr.h>
#else
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

/* The typedefs are done using a preprocessor symbol so that autoconf's
   `CONFIG_HEADER' can be used. The configure script will then only modify
   the dods-datatypes.h header when its contents change. This saves on
   compilation since the header is used by many files in the dap++ library.
   The downside is that the typedefs are so ugly... 2/14/2001 jhrg */

#define XDR_INT32 xdr_long
#define XDR_UINT32 xdr_u_long

#define XDR_INT16 xdr_short
#define XDR_UINT16 xdr_u_short

#define XDR_FLOAT64 xdr_double
#define XDR_FLOAT32 xdr_float

#endif /* __XDR_DATATYPES__ */

