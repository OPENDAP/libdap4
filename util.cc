
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

static char rcsid[] not_used =
    {"$Id$"
    };

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
#include "util.h"
#include "GNURegex.h"
#include "debug.h"


using namespace std;

namespace libdap {

// Remove spaces from the start of a URL and from the start of any constraint
// expression it contains. 4/7/98 jhrg

/** Removed spaces from the front of a URL and also from the front of the CE.
    This function assumes that there are no holes in both the URL and the CE.
    It will remove \e leading space, but not other spaces.

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
        tmp_name.erase(j, i - j);

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
    for (citer i = l.begin(); i != l.end(); i++) {
        assert(*i);
        names[nelem++] = (*i)->name();
        DBG(cerr << "NAMES[" << nelem - 1 << "]=" << names[nelem-1] << endl);
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
    for (int j = 1; j < nelem; ++j) {
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

const char *
libdap_root()
{
    return LIBDAP_ROOT;
#if 0
    // I've changed this because this could be used to get the library to
    // use a different compression function when it builds compressed 
    // responses. The use of 'deflate' to compress responses should be
    // removed since Hyrax now uses Tomcat to perform this function.
    char *libdap_root = 0;
    return ((libdap_root = getenv("LIBDAP_ROOT")) ? libdap_root : LIBDAP_ROOT);
#endif
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

// Since Server4 can get compressed responses using Tomcat, bail on this
// software (which complicates building under Win32). It can be turned on
// for use with Server3 in configure.ac.

#if COMPRESSION_FOR_SERVER3

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
    int hStdIn, hStdOut;

    if (_pipe(data, 512, O_BINARY | O_NOINHERIT) < 0) {
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
    if (_dup2(data[0], _fileno(stdin)) != 0) {
        cerr << "dup of child stdin failed" << endl;
        return NULL;
    }
    // Child is to write its's stdout to file
    if (_dup2(_fileno(output), _fileno(stdout)) != 0) {
        cerr << "dup of child stdout failed" << endl;
        return NULL;
    }

    // Spawn child process
    string deflate = "deflate.exe";
    if ((pid = _spawnlp(_P_NOWAIT, deflate.c_str(), deflate.c_str(),
                        "-c", "5", "-s", NULL)) < 0) {
        cerr << "Could not spawn to create compressor process" << endl;
        return NULL;
    }

    // Restore stdin, stdout for parent and close duplicate copies
    if (_dup2(hStdIn, _fileno(stdin)) != 0) {
        cerr << "dup of stdin failed" << endl;
        return NULL;
    }
    if (_dup2(hStdOut, _fileno(stdout)) != 0) {
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

    if (pid > 0) {   // Parent, pid is that of the child
        close(data[0]);
        ret_file = fdopen(data[1], "w");
        setbuf(ret_file, 0);
        childpid = pid;
    }
    else {   // Child
        close(data[1]);
        dup2(data[0], 0); // Read from the pipe...
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
        _exit(127);  // Only here if an error occurred.
    }

    return ret_file ;
#endif
}

#endif // COMPRESSION_FOR_SERVER3

// This function returns a pointer to the system time formated for an httpd
// log file.

string
systime()
{
    time_t TimBin;

    if (time(&TimBin) == (time_t) - 1)
        return string("time() error");
    else {
        string TimStr = ctime(&TimBin);
        return TimStr.substr(0, TimStr.size() - 2); // remove the \n
    }
}

void
downcase(string &s)
{
    for (unsigned int i = 0; i < s.length(); i++)
        s[i] = tolower(s[i]);
}

string
remove_quotes(const string &s)
{
    if (!s.empty() && s[0] == '\"' && s[s.length()-1] == '\"')
        return s.substr(1, s.length() - 2);
    else
        return s;
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

    ios.get(buffer, 512, NULL);
    while ((nbytes = ios.gcount()) > 0) {
        fwrite(buffer, 1, nbytes, out);
        ios.get(buffer, 512, NULL);
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
    char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // result of val / base
    ldiv_t r;

    if (base > 36 || base < 2) {
        // no conversion if wrong base
        std::invalid_argument ex("The parameter base has an invalid value.");
        throw ex;
    }
    if (val < 0)
        str_val += '-';
    r = ldiv(labs(val), base);

    // output digits of val/base first
    if (r.quot > 0)
        append_long_to_string(r.quot, base, str_val);

    // output last digit

    str_val += digits[(int)r.rem];
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
static const char path_sep[] =
    {"\\"
    };
#else
static const char path_sep[] =
    {"/"
    };
#endif

string
path_to_filename(string path)
{
    string::size_type pos = path.rfind(path_sep);

    return (pos == string::npos) ? path : path.substr(++pos);
}

#if 0
// Look around for a reasonable place to put a temporary file. Check first
// the value of the TMPDIR env var. If that does not yeild a path that's
// writable (as defined by access(..., W_OK|R_OK)) then look at P_tmpdir (as
// defined in stdio.h. If both come up empty, then use `./'.
//
// This function allocates storage using new. The caller must delete the char
// array.

// Change this to a version that either returns a string or an open file
// descriptor. Use information from https://buildsecurityin.us-cert.gov/
// (see open()) to make it more secure. Ideal solution: get deserialize()
// methods to read from a stream returned by libcurl, not from a temporary
// file. 9/21/07 jhrg
char *
get_tempfile_template(char *file_template)
{
    char *c;
    
#ifdef WIN32
    // whitelist for a WIN32 directory
    Regex directory("[-a-zA-Z0-9_\\]*");
	
    c = getenv("TEMP");
    if (c && directory.match(c, strlen(c)) && (access(getenv("TEMP"), 6) == 0))
    	goto valid_temp_directory;

    c= getenv("TMP");
    if (c && directory.match(c, strlen(c)) && (access(getenv("TEMP"), 6) == 0))
    	goto valid_temp_directory;
#else
	
	c = getenv("TMPDIR");
	// Changed this so that it uses the pathname_ok() method instead
	// of using its own regex. jhrg 2/4/08
	if (c) {
	    string tmpdir = *c;
	    if (pathname_ok(tmpdir) && (access(c, W_OK | R_OK) == 0))
	        goto valid_temp_directory;
	}
	
#ifdef P_tmpdir
	if (access(P_tmpdir, W_OK | R_OK) == 0) {
        c = P_tmpdir;
        goto valid_temp_directory;
	}
#endif

#endif  // WIN32

    c = ".";
    
valid_temp_directory:
	// Sanitize allocation
	int size = strlen(c) + strlen(file_template) + 2;
	if (!size_ok(1, size))
		throw Error("Bad temporary file name.");
		
    char *temp = new char[size];
    strncpy(temp, c, size-2);
    strcat(temp, "/");

    strcat(temp, file_template);

    return temp;
}
#endif

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

/** @name Security functions */
//@{

/** @brief sanitize the size of an array.
    Test for integer overflow when dynamically allocating an array.
    @param nelem Number of elements.
    @param sz size of each element.
    @return True if the \c nelem elements of \c sz size will overflow an array. */
bool
size_ok(uint sz, uint nelem)
{
    return (sz > 0 && nelem < UINT_MAX / sz);
}

/** @brief Does the string name a potentailly valid pathname?
    Test the given pathname to verfiy that it is a valid name. We define this 
    as: Contains only printable characters; and Is less then 256 characters.
    If \e strict is true, test that the pathname consists of only letters, 
    digits, and underscore, dash and dot characters instead of the more general
    case where a pathname can be composed of any printable characters.
    
    @note Using this function does not guarentee that the path is valid, only
    that the path \e could be valid. The intent is foil attacks where an
    exploit is encoded in a string then passed to a library function. This code
    does not address whether the pathname references a valid resource.
    
    @param path The pathname to test
    @param strict Apply more restrictive tests (true by default) 
    @return true if the pathname consists of legal characters and is of legal
    size, false otherwise. */
bool
pathname_ok(const string &path, bool strict)
{
    if (path.length() > 255)
        return false;
    
    Regex name("[-0-9A-z_./]+");
    if (!strict)
        name = "[:print:]+";
        
    string::size_type len = path.length();
    int result = name.match(path.c_str(), len);
    // Protect against casting too big an uint to int
    // if LEN is bigger than the max int32, the second test can't work
    if (len > INT_MAX || result != static_cast<int>(len))
        return false;
 
    return true;
}

//@}

} // namespace libdap

