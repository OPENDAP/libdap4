
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


// Utility functions used by the api.
//
// jhrg 9/21/94

// $Log: util.cc,v $
// Revision 1.62  2000/09/21 16:22:10  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.61  2000/07/10 00:19:32  rmorris
// Removed (trivial) explicit mention of namespace for iostream in
// flush_stream().
//
// Revision 1.60  2000/07/09 22:05:37  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.58  2000/06/16 18:50:19  jimg
// Fixes leftover from the last merge plus needed for the merge with version
// 3.1.7.
//
// Revision 1.57  2000/06/16 18:15:01  jimg
// Merged with 3.1.7
//
// Revision 1.55.4.1  2000/06/14 16:59:54  jimg
// Added a new varient of unique_names for DLLists.
//
// Revision 1.56  2000/06/07 18:07:01  jimg
// Merged the pc port branch
//
// Revision 1.55.18.1  2000/06/02 18:39:04  rmorris
// Mod's for port to win32.
//
// Revision 1.55  1999/05/21 17:21:21  jimg
// Removed a bogus error message about failure to run deflate.
//
// Revision 1.54  1999/05/04 19:47:24  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.53  1999/04/29 03:04:52  jimg
// Merged ferret changes
//
// Revision 1.52  1999/04/29 02:29:37  jimg
// Merge of no-gnu branch
//
// Revision 1.51.4.1  1999/04/21 21:56:25  jimg
// Work-around in prune_spaces(). There is a bug in the old code and I
// patched it be calling the new (string based) version. Since we're
// switching over to that anyway, it seemed the kludge will be temporary.
//
// Revision 1.51  1999/03/24 23:23:44  jimg
// Removed the *_ops code. Those operations are now done by the template
// classes and function(s) in Operators.h.
// Removed old code.
//
// Revision 1.50  1999/03/09 00:23:16  jimg
// Fixed the error messages in compressor().
//
// Revision 1.55.12.1  2000/03/08 00:09:04  jgarcia
// replace ostrstream with string;added functions to convert from double and
// long to string
//
// Revision 1.49  1999/01/21 02:10:57  jimg
// Moved various CE functions to the file ce_functions.cc/.h.
//
// Revision 1.48  1998/11/10 00:47:01  jimg
// Fixed a number of memory leaks (found using purify).
//
// Revision 1.47  1998/09/02 23:59:11  jimg
// Removed func_date to avoid conflicts with a copy in ff-dods-2.15.
//
// Revision 1.46.6.2  1999/02/05 09:32:37  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code.
//
// Revision 1.46.6.1  1999/02/02 21:57:08  jimg
// String to string version
//
// Revision 1.46  1998/04/07 22:12:49  jimg
// Added prune_spaces(String) function. This can be used to remove leading
// spaces from URLs and their embedded CEs (i.e., spaces between the ? and
// the start of the CE). Doing this before passing the URL/CE into the web
// library prevents various nasty crashes. This function is called by
// Connect's default ctor. Users of Connect don't have to call it.
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
// Added dods_progress() function that looks for an environment variable to
// see if the user wants the progress indicator visible. Thus, they can use
// one environment variable to control the GUI as a whole or this one to
// control just the progress indicator.
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
// Added not_used to rcsid to suppress warnings from g++ -Wall. Fixed a bug
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

static char rcsid[] not_used = {"$Id: util.cc,v 1.62 2000/09/21 16:22:10 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <errno.h>
#include <time.h>
#include <ctype.h>
#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifdef DBMALLOC
#include <dbmalloc.h>
#endif

#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include <SLList.h>
#include <DLList.h>
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

#ifdef WIN32
using std::cerr;
using std::endl;
using std::ends;
using std::sort;
using std::ostrstream;
#endif

const char DODS_CE_PRX[]={"dods"};

// Remove spaces from the start of a URL and from the start of any constraint
// expression it contains. 4/7/98 jhrg

string 
prune_spaces(string name)
{
    // If the URL does not even have white space return.
    if (name.find_first_of(' ')==name.npos)
	return name;
    else {
	// Strip leading spaces from http://...
	unsigned int i = name.find_first_not_of(' ');
	name = name.substr(i);
	
	// Strip leading spaces from constraint part (following `?').
	unsigned int j = name.find('?') + 1;
	i = name.find_first_not_of(' ', j);
	name.erase(j, i-j);

	return name;
    }
}

// Compare elements in a SLList of (BaseType *)s and return true if there are
// no duplicate elements, otherwise return false.

bool
unique_names(SLList<BaseType *> l, const string &var_name, 
	     const string &type_name, string &msg)
{
    // copy the identifier names to a vector
    vector<string> names(l.length());

    int nelem = 0;
    for (Pix p = l.first(); p; l.next(p)) {
	assert(l(p));
	names[nelem++] = l(p)->name();
	DBG(cerr << "NAMES[" << nelem-1 << "]=" << names[nelem-1] << endl);
    }
    
    // sort the array of names
    sort(names.begin(), names.end());
	
#ifdef DODS_DEBUG2
    cout << "unique:" << endl;
    for (int ii = 0; ii < nelem; ++ii)
	cout << "NAMES[" << ii << "]=" << names[ii] << endl;
#endif
    
    // sort the array of names
    sort(names.begin(), names.end());
	
#ifdef DODS_DEBUG2
    cout << "unique:" << endl;
    for (int ii = 0; ii < nelem; ++ii)
	cout << "NAMES[" << ii << "]=" << names[ii] << endl;
#endif
    
    // look for any instance of consecutive names that are ==
    int i;
    for (i = 1; i < nelem; ++i)
	if (names[i-1] == names[i]) {
	    ostrstream oss;
	    oss << "The variable `" << names[i] 
		 << "' is used more than once in " << type_name << " `"
		 << var_name << "'" << ends;
	    msg = oss.str();
	    oss.rdbuf()->freeze(0);
	    return false;
	}

    return true;
}

bool
unique_names(DLList<BaseType *> l, const string &var_name, 
	     const string &type_name, string &msg)
{
    // copy the identifier names to a vector
    vector<string> names(l.length());

    int nelem = 0;
    for (Pix p = l.first(); p; l.next(p)) {
	assert(l(p));
	names[nelem++] = l(p)->name();
	DBG(cerr << "NAMES[" << nelem-1 << "]=" << names[nelem-1] << endl);
    }
    
    // sort the array of names
    sort(names.begin(), names.end());
	
#ifdef DODS_DEBUG2
    cout << "unique:" << endl;
    for (int ii = 0; ii < nelem; ++ii)
	cout << "NAMES[" << ii << "]=" << names[ii] << endl;
#endif
    
    // look for any instance of consecutive names that are ==
    int i;
    for (i = 1; i < nelem; ++i)
	if (names[i-1] == names[i]) {
	    msg="";
	    msg+="The variable `"+names[i]+"' is used more than once in "+type_name+" `";
	    msg+=var_name+"'\n";
	    return false;
	}

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
// XDR * and a string reference.
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
xdr_str(XDR *xdrs, string &buf)
{
    DBG(cerr << "In xdr_str, xdrs: " << xdrs << endl);

    switch (xdrs->x_op) {
      case XDR_ENCODE: {	// BUF is a pointer to a (string *)
	const char *out_tmp = buf.c_str();

	return xdr_string(xdrs, (char **)&out_tmp, max_str_len);
      }

      case XDR_DECODE: {
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

#ifdef WIN32
    string deflate = (string)dods_root() + "\\bin\\deflate";
#else
    string deflate = (string)dods_root() + "/etc/deflate";
#endif

    // Check that the file exists...
    // First look for deflate using DODS_ROOT (compile-time constant subsumed
    // by an environment variable) and if that fails in the CWD which finds
    // the program when it is in the same directory as the dispatch script
    // and other server components. 2/11/98 jhrg
    status = (stat(deflate.c_str(), &buf) == 0)
#ifdef WIN32
	|| (stat(".\\deflate", &buf) == 0);
#else
	|| (stat("./deflate", &buf) == 0);
#endif

    // and that it can be executed.
#ifdef WIN32
    status &= (buf.st_mode & _S_IEXEC);
#else
    status &= buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH);
#endif
    DBG(cerr << " returning " << (status ? "true." : "false.") << endl);
    return (status != 0);
}

// Note that decompression is handled automatically by libwww 5.1. 2/10/1998
// jhrg

FILE *
compressor(FILE *output, int &childpid)
{
//  There is no such thing as a "fork" under win32.  This makes it so that
//  we have to juggle handles more aggressively.  This code hasn't been tested
//  and shown to work as of 07/2000.
#ifdef WIN32
	int pid, data[2];
	int hStdIn,hStdOut;

	if(_pipe(data, 512, O_BINARY | O_NOINHERIT) < 0) {
	cerr << "Could not create IPC channel for compressor process" 
	     << endl;
	return NULL;
    }

	/*****************************************************************************************/
	/*  This sets up for the child process, but it has to be reversed for the parent         */
	/*  after the spawn takes place.                                                         */

	hStdIn  = _dup(_fileno(stdin));  //  Store stdin, stdout so we have sometime to restore to
	hStdOut = _dup(_fileno(stdout));

	if(_dup2(data[0], _fileno(stdin)) != 0) {  //  Child is to read from read end of pipe
		cerr << "dup of child stdin failed" << endl;
		return NULL;
	}
	if(_dup2(_fileno(output), _fileno(stdout)) != 0) {  //  Child is to write its's stdout to file
		cerr << "dup of child stdout failed" << endl;
		return NULL;
	}
	/****************************************************************************************/
	
	//  Spawn child process
	string deflate = "deflate.exe";
	if((pid = _spawnlp(_P_NOWAIT,deflate.c_str(),deflate.c_str(), "-c", "5", "-s", NULL)) < 0) {
	cerr << "Could not spawn to create compressor process" << endl;
	return NULL;
	}

	//  Restore stdin, stdout for parent and close duplicate copies
	if(_dup2(hStdIn, _fileno(stdin)) != 0) {
		cerr << "dup of stdin failed" << endl;
		return NULL;
	}
	if(_dup2(hStdOut, _fileno(stdout)) != 0) {
		cerr << "dup of stdout failed" << endl;
		return NULL;
	}
	close(hStdIn);
	close(hStdOut);

	//  Tell the parent that it reads from the opposite end of the
	//  place where the child writes.
	close(data[0]);
	FILE *input = fdopen(data[1], "w");
	setbuf(input, 0);
	childpid = pid;
	return input;

#else
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
	string deflate = (string)dods_root() + "/etc/deflate";
	(void) execl(deflate.c_str(), "deflate", "-c",  "5", "-s", NULL);
	(void) execl("./deflate", "deflate", "-c",  "5", "-s", NULL);
	cerr << "Warning: Could not start compressor!" << endl;
	cerr << "defalte should be in DODS_ROOT/etc or in the CWD!" 
	     << endl;
	_exit(127);		// Only here if an error occurred.
    }
#endif
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

void downcase(string &s) {
  for(unsigned int i=0; i<s.length(); i++)
    s[i] = tolower(s[i]);
}

#ifdef WIN32
//  Sometime need to buffer within an iostream under win32 when
//  we want the output to go to a FILE *.  This is because
//  it's not possible to associate an ofstream with a FILE *
//  under the Standard ANSI C++ Library spec.  Unix systems
//  don't follow the spec in this regard.
void flush_stream(iostream ios, FILE *out)
{
	int nbytes;
	char buffer[512];

	ios.get(buffer,512,NULL);
	while((nbytes = ios.gcount()) > 0)
		{
		fwrite(buffer, 1, nbytes, out);
		ios.get(buffer,512,NULL);
		}
	return;	
}
#endif

// Jose Garcia
void append_long_to_string(long val, int base, string &str_val)                         
{
  // The array digits contains 36 elements which are the 
  // posible valid digits for out bases in the range
  // [2,36]
  char digits[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  // result of val / base  
  ldiv_t r;                                
  
  if (base > 36 || base < 2)
    {
      // no conversion if wrong base 
      std::invalid_argument ex("The parameter base has an invalid value.");
      throw ex;
    }
  if (val < 0)
    str_val+= '-';
  r = ldiv (labs(val), base);

  // output digits of val/base first 
  if (r.quot > 0)
    append_long_to_string (r.quot, base, str_val);
  
  // output last digit 
  
  str_val+= digits[(int)r.rem];
}

// Jose Garcia
void append_double_to_string(const double &num, string &str)
{
  // s having 100 characters should be enough for sprintf to do its job.
  char s[80];
  sprintf(s, "%.9f", num);
  str+=s;
}
