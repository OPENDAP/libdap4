/*  This file is typically generated during configuation      */
/*  time, but that is not the case under win32.  This file    */
/*  was generated under linux on an x86 and then stripped     */
/*  down to the bare essentials and modified where necessary  */
/*  to form something appropriate for win32.                  */

#ifndef __DODS_DATATYPES__
#define __DODS_DATATYPES__

typedef long dods_int32;
typedef unsigned long dods_uint32;
#define XDR_INT32 xdr_long
#define XDR_UINT32 xdr_u_long

typedef short dods_int16;
typedef unsigned short dods_uint16;
#define XDR_INT16 xdr_short
#define XDR_UINT16 xdr_u_short

typedef unsigned char dods_byte;

typedef double dods_float64;
typedef float dods_float32;		
#define XDR_FLOAT64 xdr_double
#define XDR_FLOAT32 xdr_float

#endif /* __DODS_DATATYPES__ */
