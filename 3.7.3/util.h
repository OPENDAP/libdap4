
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// declarations for utility functions
//
// jhrg 9/21/94

#ifndef _util_h
#define _util_h 1

#include <stdio.h>
#include <vector>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _byte_h
#include "Byte.h"
#endif

#ifndef _int16_h
#include "Int16.h"
#endif

#ifndef _uint16_h
#include "UInt16.h"
#endif

#ifndef _int32_h
#include "Int32.h"
#endif

#ifndef _uint32_h
#include "UInt32.h"
#endif

#ifndef _float32_h
#include "Float32.h"
#endif

#ifndef _float64_h
#include "Float64.h"
#endif

#ifndef _str_h
#include "Str.h"
#endif

#ifndef _url_h
#include "Url.h"
#endif

#ifndef _array_h
#include "Array.h"
#endif

#ifndef _structure_h
#include "Structure.h"
#endif

#ifndef _sequence_h
#include "Sequence.h"
#endif

#ifndef _grid_h
#include "Grid.h"
#endif

using std::iostream;

string prune_spaces(const string &);
bool unique_names(vector<BaseType *> l, const string &var, const string &type,
		  string &msg);
//  These func's moved to xdrutil_ppc.* under the PPC as explained there
#ifdef __POWERPC__
extern "C" XDR *new_xdrstdio(FILE *stream, enum xdr_op xop);
extern "C" XDR *set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop);
extern "C" void delete_xdrstdio(XDR *xdr);
#else
XDR *new_xdrstdio(FILE *stream, enum xdr_op xop);
XDR *set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop);
void delete_xdrstdio(XDR *xdr);
#endif
FILE *text_to_temp(string text);
string systime();
FILE *compressor(FILE *output, int &childpid);
bool deflate_exists();
const char *libdap_root();
/** Return the version string for this package.
    @note This function has C linkage so that it can be found using autoconf
    tests.
    @return The version string. */
extern "C" const char *libdap_version();
extern "C" const char *libdap_name();
const char *dods_progress();
#ifdef WIN32
void flush_stream(iostream ios, FILE *out);
#endif

extern "C" bool_t xdr_str(XDR *xdrs, string &buf);

void downcase(string &s);
// Jose Garcia
/** @name Interger to string conversion functions
   Fast, safe conversions from long to a character representation which gets
   appended to a string. This method will take a long value 'val' and it will
   recursively divide it by 'base' in order to "extract" one by one the
   digits which compose it; these digits will be <i>appended</i> to the
   string <tt>str_val</tt> which will become the textual representation of
   'val'. Please notice that the digits ``extracted'' from `val' will vary
   depending on the base chosen for the conversion; for example val=15
   converted to base 10 will yield the digits (1,5), converted to base 16
   will yield (F) and converted to base 2 will yield (1,1,1,1).

   @param val The long value we which to convert to string.

   @param base A value in the range [2,36] which is the base to use while
   transforming the long value 'val' to its textual representation. Typical
   bases are 2 (binary), 10 (decimal) and 16 (hexadecimal).

   @param str_val This is the string that will hold the textual
   representation of 'val'. The string <tt>str_val</tt> should be
   pre-set to an empty 
   string ("") otherwise the output of this function will just append the
   textual representation of val to whatever data is there; these feature may
   be useful if you wish to append a long value to a string s1 (just like
   operator+ does) without having to create a new string object s2 and then
   use string::operator+ between s1 and s2.

   @return void. This method returns nothing however be aware that it will
   throw and exception of type <tt>std::invalid_argument</tt> if the parameter
   base is not in the valid range. */
//@{
void append_long_to_string(long val, int base, string &str_val);
string long_to_string(long val, int base = 10);
//@}

// Jose Garcia
/** @name Double to string conversion functions
    Conversions from double to a character representation which gets appended
    to a string. This function depends on the standard routine sprintf to
    convert a double to a textual representation which gets appended to the
    string 'str'.

    @param num The double you wish to append to str.

    @param str The string where the textual representation of num will be
    appended.

    @return void. */
//@{
void append_double_to_string(const double &num, string &str);
string double_to_string(const double &num);
//@}

/** Get the version of the DAP library. */
string dap_version();

/** Get the filename part from a path. This function can be used to return a
    string that has the directory compnents stripped from a path. This is
    useful when building error message strings. 

    If WIN32 is defined, use '\' as the path separator, otherwise use '/' as
    the path separator.

    @return A string containing only the filename given a path. */
string path_to_filename(string path);

/** Build a template for a temporary file suitable for use with mkstemp.
    Look around for a reasonable place to put a temporary file. Check first
    the value of the TMPDIR env var. If that does not yeild a path that's
    writable (as defined by access(..., W_OK|R_OK)) then look at P_tmpdir (as
    defined in stdio.h. If both come up empty, then use `./'.

    This function allocates storage using new. The caller must delete the
    char array. 

    @param file_template A template suitable for use with mkstemp (so it has
    to have six extra chars at its end. This is combined with the path to
    some temporary directory (see above).
    @return A writable char[] that holds the fully qualified
    filename/template to use with mkstemp. */
char *get_tempfile_template(char *file_template);

FILE *get_temp_file(char *temp);
string file_to_string(FILE *fp);

time_t parse_time(const char * str, bool expand);

#endif
