
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

#ifndef XDR_INT32
#define XDR_INT32 xdr_int32_t
#endif

#ifndef XDR_UINT32
#define XDR_UINT32 xdr_uint32_t
#endif

#ifndef XDR_INT16
#define XDR_INT16 xdr_int16_t
#endif

#ifndef XDR_UINT16
#define XDR_UINT16 xdr_uint16_t
#endif

#ifndef XDR_FLOAT64
#define XDR_FLOAT64 xdr_double
#endif

#ifndef XDR_FLOAT32
#define XDR_FLOAT32 xdr_float
#endif

#endif /* __XDR_DATATYPES__ */

