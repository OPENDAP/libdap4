
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// These functions are utility functions used by the various DAP parsers (the
// DAS, DDS and constraint expression parsers). 
// jhrg 9/7/95

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: parser-util.cc,v 1.32 2003/04/22 19:40:29 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <errno.h>

#include <iostream>
#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif

#include "debug.h"
#include "parser.h"		// defines constants such as ID_MAX
#include "dods-limits.h"
#include "util.h"		// Jose Garcia: for append_long_to_string.

using std::cerr;
using std::endl;

// Deprecated, but still used by the HDF4 EOS server code. 
void
parse_error(parser_arg *arg, const char *msg, const int line_num,
	    const char *context)
{ 
    // Jose Garcia
    // This assert(s) is (are) only for developing purposes
    // For production servers remove it by compiling with NDEBUG
    assert(arg);
    assert(msg);

    arg->set_status(FALSE);

    string oss="";

    if (line_num != 0)
      {
	oss+= "Error parsing the text on line " ;
	append_long_to_string(line_num,10,oss);
      }
    else 
      {
	oss+= "Parse error." ;
      }

    if (context)
	oss+= (string)" at or near: " + context + (string)"\n" + msg 
	  + (string)"\n";
    else
	oss+= (string)"\n" + msg + (string)"\n";

    arg->set_error(new Error(unknown_error, oss.c_str()));
}

void
parse_error(const char *msg, const int line_num, const char *context)
    throw (Error)
{ 
    // Jose Garcia
    // This assert(s) is (are) only for developing purposes
    // For production servers remove it by compiling with NDEBUG
    assert(msg);

    string oss="";

    if (line_num != 0)
      {
	oss+= "Error parsing the text on line " ;
	append_long_to_string(line_num,10,oss);
      }
    else 
      {
	oss+= "Parse error." ;
      }

    if (context)
	oss+= (string)" at or near: " + context + (string)"\n" + msg 
	  + (string)"\n";
    else
	oss+= (string)"\n" + msg + (string)"\n";

    throw Error(oss);
}

// context comes from the parser and will always be a char * unless the
// parsers change dramatically.
void
parse_error(const string &msg, const int line_num, const char *context)
    throw (Error)
{
    parse_error(msg.c_str(), line_num, context);
}

void
save_str(char *dst, const char *src, const int line_num) throw (Error)
{
    if (strlen(src) >= ID_MAX)
	parse_error(string("The word `") + string(src) 
		    + string("' is too long (it should be no longer than ")
		    + long_to_string(ID_MAX) + string(")."), line_num);

    strncpy(dst, src, ID_MAX);
    dst[ID_MAX-1] = '\0';		/* in case ... */
}

void
save_str(string &dst, const char *src, const int)
{
    dst = src;
}

bool
is_keyword(string id, const string &keyword)
{
    downcase(id);
    id = prune_spaces(id);
    DBG(cerr << "is_keyword: " << keyword << " = " << id << endl);
    return id == keyword;
}

int
check_byte(const char *val)
{
    char *ptr;
    long v = strtol(val, &ptr, 0);
    
    if ((v == 0 && val == ptr) || *ptr != '\0') {
	return FALSE;
    }
    
    DBG(cerr << "v: " << v << endl);
    
    // We're very liberal here with values. Anything that can fit into 8 bits
    // is allowed through. Clients will have to deal with the fact that the
    // ASCII representation for the value might need to be tweaked. This is
    // especially the case for Java clients where Byte datatypes are
    // signed. 3/20/2000 jhrg
    if ((v < 0 && v < DODS_SCHAR_MIN)
	|| v > 0 && static_cast<unsigned long>(v) > DODS_UCHAR_MAX)
	return FALSE;

    return TRUE;
}

// This version of check_int will pass base 8, 10 and 16 numbers when they
// use the ANSI standard for string representation of those number bases.

int
check_int16(const char *val)
{
    char *ptr;
    long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if ((v == 0 && val == ptr) || *ptr != '\0') {
	return FALSE;
    }

    // Don't use the constant from limits.h, use the ones in dods-limits.h
    if (v > DODS_SHRT_MAX || v < DODS_SHRT_MIN) { 
	return FALSE;
    }

    return TRUE;
}

int
check_uint16(const char *val)
{
    char *ptr;
    unsigned long v = strtol(val, &ptr, 0); 

    if ((v == 0 && val == ptr) || *ptr != '\0') {
	return FALSE;
    }

    if (v > DODS_USHRT_MAX) { 
	return FALSE;
    }

    return TRUE;
}

int
check_int32(const char *val)
{
    char *ptr;
    long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if ((v == 0 && val == ptr) || *ptr != '\0') {
	return FALSE;
    }

    if (v > DODS_INT_MAX || v < DODS_INT_MIN) { 
	return FALSE;
    }

    return TRUE;
}

int
check_uint32(const char *val)
{
    char *ptr;
    unsigned long v = strtol(val, &ptr, 0);

    if ((v == 0 && val == ptr) || *ptr != '\0') {
	return FALSE;
    }

    return TRUE;
}

// Check first for system errors (like numbers so small they convert
// (erroneously) to zero. Then make sure that the value is within DODS'
// limits. 

int
check_float32(const char *val)
{
    char *ptr;
    errno = 0;			// Clear previous value. Fix for the 64bit
				// IRIX from Rob Morris. 5/21/2001 jhrg
    double v = strtod(val, &ptr);

    DBG(cerr << "v: " << v << ", ptr: " << ptr
	<< ", errno: " << errno << ", val==ptr: " << (val==ptr) << endl);
    if ((v == 0.0 && (val == ptr || errno == HUGE_VAL || errno == ERANGE))
	|| *ptr != '\0') {
	return FALSE;
    }

    DBG(cerr << "fabs(" << val << ") = " << fabs(v) << endl);
    double abs_val = fabs(v);
    if (abs_val > DODS_FLT_MAX || (abs_val != 0.0 && abs_val < DODS_FLT_MIN))
	return FALSE;

    return TRUE;
}

int
check_float64(const char *val)
{
    DBG(cerr << "val: " << val << endl);
    char *ptr;
    errno = 0;			// Clear previous value. 5/21/2001 jhrg
    double v = strtod(val, &ptr);
    DBG(cerr << "v: " << v << ", ptr: " << ptr 
	<< ", errno: " << errno << ", val==ptr: " << (val==ptr) << endl);

    if ((v == 0.0 && (val == ptr || errno == HUGE_VAL || errno == ERANGE))
	|| *ptr != '\0') {
	return FALSE;
    }

    DBG(cerr << "fabs(" << val << ") = " << fabs(v) << endl);
    double abs_val = fabs(v);
    if (abs_val > DODS_DBL_MAX || (abs_val != 0.0 && abs_val < DODS_DBL_MIN))
	return FALSE;

    return TRUE;
}

/*
  Maybe someday we will really check the Urls to see if they are valid...
*/

int
check_url(const char *)
{
    return TRUE;
}

// $Log: parser-util.cc,v $
// Revision 1.32  2003/04/22 19:40:29  jimg
// Merged with 3.3.1.
//
// Revision 1.31  2003/04/02 19:13:12  pwest
// Fixed bug in parser-util that did not recognize illegal characters when
// checking float32 and float64 strings, added tests to parserUtilTest to
// test for illegal characters as in das-test, updated das-test as in 3.3
// version.
//
// Revision 1.30  2003/03/04 23:19:37  jimg
// Fixed some of the unit tests.
//
// Revision 1.29  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.28.2.1  2003/02/21 00:10:08  jimg
// Repaired copyright.
//
// Revision 1.28  2003/01/23 00:22:25  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.27  2003/01/15 19:29:22  jimg
// Added a cast to unsigned in check_byte().
//
// Revision 1.26  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.21.4.6  2002/12/01 14:37:52  rmorris
// Smalling changes for the win32 porting and maintenance work.
//
// Revision 1.25  2002/06/03 22:21:16  jimg
// Merged with release-3-2-9
//
// Revision 1.21.4.5  2001/11/01 00:43:52  jimg
// Fixes to the scanners and parsers so that dataset variable names may
// start with digits. I've expanded the set of characters that may appear
// in a variable name and made it so that all except `#' may appear at
// the start. Some characters are not allowed in variables that appear in
// a DDS or CE while they are allowed in the DAS. This makes it possible
// to define containers with names like `COARDS:long_name.' Putting a colon
// in a variable name makes the CE parser much more complex. Since the set
// of characters that people want seems pretty limited (compared to the
// complete ASCII set) I think this is an OK approach. If we have to open
// up the expr.lex scanner completely, then we can but not without adding
// lots of action clauses to teh parser. Note that colon is just an example,
// there's a host of characters that are used in CEs that are not allowed
// in IDs.
//
// Revision 1.24  2001/08/24 17:46:23  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.21.4.4  2001/08/18 00:00:32  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.21.4.3  2001/06/23 00:47:51  jimg
// Added is_keyword(). This is used to test an ID to see if it's a keyword.
// This was added because the dds.y grammar had to be changed when `:' was
// added to the set of characters allowed in ID names. See dds.y.
//
// Revision 1.23  2001/06/15 23:49:04  jimg
// Merged with release-3-2-4.
//
// Revision 1.21.4.2  2001/05/21 18:08:51  jimg
// Set errno to zero before the calls to strtod in check_float32 and
// check_float64. This fixes a bug that shows up n the 64bit IRIX (reported by
// Rob Morris).
//
// Revision 1.22  2001/01/26 19:48:10  jimg
// Merged with release-3-2-3.
//
// Revision 1.21.4.1  2000/11/30 05:24:46  jimg
// Significant changes and improvements to the AttrTable and DAS classes. DAS
// now is a child of AttrTable, which makes attributes behave uniformly at
// all levels of the DAS object. Alias now work. I've added unit tests for
// several methods in AttrTable and some of the functions in parser-util.cc.
// In addition, all of the DAS tests now work.
//
// Revision 1.21  2000/09/22 02:17:23  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.20  2000/09/21 16:22:10  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.19  2000/07/09 22:05:37  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.18  2000/06/07 18:07:01  jimg
// Merged the pc port branch
//
// Revision 1.17.4.1  2000/06/02 18:39:04  rmorris
// Mod's for port to win32.
//
// Revision 1.17  2000/03/31 21:07:04  jimg
// Merged with release-3-1-5
//
// Revision 1.16.6.2  2000/03/31 18:02:43  jimg
// Removed old code
//
// Revision 1.16.6.1  2000/03/20 19:25:46  jimg
// I changed check_byte to allow bytes to have values between -128 and 255 even
// though that range is not possible for a single instance. This test now
// assumes that the client will determine whether the byte is signed or not. I
// think that this is OK since most clients getting byte data are making
// assumptions about the organization of those bytes.
//
// Revision 1.16  1999/04/29 02:29:37  jimg
// Merge of no-gnu branch
//
// Revision 1.15  1999/04/22 22:31:11  jimg
// Comments
//
// Revision 1.14  1999/03/29 17:35:50  jimg
// Fixed (I hope) a bug in check_float{32,64} where 0.0 did not check out as a
// valid floating point number. Note that the DODS_{FLT,DBL}_{MIN,MAX}
// constants are the absolute values of the bigest and smallest numbers
// representable, unlike the similar constants for integer types.
//
// Revision 1.13  1999/03/24 23:29:35  jimg
// Added support for the new Int16, UInt16 and Float32 types.
// Removed unused error printing functions.
//
// Revision 1.16.14.1  2000/03/08 00:09:04  jgarcia
// replace ostrstream with string;added functions to convert from double and long to string
//
// Revision 1.12.6.2  1999/02/05 09:32:36  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.12.6.1  1999/02/02 21:57:08  jimg
// String to string version
//
// Revision 1.12  1998/03/19 23:28:42  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.11  1998/02/05 20:14:05  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.10  1997/02/28 01:20:51  jimg
// Removed `parse error' message from parse_error() function.
//
// Revision 1.9  1996/10/28 23:05:54  jimg
// Fixed tests in check_uint().
// NB: strtol() does not check for overflow on SunOS.
//
// Revision 1.8  1996/10/28 18:53:13  jimg
// Added functions to test unsigned integers.
//
// Revision 1.7  1996/08/13 20:43:38  jimg
// Added not_used to definition of char rcsid[].
// Added a new parse_error function that builds an Error object and returns it
// instead of printing to stderr.
// Added versions of check_*() that take parser_arg and a context string. These
// call the new parse_error() function.
//
// Revision 1.6  1996/06/14 23:30:33  jimg
// Added `<< ends' to the lines where ostrstream objects are used (without this
// there is no null added to the end of the streams).
//
// Revision 1.5  1996/06/08 00:12:25  jimg
// Improved error messages in some of the type checking functions.
//
// Revision 1.4  1996/05/31 23:31:05  jimg
// Updated copyright notice.
//
// Revision 1.3  1996/05/06 18:33:24  jimg
// Replaced calls to atoi with calls to strtol.
//
// Revision 1.2  1996/05/04 00:07:33  jimg
// Fixed a bug where Float attributes with the value 0.0 were considered `bad
// values'.
//
// Revision 1.1  1996/04/04 22:12:19  jimg
// Added.
