
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
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Utility functions used by the api.
//
// jhrg 9/21/94

#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#ifndef TM_IN_SYS_TIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "BaseType.h"
#include "Str.h"
#include "Url.h"
#include "Sequence.h"
#include "Error.h"
#include "parser.h"
#include "expr.tab.h"
#include "util.h"
#include "debug.h"


using namespace std;

// Remove spaces from the start of a URL and from the start of any constraint
// expression it contains. 4/7/98 jhrg

/** Removed spaces from the front of a URL and also from the front of the CE.
    This function assumes that there are no holes in both the URL and the CE.
    It will remove \i leading space, but not other spaces.

    @todo Is this still needed? This function may predate the switch from
    libwww to libcurl and the latter may not need to have spaces removed.
    @param name The URL to process
    @return Returns a new string object that contains the pruned URL. */
string 
prune_spaces(const string &name)
{
    // If the URL does not even have white space return.
    if (name.find_first_of(' ') == name.npos)
	return name;
    else {
	// Strip leading spaces from http://...
	unsigned int i = name.find_first_not_of(' ');
	string tmp_name = name.substr(i);
	
	// Strip leading spaces from constraint part (following `?').
	unsigned int j = tmp_name.find('?') + 1;
	i = tmp_name.find_first_not_of(' ', j);
	tmp_name.erase(j, i-j);

	return tmp_name;
    }
}

// Compare elements in a list of (BaseType *)s and return true if there are
// no duplicate elements, otherwise return false.

bool
unique_names(vector<BaseType *> l, const string &var_name, 
	     const string &type_name, string &msg)
{
    // copy the identifier names to a vector
    vector<string> names(l.size());

    int nelem = 0;
    typedef std::vector<BaseType *>::const_iterator citer ;
    for (citer i = l.begin(); i != l.end(); i++)
    {
	assert(*i);
	names[nelem++] = (*i)->name();
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
    for (int j = 1; j < nelem; ++j)
    {
	if (names[j-1] == names[j]) {
	    ostringstream oss;
	    oss << "The variable `" << names[j] 
		 << "' is used more than once in " << type_name << " `"
		 << var_name << "'";
	    msg = oss.str();

	    return false;
	}
    }

    return true;
}

// This function is used to allocate memory for, and initialize, a new XDR
// pointer. It sets the stream associated with the (XDR *) to STREAM.
//
// NB: STREAM is not one of the C++/libg++ iostream classes; it is a (FILE
// *).

//  These func's moved to xdrutil_ppc.* under the PPC as explained there
#ifndef __POWERPC__
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

    delete xdr; xdr = 0;
}
#endif

// This function is used to en/decode Str and Url type variables. It is
// defined as extern C since it is passed via function pointers to routines
// in the xdr library where it is executed. This function is defined so
// that Str and Url have an en/decoder which takes exactly two argumnets: an
// XDR * and a string reference.
//
// NB: this function is *not* used for arrays (i.e., it is not the function
// referenced by BaseType's _xdr_coder field when the object is a Str or Url.
// Also note that \e max_str_len is an obese number but that really does not
// matter; xdr_string() would never actually allocate that much memory unless
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
libdap_root()
{
    char *libdap_root = 0;
    return ((libdap_root = getenv("LIBDAP_ROOT")) ? libdap_root : LIBDAP_ROOT);
}

extern "C"
const char *
libdap_version()
{
    return PACKAGE_VERSION;
}

extern "C"
const char *
libdap_name()
{
    return PACKAGE_NAME;
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
    string deflate = (string)libdap_root() + "\\bin\\deflate";
#else
    string deflate = (string)libdap_root() + "/sbin/deflate";
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

FILE *
compressor(FILE *output, int &childpid)
{
#ifdef WIN32
    //  There is no such thing as a "fork" under win32. This makes it so that
    //  we have to juggle handles more aggressively. This code hasn't been
    //  tested and shown to work as of 07/2000.
    int pid, data[2];
    int hStdIn,hStdOut;

    if(_pipe(data, 512, O_BINARY | O_NOINHERIT) < 0) {
	cerr << "Could not create IPC channel for compressor process" 
	     << endl;
	return NULL;
    }


    // This sets up for the child process, but it has to be reversed for the
    // parent after the spawn takes place.

    // Store stdin, stdout so we have something to restore to
    hStdIn  = _dup(_fileno(stdin));  
    hStdOut = _dup(_fileno(stdout));

    // Child is to read from read end of pipe
    if(_dup2(data[0], _fileno(stdin)) != 0) {
	cerr << "dup of child stdin failed" << endl;
	return NULL;
    }
    // Child is to write its's stdout to file
    if(_dup2(_fileno(output), _fileno(stdout)) != 0) {
	cerr << "dup of child stdout failed" << endl;
	return NULL;
    }
	
    // Spawn child process
    string deflate = "deflate.exe";
    if((pid = _spawnlp(_P_NOWAIT, deflate.c_str(), deflate.c_str(), 
		       "-c", "5", "-s", NULL)) < 0) {
	cerr << "Could not spawn to create compressor process" << endl;
	return NULL;
    }

    // Restore stdin, stdout for parent and close duplicate copies
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

    // Tell the parent that it reads from the opposite end of the
    // place where the child writes.
    close(data[0]);
    FILE *input = fdopen(data[1], "w");
    setbuf(input, 0);
    childpid = pid;
    return input;

#else
    FILE *ret_file = NULL ;

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

    if (pid > 0) { 		// Parent, pid is that of the child
	close(data[0]);
	ret_file = fdopen(data[1], "w");
	setbuf(ret_file, 0);
	childpid = pid;
    }
    else {			// Child
	close(data[1]);
	dup2(data[0], 0);	// Read from the pipe...
	dup2(fileno(output), 1); // Write to the FILE *output.

	DBG(cerr << "Opening compression stream." << endl);

	// First try to run deflate using DODS_ROOT (the value read from the
	// DODS_ROOT environment variable takes precedence over the value set
	// at build time. If that fails, try the CWD.
	string deflate = (string)libdap_root() + "/sbin/deflate";
	(void) execl(deflate.c_str(), "deflate", "-c",  "5", "-s", NULL);
	(void) execl("./deflate", "deflate", "-c",  "5", "-s", NULL);
	cerr << "Warning: Could not start compressor!" << endl;
	cerr << "defalte should be in DODS_ROOT/etc or in the CWD!" 
	     << endl;
	_exit(127);		// Only here if an error occurred.
    }

    return ret_file ;
#endif
}

// This function returns a pointer to the system time formated for an httpd
// log file.

string
systime()
{
    time_t TimBin;

    if (time(&TimBin) == (time_t)-1)
	return string("time() error");
    else {
	string TimStr = ctime(&TimBin);
	return TimStr.substr(0, TimStr.size() - 2); // remove the \n 
    }
}

void 
downcase(string &s) 
{
    for(unsigned int i=0; i<s.length(); i++)
	s[i] = tolower(s[i]);
}

#ifdef WIN32
//  Sometimes need to buffer within an iostream under win32 when
//  we want the output to go to a FILE *.  This is because
//  it's not possible to associate an ofstream with a FILE *
//  under the Standard ANSI C++ Library spec.  Unix systems
//  don't follow the spec in this regard.
void flush_stream(iostream ios, FILE *out)
{
    int nbytes;
    char buffer[512];

    ios.get(buffer,512,NULL);
    while((nbytes = ios.gcount()) > 0) {
	fwrite(buffer, 1, nbytes, out);
	ios.get(buffer,512,NULL);
    }

    return;	
}
#endif

// Jose Garcia
void 
append_long_to_string(long val, int base, string &str_val) 
{
    // The array digits contains 36 elements which are the 
    // posible valid digits for out bases in the range
    // [2,36]
    char digits[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // result of val / base  
    ldiv_t r;                                
  
    if (base > 36 || base < 2) {
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

// base defaults to 10
string
long_to_string(long val, int base)
{
    string s;
    append_long_to_string(val, base, s);
    return s;
}

// Jose Garcia
void append_double_to_string(const double &num, string &str)
{
    // s having 100 characters should be enough for sprintf to do its job.
    // I want to banish all instances of sprintf. 10/5/2001 jhrg
    ostringstream oss;
    oss.precision(9);
    oss << num;
    str += oss.str();
}

string
double_to_string(const double &num)
{
    string s;
    append_double_to_string(num, s);
    return s;
}

// Get the version number of the core software. Defining this means that
// clients of the DAP don't have to rely on config.h for the version
// number. 
string
dap_version()
{
    return (string)"OPeNDAP DAP/" + libdap_version() + ": compiled on " + __DATE__ + ":" + __TIME__ ;
}

// Given a pathname, return the file at the end of the path. This is used
// when reporting errors (maybe other times, too) to keep the server from
// revealing too much about its organization when sending error responses
// back to clients. 10/11/2000 jhrg
// MT-safe. 08/05/02 jhrg

#ifdef WIN32
static const char path_sep[]={"\\"};
#else
static const char path_sep[]={"/"};
#endif

string
path_to_filename(string path)
{
    string::size_type pos = path.rfind(path_sep);
  
    return (pos == string::npos) ? path : path.substr(++pos);
}

// Look around for a reasonable place to put a temporary file. Check first
// the value of the TMPDIR env var. If that does not yeild a path that's
// writable (as defined by access(..., W_OK|R_OK)) then look at P_tmpdir (as
// defined in stdio.h. If both come up empty, then use `./'.
//
// This function allocates storage using new. The caller must delete the char
// array. 
char *
get_tempfile_template(char *file_template)
{
    char *c;
#ifdef WIN32
    if (getenv("TEMP") && (access(getenv("TEMP"), 6) == 0))
	c = getenv("TEMP");
    else if (getenv("TMP"))
	c = getenv("TMP");
#else
    if (getenv("TMPDIR") && (access(getenv("TMPDIR"), W_OK|R_OK) == 0)) 
	c = getenv("TMPDIR");
#ifdef P_tmpdir
    else if (access(P_tmpdir, W_OK|R_OK) == 0)
	c = P_tmpdir;
#endif
#endif
    else 
	c = ".";

    char *temp = new char[strlen(c) + strlen(file_template) + 2];
    strcpy(temp, c);
    strcat(temp, "/");

    strcat(temp, file_template);

    return temp;
}

/** Intended for testing, this may have other uses. The template should be
    the pathname of the temporary file ending in 'XXXXXX' (as for mkstemp)
    and will be modified.
    @param temp Pathname, ending in 'XXXXXX'
    @return A FILE pointer opened for update. */
#ifndef WIN32
FILE *
get_temp_file(char *temp)
{
    int fd = mkstemp(temp);
    if (fd < 0)
	return 0;
    FILE *tmp = fdopen(fd, "a+");
    return tmp;
}
#endif

/** Read stuff from a file and dump it into a string. This assumes the file
    holds character data only. Intended for testing...
    @param fp Read from this file
    @return Returns a string which holds the character data. */
string
file_to_string(FILE *fp)
{
    rewind(fp);
    ostringstream oss;
    char c;
    while (fread(&c, 1, 1, fp))
	oss << c;
    return oss.str();
}

// $Log: util.cc,v $
// Revision 1.84  2005/05/17 17:37:55  pwest
// updated dap_version using libdap_version
//
// Revision 1.83  2005/04/22 23:31:40  jimg
// Version 3.5.0 builds; passes most tests
//
// Revision 1.82  2004/07/19 07:23:23  rmorris
// Remove use of function under win32 via #ifdef.  It uses mkstemp.
//
// Revision 1.81  2004/07/07 21:08:49  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.76.2.5  2004/07/06 11:18:33  rmorris
// Have temp names under win32 go under TEMP or TMP. Note that under Apache
// there are no such env vars under win32 as apache doesn't get them from the
// environment (system or user). As a result we use TMP or TEMP user env vars
// on the client side and TMP or TEMP as setup via hard code in perl on the
// server side.
//
// Revision 1.80  2004/06/28 17:01:48  pwest
// Adding compile date and time to version
//
// Revision 1.79  2003/12/10 21:11:58  jimg
// Merge with 3.4. Some of the files contains erros (some tests fail). See
// the ChangeLog for information about fixes.
//
// Revision 1.78  2003/12/08 18:02:31  edavis
// Merge release-3-4 into trunk
//
// Revision 1.76.2.4  2003/09/06 23:02:50  jimg
// Updated the documentation.
//
// Revision 1.76.2.3  2003/07/25 06:04:28  jimg
// Refactored the code so that DDS:send() is now incorporated into
// DODSFilter::send_data(). The old DDS::send() is still there but is
// depracated.
// Added 'smart timeouts' to all the variable classes. This means that
// the new server timeouts are active only for the data read and CE
// evaluation. This went inthe BaseType::serialize() methods because it
// needed to time both the read() calls and the dds::eval() calls.
//
// Revision 1.76.2.2  2003/06/23 01:31:19  rmorris
// Migrated new_xdrstdio, set_xdrstdio and delete_xdrstdio conditionally
// out into a separate (C) source file for use in the case of the POWERPC/OSX.
//
// Revision 1.76.2.1  2003/06/05 20:15:26  jimg
// Removed many uses of strstream and replaced them with stringstream.
//
// Revision 1.77  2003/05/23 03:24:58  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.76  2003/04/22 19:40:29  jimg
// Merged with 3.3.1.
//
// Revision 1.75  2003/02/27 23:17:56  jimg
// Changed prune_spaces() so taht it takes a const string &.
//
// Revision 1.74  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.73.2.1  2003/02/21 00:10:08  jimg
// Repaired copyright.
//
// Revision 1.73  2003/01/23 00:22:25  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.72  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.65.2.19  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.65.2.18  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.65.2.17  2002/10/23 17:40:39  jimg
// Added compile-time switch for time.h versus sys/time.h.
//
// Revision 1.65.2.16  2002/10/18 22:52:00  jimg
// Combined some win32 #ifdefs and removed an unused global variable.
//
// Revision 1.65.2.15  2002/09/22 14:34:20  rmorris
// VC++ considers 'x' in 'for(int x,...)' to not be just for that scope of the
// block associated with that for.  When there are multiple of such type of
// thing - VC++ see redeclarations of the same var - moved to use different
// var names to prevent the error.
//
// Revision 1.65.2.14  2002/09/05 22:52:55  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.65.2.13  2002/08/22 21:23:23  jimg
// Fixes for the Win32 Build made at ESRI by Vlad Plenchoy and myslef.
//
// Revision 1.65.2.12  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.65.2.11  2002/08/06 23:22:33  jimg
// Removed systime() because it's not used by the library and it's not
// MT-Safe. If this is used by any of the servers, I'll recode the function
// so that it returns a string.
//
// Revision 1.71  2002/06/03 22:21:16  jimg
// Merged with release-3-2-9
//
// Revision 1.65.2.10  2002/02/04 00:35:33  rmorris
// Ported some newer code to win32.  In win32 W_OK, etc are not available -
// a digit is just used per MSDN.  Also had it look for the TEMP environment
// var under win32 instead of the TMPDIR env var under unix.
//
// Revision 1.65.2.9  2002/01/28 20:34:25  jimg
// *** empty log message ***
//
// Revision 1.70  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.65.2.8  2001/10/08 16:55:59  jimg
// I replaced the sprintf-based implementation of append_double_to_string with
// one based on ostrstreams. sprintf was causing too many problems in other
// places.
//
// Revision 1.69  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.65.2.7  2001/09/25 20:13:39  jimg
// Added debug.h include.
//
// Revision 1.68  2001/08/24 17:46:23  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.65.2.6  2001/08/17 23:59:14  jimg
// Removed WIN32 compile guards from using statements.
// Changed some formatting to comply with K&R style.
//
// Revision 1.65.2.5  2001/07/28 01:10:42  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.67  2001/06/15 23:49:04  jimg
// Merged with release-3-2-4.
//
// Revision 1.65.2.4  2001/05/31 17:12:36  jimg
// Fixed dods_progress().
//
// Revision 1.65.2.3  2001/05/25 20:53:18  jimg
// Changed the environment variable read by dods_progress(). I doubt anyone
// uses this function, but just in case...
//
// Revision 1.65.2.2  2001/05/04 21:59:58  jimg
// Ripped out the (lame) tests and moved them to the (less lame)
// generalUtilTest.cc unit tests.
//
// Revision 1.66  2001/01/26 19:48:10  jimg
// Merged with release-3-2-3.
//
// Revision 1.65.2.1  2000/11/30 05:24:46  jimg
// Significant changes and improvements to the AttrTable and DAS classes. DAS
// now is a child of AttrTable, which makes attributes behave uniformly at
// all levels of the DAS object. Alias now work. I've added unit tests for
// several methods in AttrTable and some of the functions in parser-util.cc.
// In addition, all of the DAS tests now work.
//
// Revision 1.65  2000/10/30 17:21:28  jimg
// Added support for proxy servers (from cjm).
//
// Revision 1.64  2000/10/03 21:03:22  jimg
// Added dap_version() function
//
// Revision 1.63  2000/09/22 02:17:23  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
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
// Failed to change `config.h' to `config.h' in these files.
//
// Revision 1.10  1995/05/10  13:45:46  jimg
// Changed the name of the configuration header file from `config.h' to
// `config.h' so that other libraries could have header files which were
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
