
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// These functions are utility functions used by the various DAP parsers (the
// DAS, DDS and constraint expression parsers). 
// jhrg 9/7/95

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: parser-util.cc,v 1.21 2000/09/22 02:17:23 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <iostream>
#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif

#include "parser.h"		// defines constants such as ID_MAX
#include "dods-limits.h"
// Jose Garcia: we need to include this to get the prototype for append_long_to_string.
#include "util.h"

#ifdef WIN32
using std::cerr;
using std::endl;
using std::ends;
using std::ostrstream;
#endif

// I think we should get rid of this function since it is writing to cerr.
// 9/21/2000 jhrg
void 
parse_error(const char *s, const int line_num)
{
    // Jose Garcia
    // This assert(s) is (are) only for developing purposes
    // For production servers remove it by compiling with NDEBUG 
    assert(s);

    cerr << "Parse error (line: " << line_num << "):" << endl
	 << s << endl;
}

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
save_str(char *dst, const char *src, const int line_num)
{
    strncpy(dst, src, ID_MAX);
    dst[ID_MAX-1] = '\0';		/* in case ... */
    if (strlen(src) >= ID_MAX)
	cerr << "line: " << line_num << "`" << src << "' truncated to `"
             << dst << "'" << endl;
}

void
save_str(string &dst, const char *src, const int)
{
    dst = src;
}

int
check_byte(const char *val)
{
    char *ptr;
    long v = strtol(val, &ptr, 0);

    if (v == 0 && val == ptr) {
#if 0
	parse_error("Not decodable to an integer value", line);
#endif
	return FALSE;
    }

    // We're very liberal here with values. Anything that can fit into 8 bits
    // is allowed through. Clients will have to deal with the fact that the
    // ASCII representation for the value might need to be tweaked. This is
    // especially the case for Java clients where Byte datatypes are
    // signed. 3/20/2000 jhrg
    if (v > DODS_UCHAR_MAX || v < DODS_SCHAR_MIN) {
#if 0
	parse_error("Not a byte value", line);
#endif
	return FALSE;
    }

    return TRUE;
}

// This version of check_int will pass base 8, 10 and 16 numbers when they
// use the ANSI standard for string representation of those number bases.

int
check_int16(const char *val)
{
    char *ptr;
    long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if (v == 0 && val == ptr) {
#if 0
	string oss="";
	oss+= "`" +(string)val + "' cannot be decoded as an integer value.\n";

	parse_error(oss.c_str(), line);
#endif
	return FALSE;
    }

    // Don't use the constant from limits.h, use the ones in dods-limits.h
    if (v > DODS_SHRT_MAX || v < DODS_SHRT_MIN) { 
#if 0
	string oss="";

	oss +="`" + (string)val + "' is not a 16-bit integer value value.\nIt must be between ";
	append_long_to_string(DODS_SHRT_MIN,10,oss);
	oss+=" and ";
	append_long_to_string(DODS_SHRT_MAX,10,oss);
	oss+= ".\n";
	parse_error(oss.c_str(), line);
#endif
	return FALSE;
    }

    return TRUE;
}

int
check_uint16(const char *val)
{
    char *ptr;
    unsigned long v = strtol(val, &ptr, 0); 

    if (v == 0 && val == ptr) {
#if 0
        string oss="";
        oss+= "`" +(string)val + "' cannot be decoded as an integer value.\n";

	parse_error(oss.c_str(), line);
#endif
	return FALSE;
    }

    if (v > DODS_USHRT_MAX) { 
#if 0
        string oss="";
	oss+="`" + (string)val+ "' is not a 16-bit integer value value.\nIt must be less than or equal to ";
	append_long_to_string(DODS_USHRT_MAX,10,oss);
	oss+=".\n";

	parse_error(oss.c_str(), line);
#endif
	return FALSE;
    }

    return TRUE;
}

int
check_int32(const char *val)
{
    char *ptr;
    long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if (v == 0 && val == ptr) {
#if 0
        string oss="";
        oss+= "`" +(string)val + "' cannot be decoded as an integer value.\n";
	
	parse_error(oss.c_str(), line);
#endif
	return FALSE;
    }

    if (v > DODS_INT_MAX || v < DODS_INT_MIN) { 
#if 0
	string oss="";
	oss+= "`" +(string)val+ "' is not a 32-bit integer value value.\nIt must be between ";
	append_long_to_string(DODS_INT_MIN,10,oss);
	oss+=" and ";
	append_long_to_string( DODS_INT_MAX,10,oss);
	oss+=".\n";

	parse_error(oss.c_str(), line);
#endif
	return FALSE;
    }

    return TRUE;
}

int
check_uint32(const char *val)
{
    char *ptr;
    unsigned long v = strtol(val, &ptr, 0);

    if (v == 0 && val == ptr) {
#if 0
	string oss="";
	oss+="`"+(string)val+"' cannot be decoded as an integer value.\n" ;
	parse_error(oss.c_str(), line);
#endif
	return FALSE;
    }

    return TRUE;
}

// This function does not test for numbers that are smaller than
// DODS_FLT_MIN. That is hard to do without eliminating valid numbers such as
// 0.0. Maybe the solution is to test for 0.0 specially? 4/12/99 jhrg 

int
check_float32(const char *val)
{
    char *ptr;
    double v = strtod(val, &ptr);

    if (v == 0.0 && val == ptr) {
#if 0
	parse_error("Not decodable to a 32-bit float value", num);
#endif
	return FALSE;
    }

#if 0
    static double range = fabs(log10(DODS_FLT_MAX));
    if (v != 0.0 && fabs(log10(fabs(v))) > range) { 
        string oss="";

	oss+= "`" +(string)val+"' is not a 32 bit floating point value value.\nIt must be between (+/-)";
	append_double_to_string(DODS_FLT_MAX,oss);
	oss+=" and (+/-)";
	append_double_to_string(DODS_FLT_MIN,oss);
	oss+= ".\n";
	parse_error(oss.c_str(), num);

	return FALSE;
    }
#endif

    return TRUE;
}

int
check_float64(const char *val)
{
    char *ptr;
    double v = strtod(val, &ptr);

    if (v == 0.0 && val == ptr) {
#if 0
	parse_error("Not decodable to a 64-bit float value", num);
#endif
	return FALSE;
    }

#if 0
    static double range = fabs(log10(DODS_DBL_MAX));
    if (v != 0.0 && fabs(log10(fabs(v))) > range) { 
	string oss="";

	oss+="`"+(string)val+"' is not a 64 bit floating point value value.\nIt must be between (+/-)";
	append_double_to_string(DODS_DBL_MAX,oss);
	oss+= " and (+/-)" ;
	append_double_to_string(DODS_DBL_MIN,oss);
	oss+=".\n";
	parse_error(oss.c_str(), num);

	return FALSE;
    }
#endif

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
