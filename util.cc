
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


// Utility functions used by the api.
//
// jhrg 9/21/94

// $Log: util.cc,v $
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

static char rcsid[] __unused__ = {"$Id: util.cc,v 1.29 1996/11/21 23:56:21 jimg Exp $"};

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#ifdef DBMALLOC
#include <dbmalloc.h>
#endif

#include <SLList.h>

#include "BaseType.h"
#include "Str.h"
#include "Url.h"
#include "List.h"
#include "Sequence.h"
#include "Error.h"
#include "parser.h"
#include "expr.tab.h"
#include "util.h"

#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

static const char *dods_root = getenv("DODS_ROOT") ? getenv("DODS_ROOT") 
    : DODS_ROOT;

const char DODS_CE_PRX[]={"dods"};

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

    int nelem = 0;
    String s;
    for (Pix p = l.first(); p; l.next(p)) {
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
// Also note that #max_str_len# is an obese number but that really does not
// matter; #xdr_string# would never actually allocate taht much memory unless
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

    fputs((const char *)text, fp); // dump information

    if (fseek(fp, 0L, 0 == -1)) { // rewind in preparation for reading
	cerr << "Could not rewind anonymous temporary file: "
	    << strerror(errno) << endl;
	return NULL;
    }

    return fp;
}

FILE *
compressor(FILE *output)
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
	if (!input) {
	    cerr << "Parent process could not open channel for compression"
		 << endl;
	    return NULL;
	}
	return input;
    }
    else {
	close(data[1]);
	dup2(data[0], 0);	// Read from the pipe...
	dup2(fileno(output), 1); // Write to the FILE *output.

	DBG2(cerr << "Opening compression stream." << endl);

	// First try to run gzip using DODS_ROOT (the value read from the
	// DODS_ROOT environment variable takes precedence over the value set
	// at build time. If that fails, try the users PATH.
	String gzip = (String)dods_root + "/etc/gzip";
	(void) execl(gzip, "gzip", "-cf", NULL);

	(void) execlp("gzip", "gzip", "-cf", NULL);

	cerr << "Could not start compressor!" << endl;
	cerr << "gzip must be in DODS_ROOT/etc or on your PATH" << endl;
	_exit(127);		// Only get here if an error
    }
}

FILE *
decompressor(FILE *input)
{
    int pid, data[2];

    if (pipe(data) < 0) {
	cerr << "Could not create IPC channel for decompresser process" 
	     << endl;
	return NULL;
    }
    
    if ((pid = fork()) < 0) {
	cerr << "Could not fork to create decompresser process" << endl;
	return NULL;
    }

    // The parent process closes the write end of the Pipe, and creates a
    // FILE * using fdopen(). The FILE * is used by the calling program to
    // access the read end of the Pipe.

    if (pid > 0) {
	close(data[1]);
	FILE *output = fdopen(data[0], "r");
	if (!output) {
	    cerr << "Parent process could not open channel for decompression"
		 << endl;
	    return NULL;
	}
	return output;
    }
    else {
	DBG2(cerr << "Sleep returned: " << sleep(20) << endl);

	close(data[0]);
	dup2(fileno(input), 0);	// Read from FILE *input 
	dup2(data[1], 1);	// Write to the pipe

	DBG(cerr << "Opening decompression stream." << endl);

	// First try to run gzip using DODS_ROOT (the value read from the
	// DODS_ROOT environment variable takes precedence over the value set
	// at build time. If that fails, try the users PATH.
	String gzip = (String)dods_root + "/etc/gzip";
	(void) execl(gzip, "gzip", "-cdf", NULL);

	(void) execlp("gzip", "gzip", "-cdf", NULL);

	cerr << "Could not start decompresser!" << endl;
	cerr << "gzip must be in DODS_ROOT/etc or on your PATH" << endl;
	_exit(127);		// Only get here if an error
    }
}

// This function prints the system time to the ostream object.

static const int TimLen = 26;	// length of string from asctime()

ostream &
systime(ostream &os)
{
    time_t TimBin;
    char TimStr[TimLen];

    if (time(&TimBin) == (time_t)-1)
	strcpy(TimStr, "time() error           ");
    else {
	strcpy(TimStr, ctime(&TimBin));
	TimStr[TimLen - 2] = '\0'; // overwrite the \n 
    }

    return os << TimStr;
}

// These functions are used by the CE evaluator

bool
func_member(int argc, BaseType *argv[])
{
    if (argc != 2) {
	cerr << "Wrong number of arguments." << endl;
	return false;
    }
    
    switch(argv[0]->type()) {
      case dods_list_c: {
	List *var = (List *)argv[0];
	BaseType *btp = (BaseType *)argv[1];
	bool result = var->member(btp);
    
	return result;
      }
      
      default:
	cerr << "Wrong argument type." << endl;
	return false;
    }

}

bool
func_null(int argc, BaseType *argv[])
{
    if (argc != 1) {
	cerr << "Wrong number of arguments." << endl;
	return false;
    }
    
    switch(argv[0]->type()) {
      case  dods_list_c: {
	List *var = (List *)argv[0];
	bool result = var->null();
    
	return result;
      }

      default:
	cerr << "Wrong argument type." << endl;
	return false;
    }

}

BaseType *
func_length(int argc, BaseType *argv[])
{
    if (argc != 1) {
	cerr << "Wrong number of arguments." << endl;
	return 0;
    }
    
    switch (argv[0]->type()) {
      case dods_list_c: {
	  List *var = (List *)argv[0];
	  dods_int32 result = var->length();
    
	  BaseType *ret = (BaseType *)NewInt32("constant");
	  ret->val2buf(&result);
	  ret->set_read_p(true);
	  ret->set_send_p(true);

	  return ret;
      }

      case dods_sequence_c: {
	  Sequence *var = (Sequence *)argv[0];
	  dods_int32 result = var->length();
    
	  BaseType *ret = (BaseType *)NewInt32("constant");
	  ret->val2buf(&result);
	  ret->set_read_p(true);
	  ret->set_send_p(true);
    
	  return ret;
      }

      default:
	cerr << "Wrong type argument to list operator `member'" << endl;
	return 0;
    }
}

BaseType *
func_nth(int argc, BaseType *argv[])
{
    if (argc != 2) {
	cerr << "Wrong number of arguments." << endl;
	return 0;
    }
    
    switch (argv[0]->type()) {
	case dods_list_c: {
	    if (argv[1]->type() != dods_int32_c) {
		cerr << "Second argument to NTH must be an integer." << endl;
		return 0;
	    }
	    List *var = (List *)argv[0];
	    dods_int32 n;
	    dods_int32 *np = &n;
	    argv[1]->buf2val((void **)&np);

	    return var->var(n);
	}

      default:
	cerr << "Wrong type argument to list operator `nth'" << endl;
	return 0;
    }
}

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
	cerr << "Regexp not valid for byte values" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
int_ops(int i1, int i2, int op)
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
	cerr << "Regexp not valid for byte values" << endl;
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
	cerr << "Regexp not valid for float values" << endl;
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
	  Regex r = (const char *)i2;
	  return i1.matches(r);
      }
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}
