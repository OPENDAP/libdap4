/*  This file is typically generated during configuation      */
/*  time, but that is not the case under win32.  This file    */
/*  was generated under linux on an x86 and then stripped     */
/*  down to the bare essentials and modified where necessary  */
/*  to form something appropriate for win32.                  */

#ifndef __DODS_DATATYPES__
#define __DODS_DATATYPES__

#include <xdr.h>

#define DINT32 long
typedef DINT32 dods_int32;

#define DUINT32 unsigned long
typedef DUINT32 dods_uint32;

#define XDR_INT32 xdr_long
#define XDR_UINT32 xdr_u_long

#define DINT16 short
typedef DINT16 dods_int16;

#define DUINT16 unsigned short
typedef DUINT16 dods_uint16;

#define XDR_INT16 xdr_short
#define XDR_UINT16 xdr_u_short

#define DBYTE unsigned char
typedef DBYTE dods_byte;

#define DFLOAT64 double
typedef DFLOAT64 dods_float64;

#define DFLOAT32 float
typedef DFLOAT32 dods_float32;

#define XDR_FLOAT64 xdr_double
#define XDR_FLOAT32 xdr_float

#endif /* __DODS_DATATYPES__ */
