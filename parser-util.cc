/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

// These functions are utility functions used by the various DAP parsers (the
// DAS, DDS and constraint expression parsers). 
// jhrg 9/7/95

// $Log: parser-util.cc,v $
// Revision 1.3  1996/05/06 18:33:24  jimg
// Replaced calls to atoi with calls to strtol.
//
// Revision 1.2  1996/05/04 00:07:33  jimg
// Fixed a bug where Float attributes with the value 0.0 were considered `bad
// values'.
//
// Revision 1.1  1996/04/04 22:12:19  jimg
// Added.

static char rcsid[]= {"$Id: parser-util.cc,v 1.3 1996/05/06 18:33:24 jimg Exp $"};

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

