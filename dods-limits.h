
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1996,1998,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Limits for DODS. Use these *instead* of <limits.h> since DODS needs to see
// the same values on all machines.

#ifndef _dods_limits_h
#define _dods_limits_h

#define	DODS_CHAR_BIT	8
#define	DODS_SCHAR_MIN	-128
#define	DODS_SCHAR_MAX	127
#define	DODS_UCHAR_MAX	255U
#define	DODS_UCHAR_MIN	0

#define	DODS_SHRT_MIN	-32768
#define	DODS_SHRT_MAX	32767
#define	DODS_USHRT_MAX	65535U

#define	DODS_INT_MIN	(-2147483647 - 1)
#define	DODS_INT_MAX	2147483647
#define	DODS_UINT_MAX	4294967295U

#define	DODS_LONG_MIN	(-2147483647 - 1)
#define	DODS_LONG_MAX	2147483647
#define	DODS_ULONG_MAX	4294967295UL

#define	DODS_DBL_DIG	15	/* digits of precision of a "double" */
#define	DODS_DBL_MAX	1.7976931348623157E+308	/* max decimal value of a */
						/* "double" */
#define	DODS_DBL_MIN	2.2250738585072014E-308	/* min decimal value of a */
						/* "double" */
#define	DODS_FLT_DIG	6		/* digits of precision of a "float" */
#define	DODS_FLT_MAX	3.402823466E+38F  /* max decimal value of a "float" */
#define	DODS_FLT_MIN	1.175494351E-38F  /* min decimal value of a "float" */

#define	DODS_MB_LEN_MAX	4

// $Log: dods-limits.h,v $
// Revision 1.16  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.15  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.14  2003/01/15 19:28:07  jimg
// Removed useless casts associated with the unsigned types.
//
// Revision 1.13  2002/06/03 21:45:41  jimg
// Added `U' suffix to the numerical constants that are unsigned. This
// prevents messages about values being so large that they can only be
// represented as unsigned values.
//
// Revision 1.12  2001/01/26 19:48:09  jimg
// Merged with release-3-2-3.
//
// Revision 1.11.4.1  2000/11/30 05:24:46  jimg
// Significant changes and improvements to the AttrTable and DAS classes. DAS
// now is a child of AttrTable, which makes attributes behave uniformly at
// all levels of the DAS object. Alias now work. I've added unit tests for
// several methods in AttrTable and some of the functions in parser-util.cc.
// In addition, all of the DAS tests now work.
//
// Revision 1.11  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.10  2000/03/31 21:03:36  jimg
// Merged with version 3.1.5.
//
// Revision 1.8.6.2  2000/03/20 19:20:33  jimg
// I removed DODS_CHAR_MAX since we have DODS_UCHAR_MAX and it was the same
// value. I changed DODS_CHAR_MIN to DODS_UCHAR_MIN.
//
// Revision 1.9  1999/09/03 22:07:45  jimg
// Merged changes from release-3-1-1
//
// Revision 1.8.6.1  1999/08/28 06:42:10  jimg
// Fixed the value of DODS_SHRT_MIN
//
// Revision 1.1  1999/08/23 21:25:48  jimg
// Initial revision
//
// Revision 1.8  1999/05/04 19:47:24  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.7  1999/03/19 21:42:07  jimg
// Added constants for floating point values. Changed some formatting.
//
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

#endif // _dods_limits_h
