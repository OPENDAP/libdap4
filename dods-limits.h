
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Limits for DODS. Use these *instead* of <limits.h> since DODS needs to see
// the same values on all machines.

// $Log: dods-limits.h,v $
// Revision 1.2  1996/05/31 23:30:57  jimg
// Updated copyright notice.
//
// Revision 1.1  1996/02/05 19:02:48  jimg
// Added. Built using the SunOS 4.1.3 limits.h file.
//

#define	DODS_CHAR_BIT		 0x8
#define	DODS_SCHAR_MIN		-0x80
#define	DODS_SCHAR_MAX		 0x7F
#define	DODS_UCHAR_MAX		 0xFF
#define	DODS_CHAR_MIN		-0x80
#define	DODS_CHAR_MAX		 0x7F
#define	DODS_SHRT_MIN		-0x8000
#define	DODS_SHRT_MAX		 0x7FFF
#define	DODS_USHRT_MAX		 0xFFFF
#define	DODS_INT_MIN		-0x80000000
#define	DODS_INT_MAX		 0x7FFFFFFF
#define	DODS_UINT_MAX		 0xFFFFFFFF
#define	DODS_LONG_MIN		-0x80000000
#define	DODS_LONG_MAX		 0x7FFFFFFF
#define	DODS_ULONG_MAX		 0xFFFFFFFF
#define	DODS_MB_LEN_MAX		4

