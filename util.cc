
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


// Utility functions used by the api.
//
// jhrg 9/21/94

// $Log: util.cc,v $
// Revision 1.49  1999/01/21 02:10:57  jimg
// Moved various CE functions to the file ce_functions.cc/.h.
//
// Revision 1.48  1998/11/10 00:47:01  jimg
// Fixed a number of memory leaks (found using purify).
//
// Revision 1.47  1998/09/02 23:59:11  jimg
// Removed func_date to avoid conflicts with a copy in ff-dods-2.15.
//
// Revision 1.46  1998/04/07 22:12:49  jimg
// Added prune_spaces(String) function. This can be used to remove leading
// spaces from URLs and their embedded CEs (i.e., spaces between the ? and the
// start of the CE). Doing this before passing the URL/CE into the web library
// prevents various nasty crashes. This function is called by Connect's default
// ctor. Users of Connect don't have to call it.
//
// Revision 1.45  1998/02/11 20:28:17  jimg
// Added/fixed support for on-the-fly compression of data. The current code
// uses a subprocess to run a compression filter, deflate, which uses the LZW
// compression algorithm in zlib 1.0.4.
//
// Revision 1.44  1998/02/05 20:14:09  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.43  1997/07/15 21:57:13  jimg
// Fixed error message text for length function.
//
// Revision 1.42  1997/06/06 17:52:20  jimg
// Last minute changes for version 2.14
//
// Revision 1.41  1997/06/05 23:04:33  jimg
// Fixed the decompressor() function - made int data[2] automatic instead of
// static.
// Fixed the spelling mistake in the function names compressor and
// decompressor.
//
// Revision 1.40  1997/05/13 23:40:07  jimg
// Added instrumentation for function decompressor().
//
// Revision 1.39  1997/05/01 18:54:35  jimg
// Fixed an error in date_func where argv[0] was used as a BaseType instead of
// a BaseType *.
//
// Revision 1.38  1997/03/27 18:18:28  jimg
// Added dods_progress() function that looks for an environment variable to see
// if the user wants the progress indicator visible. Thus, they can use one
// environment variable to control the GUI as a whole or this one to control
// just the progress indicator.
//
// Revision 1.37  1997/03/08 18:58:22  jimg
// Changed name of function unique() to unique_names() to avoid a conflict
// with the STL.
//
// Revision 1.36  1997/02/28 01:24:28  jimg
// Added String &msg parameter to unique(). Instead of writing messages to
// cerr, messages are now written to this string object.
//
// Revision 1.35  1997/02/19 02:09:47  jimg
// Added childpid parameter.
//
// Revision 1.34  1997/02/10 02:38:42  jimg
// Added assert() calls for pointers.
// Added code to fix warnings about comparisons between int and unsigned
// numbers.
// Changed return type of dods_root() from String to char *.
//
// Revision 1.33  1996/12/02 23:20:29  jimg
// func_member now does the right thing with the dds by sending dds.filename()
// to ops().
//
// Revision 1.32  1996/12/02 19:45:58  jimg
// Added versions of int_ops for various combinations of signed and unsigned
// operands. Ignore the warnings about comparisons between signed and
// unsigned...
//
// Revision 1.31  1996/11/27 22:40:29  jimg
// Added DDS as third parameter to function in the CE evaluator
//
// Revision 1.30  1996/11/25 03:44:39  jimg
// Added new dods_root function.
// Fixed compressor().
// Fixed systime().
//
// Revision 1.29  1996/11/21 23:56:21  jimg
// Added compressor and decompressor functions.
//
// Revision 1.28  1996/11/13 19:23:17  jimg
// Fixed debugging.
//
// Revision 1.27 1996/09/12 00:21:30 jimg Fixed two errors in xdr_str: 1) Use
// of max_str_len as an array dimension (max_str_len is approx 4e9; a large
// array by most standards...) and 2) use of storage on the stack for the
// return string. Now xdr_strings internal allocation mechanism (which uses
// malloc) is used to create the space needed by the incoming string. After
// that string is copied to the String object parameter it is freed using
// free() (Not xdr_free() as the SunOS 4 man page erroneously suggests).
//
// Revision 1.26  1996/08/13 20:46:39  jimg
// Added the *_ops() functions (moved from various class files).
//
// Revision 1.25  1996/07/15 20:30:35  jimg
// Added __unused__ to rcsid to suppress warnings from g++ -Wall. Fixed a bug
// in xdr_str(): a pointer to the decoded string was assigned to the formal
// parameter BUF instead of the string value itself.
//
// Revision 1.24  1996/05/31 23:31:09  jimg
// Updated copyright notice.
//
// Revision 1.23  1996/05/29 22:08:59  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.22  1996/05/22 18:05:45  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.21  1996/05/14 15:39:09  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.20  1996/04/05 00:22:24  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
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
// Fixed problems with xdr.h on an SGI. Fixed conflict of d_int32_t (which
// was in an enum type defined by BaseType) on the SGI.
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

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: util.cc,v 1.49 1999/01/21 02:10:57 jimg Exp $"};

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef DBMALLOC
#include <dbmalloc.h>
#endif

#include <strstream.h>
#include <SLList.h>
#include <String.h>
#include <Regex.h>

#include "BaseType.h"
#include "Str.h"
#include "Url.h"
#include "List.h"
#include "Sequence.h"
#include "Error.h"
#include "parser.h"
#include "expr.tab.h"
#include "util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

const char DODS_CE_PRX[]={"dods"};

// Remove spaces from the start of a URL and fromthe start of any constraint
// expression it contains. 4/7/98 jhrg

String 
prune_spaces(String name)
{
    // If the URL does not even have white space return.
    if (!name.contains(RXwhite))
	return name;
    else {
	// Strip leading spaces from http://...
	int i = 0;
	while (name.at(i, 1) == " ")
	    i++;
	name = name.after(--i);
	
	// Strip leading spaces from constraint part (following `?').
	int j = i = name.index("?") + 1;
	while (name.at(i, 1) == " ")
	    i++;
	--i;
	name.at(j, name.after(i).length()) = name.after(i);

	return name;
    }
}

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
unique_names(SLList<BaseTypePtr> l, const char *var_name, 
	     const char *type_name, String &msg)
{
    // copy the identifier names to an array of char
    char **names = new char *[l.length()];

    int nelem = 0;
    for (Pix p = l.first(); p; l.next(p)) {
	assert(l(p));
	names[nelem++] = strdup((const char *)l(p)->name());
	DBG(cerr << "NAMES[" << nelem-1 << "]=" << names[nelem-1] << endl);
    }
    
    // sort the array of names
    qsort(names, nelem, sizeof(char *), char_cmp);
	
#ifdef DODS_DEBUG2
    cout << "unique:" << endl;
    for (int ii = 0; ii < nelem; ++ii)
	cout << "NAMES[" << ii << "]=" << names[ii] << endl;
#endif

    // look for any instance of consecutive names that are ==
    int i;
    for (i = 1; i < nelem; ++i)
	if (!strcmp(names[i-1], names[i])) {
	    ostrstream oss;
	    oss << "The variable `" << names[i] 
		 << "' is used more than once in " << type_name << " `"
		 << var_name << "'" << ends;
	    msg = oss.str();
	    oss.freeze(0);
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

// This function is used to allocate memory for, and initialize, a new XDR
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

    delete xdr;
}

// This function is used to en/decode Str and Url type variables. It is
// defined as extern C since it is passed via function pointers to routines
// in the xdr library where it is executed. This function is defined so
// that Str and Url have an en/decoder which takes exactly two argumnets: an
// XDR * and a String reference.
//
// NB: this function is *not* used for arrays (i.e., it is not the function
// referenced by BaseType's _xdr_coder field when the object is a Str or Url.
// Also note that #max_str_len# is an obese number but that really does not
// matter; #xdr_string# would never actually allocate that much memory unless
// a string that size was sent from the server.
//
// Returns: XDR's bool_t; TRUE if no errors are detected, FALSE
// otherwise. The formal parameter BUF is modified as a side effect.

extern "C" bool_t
xdr_str(XDR *xdrs, String &buf)
{
    DBG(cerr << "In xdr_str, xdrs: " << xdrs << endl);

    switch (xdrs->x_op) {
      case XDR_ENCODE: {	// BUF is a pointer to a (String *)
	assert(buf);
	
	const char *out_tmp = (const char *)buf;

	return xdr_string(xdrs, (char **)&out_tmp, max_str_len);
      }

      case XDR_DECODE: {
	assert(buf);
	char *in_tmp = NULL;

	bool_t stat = xdr_string(xdrs, &in_tmp, max_str_len);
	if (!stat)
	    return stat;

	buf = in_tmp;

	free(in_tmp);
	
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

#if 0
FILE *
text_to_temp(String text)
{
    char *c = tempnam(NULL, DODS_CE_PRX);
    FILE *fp = fopen(c, "w+");	// create temp
    unlink(c);			// make anonymous
    if (!fp) {
	cerr << "Could not create anonymous temporary file: "
	    << strerror(errno) << endl;
	return NULL;
    }
    free(c);			// tempnam uses malloc! 10/27/98 jhrg
    fputs((const char *)text, fp); // dump information

    if (fseek(fp, 0L, 0 == -1)) { // rewind in preparation for reading
	cerr << "Could not rewind anonymous temporary file: "
	    << strerror(errno) << endl;
	return NULL;
    }

    return fp;
}
#endif

const char *
dods_root()
{
    static const char *dods_root = (getenv("DODS_ROOT") ? getenv("DODS_ROOT") 
				    : DODS_ROOT);

    return dods_root;
}

const char *
dods_progress()
{
    static const char *dods_progress = (getenv("SHOW_DODS_PROGRESS") 
					? getenv("SHOW_DODS_PROGRESS") 
					: "yes");

    return dods_progress;
}

// Return true if the program deflate exists and is executable by user, group
// and world. If this returns false the caller should assume that server
// filter programs won't be able to find the deflate program and thus won't
// be able to compress the return document. 
// NB: this works because this function uses the same rules as compressor()
// (which follows) to look for deflate. 2/11/98 jhrg

bool
deflate_exists()
{
    DBG(cerr << "Entering deflate_exists...");
    int status = false;
    struct stat buf;

    String deflate = (String)dods_root() + "/etc/deflate";

    // Check that the file exists...
    // First look for deflate using DODS_ROOT (compile-time constant subsumed
    // by an environment variable) and if that fails in the CWD which finds
    // the program when it is in the same directory as the dispatch script
    // and other server components. 2/11/98 jhrg
    status = (stat((const char *)deflate, &buf) == 0)
	|| (stat("./deflate", &buf) == 0);

    // and that it can be executed.
    status &= buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH);
    DBG(cerr << " returning " << (status ? "true." : "false.") << endl);
    return status;
}

// Note that decompression is handled automatically by libwww 5.1. 2/10/1998
// jhrg

FILE *
compressor(FILE *output, int &childpid)
{
    int pid, data[2];

    if (pipe(data) < 0) {
	cerr << "Could not create IPC channel for compressor process" 
	     << endl;
	return NULL;
    }
    
    if ((pid = fork()) < 0) {
	cerr << "Could not fork to create compressor process" << endl;
	return NULL;
    }

    // The parent process closes the write end of the Pipe, and creates a
    // FILE * using fdopen(). The FILE * is used by the calling program to
    // access the read end of the Pipe.

    if (pid > 0) {
	close(data[0]);
	FILE *input = fdopen(data[1], "w");
	setbuf(input, 0);
	childpid = pid;
	return input;
    }
    else {
	close(data[1]);
	dup2(data[0], 0);	// Read from the pipe...
	dup2(fileno(output), 1); // Write to the FILE *output.

	DBG(cerr << "Opening compression stream." << endl);

	// First try to run deflate using DODS_ROOT (the value read from the
	// DODS_ROOT environment variable takes precedence over the value set
	// at build time. If that fails, try the CWD.
	String deflate = (String)dods_root() + "/etc/deflate";
	(void) execl(deflate, "deflate", "-c",  "5", "-s", NULL);
	cerr << "Could not run " << deflate << endl;
	(void) execl("./deflate", "deflate", "-c",  "5", "-s", NULL);
	cerr << "Could not run ./deflate" << endl;

	cerr << "Could not start compressor!" << endl;
	cerr << "defalte must be in DODS_ROOT/etc or in the CWD!" 
	     << endl;
	_exit(127);		// Only here if an error occurred.
    }
}

// This function returns a pointer to the system time formated for an httpd
// log file.

static const int TimLen = 26;	// length of string from asctime()

char *
systime()
{
    time_t TimBin;
    static char TimStr[TimLen];

    if (time(&TimBin) == (time_t)-1)
	strcpy(TimStr, "time() error           ");
    else {
	strcpy(TimStr, ctime(&TimBin));
	TimStr[TimLen - 2] = '\0'; // overwrite the \n 
    }

    return &TimStr[0];
}

// Some of these *_ops functions could probably be rolled into a single
// template function. 1/15/99 jhrg

bool
byte_ops(int i1, int i2, int op)
{
    switch (op) {
      case EQUAL:
	return i1 == i2;
      case NOT_EQUAL:
	return i1 != i2;
      case GREATER:
	return i1 > i2;
      case GREATER_EQL:
	return i1 >= i2;
      case LESS:
	return i1 < i2;
      case LESS_EQL:
	return i1 <= i2;
      case REGEXP:
	cerr << "Regular expression not valid for byte values" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

// There are four versions of int_ops to take into account the four
// combinations of signed and unsigned types. Note that the 16 bit variables
// are promoted to 32 bits for the purposes of comparison. 

bool
int_ops(dods_int32 i1, dods_int32 i2, int op)
{
    switch (op) {
      case EQUAL:
	return i1 == i2;
      case NOT_EQUAL:
	return i1 != i2;
      case GREATER:
	return i1 > i2;
      case GREATER_EQL:
	return i1 >= i2;
      case LESS:
	return i1 < i2;
      case LESS_EQL:
	return i1 <= i2;
      case REGEXP:
	cerr << "Regular expression not valid for integer values" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

// Some machines define MAX, some don't. 11/1/98 jhrg

#ifndef MAX
static unsigned
MAX(int i1, int i2)
{
    return (unsigned)((i1 < i2) ? i1 : i2);
}
#endif

bool
int_ops(dods_uint32 i1, dods_int32 i2, int op)
{
    switch (op) {
      case EQUAL:
	return i1 == MAX(0, i2);
      case NOT_EQUAL:
	return i1 != MAX(0, i2);
      case GREATER:
	return i1 > MAX(0, i2);
      case GREATER_EQL:
	return i1 >= MAX(0, i2);
      case LESS:
	return i1 < MAX(0, i2);
      case LESS_EQL:
	return i1 <= MAX(0, i2);
      case REGEXP:
	cerr << "Regular expression not valid for integer values" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
int_ops(dods_int32 i1, dods_uint32 i2, int op)
{
    switch (op) {
      case EQUAL:
	return MAX(0, i1) == i2;
      case NOT_EQUAL:
	return MAX(0, i1) != i2;
      case GREATER:
	return MAX(0, i1) > i2;
      case GREATER_EQL:
	return MAX(0, i1) >= i2;
      case LESS:
	return MAX(0, i1) < i2;
      case LESS_EQL:
	return MAX(0, i1) <= i2;
      case REGEXP:
	cerr << "Regular expression not valid for integer values" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
int_ops(dods_uint32 i1, dods_uint32 i2, int op)
{
    switch (op) {
      case EQUAL:
	return i1 == i2;
      case NOT_EQUAL:
	return i1 != i2;
      case GREATER:
	return i1 > i2;
      case GREATER_EQL:
	return i1 >= i2;
      case LESS:
	return i1 < i2;
      case LESS_EQL:
	return i1 <= i2;
      case REGEXP:
	cerr << "Regular expression not valid for integer values" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
double_ops(double i1, double i2, int op)
{
    switch (op) {
      case EQUAL:
	return i1 == i2;
      case NOT_EQUAL:
	return i1 != i2;
      case GREATER:
	return i1 > i2;
      case GREATER_EQL:
	return i1 >= i2;
      case LESS:
	return i1 < i2;
      case LESS_EQL:
	return i1 <= i2;
      case REGEXP:
	cerr << "Regular expression not valid for float values" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
string_ops(String &i1, String &i2, int op)
{
    switch (op) {
      case EQUAL:
	return i1 == i2;
      case NOT_EQUAL:
	return i1 != i2;
      case GREATER:
	return i1 > i2;
      case GREATER_EQL:
	return i1 >= i2;
      case LESS:
	return i1 < i2;
      case LESS_EQL:
	return i1 <= i2;
      case REGEXP: {
	  Regex r((const char *)i2);
	  return i1.matches(r);
      }
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}
