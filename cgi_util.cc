
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//         Reza Nekovei <rnekovei@intcomm.net> 
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

// (c) COPYRIGHT URI/MIT 1994-2001
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>
//      reza            Reza Nekovei <rnekovei@intcomm.net>

// A few useful routines which are used in CGI programs.
//
// ReZa 9/30/94 

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: cgi_util.cc,v 1.58 2003/05/23 03:24:57 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef TM_IN_SYS_TIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>
#include <sys/wait.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
// Win32 does not define this. 08/21/02 jhrg
#define F_OK 0
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "cgi_util.h"
#include "util.h"		// This supplies flush_stream for WIN32.
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
#define FILE_DELIMITER	'\\'
#else  //  default to unix
#define FILE_DELIMITER '/'
#endif

using namespace std;

static const int TimLen = 26;	// length of string from asctime()
static const int CLUMP_SIZE = 1024; // size of clumps to new in fmakeword()

/** This sends a formatted block of text to the client, containing
    version information about various aspects of the server.  The
    arguments allow you to enclose version information about the
    filter program and the dataset in the message.  Either argument
    (or both) may be omitted, in which case no script or dataset
    version information will be printed.

    @brief Send a version number.
    @param script_ver The version of the filter script executing this
    function. 
    @param dataset_ver The version of the dataset.
    @return TRUE for success.
*/
bool
do_version(const string &script_ver, const string &dataset_ver)
{
    fprintf( stdout, "HTTP/1.0 200 OK\n" ) ;
    fprintf( stdout, "XDODS-Server: %s\n", DVR ) ;
    fprintf( stdout, "Content-Type: text/plain\n" ) ;
    fprintf( stdout, "\n" ) ;
    
    fprintf( stdout, "Core software version: %s\n", DVR ) ;

    if (script_ver != "")
	fprintf( stdout, "Server Script Revision: %s\n", script_ver.c_str() ) ;

    if (dataset_ver != "")
	fprintf( stdout,  "Dataset version: %s\n", dataset_ver.c_str() ) ;

    return true;
}

#ifndef WIN32
// // 02/13/98 jhrg
/** This function sends data to the client.

    This function has been superseded by the DODSFilter::send_data() mfunc.

    @deprecated
    @brief Send data. (deprecated)
    @param compression A Boolean value indicating whether the data is
    to be sent in compressed form or not.  A value of TRUE indicates
    that the data is to be comressed.
    @param data_stream A file pointer indicating where to print the
    data.  This is typically an XDR pointer.
    @param dds The DODS Dataset Descriptor Structure of the dataset
    whose data is to be sent.  The <tt>do_data_transfer</tt> uses the
    <tt>send</tt> method of this DDS to do the sending.
    @param dataset The (local) name of the dataset whose data is to be
    sent.
    @param constraint A constraint expression that may have been sent
    with the original data request.
    @return TRUE for success, FALSE otherwise.
*/
bool
do_data_transfer(bool compression, FILE *data_stream, DDS &dds,
		 const string &dataset, const string &constraint)
{
    if (compression) {
	int childpid;
	FILE *data_sink = compressor(data_stream, childpid);
	if (!dds.send(dataset, constraint, data_sink, true)) {
	    ErrMsgT("Could not send compressed data");
	    return false;
	}
	fclose(data_sink);
	int pid;
	while ((pid = waitpid(childpid, 0, 0)) > 0) {
	    DBG(cerr << "pid: " << pid << endl);
	}
    }
    else {
	if (!dds.send(dataset, constraint, data_stream, false)) {
	    ErrMsgT("Could not send data");
	    return false;
	}
    }

    return true;
}
#endif


/** This function accepts a dataset path name, and searches for a
    matching ancillary data file name with a very specific set of
    search rules, given here:

    <pre>
    directory           filename          extension
    same                same            `.'given
    given               same            `.'given
    same                given           `.'given
    given               given           `.'given
    </pre>

    Where ``same'' refers to the input dataset pathname, and ``given''
    refers to the function arguments.

    For example, If you call this function with a
    dataset name of <tt>/a/data</tt>, an extension of <tt>das</tt>, a
    directory of 
    <tt>b</tt>, and a filename of <tt>ralph</tt>, the function will
    look (in order) 
    for the following files:

    <pre>
    /a/data.das
    /b/data.das
    /a/ralph.das
    /b/ralph.das
    </pre>

    The function will return a string containing the name of the first
    file in the list that exists, if any. 

    NB: This code now checks for <pathname>.ext 3/17/99 jhrg

    @brief Find a file with ancillary data.  
    @param pathname The input pathname of a dataset.
    @param ext The input extension the desired file is to have.
    @param dir The input directory in which the desired file may be
    found. 
    @param file The input filename the desired file may have.
    @return A string containing the pathname of the file found by
    searching with the given components.  If no file was found, the
    null string is returned.
*/
string
find_ancillary_file(string pathname, string ext, string dir, string file)
{
    string::size_type slash = pathname.rfind('/') + 1;
    string directory = pathname.substr(0, slash);
    string filename = pathname.substr(slash);
    string basename = pathname.substr(slash, pathname.rfind('.')-slash);

    DBG(cerr << "find ancillary file params: " << pathname << ", " << ext 
	<< ", " << dir << ", " << file << endl);
    DBG(cerr << "find ancillary file comp: " << directory << ", " << filename
	<< ", " << basename << endl);

    string dot_ext = "." + ext;

    string name = directory + basename + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = pathname + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = directory + ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = dir + basename + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = directory + file + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = dir + file + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = dir + ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    return "";
}

// Given a pathname to a datafile, take that pathname apart and look for an
// ancillary file that describes a group of datafiles of which this datafile
// is a member. Assume that groups follow a simple naming convention where
// files use either leading or trailing digits and a common basename to name
// group memebrs. For example, 00stuff.hdf, 01stuff.hdf, 02stuff.hdf, ..., is
// a group and is has `stuff' as its basename. 

/** Assume that <tt>name</tt> refers to a file that is one of a
    group of files which share a common `base' name and differ only by
    some prefix or suffix digits (e.g. <tt>00base</tt>, <tt>01base</tt>,
    ... or <tt>base00</tt>, ... have the base name <tt>base</tt>). This
    function looks for a file <tt>base.ext</tt>.

    @param name The name (full or relative) to one member of a group
    of files.
    @param ext The extension of the group's ancillary file. Note that
    <tt>ext</tt> should include a period (.) if that needs to
    separate the base name from the extension.
    @return The pathname to the group's ancillary file if found, otherwise
    the empty string (""). */
string
find_group_ancillary_file(string name, string ext)
{
    // Given /usr/local/data/stuff.01.nc
    // pathname = /usr/local/data, filename = stuff.01.nc and
    // rootname = stuff.01
    string::size_type slash = name.find_last_of('/');
    string dirname = name.substr(0, slash);
    string filename = name.substr(slash+1);
    string rootname = filename.substr(0, filename.find_last_of('.'));

    // Instead of using regexs, scan the filename for leading and then
    // trailing digits.
    string::iterator rootname_iter = rootname.begin();
    string::iterator rootname_end_iter = rootname.end();
    if (isdigit(*rootname_iter)) {
	while(rootname_iter != rootname_end_iter 
	      && isdigit(*++rootname_iter))
	    ;

	// We want: new_name = dirname + "/" + <base> + ext but without
	// creating a bunch of temp objects.
	string new_name = dirname;
	new_name.append("/");
	new_name.append(rootname_iter, rootname_end_iter);
	new_name.append(ext);
	DBG(cerr << "New Name (iter): " << new_name << endl);
	if (access(new_name.c_str(), F_OK) == 0) {
	    return new_name;
	}
    }

    string::reverse_iterator rootname_riter = rootname.rbegin();
    string::reverse_iterator rootname_end_riter = rootname.rend();
    if (isdigit(*rootname_riter)) {
	while(rootname_riter != rootname_end_riter 
	      && isdigit(*++rootname_riter))
	    ;
	string new_name = dirname;
	new_name.append("/");
	// I used reverse iters to scan rootname backwards. To avoid
	// reversing the fragement between end_riter and riter, pass append
	// regular iters obtained using reverse_iterator::base(). See Meyers
	// p. 123. 1/22/2002 jhrg
	new_name.append(rootname_end_riter.base(), rootname_riter.base());
	new_name.append(ext);
	DBG(cerr << "New Name (riter): " << new_name << endl);
	if (access(new_name.c_str(), F_OK) == 0) {
	    return new_name;
	}
    }

    // If we're here either the file does not begin with leading digits or a
    // template made by removing those digits was not found.

    return "";
}

/** Prints an error message in the <tt>httpd</tt> system log file, along with
    a time stamp and the client host name (or address).

    Use this instead of the functions in liberrmsg.a in the programs run by
    the CGIs to report errors so those errors show up in HTTPD's log files.

    @brief Logs an error message.
    @return void
*/
void 
ErrMsgT(const string &Msgt)
{
    time_t TimBin;
    char TimStr[TimLen];

    if (time(&TimBin) == (time_t)-1)
	strcpy(TimStr, "time() error           ");
    else {
	strcpy(TimStr, ctime(&TimBin));
	TimStr[TimLen - 2] = '\0'; // overwrite the \n 
    }

    const char *host_or_addr = getenv("REMOTE_HOST") ? getenv("REMOTE_HOST") :
	getenv("REMOTE_ADDR");
    const char *script = getenv("SCRIPT_NAME") ? getenv("SCRIPT_NAME") : 
	"DODS server"; 

    cerr << "[" << TimStr << "] CGI: " << script << " failed for " 
	 << host_or_addr << ": "<< Msgt << endl;
}

// Given a pathname, return just the filename component with any extension
// removed. The new string resides in newly allocated memory; the caller must
// delete it when done using the filename.
// Originally from the netcdf distribution (ver 2.3.2).
// 
// *** Change to string class argument and return type. jhrg
// *** Changed so it also removes the#path#of#the#file# from decompressed
//     files.  rph.
// Returns: A filename, with path and extension information removed. If
// memory for the new name cannot be allocated, does not return!

/** Given a pathname, this function returns just the file name
    component of the path.  That is, given <tt>/a/b/c/ralph.nc.das</tt>, it
    returns <tt>ralph.nc</tt>.

    @brief Returns the filename portion of a pathname.
    @param path A C-style simple string containing a pathname to be
    parsed. 
    @return A C-style simple string containing the filename component
    of the given pathname.
*/
string
name_path(const string &path)
{
    if (path == "")
	return string("");

    string::size_type delim = path.find_last_of(FILE_DELIMITER);
    string::size_type pound = path.find_last_of("#");
    string new_path;
    
    if(pound != string::npos)
        new_path = path.substr(pound + 1);
    else
        new_path = path.substr(delim + 1);

    return new_path;
}

// Return a MIME rfc-822 date. The grammar for this is:
//       date-time   =  [ day "," ] date time        ; dd mm yy
//                                                   ;  hh:mm:ss zzz
//
//       day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
//                   /  "Fri"  / "Sat" /  "Sun"
//
//       date        =  1*2DIGIT month 2DIGIT        ; day month year
//                                                   ;  e.g. 20 Jun 82
//                   NB: year is 4 digit; see RFC 1123. 11/30/99 jhrg
//
//       month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
//                   /  "May"  /  "Jun" /  "Jul"  /  "Aug"
//                   /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"
//
//       time        =  hour zone                    ; ANSI and Military
//
//       hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
//                                                   ; 00:00:00 - 23:59:59
//
//       zone        =  "UT"  / "GMT"                ; Universal Time
//                                                   ; North American : UT
//                   /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
//                   /  "CST" / "CDT"                ;  Central:  - 6/ - 5
//                   /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
//                   /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
//                   /  1ALPHA                       ; Military: Z = UT;
//                                                   ;  A:-1; (J not used)
//                                                   ;  M:-12; N:+1; Y:+12
//                   / ( ("+" / "-") 4DIGIT )        ; Local differential
//                                                   ;  hours+min. (HHMM)

static const char *days[]={"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *months[]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
			"Aug", "Sep", "Oct", "Nov", "Dec"};

/** Given a constant pointer to a <tt>time_t</tt>, return a RFC
    822/1123 style date.

    This function returns the RFC 822 date with the exception that the RFC
    1123 modification for four-digit years is implemented.

    @return The RFC 822/1123 style date in a C++ string.
    @param t A const <tt>time_t</tt> pointer. */
string
rfc822_date(const time_t t)
{
    struct tm *stm = gmtime(&t);
    char d[256];

    sprintf(d, "%s, %02d %s %4d %02d:%02d:%02d GMT", days[stm->tm_wday], 
	    stm->tm_mday, months[stm->tm_mon], 
#if 0
	    // On Solaris 2.7 this tm_year is years since 1900. 3/17/2000
	    // jhrg
	    stm->tm_year < 100 ? 1900 + stm->tm_year : stm->tm_year, 
#endif
	    1900 + stm->tm_year,
	    stm->tm_hour, stm->tm_min, stm->tm_sec);
    return string(d);
}

/** Get the last modified time. Assume <tt>name</tt> is a file and
    find its last modified time. If <tt>name</tt> is not a file, then
    return now as the last modified time. 
    @param name The name of a file.
    @return The last modified time or the current time. */
time_t
last_modified_time(string name)
{
    struct stat m;

	if (stat(name.c_str(), &m) == 0 && (S_IFREG & m.st_mode))

	return m.st_mtime;
    else
	return time(0);
}

// Send string to set the transfer (mime) type and server version
// Note that the content description filed is used to indicate whether valid
// information of an error message is contained in the document and the
// content-encoding field is used to indicate whether the data is compressed.
// If the data stream is to be compressed, arrange for a compression output
// filter so that all information sent after the header will be compressed.
//
// Returns: false if the compression output filter was to be used but could
// not be started, true otherwise.

static const char *descrip[]={"unknown", "dods_das", "dods_dds", "dods_data",
			"dods_error", "web_error"};
static const char *encoding[]={"unknown", "deflate", "x-plain"};

/**
   @param out Write the MIME header to this FILE pointer. */
void
set_mime_text(FILE *out, ObjectType type, const string &ver, 
	      EncodingType enc, const time_t last_modified)
{
    fprintf( out, "HTTP/1.0 200 OK\n" ) ;
    fprintf( out, "XDODS-Server: %s\n", ver.c_str() ) ;

    const time_t t = time(0);
    fprintf( out, "Date: %s\n", rfc822_date(t).c_str() ) ;

    fprintf( out, "Last-Modified: " ) ;
    if (last_modified > 0)
	fprintf( out, "%s\n", rfc822_date(last_modified).c_str() ) ;
    else 
	fprintf( out, "%s\n", rfc822_date(t).c_str() ) ;

    fprintf( out, "Content-type: text/plain\n" ) ;
    fprintf( out, "Content-Description: %s\n", descrip[type] ) ;
    if (type == dods_error)	// don't cache our error responses.
	fprintf( out, "Cache-Control: no-cache\n" ) ;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
	fprintf( out, "Content-Encoding: %s\n", encoding[enc] ) ;
    fprintf( out, "\n" ) ;
}

/** Use this function to create a MIME header for a text message.

    @brief Set the MIME type to text.
    @param os Write the MIME header to this stream.
    @param type The type of the response (i.e., is it a DAS, DDS, et cetera).
    @param ver The version of the server.
    @param enc Indicates an encoding was applied to the response payload.
    Used primarily to tell clients they need to decompress the payload.
    @param last_modified A RFC 822 date which gives the time the information
    in the repsonse payload was last changed.
    @see ObjectType
    @see EncodingType
    @see Connect */
void
set_mime_text(ostream &os, ObjectType type, const string &ver, 
	      EncodingType enc, const time_t last_modified)
{
    os << "HTTP/1.0 200 OK" << endl;
    os << "XDODS-Server: " << ver << endl;

    const time_t t = time(0);
    os << "Date: " << rfc822_date(t) << endl;

    os << "Last-Modified: ";
    if (last_modified > 0)
	os << rfc822_date(last_modified) << endl;
    else 
	os << rfc822_date(t) << endl;

    os << "Content-type: text/plain" << endl; 
    os << "Content-Description: " << descrip[type] << endl;
    if (type == dods_error)	// don't cache our error responses.
	os << "Cache-Control: no-cache" << endl;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
	os << "Content-Encoding: " << encoding[enc] << endl;
    os << endl;
}

/**
   @param out Write the MIME header to this FILE pointer. */
void
set_mime_binary(FILE *out, ObjectType type, const string &ver, 
		EncodingType enc, const time_t last_modified)
{
    fprintf( out, "HTTP/1.0 200 OK\n" ) ;
    fprintf( out, "XDODS-Server: %s\n", ver.c_str() ) ;
    const time_t t = time(0);
    fprintf( out, "Date: %s\n", rfc822_date(t).c_str() ) ;

    fprintf( out, "Last-Modified: " ) ;
    if (last_modified > 0)
	fprintf( out, "%s\n", rfc822_date(last_modified).c_str() ) ;
    else 
	fprintf( out, "%s\n", rfc822_date(t).c_str() ) ;

    fprintf( out, "Content-type: application/octet-stream\n" ) ;
    fprintf( out, "Content-Description: %s\n", descrip[type] ) ;
    if (enc != x_plain) {
	// Until we fix the bug in the cache WRT compressed data, supress
	// caching for those requests. 11/30/99 jhrg
#if 0
       	fprintf( out, "Cache-Control: no-cache\n" ) ;
#endif
	// Fixed the bug in the libwww. 3/17/2000 jhrg
	fprintf( out, "Content-Encoding: %s\n", encoding[enc] ) ;
    }
    fprintf( out, "\n" ) ;
}

/** Use this function to create a MIME header for a message containing binary
    data.

    @brief Create MIME headers for binary data.
    @param os Write the MIME header to this stream.
    @param type The type of the response (i.e., is it data, et cetera).
    @param ver The version of the server.
    @param enc Indicates an encoding was applied to the response payload.
    Used primarily to tell clients they need to decompress the payload.
    @param last_modified A RFC 822 date which gives the time the information
    in the repsonse payload was last changed.
    @see ObjectType
    @see EncodingType
    @see DDS */
void
set_mime_binary(ostream &os, ObjectType type, const string &ver, 
		EncodingType enc, const time_t last_modified)
{
    os << "HTTP/1.0 200 OK" << endl;
    os << "XDODS-Server: " << ver << endl;
    const time_t t = time(0);
    os << "Date: " << rfc822_date(t) << endl;

    os << "Last-Modified: ";
    if (last_modified > 0)
	os << rfc822_date(last_modified) << endl;
    else 
	os << rfc822_date(t) << endl;

    os << "Content-type: application/octet-stream" << endl; 
    os << "Content-Description: " << descrip[type] << endl;
    if (enc != x_plain) {
	// Fixed the bug in the libwww. 3/17/2000 jhrg
	os << "Content-Encoding: " << encoding[enc] << endl;
    }
    os << endl;
}

/**
   @param out Write the MIME header to this FILE pointer. */
void 
set_mime_error(FILE *out, int code, const string &reason,
	       const string &version)
{
    fprintf( out, "HTTP/1.0 %d %s\n", code, reason.c_str() ) ;
    if (version == "")
      fprintf( out, "XDODS-Server: %s\n", DVR ) ;
    else
      fprintf( out, "XDODS-Server: %s\n", version.c_str() ) ;
    const time_t t = time(0);
    fprintf( out, "Date: %s\n", rfc822_date(t).c_str() ) ;
    fprintf( out, "Cache-Control: no-cache\n" ) ;
    fprintf( out, "\n" ) ;
}

/** Use this function to create a MIME header for a message signaling an
    error.

    @brief Set the MIME text type to ``error.''
    @param os Write the MIME header to this stream.
    @param code An error code for the given error. 
    @param reason A message to be sent to the client.
    @see ErrMsgT */
void
set_mime_error(ostream &os, int code, const string &reason,
	       const string &version)
{
    os << "HTTP/1.0 " << code << " " << reason << endl;
    if (version == "")
      os << "XDODS-Server: " << DVR << endl;
    else
      os << "XDODS-Server: " << version << endl;
    const time_t t = time(0);
    os << "Date: " << rfc822_date(t) << endl;
    os << "Cache-Control: no-cache" << endl;
    os << endl;
}

/**
   @param out Write the response to this FILE pointer. */
void 
set_mime_not_modified(FILE *out)
{
    fprintf( out, "HTTP/1.0 304 NOT MODIFIED\n" ) ;
    const time_t t = time(0);
    fprintf( out, "Date: %s\n", rfc822_date(t).c_str() ) ;
    fprintf( out, "\n" ) ;
}

/** Use this function to create a response signalling that the target of a
    conditional get has not been modified relative to the condition given in
    the request. For DODS this will have to be a date until the servers
    support ETags

    @brief Send a `Not Modified' response.
    @param os Write the MIME header to this stream. */
void
set_mime_not_modified(ostream &os)
{
    os << "HTTP/1.0 304 NOT MODIFIED" << endl;
    const time_t t = time(0);
    os << "Date: " << rfc822_date(t) << endl;
    os << endl;
}

/** Look for the override file by taking the dataset name and
    appending `.ovr' to it. If such a file exists, then read it in and
    store the contents in <tt>doc</tt>. Note that the file contents
    are not checked to see if they are valid HTML (which they must
    be).

    @return True if the `override file' is present, false otherwise. in the
    later case <tt>doc</tt>'s contents are undefined.  */
bool
found_override(string name, string &doc)
{
    ifstream ifs((name + ".ovr").c_str());
    if (!ifs)
	return false;

    char tmp[256];
    doc = "";
    while (!ifs.eof()) {
	ifs.getline(tmp, 255);
	strcat(tmp, "\n");
	doc += tmp;
    }

    return true;
}

/** Read the input stream <tt>in</tt> and discard the MIME header. The MIME header
    is separated from the body of the document by a single blank line. If no
    MIME header is found, then the input stream is `emptied' and will contain
    nothing.

    @brief Read and discard the MIME header of the stream <tt>in</tt>.
    @return True if a MIME header is found, false otherwise.
*/
bool
remove_mime_header(FILE *in)
{
    char tmp[256];
    while (!feof(in)) {
	fgets(tmp, 255, in);
	if (tmp[0] == '\n')
	    return true;
    }

    return false;
}    


/** Look in the CGI directory (given by <tt>cgi</tt>) for a per-cgi
    HTML* file. Also look for a dataset-specific HTML*
    document. Catenate the documents and return them in a single
    String variable.

    The <tt>cgi</tt> path must include the `API' prefix at the end of
    the path. For example, for the NetCDF server whose prefix is `nc'
    and resides in the DODS_ROOT/etc directory of my computer,
    <tt>cgi</tt> is `/home/dcz/jimg/src/DODS/etc/nc'. This function
    then looks for the file named <tt>cgi</tt>.html.

    Similarly, to locate the dataset-specific HTML* file it catenates
    `.html' to <tt>name</tt>, where <tt>name</tt> is the name of the
    dataset. If the filename part of <tt>name</tt> is of the form
    [A-Za-z]+[0-9]*.* then this function also looks for a file whose
    name is [A-Za-z]+.html For example, if <tt>name</tt> is
    .../data/fnoc1.nc this function first looks for
    .../data/fnoc1.nc.html.  However, if that does not exist it will
    look for .../data/fnoc.html. This allows one `per-dataset' file to
    be used for a collection of files with the same root name.

    NB: An HTML* file contains HTML without the <html>, <head> or <body> tags
    (my own notation).

    @brief Look for the user supplied CGI- and dataset-specific HTML* documents.
    @return A String which contains these two documents catenated. Documents
    that don't exist are treated as `empty'.  */

string
get_user_supplied_docs(string name, string cgi)
{
    char tmp[256];
    ostringstream oss;
    ifstream ifs((cgi + ".html").c_str());

    if (ifs) {
	while (!ifs.eof()) {
	    ifs.getline(tmp, 255);
	    oss << tmp << "\n";
	}
	ifs.close();
	
	oss << "<hr>";
    }

    ifs.open((name + ".html").c_str());

    // If name.html cannot be opened, look for basename.html
    if (!ifs) {
	string new_name = find_group_ancillary_file(name, ".html");
	if (new_name != "")
	    ifs.open(new_name.c_str());
    }

    if (ifs) {
	while (!ifs.eof()) {
	    ifs.getline(tmp, 255);
	    oss << tmp << "\n";
	}
	ifs.close();
    }

    return oss.str();
}

// This test code is pretty much obsolete; look at cgiUtilTest.cc 4/17/2001
// jhrg.

#ifdef TEST_CGI_UTIL

int
main(int argc, char *argv[])
{
    // test ErrMsgT
    ErrMsgT("Error");
    string smsg = "String Error";
    ErrMsgT(smsg);
    char *cmsg = "char * error";
    ErrMsgT(cmsg);
    ErrMsgT("");

    // test name_path
    string name_path_p;
    string name = "stuff";
    name_path_p = name_path(name);
    fprintf( stdout, "%s: %s\n", name.c_str(), name_path_p.c_str() ) ;

    name = "stuff.Z";
    name_path_p = name_path(name);
    fprintf( stdout, "%s: %s\n", name.c_str(), name_path_p.c_str() ) ;

    name = "/usr/local/src/stuff.Z";
    name_path_p = name_path(name);
    fprintf( stdout, "%s: %s\n", name.c_str(), name_path_p.c_str() ) ;

    name = "/usr/local/src/stuff.tar.Z";
    name_path_p = name_path(name);
    fprintf( stdout, "%s: %s\n", name.c_str(), name_path_p.c_str() ) ;

    name = "/usr/local/src/stuff";
    name_path_p = name_path(name);
    fprintf( stdout, "%s: %s\n", name.c_str(), name_path_p.c_str() ) ;

    name = "";
    name_path_p = name_path(name);
    fprintf( stdout, "%s: %s\n", name.c_str(), name_path_p.c_str() ) ;

    // Test mime header generators and compressed output
    fprintf( stdout, "MIME text header:\n" ) ;
    set_mime_text(stdout, dods_das, "dods-test/0.00");
    fprintf( stdout, "MIME binary header:\n" ) ;
    set_mime_binary(stdout, dods_data, "dods-test/0.00");

#if 0
    fprintf( stdout, "Some data...\n" ) ;

    fprintf( stdout, "MIME binary header and compressed data:\n" ) ;
    set_mime_binary(dods_data, x_gzip);
    FILE *out = compress_stdout();
    fprintf(out, "Compresses data...\n");
    fflush(out);
    pclose(out);
#endif
}

#endif

// $Log: cgi_util.cc,v $
// Revision 1.58  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.57  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.56  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.55.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.55  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.54  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.47.4.22  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.47.4.21  2002/10/23 17:40:38  jimg
// Added compile-time switch for time.h versus sys/time.h.
//
// Revision 1.47.4.20  2002/08/22 21:23:23  jimg
// Fixes for the Win32 Build made at ESRI by Vlad Plenchoy and myslef.
//
// Revision 1.47.4.19  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.47.4.18  2002/06/18 22:52:19  jimg
// Remove do_data_transfer(); use the corresponding DODSFilter method instead.
//
// Revision 1.53  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.52  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.47.4.17  2002/03/29 18:33:03  jimg
// Some code that was meant to help with debuggin inadvertantly made it
// into CVS wihout the DBG macros; I added them.
//
// Revision 1.47.4.16  2002/03/26 19:58:10  jimg
// I changed the way name_path works so that the whole filename is now used as
// the dataset name. This means that extensions to the filename are not stripped
// off which also fixes bug 64; that filename that contained dots were chopped
// up.
//
// Revision 1.47.4.15  2002/03/12 19:06:45  jimg
// Fix for bug 400. Files which end in extensions that indicate they're
// compressed were breaking the function find_ancillary_file().
// Added searches for <directory>/<ext> to find_ancillary_file(). This means
// that a DAS that should be applied to a group of files can be by simply
// including the DAS in a file called `das' and putting that in the directory
// that contains the files.
//
// Revision 1.47.4.14  2002/02/05 03:23:46  rmorris
// Added some things VC++ needs in the std namespace after James moved
// things around a little in this file (ends, ostrstream & ifstream).
//
// Revision 1.47.4.13  2002/02/04 17:34:10  jimg
// I removed a lot of code that was duplicated from src/dap/usage.cc into cgi_util.
//
// Revision 1.47.4.12  2002/01/23 20:33:18  jimg
// The function read_ancillary_das and _dds are no longer here (i've
// commented them out of the code). Use DODSFilter instead.
//
// Revision 1.47.4.11  2002/01/23 03:18:38  jimg
// I added a new function to find group ancillary files based on simple
// filename patterns (leading or trailing digits). See find_group_anc_file.
//
// Revision 1.47.4.10  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.51  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.47.4.9  2001/10/08 17:01:30  jimg
// Got paranoid about sprintf; increased the size of the buffer used by
// rfc822_date() to 256. It's still not that good; should use an output string
// stream, et c.
//
// Revision 1.50  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.47.4.8  2001/09/25 20:25:20  jimg
// Added include of debug.h
//
// Revision 1.49  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.47.4.7  2001/08/18 00:06:00  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.47.4.6  2001/07/28 01:10:42  jimg
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
// Revision 1.48  2001/06/15 23:49:03  jimg
// Merged with release-3-2-4.
//
// Revision 1.47.4.5  2001/06/14 17:33:38  rich
// Modified name_path() so it also removes the path of the uncompressed file from
// the beginning of decompressed files.
//
// Revision 1.47.4.4  2001/05/03 21:49:03  jimg
// Added Cache-Control: no-cache header to the output of set_mime_text when that
// function is used to generate a response for an Error object. This will
// prevent the caches from caching out server's errors.
//
// Revision 1.47.4.3  2001/05/03 20:25:45  jimg
// Added the set_mime_not_modified() functions. These send 304 (not modified)
// responses.
//
// Revision 1.47.4.2  2001/04/23 22:34:46  jimg
// Added support for the Last-Modified MIME header in server responses.`
//
// Revision 1.47.4.1  2001/04/17 21:43:38  jimg
// Added Last-Modified support to set_mime_text and set_mime_binary. Added tests
// for set_mime_text to cgiUtilTest (I didn't both with tests for
// set_mime_binary since it is so similar to the text routine).
//
// Revision 1.47  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.46  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.45  2000/08/14 23:54:11  jimg
// Removed the usage() functions. These should be supplied by cgis that
// need them. See DODSFilter.
//
// Revision 1.44  2000/08/07 21:08:43  rmorris
// Removed default argument definition.  Under MS VC++ one can't define a
// default argument in both a method declaration and its's definition -
// plus that's redundant anyway.  Just setup default args in the declaration
// (in the .h) only or VC++ will choke.
//
// Revision 1.43  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.36.6.4  2000/08/02 20:54:08  jimg
// Changed the definitions of some of the set_mime_*() functions so that they
// match the changes in declaration in cgi_util.h. These changes removed the
// symbol DVR from the header so that config_dap.h is no longer needed by
// cgi_util.h.
//
// Revision 1.42  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.41  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.40.4.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
//
// Revision 1.40  2000/03/28 16:36:08  jimg
// Removed code that sent bad dates in the response header. Also removed code
// that sent a Cache-Control: no-cache header.
//
// Revision 1.38.2.1  2000/02/17 05:03:16  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.39  2000/01/27 06:29:58  jimg
// Resolved conflicts from merge with release-3-1-4
//
// Revision 1.36.6.3  2000/01/26 23:58:51  jimg
// Fixed the return type of string::find.
// Removed the old name_path() function.
//
// Revision 1.38  1999/12/01 21:33:01  jimg
// Added rfc822_date(...).
// Added Date headers to all the mime header output functions.
// Added Cache-Control: no-cache to the binary header with deflate is true.
// Removed old code.
//
// Revision 1.36.6.2  1999/10/19 16:46:33  jimg
// Removed the usage() function.
//
// Revision 1.37  1999/09/03 22:07:45  jimg
// Merged changes from release-3-1-1
//
// Revision 1.36.6.1  1999/08/28 06:43:04  jimg
// Fixed the implementation/interface pragmas and misc comments
//
// Revision 1.36  1999/05/05 00:49:21  jimg
// Added version number optional argument. This provides a way for code that
// uses these functions to pass version information in so that it can be
// included in the response doc's MIME header.
//
// Revision 1.35  1999/05/04 19:47:23  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.34  1999/04/29 02:29:34  jimg
// Merge of no-gnu branch
//
// Revision 1.33  1999/03/17 23:05:46  jimg
// Added to find_ancillary_file() so that <pathname>.ext will also be checked.
// This ensures that ancillary DAS files such as 1998-6-avhrr.dat.das will be
// used properly.
//
// Revision 1.32  1998/12/16 19:10:53  jimg
// Added support for XDODS-Server MIME header. This fixes a problem where our
// use of Server clashed with Java 
//
// Revision 1.31.6.3  1999/03/17 23:43:05  jimg
// Added pathanme.ext patch from the GNU String version.
//
// Revision 1.31.6.2  1999/02/05 09:32:35  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code.
//
// Revision 1.31.6.1  1999/02/02 21:57:05  jimg
// String to string version
//
// Revision 1.31  1998/03/19 23:30:08  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.30  1998/02/19 19:42:34  jimg
// Added do_data_transfer() back in since the jgofs servers use it.
//
// Revision 1.29  1998/02/11 22:12:45  jimg
// Changed x_gzip to deflate. See Connect.cc/.h
// Removed old code.
//
// Revision 1.28  1997/12/16 01:38:22  jimg
// Merged release 2.14d changes.
//
// Revision 1.27  1997/09/22 22:36:02  jimg
// Added new function to read ancillary DAS and DDS files.
//
// Revision 1.26  1997/06/05 17:24:39  jimg
// Added four function that help with writing the *_dods filter programs:
// usage(), do_version(), do_data_transfer() and read_ancillary_dds().
//
// Revision 1.25  1997/03/27 18:13:27  jimg
// Fixed a problem in set_mime_*() where Content-Encoding was sent with the
// value x-plain. This caused Netscape on Windows to barf. I'm not sure that it
// is a real error, but sending C-E only when its value is x-gzip or
// x-compressed apparently causes the message to go away.
//
// Revision 1.24  1997/03/12 01:07:23  jimg
// Removed code that set the HTTP protocol version header based on the value
// of the SERVER_PROTOCOL environment variable. In servers that support
// HTTP/1.1, the value of this env var was "HTTP/1.1" which broke our clients
// since the servers (which are `nph') do *not* support HTTP/1.1.
//
// Revision 1.23  1997/03/05 08:28:15  jimg
// Now correctly gets the server protocol (HTTP 0.9, 1.0 or 1.1) from the
// environment variables and sets the return doc header accordingly.
//
// Revision 1.22  1996/12/18 18:48:53  jimg
// Fixed spelling in ErrMsgT()'s message.
//
// Revision 1.21  1996/12/03 17:51:58  jimg
// Fixed a bug in the char * array DESCRIPT where "dods_error" was catenated
// with "web_error" (due to a missing comma).
//
// Revision 1.20  1996/11/25 03:42:37  jimg
// Removed compress/decompress functions. Better versions are in util.cc
// Changed from static global version of dods_root to function version.
//
// Revision 1.19  1996/11/20 01:00:17  jimg
// Fixed lingering bug in compress_stdout where the user's path was not
// searched correctly.
//
// Revision 1.18  1996/11/13 19:10:03  jimg
// Added set_mime_error() function. Use this to send MIME headers indicating
// that an error has occurred. NB: Don't use this when sending back an Error
// object - this is for those cases where an error object won't do and you must
// signal an error to the WWW/HTTP software on the client side.
//
// Revision 1.17  1996/10/18 16:33:14  jimg
// Changed set_mime_binary() and set_mime_text() so that they produce a full
// HTTP/MIME header.
//
// Revision 1.16  1996/08/13 18:42:01  jimg
// Added not_used to definition of char rcsid[].
//
// Revision 1.15  1996/06/18 23:48:46  jimg
// Modified so that the compress/decompress functions use the DODS_ROOT
// enviroment-variable/define or the user's PATH to find gzip.
//
// Revision 1.14  1996/06/08 00:16:42  jimg
// Fixed a bug in name_path().
// Added compression functions which create filter processes which
// automatically compress stdio file streams.
// Added to set_mime_text() and set_mime_binary() support for compression.
// These now correctly set the content-encoding field of the mime header.
// Fixed ErrMsgT so that it says `DODS server' when the name of the server is
// not known.
//
// Revision 1.13  1996/06/04 21:33:53  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.12  1996/05/31 23:30:46  jimg
// Updated copyright notice.
//
// Revision 1.11  1996/05/21 23:52:42  jimg
// Changed include netio.h to cgi_util.h.
//
// Revision 1.10  1996/03/05 23:22:06  jimg
// Addedconst to the char * function definitions.
//
// Revision 1.9  1995/07/09  21:20:42  jimg
// Fixed date in copyright (it now reads `Copyright 1995 ...').
//
// Revision 1.8  1995/07/09  21:14:43  jimg
// Added copyright.
//
// Revision 1.7  1995/06/27  17:38:43  jimg
// Modified the cgi-util-test code so that it correctly uses name_path(); the
// pointer returned by that function must be delteted.
//
// Revision 1.6  1995/05/30  18:28:59  jimg
// Added const to ErrMsgT prototype.
//
// Revision 1.5  1995/05/22  20:36:10  jimg
// Added #include "config_netio.h"
// Removed old code.
//
// Revision 1.4  1995/03/16  16:29:24  reza
// Fixed bugs in ErrMsgT and mime type.
//
// Revision 1.3  1995/02/22  21:03:59  reza
// Added version number capability using CGI status_line.
//
// Revision 1.2  1995/02/22  19:53:32  jimg
// Fixed usage of time functions in ErrMsgT; use ctime instead of localtime
// and asctime.
// Fixed TimStr bug in ErrMsgT.
// Fixed dynamic memory bugs in name_path and fmakeword.
// Replaced malloc calls with calls to new char[]; C++ code will expect to
// be able to use delete.
// Fixed memory overrun error in fmakeword.
// Fixed potential bug in name_path (when called with null argument).
// Added assetions.
//
// Revision 1.1  1995/01/10  16:23:01  jimg
// Created new `common code' library for the net I/O stuff.
//
// Revision 1.1  1994/10/28  14:34:01  reza
// First version

