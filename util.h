
// -*- C++ -*-

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

#ifndef _list_h
#include "List.h"
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

#ifdef WIN32
using std::iostream;
#endif

string prune_spaces(string);
bool unique_names(SLList<BaseType *> l, const string &var, const string &type,
		  string &msg);
bool unique_names(DLList<BaseType *> l, const string &var, const string &type,
		  string &msg);
XDR *new_xdrstdio(FILE *stream, enum xdr_op xop);
XDR *set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop);
void delete_xdrstdio(XDR *xdr);
FILE *text_to_temp(string text);
char *systime();
FILE *compressor(FILE *output, int &childpid);
bool deflate_exists();
const char *dods_root();
const char *dods_progress();
#ifdef WIN32
void flush_stream(iostream ios, FILE *out);
#endif

bool func_member(int argc, BaseType *argv[], DDS &dds);
bool func_null(int argc, BaseType *argv[], DDS &dds);
BaseType *func_nth(int argc, BaseType *argv[], DDS &dds);
BaseType *func_length(int argc, BaseType *argv[], DDS &dds);

#ifdef WIN32
extern "C" int xdr_str(XDR *xdrs, string &buf);
#else
extern "C" bool_t xdr_str(XDR *xdrs, string &buf);
#endif

Byte *NewByte(const string &n = "");
Int16 *NewInt16(const string &n = "");
UInt16 *NewUInt16(const string &n = "");
Int32 *NewInt32(const string &n = "");
UInt32 *NewUInt32(const string &n = "");
Float32 *NewFloat32(const string &n = "");
Float64 *NewFloat64(const string &n = "");
Str *NewStr(const string &n = "");
Url *NewUrl(const string &n = "");
Array *NewArray(const string &n = "", BaseType *v = 0);
List *NewList(const string &n = "", BaseType *v = 0);
Structure *NewStructure(const string &n = "");
Sequence *NewSequence(const string &n = "");
Grid *NewGrid(const string &n = "");

void downcase(string &s);
// Jose Garcia
/** Fast, safe conversions from long to a character representation which gets
   appended to a string. This method will take a long value 'val' and it will
   recursively divide it by 'base' in order to "extract" one by one the
   digits which compose it; these digits will be {\bfappended} to the string
   'str_val' which will become the textual representation of 'val'. Please
   notice that the digits "extracted" from 'val' will vary depending on the
   base chosen for the conversion; for example val=15 converted to base 10
   will yield the digits {1,5}, converted to base 16 will yield {F} and
   converted to base 2 will yield {1,1,1,1}.

   @param val The long value we which to convert to string.

   @param base A value in the range [2,36] which is the base to use while
   transforming the long value 'val' to its textual representation. Typical
   bases are 2 (binary), 10 (decimal) and 16 (hexadecimal).

   @param str_val This is the string that will hold the textual
   representation of 'val'. The string str_val should be pre-set to an empty
   string ("") otherwise the output of this function will just append the
   textual representation of val to whatever data is there; these feature may
   be useful if you wish to append a long value to a string s1 (just like
   operator+ does) without having to create a new string object s2 and then
   use string::operator+ between s1 and s2.

   @return void. This method returns nothing however be aware that it will
   throw and exception of type {\bfstd::invalid_argument} if the parameter
   base is not in the valid range. */
void append_long_to_string(long val, int base, string &str_val);

// Jose Garcia
/**
    Conversions from double to a character representation which gets appended
    to a string. This function depends on the standard routine sprintf to
    convert a double to a textual representation which gets appended to the
    string 'str'.

    @param num The double you wish to append to str.

    @param str The string where the textual representation of num will be
    appended.

    @return void. */
void append_double_to_string(const double &num, string &str);

/** Get the version of the DAP library. */
string dap_version();

/* 
 * $Log: util.h,v $
 * Revision 1.37  2000/10/03 21:03:22  jimg
 * Added dap_version() function
 *
 * Revision 1.36  2000/09/22 02:17:23  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.35  2000/09/21 16:22:10  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.34  2000/07/09 21:57:10  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.33  2000/06/16 18:15:01  jimg
 * Merged with 3.1.7
 *
 * Revision 1.31.6.1  2000/06/14 16:59:54  jimg
 * Added a new varient of unique_names for DLLists.
 *
 * Revision 1.32  2000/06/07 18:07:01  jimg
 * Merged the pc port branch
 *
 * Revision 1.31.20.1  2000/06/02 18:39:04  rmorris
 * Mod's for port to win32.
 *
 * Revision 1.31  1999/05/04 19:47:24  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.30  1999/04/29 21:14:49  jimg
 * Removed Function include
 *
 * Revision 1.29  1999/04/29 02:29:37  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.28  1999/03/24 23:25:10  jimg
 * Added includes and New*() definitions for the Int16, UInt16 and Float32
 * types.
 * Removed declarations for the old *_ops functions.
 *
 * Revision 1.31.14.1  2000/03/08 00:09:05  jgarcia
 * replace ostrstream with string;added functions to convert from double and long to string
 *
 * Revision 1.27  1999/01/21 02:12:35  jimg
 * Moved prototypes to ce_functions.h
 *
 * Revision 1.26.6.1  1999/02/02 21:57:08  jimg
 * String to string version
 *
 * Revision 1.26  1998/04/07 22:15:35  jimg
 * Added declaration of prune_spaces(String)
 *
 * Revision 1.25  1998/02/11 22:35:18  jimg
 * Added prototype for deflate_exists().
 * Removed prototype for decompressor().
 *
 * Revision 1.24  1997/08/11 18:18:42  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.23  1997/03/27 18:18:56  jimg
 * Added dods_progress() prototype. See util.cc.
 *
 * Revision 1.22  1997/03/08 18:58:57  jimg
 * Changed name of function unique() to unique_names() to avoid a conflict
 * with the STL.
 *
 * Revision 1.21  1997/02/28 01:24:54  jimg
 * Added String &msg parameter to unique(). Instead of writing messages to
 * cerr, messages are now written to this string object.
 *
 * Revision 1.20  1997/02/19 02:13:43  jimg
 * Fixed decompressor and compressor parameter lists.
 *
 * Revision 1.19  1997/02/10 02:39:26  jimg
 * Changed return type of dods_root() from String to char *.
 *
 * Revision 1.18  1996/12/02 23:19:05  jimg
 * Changed int_ops prototypes so that they use the dods_{uint,int}32 datatypes.
 *
 * Revision 1.17  1996/11/27 22:40:30  jimg
 * Added DDS as third parameter to function in the CE evaluator
 *
 * Revision 1.16  1996/11/25 03:45:08  jimg
 * Added prototypes for new functions.
 *
 * Revision 1.15  1996/11/22 00:08:34  jimg
 * Added compressor and decompresor prototypes.
 *
 * Revision 1.14  1996/10/28 23:07:37  jimg
 * Added include of UInt32.h and prototype of NewUInt32().
 *
 * Revision 1.13  1996/08/13 20:47:21  jimg
 * Added prototypes for the *_ops() functions.
 *
 * Revision 1.12  1996/05/31 23:31:11  jimg
 * Updated copyright notice.
 *
 * Revision 1.11  1996/05/29 22:09:00  jimg
 * Made changes necessary to support CEs that return the value of a function
 * instead of the value of a variable. This was done so that it would be
 * possible to translate Sequences into Arrays without first reading the
 * entire sequence over the network.
 *
 * Revision 1.10  1996/04/05 00:22:25  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.9  1996/04/04 17:38:35  jimg
 * Merged changes from version 1.1.1.
 *
 * Revision 1.8  1996/02/02 00:31:25  jimg
 * Merge changes for DODS-1.1.0 into DODS-2.x
 *
 * Revision 1.7  1995/12/06  18:33:34  jimg
 * Added forward decalration of text_to_temp();
 *
 * Revision 1.6  1995/08/26  00:32:12  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.5  1995/08/23  00:41:57  jimg
 * xdr_str() now takes a String & instead of a String ** for arg 2.
 *
 * Revision 1.4.2.2  1996/02/23 21:37:34  jimg
 * Updated for new configure.in.
 * Fixed problems on Solaris 2.4.
 *
 * Revision 1.4.2.1  1995/09/29  19:28:06  jimg
 * Fixed problems with xdr.h on an SGI.
 * Fixed conflict of d_int32_t (which was in an enum type defined by BaseType) on
 * the SGI.
 *
 * Revision 1.4  1995/03/04  14:36:50  jimg
 * Fixed xdr_str so that it works with the new String objects.
 * Added xdr_str_array for use with arrays of String objects.
 *
 * Revision 1.3  1995/01/19  20:14:59  jimg
 * Added declarations for new utility functions which return pointers to
 * objects in the BaseType hierarchy.
 *
 * Revision 1.2  1994/11/29  20:21:24  jimg
 * Added xdr_str and xdr_url functions (C linkage). These provide a way for
 * the Str and Url classes to en/decode strings (Urls are effectively strings)
 * with only two parameters. Thus the Array and List classes might actually
 * work as planned.
 */

#endif
