
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      reza            Reza Nekovei (reza@intcomm.net)

// A few useful routines which are used in CGI programs.
//
// ReZa 9/30/94 

// $Log: cgi_util.cc,v $
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

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: cgi_util.cc,v 1.45 2000/08/14 23:54:11 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#ifdef WIN32
#include <strstream>
#else
#include <fstream>
#endif
#include <string>

#include "cgi_util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
#define FILE_DELIMITER	'\\'
#else  //  default to unix
#define FILE_DELIMITER '/'
#endif

#ifdef WIN32
using std::cerr;
using std::endl;
using std::strstream;
#endif

static const int TimLen = 26;	// length of string from asctime()
static const int CLUMP_SIZE = 1024; // size of clumps to new in fmakeword()

#if 0
// The next two functions are helpers used to simplify creating the *_dods
// filter programs.

void
usage(const string &name)
{
    // Write a message to the WWW server error log file.
    const string msg = " [-c] <dataset> [constraint]";
    const string msg2 = " -v [<script_version> <dataset>]";
    string usage = "Usage: " + name + msg + "\n" + name + msg2;

    ErrMsgT(usage);

    // Build an error object to return to the user.
    Error *ErrorObj = new Error(no_such_file, 
			(string)"\"DODS internal error; missing parameter.\"");
    set_mime_text(cout, dods_error);
    ErrorObj->print(cout);
}    

void
usage(const char *name)
{
    // Write a message to the WWW server error log file.
    const char *msg = "[-c] <dataset> [constraint]";
    const char *msg2 = "-v [<script_version> <dataset>]";
    char usage[256];
    sprintf(usage, "Usage: %s %s\n%s %s", name, msg, name, msg2);
    ErrMsgT(usage);

    // Build an error object to return to the user.
    Error *ErrorObj = new Error(no_such_file, 
			(string)"\"DODS internal error; missing parameter.\"");
    set_mime_text(cout, dods_error);
    ErrorObj->print(cout);
}
#endif

// Note that the filter program must define `find_dataset_version()' for this
// function to work.

bool
do_version(const string &script_ver, const string &dataset_ver)
{
    cout << "HTTP/1.0 200 OK" << endl
	 << "XDODS-Server: " << DVR << endl
	 << "Content-Type: text/plain" << endl
	 << endl;
    
    cout << "Core software version: " << DVR << endl;

    if (script_ver != "")
	cout << "Server Script Revision: " << script_ver << endl;

    if (dataset_ver != "")
	cout << "Dataset version: " << dataset_ver << endl;

    return true;
}

#ifndef WIN32
// This function has been superseded by the DODSFilter::send_data() mfunc.
// 02/13/98 jhrg
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

// Look for an ancillary das/dds file. Use the following rules to search:
//
// directory           filename          extension
//   same                same            `.'given
//   given               same            `.'given
//   same                given           `.'given
//   given               given           `.'given
//
// Where `same' means the ancillary file name matches that part of the data
// file name. For example, given a data file name of /data/avhrr/k97.pvu,
// this function will return the first name found by looking for:
// /data/avhrr/k97.das, <given dir>/k97.das, <given dir>/<given file>.das.
// This function assumes that directories are separated by `/'s and the
// filename and extension are separated by `.'s. If the filename contains
// several `.' characters, the last one is taken as the name.extenstion
// separator and the `.'s to the left are assumed to be part of the filename.
//
// Returns: A string naming the ancillary file of a null string if no
// matching file was found.
//
// NB: This code now checks for <pathname>.ext 3/17/99 jhrg

string
find_ancillary_file(string pathname, string ext, string dir, string file)
{
    string::size_type slash = pathname.rfind('/') + 1;
    string directory = pathname.substr(0, slash);
    string basename = pathname.substr(slash, pathname.find('.',slash)-slash);
    
    ext = "." + ext;

    string name = directory + basename + ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = pathname + ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = dir + basename + ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = directory + file + ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    name = dir + file + ext;
    if (access(name.c_str(), F_OK) == 0)
	return name;

    return "";
}

// If external descriptor file exists, read it... This file must be in the
// same directory that the netCDF file is located. Its name must be the
// netCDF file name with the addition of .dds

bool
read_ancillary_dds(DDS &dds, string dataset, string dir, string file)
{
    string name = find_ancillary_file(dataset, "dds", dir, file);
    FILE *in = fopen(name.c_str(), "r");
 
    if (in) {
	int status = dds.parse(in);
	fclose(in);
    
	if(!status) {
	    string msg = "Parse error in external file " + dataset + ".dds";

	    // server error message
	    ErrMsgT(msg);

	    // client error message
	    set_mime_text(cout, dods_error);

	    Error *ErrorObj = new Error(malformed_expr, msg);
	    ErrorObj->print(cout);

	    return false;
	}
    }

    return true;
}
    
bool
read_ancillary_das(DAS &das, string dataset, string dir, string file)
{
    string name = find_ancillary_file(dataset, "das", dir, file);
    FILE *in = fopen(name.c_str(), "r");
 
    if (in) {
	int status = das.parse(in);
	fclose(in);
    
	if(!status) {
	    string msg = "Parse error in external file " + dataset + ".das";

	    // server error message
	    ErrMsgT(msg);

	    // client error message
	    set_mime_text(cout, dods_error);

	    Error *ErrorObj = new Error(malformed_expr, msg);
	    ErrorObj->print(cout);

	    return false;
	}
    }

    return true;
}
    
// An error handling routine to append the error messege from CGI programs, 
// a time stamp, and the client host name (or address) to HTTPD error-log.
// Use this instead of the functions in liberrmsg.a in the programs run by
// the CGIs to report errors so those errors show up in HTTPD's log files.
//
// Returns: void

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
//
// Returns: A filename, with path and extension information removed. If
// memory for the new name cannot be allocated, does not return!

string
name_path(const string &path)
{
    if (path == "")
	return string("");

    string::size_type delim = path.find_last_of(FILE_DELIMITER);

    string new_path = path.substr(delim + 1);

    if ((delim = new_path.find(".")) != string::npos)
	new_path = new_path.substr(0, delim);

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

static char *days[]={"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static char *months[]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
			"Aug", "Sep", "Oct", "Nov", "Dec"};

/** Given a constant pointer to a time_t, return a RFC 822 style date. This
    function returns the RFC 822 date with the exception that the RFC 1123
    modification for four-digit years is implemented. The date is returned in
    a statically allocated char array so it must be copied before being
    used. 

    @return The RFC 822 style date in a statically allocated char [].
    @param t A const time_t pointer. */
char *
rfc822_date(const time_t *t)
{
    struct tm *stm = gmtime(t);
    static char d[64];

    sprintf(d, "%s, %02d %s %4d %02d:%02d:%02d GMT", days[stm->tm_wday], 
	    stm->tm_mday, months[stm->tm_mon], 
#if 0
	    // On Solaris 2.7 this tm_year is years since 1900. 3/17/2000
	    // jhrg
	    stm->tm_year < 100 ? 1900 + stm->tm_year : stm->tm_year, 
#endif
	    1900 + stm->tm_year,
	    stm->tm_hour, stm->tm_min, stm->tm_sec);
    return d;
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

static char *descrip[]={"unknown", "dods_das", "dods_dds", "dods_data",
			"dods_error", "web_error"};
static char *encoding[]={"unknown", "deflate", "x-plain"};

void
set_mime_text(FILE *out, ObjectType type, const string &ver, EncodingType enc)
{
#ifdef WIN32
	strstream os;
	set_mime_text(os, type, ver, enc);	
	flush_stream(os, out);
#else
    ofstream os(fileno(out));
	set_mime_text(os, type, ver, enc);
#endif
	return;
}

void
set_mime_text(ostream &os, ObjectType type, const string &ver, 
	      EncodingType enc)
{
    os << "HTTP/1.0 200 OK" << endl;
    os << "XDODS-Server: " << ver << endl;
    const time_t t = time(0);
    os << "Date: " << rfc822_date(&t) << endl;
    os << "Content-type: text/plain" << endl; 
    os << "Content-Description: " << descrip[type] << endl;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
	os << "Content-Encoding: " << encoding[enc] << endl;
    os << endl;
}

void
set_mime_binary(FILE *out, ObjectType type, const string &ver, 
		EncodingType enc)
{
#ifdef WIN32
	strstream os;
	set_mime_binary(os, type, ver, enc);
	flush_stream(os, out);
#else
    ofstream os(fileno(out));
    set_mime_binary(os, type, ver, enc);
#endif

	return;
}

void
set_mime_binary(ostream &os, ObjectType type, const string &ver, 
		EncodingType enc)
{
    os << "HTTP/1.0 200 OK" << endl;
    os << "XDODS-Server: " << ver << endl;
    const time_t t = time(0);
    os << "Date: " << rfc822_date(&t) << endl;
    os << "Content-type: application/octet-stream" << endl; 
    os << "Content-Description: " << descrip[type] << endl;
    if (enc != x_plain) {
	// Until we fix the bug in the cache WRT compressed data, supress
	// caching for those requests. 11/30/99 jhrg
#if 0
       	os << "Cache-Control: no-cache" << endl;
#endif
	// Fixed the bug in the libwww. 3/17/2000 jhrg
	os << "Content-Encoding: " << encoding[enc] << endl;
    }
    os << endl;
}

void 
set_mime_error(FILE *out, int code, const string &reason,
	       const string &version)
{
#ifdef WIN32
	strstream os;
	set_mime_error(os, code, reason, version);	
	flush_stream(os, out);
#else
    ofstream os(fileno(out));
	set_mime_error(os, code, reason, version);
#endif
	return;
}

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
    os << "Date: " << rfc822_date(&t) << endl;
    os << endl;
}

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
    ErrMsgT(NULL);

    // test fmakeword
    FILE *in = fopen("./cgi-util-tests/fmakeword.input", "r");
    char stop = ' ';
    int content_len = 68;
    while (content_len) {
	char *word = fmakeword(in, stop, &content_len);
	cout << "Word: " << word << endl;
	delete word;
    }
    fclose(in);

    // this tests buffer extension in fmakeword, two words are 1111 and 11111
    // char respectively.
    in = fopen("./cgi-util-tests/fmakeword2.input", "r");
    stop = ' ';
    content_len = 12467;
    while (content_len) {
	char *word = fmakeword(in, stop, &content_len);
	cout << "Word: " << word << endl;
	delete word;
    }
    fclose(in);

    // test name_path
    char *name_path_p;
    char *name = "stuff";
    name_path_p = name_path(name);
    cout << name << ": " << name_path_p << endl;
    delete name_path_p;

    name = "stuff.Z";
    name_path_p = name_path(name);
    cout << name << ": " << name_path_p << endl;
    delete name_path_p;

    name = "/usr/local/src/stuff.Z";
    name_path_p = name_path(name);
    cout << name << ": " << name_path_p << endl;
    delete name_path_p;

    name = "/usr/local/src/stuff.tar.Z";
    name_path_p = name_path(name);
    cout << name << ": " << name_path_p << endl;
    delete name_path_p;

    name = "/usr/local/src/stuff";
    name_path_p = name_path(name);
    cout << name << ": " << name_path_p << endl;
    delete name_path_p;

    name = "";
    name_path_p = name_path(name);
    cout << name << ": " << name_path_p << endl;
    delete name_path_p;

    name = 0;
    name_path_p = name_path(name);
    cout << name << ": " << name_path_p << endl;
    delete name_path_p;

    // Test mime header generators and compressed output
    cout << "MIME text header:" << endl;
    set_mime_text(dods_das);
    cout << "MIME binary header:" << endl;
    set_mime_binary(dods_data);

    cout << "Some data..." << endl;

	cout << "MIME binary header and compressed data:" << endl;
    set_mime_binary(dods_data, x_gzip);
    FILE *out = compress_stdout();
    fprintf(out, "Compresses data...\n");
    fflush(out);
    pclose(out);
}

#endif

