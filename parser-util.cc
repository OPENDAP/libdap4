
// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// These functions are utility functions used by the various DAP parsers (the
// DAS, DDS and constraint expression parsers). 
// jhrg 9/7/95

// $Log: parser-util.cc,v $
// Revision 1.9  1996/10/28 23:05:54  jimg
// Fixed tests in check_uint().
// NB: strtol() does not check for overflow on SunOS.
//
// Revision 1.8  1996/10/28 18:53:13  jimg
// Added functions to test unsigned integers.
//
// Revision 1.7  1996/08/13 20:43:38  jimg
// Added __unused__ to definition of char rcsid[].
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

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: parser-util.cc,v 1.9 1996/10/28 23:05:54 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream.h>
#include <strstream.h>

#include "parser.h"		// defines constants such as ID_MAX
#include "dods-limits.h"

void 
parse_error(const char *s, const int line_num)
{
    assert(s);

    cerr << s << " line: " << line_num << endl;;
}

void
parse_error(parser_arg *arg, const char *msg, const int line_num,
	    const char *context = 0)
{ 
    assert(arg);
    assert(msg);

    arg->set_status(FALSE);

    ostrstream oss;
    oss << "Error parsing the text on line " << line_num << ":" << endl;
    if (context)
	oss << msg << endl << context << ends;
    else
	oss << msg << ends;
    arg->set_error(new Error(unknown_error, oss.str()));
    oss.freeze(0);
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
save_str(char *dst, const char *src, parser_arg *arg, const int line_num,
	 const char *context)
{
    strncpy(dst, src, ID_MAX);
    dst[ID_MAX-1] = '\0';		/* in case ... */
    if (strlen(src) >= ID_MAX) {
	ostrstream oss;
	oss << "`" << src << "' truncated to `" << dst << "'" << ends;
	parse_error(arg, oss.str(), line_num, context);
	oss.freeze(0);
    }
}

int
check_byte(const char *val, const int line)
{
    char *ptr;
    long v = strtol(val, &ptr, 0);

    if (v == 0 && val == ptr) {
	parse_error("Not decodable to an integer value", line);
	return FALSE;
    }

    if (v > DODS_CHAR_MAX || v < DODS_CHAR_MIN) {
	parse_error("Not a byte value", line);
	return FALSE;
    }

    return TRUE;
}

int
check_byte(parser_arg *arg, const char *val, const int line, 
	   const char *context)
{
    char *ptr;
    long v = strtol(val, &ptr, 0);

    if (v == 0 && val == ptr) {
	ostrstream oss;
	oss << "`" << val << "' cannot be decoded as an integer value." 
	    << ends;
	parse_error(arg, oss.str(), line, context);
	oss.freeze(0);
	return FALSE;
    }

    if (v > DODS_CHAR_MAX || v < DODS_CHAR_MIN) {
	ostrstream oss;
	oss << "`" << val << "' is not a byte value value." << endl
	    << "It must be between " << DODS_CHAR_MIN << " and "
	    << DODS_CHAR_MAX << "." << ends;
	parse_error(arg, oss.str(), line, context);
	oss.freeze(0);
	return FALSE;
    }

    return TRUE;
}

// This version of check_int will pass base 8, 10 and 16 numbers when they
// use the ANSI standard for string representation of those number bases.

int
check_int(const char *val, const int line)
{
    char *ptr;
    long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if (v == 0 && val == ptr) {
	ostrstream oss;
	oss << "`" << val << "' cannot be decoded as an integer value." 
	    << ends;
	parse_error(oss.str(), line);
	oss.freeze(0);
	return FALSE;
    }

    // Don't use the constant from limits.h, use the on in dods-limits.h
    if (v > DODS_INT_MAX || v < DODS_INT_MIN) { 
	ostrstream oss;
	oss << "`" << val << "' is not a integer value value." << endl
	    << "It must be between " << DODS_INT_MIN << " and "
	    << DODS_INT_MAX << "." << ends;
	parse_error(oss.str(), line);
	oss.freeze(0);
	return FALSE;
    }

    return TRUE;
}

int
check_int(parser_arg *arg, const char *val, const int line, 
	  const char *context)
{
    char *ptr;
    long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if (v == 0 && val == ptr) {
	ostrstream oss;
	oss << "`" << val << "' cannot be decoded as an integer value." 
	    << ends;
	parse_error(arg, oss.str(), line, context);
	oss.freeze(0);
	return FALSE;
    }

    // Don't use the constant from limits.h, use the on in dods-limits.h
    if (v > DODS_INT_MAX || v < DODS_INT_MIN) { 
	ostrstream oss;
	oss << "`" << val << "' is not a integer value value." << endl
	    << "It must be between " << DODS_INT_MIN << " and "
	    << DODS_INT_MAX << "." << ends;
	parse_error(arg, oss.str(), line, context);
	oss.freeze(0);
	return FALSE;
    }

    return TRUE;
}

int
check_uint(const char *val, const int line)
{
    char *ptr;
    unsigned long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if (v == 0 && val == ptr) {
	ostrstream oss;
	oss << "`" << val << "' cannot be decoded as an integer value." 
	    << ends;
	parse_error(oss.str(), line);
	oss.freeze(0);
	return FALSE;
    }

    // Don't use the constant from limits.h, use the on in dods-limits.h
    if (v > DODS_UINT_MAX) { 
	ostrstream oss;
	oss << "`" << val << "' is not a integer value value." << endl
	    << "It must be between zero (0) and "
	    << DODS_UINT_MAX << "." << ends;
	parse_error(oss.str(), line);
	oss.freeze(0);
	return FALSE;
    }

    return TRUE;
}

int
check_uint(parser_arg *arg, const char *val, const int line, 
	   const char *context)
{
    char *ptr;
    unsigned long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if (v == 0 && val == ptr) {
	ostrstream oss;
	oss << "`" << val << "' cannot be decoded as an integer value." 
	    << ends;
	parse_error(arg, oss.str(), line, context);
	oss.freeze(0);
	return FALSE;
    }

    // Don't use the constant from limits.h, use the on in dods-limits.h
    if (v > DODS_UINT_MAX) { 
	ostrstream oss;
	oss << "`" << val << "' is not a integer value value." << endl
	    << "It must be between zero (0) and "
	    << DODS_UINT_MAX << "." << ends;
	parse_error(arg, oss.str(), line, context);
	oss.freeze(0);
	return FALSE;
    }

    return TRUE;
}

int
check_float(const char *val, const int num)
{
    char *ptr;
    double v = strtod(val, &ptr);

    if (v == 0.0 && val == ptr) {
	parse_error("Not decodable to a 64-bit float value", num);
	return FALSE;
    }

    return TRUE;
}

int
check_float(parser_arg *arg, const char *val, const int line,
	    const char *context)
{
    char *ptr;
    double v = strtod(val, &ptr);

    if (v == 0.0 && val == ptr) {
	ostrstream oss;
	oss << "`" << val << "' cannot be decoded as an 64-bit float value." 
	    << ends;
	parse_error(arg, oss.str(), line, context);
	oss.freeze(0);
	return FALSE;
    }

    return TRUE;
}

/*
  Maybe someday we will really check the Urls to see if they are valid...
*/

int
check_url(const char *, const int)
{
    return TRUE;
}

int
check_url(parser_arg *, const char *, const int, const char *)
{
    return TRUE;
}

