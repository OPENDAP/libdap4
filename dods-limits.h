
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Limits for DODS. Use these *instead* of <limits.h> since DODS needs to see
// the same values on all machines.

// $Log: dods-limits.h,v $
// Revision 1.6  1998/09/10 18:49:48  jehamby
// Correct DODS_INT_MIN and DODS_LONG_MIN values
//
// Revision 1.5  1998/08/13 21:18:04  jimg
// Changed DODS_CHAR_MAX to 255. Bytes are supposed to be unsigned in DODS.
//
// Revision 1.4  1998/02/05 20:14:02  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.3  1996/06/07 17:15:17  jimg
// Changed *_MIN limits so that they are in line with values in Harbison &
// Steele (which makes them one less in absolute value than the values in Sun's
// header). This fixes a problem when comparing signed int values to
// DODS_INT_MIN. Also changed the values to be base 10 (from base 16).
//
// Revision 1.2  1996/05/31 23:30:57  jimg
// Updated copyright notice.
//
// Revision 1.1  1996/02/05 19:02:48  jimg
// Added. Built using the SunOS 4.1.3 limits.h file.
//

#define	DODS_CHAR_BIT		 8
#define	DODS_SCHAR_MIN		 -127
#define	DODS_SCHAR_MAX		 127
#define	DODS_UCHAR_MAX		 (unsigned char)255
#define	DODS_CHAR_MIN		 0
#define	DODS_CHAR_MAX		 (unsigned char)255
#define	DODS_SHRT_MIN		 -32767
#define	DODS_SHRT_MAX		 32767
#define	DODS_USHRT_MAX		 (unsigned short)65535
#define	DODS_INT_MIN		 (-2147483647 - 1)
#define	DODS_INT_MAX		 2147483647
#define	DODS_UINT_MAX		 (unsigned int)4294967295
#define	DODS_LONG_MIN		 (-2147483647 - 1)
#define	DODS_LONG_MAX		 2147483647
#define	DODS_ULONG_MAX		 (unsigned long)4294967295

#define	DODS_MB_LEN_MAX		 4

