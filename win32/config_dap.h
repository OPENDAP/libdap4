#ifndef _DODS_CONFIG_H_
#define _DODS_CONFIG_H_

/* Wrap the defines in this section with HAVE_CONFIG_H so that
   libraries/servers that define these symbols on the command line won't
   redefine them when/if this file is included in their code. */

/* #undef HAVE_CONFIG_H */

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Client application name */
#define CNAME "DODS"

/* Client Application version */
#define CVER "$VERSION"

/* DODS Version */
#define DVR "DODS/3.2"

/* If defined, enable memory leak detection through libdbnew.a. Make sure to */
/* link with that library. */
/* #undef TRACE_NEW */

/* If defined, enable memory leak detection through libdbmalloc.a. Nominally */
/* not necessary with c++, but useful with the DODS library because it (and  */
/* xdr) make use of malloc/free. */
/* #undef DBMALLOC */

/* If defined, the DBG() macro defined in debug.h is activated. This macro */
/* is used for nominal program instrumentation */
/* #undef DEBUG */

/* If defined, the DBG2() macro defined in debug.h is activated. This macro */
/* is used for detailed program instrumentation. Anything that prints half a */
/* page or more of stuff every time it is executed should be inside DBG2(), */
/* not DBG(). */
/* #undef DEBUG2 */

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a double.  */
#define SIZEOF_DOUBLE 8

/* The number of bytes in a float.  */
#define SIZEOF_FLOAT 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* Define as 1 if you have the expect library (which requries tcl) */
#ifndef HAVE_EXPECT
/* #undef HAVE_EXPECT */
#endif

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <unistd.h> header file.  */
/* #define HAVE_UNISTD_H 1 */

/* Where is DODS_ROOT? This can be changed at runtime by an environment
   variable. */
#ifndef DODS_ROOT
#ifdef WIN32
#define DODS_ROOT "C:\\DODS"
#endif
#endif

/* Does <sys/sem.h> contain prototypes for semctl(), ... ? */
#define HAVE_SEM_PROTO 1

/* Does <sys/sem.h> contain the definition of union semun {}? */
#define HAVE_SEM_UNION 0

/* What version of Bison are we using */
#define DODS_BISON_VER 127

/* XXX */
/* #endif  */ /* HAVE_CONFIG_H */

/* GNU gcc/g++ provides a way to mark variables, etc. as unused */

#if defined(__GNUG__) || defined(__GNUC__)
#define not_used __attribute__ ((unused))
#else
#define not_used 
#endif

/*
  Determine at compile-time the sizes of various datatypes. This uses symbols
  defined by configure (See configure.in).
  jhrg 10/24/94
*/

#ifndef __DODS_DATATYPES__
#define __DODS_DATATYPES__

#if SIZEOF_LONG == 4
typedef long dods_int32;
typedef unsigned long dods_uint32;
#define XDR_INT32 xdr_long
#define XDR_UINT32 xdr_u_long
#elif SIZEOF_INT == 4
typedef int dods_int32;
typedef unsigned int dods_uint32;
#define XDR_INT32 xdr_int
#define XDR_UINT32 xdr_u_int
#else
#error "How do I get a 32 bit integer on this machine?"
#endif

/* Assume short is 16 bits... */
typedef short dods_int16;
typedef unsigned short dods_uint16;
#define XDR_INT16 xdr_short
#define XDR_UINT16 xdr_u_short

#if SIZEOF_CHAR == 1
typedef unsigned char dods_byte;
#else
#error "How do I get an 8 bit unsigned integer on this machine?"
#endif

#if SIZEOF_DOUBLE == 8
typedef double dods_float64;
typedef float dods_float32;		
#define XDR_FLOAT64 xdr_double
#define XDR_FLOAT32 xdr_float
#else
#error "How do I get a 64 bit floating point type on this machine?"
#endif

#endif /* __DODS_DATATYPES__ */

#endif /* _DODS_CONFIG_H_ */


