
// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// These functions are utility functions used by the various DAP parsers (the
// DAS, DDS and constraint expression parsers). 
// jhrg 9/7/95

// $Log: parser-util.cc,v $
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

static char rcsid[]= {"$Id: parser-util.cc,v 1.4 1996/05/31 23:31:05 jimg Exp $"};

#include <stdlib.h>
#include <string.h>

#include <iostream.h>

#include "parser.h"		// defines constants such as ID_MAX
#include "dods-limits.h"

void 
parse_error(const char *s, const int line_num)
{
    cerr << s << " line: " << line_num << endl;;
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

int
check_byte(const char *val, const int num)
{
    char *ptr;
    long v = strtol(val, &ptr, 0);

    if (v == 0 && val == ptr) {
	parse_error("Not decodable to an integer value", num);
	return FALSE;
    }

    if (v > DODS_CHAR_MAX || v < DODS_CHAR_MIN) {
	parse_error("Not a byte value", num);
	return FALSE;
    }

    return TRUE;
}

// This version of check_int will pass base 8, 10 and 16 numbers when they
// use the ANSI standard for string representation of those number bases.

int
check_int(const char *val, const int num)
{
    char *ptr;
    long v = strtol(val, &ptr, 0); // `0' --> use val to determine base

    if (v == 0 && val == ptr) {
	parse_error("Not decodable to an integer value", num);
	return FALSE;
    }

    /* don't use the constant from limits.h, use the on in dods-limits.h */
    if (v > DODS_INT_MAX || v < DODS_INT_MIN) { 
	parse_error("Not a 32-bit integer value", num);
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

/*
  Maybe someday we will really check the Urls to see if they are valid...
*/

int
check_url(const char *, const int)
{
    return TRUE;
}

