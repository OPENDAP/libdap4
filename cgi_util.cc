
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

#include "config.h"

static char rcsid[] not_used =
    {"$Id$"
    };

#include <cstring>
#include <cstdio>
#include <ctype.h>

#ifndef TM_IN_SYS_TIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>    // for access
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
#include "Ancillary.h"
#include "util.h"  // This supplies flush_stream for WIN32.
#include "debug.h"


#ifdef WIN32
#define FILE_DELIMITER '\\'
#else  //  default to unix
#define FILE_DELIMITER '/'
#endif

// ...not using a const string here to avoid global objects. jhrg 12/23/05
#define CRLF "\r\n"             // Change here, expr-test.cc and DODSFilter.cc

using namespace std;

namespace libdap {

static const int TimLen = 26; // length of string from asctime()
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
    @return TRUE for success. Always returns true.
*/
bool
do_version(const string &script_ver, const string &dataset_ver)
{
    fprintf(stdout, "HTTP/1.0 200 OK%s", CRLF) ;
    fprintf(stdout, "XDODS-Server: %s%s", DVR, CRLF) ;
    fprintf(stdout, "XOPeNDAP-Server: %s%s", DVR, CRLF) ;
    fprintf(stdout, "XDAP: %s%s", DAP_PROTOCOL_VERSION, CRLF) ;
    fprintf(stdout, "Content-Type: text/plain%s", CRLF) ;
    fprintf(stdout, CRLF) ;

    fprintf(stdout, "Core software version: %s%s", DVR, CRLF) ;

    if (script_ver != "")
        fprintf(stdout, "Server Script Revision: %s%s", script_ver.c_str(), CRLF) ;

    if (dataset_ver != "")
        fprintf(stdout,  "Dataset version: %s%s", dataset_ver.c_str(), CRLF) ;

    fflush(stdout) ;            // Not sure this is needed. jhrg 12/23/05

    return true;
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

    if (time(&TimBin) == (time_t) - 1)
        strncpy(TimStr, "time() error           ", TimLen-1);
    else {
        strncpy(TimStr, ctime(&TimBin), TimLen-1);
        TimStr[TimLen - 2] = '\0'; // overwrite the \n
    }

#if 0
	// This was removed because writing these values out 'leaks' system information.
	// Since we're not going to write out the script or host, I also removed the
	// calls to getenv(). jhrg 8/7/2007
    const char *host_or_addr = getenv("REMOTE_HOST") ? getenv("REMOTE_HOST") :
                               getenv("REMOTE_ADDR") ? getenv("REMOTE_ADDR") : "local (a non-CGI run)";
    const char *script = getenv("SCRIPT_NAME") ? getenv("SCRIPT_NAME") :
                         "OPeNDAP server";

    cerr << "[" << TimStr << "] CGI: " << script << " failed for "
         << host_or_addr << ": " << Msgt << endl;
#endif
    cerr << "[" << TimStr << "] DAP server error: " << Msgt << endl;
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

    if (pound != string::npos)
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

static const char *days[] =
    {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
static const char *months[] =
    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
     "Aug", "Sep", "Oct", "Nov", "Dec"
    };

#ifdef _MSC_VER
#define snprintf sprintf_s
#endif
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

    snprintf(d, 255, "%s, %02d %s %4d %02d:%02d:%02d GMT", days[stm->tm_wday],
            stm->tm_mday, months[stm->tm_mon],
            1900 + stm->tm_year,
            stm->tm_hour, stm->tm_min, stm->tm_sec);
    d[255] = '\0';
    return string(d);
}

/** Get the last modified time. Assume <tt>name</tt> is a file and
    find its last modified time. If <tt>name</tt> is not a file, then
    return now as the last modified time.
    @param name The name of a file.
    @return The last modified time or the current time. */
time_t
last_modified_time(const string &name)
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

static const char *descrip[] =
    {"unknown", "dods_das", "dods_dds", "dods_data",
     "dods_error", "web_error", "dap4-ddx", "dap4-data", "dap4-error",
     "dap4-data-ddx"
    };
static const char *encoding[] =
    {"unknown", "deflate", "x-plain", "gzip", "binary"
    };

/** Generate an HTTP 1.0 response header for a text document. This is used
    when returning a serialized DAS or DDS object.

    @param out Write the MIME header to this FILE pointer.
    @param type The type of this this response. Defaults to
    application/octet-stream.
    @param ver The version string; denotes the libdap implementation
    version.
    @param enc How is this response encoded? Can be plain or deflate or the
    x_... versions of those. Default is x_plain.
    @param last_modified The time to use for the Last-Modified header value.
    Default is zero which means use the current time. */
void
set_mime_text(FILE *out, ObjectType type, const string &ver,
              EncodingType enc, const time_t last_modified)
{
    fprintf(out, "HTTP/1.0 200 OK%s", CRLF) ;
    if (ver == "") {
        fprintf(out, "XDODS-Server: %s%s", DVR, CRLF) ;
        fprintf(out, "XOPeNDAP-Server: %s%s", DVR, CRLF) ;
    }
    else {
        fprintf(out, "XDODS-Server: %s%s", ver.c_str(), CRLF) ;
        fprintf(out, "XOPeNDAP-Server: %s%s", ver.c_str(), CRLF) ;
    }
    fprintf(out, "XDAP: %s%s", DAP_PROTOCOL_VERSION, CRLF) ;

    const time_t t = time(0);
    fprintf(out, "Date: %s%s", rfc822_date(t).c_str(), CRLF) ;

    fprintf(out, "Last-Modified: ") ;
    if (last_modified > 0)
        fprintf(out, "%s%s", rfc822_date(last_modified).c_str(), CRLF) ;
    else
        fprintf(out, "%s%s", rfc822_date(t).c_str(), CRLF) ;

    if (type == dap4_ddx)
        fprintf(out, "Content-Type: text/xml%s", CRLF) ;
    else
        fprintf(out, "Content-Type: text/plain%s", CRLF) ;

    // Note that Content-Description is from RFC 2045 (MIME, pt 1), not 2616.
    // jhrg 12/23/05
    fprintf(out, "Content-Description: %s%s", descrip[type], CRLF) ;
    if (type == dods_error) // don't cache our error responses.
        fprintf(out, "Cache-Control: no-cache%s", CRLF) ;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
        fprintf(out, "Content-Encoding: %s%s", encoding[enc], CRLF) ;
    fprintf(out, CRLF) ;
}

/** Generate an HTTP 1.0 response header for a text document. This is used
    when returning a serialized DAS or DDS object.

    @param strm Write the MIME header to this stream.
    @param type The type of this this response. Defaults to
    application/octet-stream.
    @param ver The version string; denotes the libdap implementation
    version.
    @param enc How is this response encoded? Can be plain or deflate or the
    x_... versions of those. Default is x_plain.
    @param last_modified The time to use for the Last-Modified header value.
    Default is zero which means use the current time. */
void
set_mime_text(ostream &strm, ObjectType type, const string &ver,
              EncodingType enc, const time_t last_modified)
{
    strm << "HTTP/1.0 200 OK" << CRLF ;
    if (ver == "") {
        strm << "XDODS-Server: " << DVR << CRLF ;
        strm << "XOPeNDAP-Server: " << DVR << CRLF ;
    }
    else {
        strm << "XDODS-Server: " << ver.c_str() << CRLF ;
        strm << "XOPeNDAP-Server: " << ver.c_str() << CRLF ;
    }
    strm << "XDAP: " << DAP_PROTOCOL_VERSION << CRLF ;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF ;

    strm << "Last-Modified: " ;
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF ;
    else
        strm << rfc822_date(t).c_str() << CRLF ;

    if (type == dap4_ddx)
        strm << "Content-Type: text/xml" << CRLF ;
    else
        strm << "Content-Type: text/plain" << CRLF ;

    // Note that Content-Description is from RFC 2045 (MIME, pt 1), not 2616.
    // jhrg 12/23/05
    strm << "Content-Description: " << descrip[type] << CRLF ;
    if (type == dods_error) // don't cache our error responses.
        strm << "Cache-Control: no-cache" << CRLF ;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF ;
    strm << CRLF ;
}

/** Generate an HTTP 1.0 response header for a html document.

    @param out Write the MIME header to this FILE pointer.
    @param type The type of this this response.
    @param ver The version string; denotes the libdap implementation
    version.
    @param enc How is this response encoded? Can be plain or deflate or the
    x_... versions of those. Default is x_plain.
    @param last_modified The time to use for the Last-Modified header value.
    Default is zero which means use the current time. */
void
set_mime_html(FILE *out, ObjectType type, const string &ver,
              EncodingType enc, const time_t last_modified)
{
    fprintf(out, "HTTP/1.0 200 OK%s", CRLF) ;
    if (ver == "") {
        fprintf(out, "XDODS-Server: %s%s", DVR, CRLF) ;
        fprintf(out, "XOPeNDAP-Server: %s%s", DVR, CRLF) ;
    }
    else {
        fprintf(out, "XDODS-Server: %s%s", ver.c_str(), CRLF) ;
        fprintf(out, "XOPeNDAP-Server: %s%s", ver.c_str(), CRLF) ;
    }
    fprintf(out, "XDAP: %s%s", DAP_PROTOCOL_VERSION, CRLF) ;

    const time_t t = time(0);
    fprintf(out, "Date: %s%s", rfc822_date(t).c_str(), CRLF) ;

    fprintf(out, "Last-Modified: ") ;
    if (last_modified > 0)
        fprintf(out, "%s%s", rfc822_date(last_modified).c_str(), CRLF) ;
    else
        fprintf(out, "%s%s", rfc822_date(t).c_str(), CRLF) ;

    fprintf(out, "Content-type: text/html%s", CRLF) ;
    // See note above about Content-Description header. jhrg 12/23/05
    fprintf(out, "Content-Description: %s%s", descrip[type], CRLF) ;
    if (type == dods_error) // don't cache our error responses.
        fprintf(out, "Cache-Control: no-cache%s", CRLF) ;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
        fprintf(out, "Content-Encoding: %s%s", encoding[enc], CRLF) ;
    fprintf(out, CRLF) ;
}

/** Generate an HTTP 1.0 response header for a html document.

    @param strm Write the MIME header to this stream.
    @param type The type of this this response.
    @param ver The version string; denotes the libdap implementation
    version.
    @param enc How is this response encoded? Can be plain or deflate or the
    x_... versions of those. Default is x_plain.
    @param last_modified The time to use for the Last-Modified header value.
    Default is zero which means use the current time. */
void
set_mime_html(ostream &strm, ObjectType type, const string &ver,
              EncodingType enc, const time_t last_modified)
{
    strm << "HTTP/1.0 200 OK" << CRLF ;
    if (ver == "") {
        strm << "XDODS-Server: " << DVR << CRLF ;
        strm << "XOPeNDAP-Server: " << DVR << CRLF ;
    }
    else {
        strm << "XDODS-Server: " << ver.c_str() << CRLF ;
        strm << "XOPeNDAP-Server: " << ver.c_str() << CRLF ;
    }
    strm << "XDAP: " << DAP_PROTOCOL_VERSION << CRLF ;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF ;

    strm << "Last-Modified: " ;
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF ;
    else
        strm << rfc822_date(t).c_str() << CRLF ;

    strm << "Content-type: text/html" << CRLF ;
    // See note above about Content-Description header. jhrg 12/23/05
    strm << "Content-Description: " << descrip[type] << CRLF ;
    if (type == dods_error) // don't cache our error responses.
        strm << "Cache-Control: no-cache" << CRLF ;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF ;
    strm << CRLF ;
}

/** Write an HTTP 1.0 response header for our binary response document (i.e.,
    the DataDDS object).

    @param out Write the MIME header to this FILE pointer.
    @param type The type of this this response. Defaults to
    application/octet-stream.
    @param ver The version string; denotes the libdap implementation
    version.
    @param enc How is this response encoded? Can be plain or deflate or the
    x_... versions of those. Default is x_plain.
    @param last_modified The time to use for the Last-Modified header value.
    Default is zero which means use the current time.
 */
void
set_mime_binary(FILE *out, ObjectType type, const string &ver,
                EncodingType enc, const time_t last_modified)
{
    fprintf(out, "HTTP/1.0 200 OK%s", CRLF) ;
    if (ver == "") {
        fprintf(out, "XDODS-Server: %s%s", DVR, CRLF) ;
        fprintf(out, "XOPeNDAP-Server: %s%s", DVR, CRLF) ;
    }
    else {
        fprintf(out, "XDODS-Server: %s%s", ver.c_str(), CRLF) ;
        fprintf(out, "XOPeNDAP-Server: %s%s", ver.c_str(), CRLF) ;
    }
    fprintf(out, "XDAP: %s%s", DAP_PROTOCOL_VERSION, CRLF) ;

    const time_t t = time(0);
    fprintf(out, "Date: %s%s", rfc822_date(t).c_str(), CRLF) ;

    fprintf(out, "Last-Modified: ") ;
    if (last_modified > 0)
        fprintf(out, "%s%s", rfc822_date(last_modified).c_str(), CRLF) ;
    else
        fprintf(out, "%s%s", rfc822_date(t).c_str(), CRLF) ;

    fprintf(out, "Content-Type: application/octet-stream%s", CRLF) ;
    fprintf(out, "Content-Description: %s%s", descrip[type], CRLF) ;
    if (enc != x_plain)
        fprintf(out, "Content-Encoding: %s%s", encoding[enc], CRLF) ;

    fprintf(out, CRLF) ;
}

/** Write an HTTP 1.0 response header for our binary response document (i.e.,
    the DataDDS object).

    @param strm Write the MIME header to this stream.
    @param type The type of this this response. Defaults to
    application/octet-stream.
    @param ver The version string; denotes the libdap implementation
    version.
    @param enc How is this response encoded? Can be plain or deflate or the
    x_... versions of those. Default is x_plain.
    @param last_modified The time to use for the Last-Modified header value.
    Default is zero which means use the current time.
 */
void
set_mime_binary(ostream &strm, ObjectType type, const string &ver,
                EncodingType enc, const time_t last_modified)
{
    strm << "HTTP/1.0 200 OK" << CRLF ;
    if (ver == "") {
        strm << "XDODS-Server: " << DVR << CRLF ;
        strm << "XOPeNDAP-Server: " << DVR << CRLF ;
    }
    else {
        strm << "XDODS-Server: " << ver.c_str() << CRLF ;
        strm << "XOPeNDAP-Server: " << ver.c_str() << CRLF ;
    }
    strm << "XDAP: " << DAP_PROTOCOL_VERSION << CRLF ;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF ;

    strm << "Last-Modified: " ;
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF ;
    else
        strm << rfc822_date(t).c_str() << CRLF ;

    strm << "Content-Type: application/octet-stream" << CRLF ;
    strm << "Content-Description: " << descrip[type] << CRLF ;
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF ;

    strm << CRLF ;
}

void set_mime_multipart(ostream &strm, const string &boundary,
	const string &start, ObjectType type,
        const string &version, EncodingType enc,
        const time_t last_modified)
{
    strm << "HTTP/1.0 200 OK" << CRLF ;
    if (version == "") {
        strm << "XDODS-Server: " << DVR << CRLF ;
        strm << "XOPeNDAP-Server: " << DVR << CRLF ;
    }
    else {
        strm << "XDODS-Server: " << version.c_str() << CRLF ;
        strm << "XOPeNDAP-Server: " << version.c_str() << CRLF ;
    }
    strm << "XDAP: " << DAP_PROTOCOL_VERSION << CRLF ;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF ;

    strm << "Last-Modified: " ;
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF ;
    else
        strm << rfc822_date(t).c_str() << CRLF ;

    strm << "Content-Type: Multipart/Related; boundary=" << boundary
	<< "; start=\"<" << start << ">\"; type=\"Text/xml\"" << CRLF ;
    strm << "Content-Description: " << descrip[type] << CRLF ;
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF ;

    strm << CRLF ;
}

void set_mime_ddx_boundary(ostream &strm, const string &boundary,
	const string &cid, ObjectType type, EncodingType enc)
{
    strm << "--" << boundary << CRLF;
    strm << "Content-Type: Text/xml; charset=iso-8859-1" << CRLF;
    strm << "Content-ID: <" << cid << ">" << CRLF;
    strm << "Content-Description: " << descrip[type] << CRLF ;
    if (enc != x_plain)
         strm << "Content-Encoding: " << encoding[enc] << CRLF ;

    strm << CRLF;
}

void set_mime_data_boundary(ostream &strm, const string &boundary,
	const string &cid, ObjectType type, EncodingType enc)
{
    strm << "--" << boundary << CRLF;
    strm << "Content-Type: application/octet-stream" << CRLF;
    strm << "Content-ID: <" << cid << ">" << CRLF;
    strm << "Content-Description: " << descrip[type] << CRLF ;
    if (enc != x_plain)
         strm << "Content-Encoding: " << encoding[enc] << CRLF ;

    strm << CRLF;
}

/** Generate an HTTP 1.0 response header for an Error object.
    @param out Write the MIME header to this FILE pointer.
    @param code HTTP 1.0 response code. Should be 400, ... 500, ...
    @param reason Reason string of the HTTP 1.0 response header.
    @param version The version string; denotes the DAP spec and implementation
    version. */
void
set_mime_error(FILE *out, int code, const string &reason,
               const string &version)
{
    fprintf(out, "HTTP/1.0 %d %s%s", code, reason.c_str(), CRLF) ;
    if (version == "") {
        fprintf(out, "XDODS-Server: %s%s", DVR, CRLF) ;
        fprintf(out, "XOPeNDAP-Server: %s%s", DVR, CRLF) ;
    }
    else {
        fprintf(out, "XDODS-Server: %s%s", version.c_str(), CRLF) ;
        fprintf(out, "XOPeNDAP-Server: %s%s", version.c_str(), CRLF) ;
    }
    fprintf(out, "XDAP: %s%s", DAP_PROTOCOL_VERSION, CRLF) ;

    const time_t t = time(0);
    fprintf(out, "Date: %s%s", rfc822_date(t).c_str(), CRLF) ;
    fprintf(out, "Cache-Control: no-cache%s", CRLF) ;
    fprintf(out, CRLF) ;
}

/** Generate an HTTP 1.0 response header for an Error object.
    @param strm Write the MIME header to this stream.
    @param code HTTP 1.0 response code. Should be 400, ... 500, ...
    @param reason Reason string of the HTTP 1.0 response header.
    @param version The version string; denotes the DAP spec and implementation
    version. */
void
set_mime_error(ostream &strm, int code, const string &reason,
               const string &version)
{
    strm << "HTTP/1.0 " << code << " " << reason.c_str() << CRLF ;
    if (version == "") {
        strm << "XDODS-Server: " << DVR << CRLF ;
        strm << "XOPeNDAP-Server: " << DVR << CRLF ;
    }
    else {
        strm << "XDODS-Server: " << version.c_str() << CRLF ;
        strm << "XOPeNDAP-Server: " << version.c_str() << CRLF ;
    }
    strm << "XDAP: " << DAP_PROTOCOL_VERSION << CRLF ;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF ;
    strm << "Cache-Control: no-cache" << CRLF ;
    strm << CRLF ;
}


/** Use this function to create a response signaling that the target of a
    conditional get has not been modified relative to the condition given in
    the request. This will have to be a date until the servers support ETags.

    @brief Send a `Not Modified' response.
    @param out Write the response to this FILE pointer. */
void
set_mime_not_modified(FILE *out)
{
    fprintf(out, "HTTP/1.0 304 NOT MODIFIED%s", CRLF) ;
    const time_t t = time(0);
    fprintf(out, "Date: %s%s", rfc822_date(t).c_str(), CRLF) ;
    fprintf(out, CRLF) ;
}

/** Use this function to create a response signaling that the target of a
    conditional get has not been modified relative to the condition given in
    the request. This will have to be a date until the servers support ETags.

    @brief Send a `Not Modified' response.
    @param strm Write the response to this stream. */
void
set_mime_not_modified(ostream &strm)
{
    strm << "HTTP/1.0 304 NOT MODIFIED" << CRLF ;
    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF ;
    strm << CRLF ;
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

	ifs.close();
    return true;
}

/** Read the input stream <tt>in</tt> and discard the MIME header. The MIME
    header is separated from the body of the document by a single blank line.
    If no MIME header is found, then the input stream is `emptied' and will
    contain nothing.

    @brief Read and discard the MIME header of the stream <tt>in</tt>.
    @return True if a MIME header is found, false otherwise.
*/
bool
remove_mime_header(FILE *in)
{
    char tmp[256];
    while (!feof(in)) {
        char *s = fgets(tmp, 255, in);
        if (s && strncmp(s, CRLF, 2) == 0)
            return true;
    }

    return false;
}


/** Look in the CGI directory (given by \c cgi) for a per-cgi HTML* file.
    Also look for a dataset-specific HTML* document. Catenate the documents
    and return them in a single String variable.

    Similarly, to locate the dataset-specific HTML* file it catenates `.html'
    to \c name, where \c name is the name of the dataset. If the filename
    part of \c name is of the form [A-Za-z]+[0-9]*.* then this function also
    looks for a file whose name is [A-Za-z]+.html For example, if \c name is
    .../data/fnoc1.nc this function first looks for .../data/fnoc1.nc.html.
    However, if that does not exist it will look for .../data/fnoc.html. This
    allows one `per-dataset' file to be used for a collection of files with
    the same root name.

    @note An HTML* file contains HTML without the \c html, \c head or \c body
    tags (my own notation).

    @brief Look for the user supplied CGI- and dataset-specific HTML*
    documents.

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

    // Problem: This code is run with the CWD as the CGI-BIN directory but
    // the data are in DocumentRoot (and we don't have the pathname of the
    // data relative to DocumentRoot). So the only time this will work is
    // when the server is in the same directory as the data. See bug 815.
    // 10/08/04 jhrg
    ifs.open((name + ".html").c_str());

    // If name.html cannot be opened, look for basename.html
    if (!ifs) {
        string new_name = Ancillary::find_group_ancillary_file(name, ".html");
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

} // namespace libdap

