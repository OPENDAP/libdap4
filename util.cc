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

// Utility functions used by the api.
//
// jhrg 9/21/94

// $Log: util.cc,v $
// Revision 1.19  1996/04/04 17:52:36  jimg
// Merged changes from version 1.1.1.
//
// Revision 1.18  1996/03/02 01:13:47  jimg
// Fixed problems with case labels and local variables.
//
// Revision 1.17  1996/02/02 00:31:24  jimg
// Merge changes for DODS-1.1.0 into DODS-2.x
//
// Revision 1.16  1995/12/06  18:36:23  jimg
// Added text_to_temp(). This copies the given String to a temporary file and
// returns a FILE * to that file. The file is unlinked, so one the FILE * is
// closed, it is removed from the file system.
//
// Revision 1.15  1995/10/23  23:06:07  jimg
// Fixed cast of out_tmp in xdr_str().
//
// Revision 1.14  1995/08/26  00:32:10  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.13  1995/08/23  00:41:58  jimg
// xdr_str() now takes a String & instead of a String ** for arg 2.
//
// Revision 1.12.2.7  1996/02/23 21:37:33  jimg
// Updated for new configure.in.
// Fixed problems on Solaris 2.4.
//
// Revision 1.12.2.6  1995/10/12  17:01:43  jimg
// Added {}'s to case statements so that gcc-2.7.0 won't complain about jumping
// over case statement labels. This only happens when returning values from
// variable defined within a case label. It is better to use the {}'s than move
// the definition of the variable.
//
// Revision 1.12.2.5  1995/09/29  19:28:04  jimg
// Fixed problems with xdr.h on an SGI.
// Fixed conflict of int32_t (which was in an enum type defined by BaseType) on
// the SGI.
//
// Revision 1.12.2.4  1995/09/27  23:17:20  jimg
// Fixed casts again...
//
// Revision 1.12.2.3  1995/09/27  21:49:06  jimg
// Fixed casts.
//
// Revision 1.12.2.2  1995/09/27  19:07:01  jimg
// Add casts to `cast away' const and unsigned in places where we call various
// xdr functions (which don't know about, or use, const or unsigned.
//
// Revision 1.12.2.1  1995/09/14  20:57:25  jimg
// Moved variable initializations in xdr_str() and xdr_str_array() out of the
// switch statement to avoid complaint from gcc 2.7.0 about initializations
// crossing case labels.
// Moved some loop index variables out of the loop statement.
//
// Revision 1.12  1995/07/09  21:29:28  jimg
// Added copyright notice.
//
// Revision 1.11  1995/05/10  15:34:09  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.10  1995/05/10  13:45:46  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.9  1995/03/16  17:39:19  jimg
// Added TRACE_NEW/dbnew checks. This includes a special kluge for the
// placement new operator.
//
// Revision 1.8  1995/03/04  15:30:11  jimg
// Fixed List so that it will compile - it still has major bugs.
// Modified Makefile.in so that test cases include List even though List
// won't work (but at least the test cases will link and run, so long
// as you don't try to do anything with Lists...
//
// Revision 1.7  1995/03/04  14:36:48  jimg
// Fixed xdr_str so that it works with the new String objects.
// Added xdr_str_array for use with arrays of String objects.
//
// Revision 1.6  1995/02/10  03:27:07  jimg
// Removed xdr_url() since it was just a copy of xdr_str().
//
// Revision 1.5  1995/01/11  16:10:47  jimg
// Added to new functions which manage XDR stdio stream pointers. One creates
// a new xdrstdio pointer associated wit the given FILE * and the other
// deletes it. The creation function returns the XDR *, so it can be used to
// initialize a new XDR * (see BaseType.cc).
//
// Revision 1.4  1994/11/29  20:21:23  jimg
// Added xdr_str and xdr_url functions (C linkage). These provide a way for
// the Str and Url classes to en/decode strings (Urls are effectively strings)
// with only two parameters. Thus the Array ad List classes might actually
// work as planned.
//
// Revision 1.3  1994/11/22  14:06:22  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.2  1994/10/18  00:25:36  jimg
// Fixed error in char * [] allocation.
// Added debugging code.
//

static char rcsid[]={"$Id: util.cc,v 1.19 1996/04/04 17:52:36 jimg Exp $"};

#include "config_dap.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef DBMALLOC
#include <dbmalloc.h>
#endif

#include <new>			// needed for placement new in xdr_str_array
#include <SLList.h>

#include "BaseType.h"
#include "Str.h"
#include "Url.h"
#include "errmsg.h"
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#define DODS_CE_PRX "dods-ce"

static int
char_cmp(const void *a, const void *b)
{
    return strcmp(*(char **)a, *(char **)b);
}

// Compare elements in a SLList of (BaseType *)s and return true if there are
// no duplicate elements, otherwise return false. Uses the same number of
// compares as qsort. (Guess why :-)
//
// NB: The elements of the array to be sorted are pointers to chars; the
// compare function gets pointers to those elements, thus the cast to (const
// char **) and the dereference to get (const char *) for strcmp's arguments.

bool
unique(SLList<BaseTypePtr> l, const char *var_name, const char *type_name)
{
    // copy the identifier names to an array of char
    char **names = new char *[l.length()];
    if (!names)
	err_quit("util.cc:unique - Could not allocate array NAMES.");

    int nelem = 0;
    String s;
    for (Pix p = l.first(); p; l.next(p)) {
	names[nelem++] = strdup((const char *)l(p)->name());
	DBG(cerr << "NAMES[" << nelem-1 << "]=" << names[nelem-1] << endl);
    }
    
    // sort the array of names
    qsort(names, nelem, sizeof(char *), char_cmp);
	
#ifdef DEBUG2
    cout << "unique:" << endl;
    for (int ii = 0; ii < nelem; ++ii)
	cout << "NAMES[" << ii << "]=" << names[ii] << endl;
#endif

    // look for any instance of consecutive names that are ==
    int i;
    for (i = 1; i < nelem; ++i)
	if (!strcmp(names[i-1], names[i])) {
	    cerr << "The variable `" << names[i] 
		 << "' is used more than once in " << type_name << " `"
		 << var_name << "'" << endl;
	    for (i = 0; i < nelem; i++)
		free(names[i]);	// strdup uses malloc
	    delete [] names;
	    return false;
	}

    for (i = 0; i < nelem; i++)
	free(names[i]);		// strdup uses malloc
    delete [] names;

    return true;
}

// This function is used to allocate memory for, and initialize, and new XDR
// pointer. It sets the stream associated with the (XDR *) to STREAM.
//
// NB: STREAM is not one of the C++/libg++ iostream classes; it is a (FILE
// *).

XDR *
new_xdrstdio(FILE *stream, enum xdr_op xop)
{
    XDR *xdr = new XDR;
    
    xdrstdio_create(xdr, stream, xop);
    
    return xdr;
}

XDR *
set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop)
{
    xdrstdio_create(xdr, stream, xop);
    
    return xdr;
}

// Delete an XDR pointer allocated using the above function. Do not close the
// associated FILE pointer.

void
delete_xdrstdio(XDR *xdr)
{
    xdr_destroy(xdr);
    delete(xdr);
}

// This function is used to en/decode Str and Url type variables. It is
// defined as extern C since it is passed via function pointers to routines
// in the xdr library where they are executed. This function is defined so
// that Str and Url have an en/decoder which takes exactly two argumnets: an
// XDR * and a String reference.
//
// NB: this function is *not* used for arrays (i.e., it is not the function
// referenced by BaseType's _xdr_coder field when the object is a Str or Url.
//
// Returns: XDR's bool_t; TRUE if no errors are detected, FALSE
// otherwise. The formal parameter BUF is modified as a side effect.

extern "C" bool_t
xdr_str(XDR *xdrs, String &buf)
{
    switch (xdrs->x_op) {
      case XDR_ENCODE: {	// BUF is a pointer to a (String *)
	assert(buf && *buf);
	
	const char *out_tmp = (const char *)**buf;

	return xdr_string(xdrs, (char **)&out_tmp, max_str_len);
      }

      case XDR_DECODE: {	// BUF is a pointer to a String * or to NULL
	assert(buf);
	char str_tmp[max_str_len];
	char *in_tmp = str_tmp;

	bool_t stat = xdr_string(xdrs, (char **)&in_tmp, max_str_len);
	if (!stat)
	    return stat;

	if (*buf) {
	    **buf = in_tmp;
	}
	else {
	    *buf = new String(in_tmp);
	}
	
	return stat;
      }
	
      default:
	assert(false);
	return 0;
    }
}

// Given a string, copy that string to a temporary file which will be removed
// from the file system upon closing.
//
// Returns: A FILE * to the temporary file.

FILE *
text_to_temp(String text)
{
    char *c = tempnam(NULL, DODS_CE_PRX);
    FILE *fp = fopen(c, "w+");
    fputs((const char *)text, fp);
    fclose(fp);		/* once full, close file */
    
    fp = fopen(c, "r");	/* get file pointer */
    if (unlink(c) < 0)	/* now when fp is closed, file is rm'd */
	err_sys("Could not unlink tmp file %s", c);

    return fp;
}
