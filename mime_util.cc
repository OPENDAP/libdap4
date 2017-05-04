
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

#include "mime_util.h"
#include "media_types.h"

#include "Ancillary.h"
#include "util.h"  // This supplies flush_stream for WIN32.
#include "debug.h"

#ifdef WIN32
#define FILE_DELIMITER '\\'
#else  //  default to unix
#define FILE_DELIMITER '/'
#endif

// ...not using a const string here to avoid global objects. jhrg 12/23/05
#define CRLF "\r\n"             // Change here, expr-test.cc, in DODSFilter and ResponseBuilder

using namespace std;

namespace libdap {

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
    if (!stm)
    	return "";

    char d[256];

    snprintf(d, 255, "%s, %02d %s %4d %02d:%02d:%02d GMT", days[stm->tm_wday],
            stm->tm_mday, months[stm->tm_mon],
            1900 + stm->tm_year,
            stm->tm_hour, stm->tm_min, stm->tm_sec);
    d[255] = '\0';
    return string(d);
}

static const int TimLen = 26; // length of string from asctime()
//static const int CLUMP_SIZE = 1024; // size of clumps to new in fmakeword()

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

    @deprecated
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
    	char *ctime_value = ctime(&TimBin);
    	if (!ctime_value)
    		strncpy(TimStr, "Unknown", TimLen-1);
    	else {
    		strncpy(TimStr, ctime_value, TimLen-1);
    		TimStr[TimLen - 2] = '\0'; // overwrite the \n
    	}
#if 0
    	strncpy(TimStr, ctime(&TimBin), TimLen-1);
    	TimStr[TimLen - 2] = '\0'; // overwrite the \n
#endif
    }

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

// Send string to set the transfer (mime) type and server version
// Note that the content description filed is used to indicate whether valid
// information of an error message is contained in the document and the
// content-encoding field is used to indicate whether the data is compressed.
// If the data stream is to be compressed, arrange for a compression output
// filter so that all information sent after the header will be compressed.
//
// Returns: false if the compression output filter was to be used but could
// not be started, true otherwise.
#if 0
static const char *descrip[] =
    {"unknown", "dods_das", "dods_dds", "dods_data", "dods_ddx",
     "dods_error", "web_error", "dap4-dmr", "dap4-data", "dap4-error"
    };
#endif

static const char *descrip[] = {
"unknown_type",
"dods_das",
"dods_dds",
"dods_data",
"dods_ddx",       // This is the old XML DDS/DAS used prior to dap4
"dods_data_ddx",  // This is used for caching data responses
"dods_error",
"web_error",

"dap4_dmr",       // DAP4 metadata
"dap4_data",      // The DMR with a data blob
"dap4_error"      // The error response for DAP4
};

static const char *encoding[] =
    {"unknown", "deflate", "x-plain", "gzip", "binary"
    };

/** This function returns the ObjectType value that matches the given string.
    Modified to include tests for the descriptions that use hyphens in addition
    to underscores. 8/1/08 jhrg

    @deprecated  */
ObjectType
get_type(const string &value)
{
	return get_description_type(value);
}

// TODO Recode to use the constants in media_types.h. jhrg 11/12/13

/** This function returns the ObjectType value that matches the given string.
    Modified to include tests for the descriptions that use hyphens in addition
    to underscores. 8/1/08 jhrg

    @param value Value from the HTTP response header */
ObjectType
get_description_type(const string &value)
{
    if ((value == DAS1) || (value == "dods-das"))
        return dods_das;
    else if ((value == "dods_dds") || (value == "dods-dds"))
        return dods_dds;
    else if ((value == "dods_data") || (value == "dods-data"))
        return dods_data;
    else if ((value == "dods_ddx") || (value == "dods-ddx"))
        return dods_ddx;
    else if ((value == "dods_data_ddx" || (value == "dods-data-ddx")))
        return dods_data_ddx;
    else if ((value == "dods_error") || (value == "dods-error"))
        return dods_error;
    else if ((value == "web_error") || (value == "web-error"))
        return web_error;

    else if ((value == "dap4_dmr") || (value == "dap4-dmr") || (value == DMR_Content_Type))
        return dap4_dmr;
    else if ((value == "dap4_data") || (value == "dap4-data") || (value == DAP4_DATA_Content_Type))
        return dap4_data;
    else if ((value == "dap4_error") || (value == "dap4-error"))
        return dap4_error;

    else
        return unknown_type;
}

/** Generate an HTTP 1.0 response header for a text document. This is used
    when returning a serialized DAS or DDS object.

    @deprecated
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
    ostringstream oss;
    set_mime_text(oss, type, ver, enc, last_modified);
    fwrite(oss.str().data(), 1, oss.str().length(), out);
}

/** Generate an HTTP 1.0 response header for a text document. This is used
    when returning a serialized DAS or DDS object.

    @deprecated
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

    if (type == dap4_dmr)
        strm << "Content-Type: application/vnd.org.opendap.dap4.dataset-metadata+xml" << CRLF ;
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

/** Generate an HTTP 1.0 response header for a text document. This is used
 when returning a serialized DAS or DDS object.

 @note In Hyrax these headers are not used. Instead the front end of the
 server will build the response headers

 @param strm Write the MIME header to this stream.
 @param type The type of this this response. Defaults to
 application/octet-stream.
 @param ver The version string; denotes the libdap implementation
 version.
 @param enc How is this response encoded? Can be plain or deflate or the
 x_... versions of those. Default is x_plain.
 @param last_modified The time to use for the Last-Modified header value.
 Default is zero which means use the current time. */
void set_mime_text(ostream &strm, ObjectType type, EncodingType enc, const time_t last_modified,
        const string &protocol)
{
    strm << "HTTP/1.0 200 OK" << CRLF;

    strm << "XDODS-Server: " << DVR << CRLF;
    strm << "XOPeNDAP-Server: " << DVR << CRLF;

    if (protocol == "")
        strm << "XDAP: " << DAP_PROTOCOL_VERSION << CRLF;
    else
        strm << "XDAP: " << protocol << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;

    strm << "Last-Modified: ";
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF;
    else
        strm << rfc822_date(t).c_str() << CRLF;

    if (type == dap4_dmr)
        strm << "Content-Type: application/vnd.org.opendap.dap4.dataset-metadata+xml" << CRLF;
    else
        strm << "Content-Type: text/plain" << CRLF;

    // Note that Content-Description is from RFC 2045 (MIME, pt 1), not 2616.
    // jhrg 12/23/05
    strm << "Content-Description: " << descrip[type] << CRLF;
    if (type == dods_error) // don't cache our error responses.
        strm << "Cache-Control: no-cache" << CRLF;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF;
    strm << CRLF;
}

/** Generate an HTTP 1.0 response header for a html document.

    @deprecated
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
    ostringstream oss;
    set_mime_html(oss, type, ver, enc, last_modified);
    fwrite(oss.str().data(), 1, oss.str().length(), out);
}

/** Generate an HTTP 1.0 response header for a html document.

    @deprecated
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

/** Generate an HTTP 1.0 response header for a html document.

 @param strm Write the MIME header to this stream.
 @param type The type of this this response.
 @param ver The version string; denotes the libdap implementation
 version.
 @param enc How is this response encoded? Can be plain or deflate or the
 x_... versions of those. Default is x_plain.
 @param last_modified The time to use for the Last-Modified header value.
 Default is zero which means use the current time. */
void set_mime_html(ostream &strm, ObjectType type, EncodingType enc, const time_t last_modified,
        const string &protocol)
{
    strm << "HTTP/1.0 200 OK" << CRLF;

    strm << "XDODS-Server: " << DVR<< CRLF;
    strm << "XOPeNDAP-Server: " << DVR<< CRLF;

    if (protocol == "")
        strm << "XDAP: " << DAP_PROTOCOL_VERSION << CRLF;
    else
        strm << "XDAP: " << protocol << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;

    strm << "Last-Modified: ";
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF;
    else
        strm << rfc822_date(t).c_str() << CRLF;

    strm << "Content-type: text/html" << CRLF;
    // See note above about Content-Description header. jhrg 12/23/05
    strm << "Content-Description: " << descrip[type] << CRLF;
    if (type == dods_error) // don't cache our error responses.
        strm << "Cache-Control: no-cache" << CRLF;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF;
    strm << CRLF;
}

/** Write an HTTP 1.0 response header for our binary response document (i.e.,
    the DataDDS object).

    @deprecated
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
    ostringstream oss;
    set_mime_binary(oss, type, ver, enc, last_modified);
    fwrite(oss.str().data(), 1, oss.str().length(), out);
}

/** Write an HTTP 1.0 response header for our binary response document (i.e.,
    the DataDDS object).

    @deprecated
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
void set_mime_binary(ostream &strm, ObjectType type, EncodingType enc, const time_t last_modified,
        const string &protocol)
{
    strm << "HTTP/1.0 200 OK" << CRLF;

    strm << "XDODS-Server: " << DVR << CRLF;
    strm << "XOPeNDAP-Server: " << DVR << CRLF;

    if (protocol.empty())
        strm << "XDAP: " << DAP_PROTOCOL_VERSION << CRLF;
    else
        strm << "XDAP: " << protocol << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;

    strm << "Last-Modified: ";
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF;
    else
        strm << rfc822_date(t).c_str() << CRLF;

    strm << "Content-Type: application/octet-stream" << CRLF;
    strm << "Content-Description: " << descrip[type] << CRLF;
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF;

    strm << CRLF;
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

/** Build the initial headers for the DAP4 data response */

void set_mime_multipart(ostream &strm, const string &boundary, const string &start, ObjectType type, EncodingType enc,
        const time_t last_modified, const string &protocol, const string &url)
{
    strm << "HTTP/1.1 200 OK" << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;

    strm << "Last-Modified: ";
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF;
    else
        strm << rfc822_date(t).c_str() << CRLF;

    strm << "Content-Type: multipart/related; boundary=" << boundary << "; start=\"<" << start
            << ">\"; type=\"text/xml\"" << CRLF;

    // data-ddx;"; removed as a result of the merge of the hyrax 1.8 release
    // branch.
    strm << "Content-Description: " << descrip[type] << ";";
    if (!url.empty())
        strm << " url=\"" << url << "\"" << CRLF;
    else
        strm << CRLF;

    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF;

    if (protocol == "")
        strm << "X-DAP: " << DAP_PROTOCOL_VERSION << CRLF;
    else
        strm << "X-DAP: " << protocol << CRLF;

    strm << "X-OPeNDAP-Server: " << DVR<< CRLF;

    strm << CRLF;
}

void set_mime_ddx_boundary(ostream &strm, const string &boundary,
	const string &cid, ObjectType type, EncodingType enc)
{
    strm << "--" << boundary << CRLF;
    // TODO - Bite the bullet and make the encoding UTF-8 as required by dap4. This will break a lot of tests but the baselines could be amended using  a bash script and sed.
    strm << "Content-Type: Text/xml; charset=iso-8859-1" << CRLF;
    strm << "Content-Id: <" << cid << ">" << CRLF;
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
    strm << "Content-Id: <" << cid << ">" << CRLF;
    strm << "Content-Description: " << descrip[type] << CRLF ;
    if (enc != x_plain)
         strm << "Content-Encoding: " << encoding[enc] << CRLF ;

    strm << CRLF;
}

const size_t line_length = 1024;

/** Read the next MIME header from the input stream and return it in a string
    object. This function consumes any leading whitespace before the next
    header. It returns an empty string when the blank line that separates
    the headers from the body is found. This function works for header and
    separator lines that use either a CRLF pair (the correct line ending) or
    just a newline (a common error).

    @deprecated
    @note FIXME: This function returns tainted data.
    @param in Read from this stream (FILE *)
    @return A string that contains the next header line or is empty indicating
    the separator has been read.
    @exception Error is thrown if no header or separator is found.
    @see parse_mime_header()
 */
string get_next_mime_header(FILE *in)
{
    // Get the header line and strip \r\n. Some headers end with just \n.
    // If a blank line is found, return an empty string.
    char line[line_length];
    while (!feof(in)) {
        if (fgets(line, line_length, in)
        	&& (strncmp(line, CRLF, 2) == 0 || line[0] == '\n'))
            return "";
        else {
            size_t slen = min(strlen(line), line_length); // Never > line_length
            line[slen - 1] = '\0'; // remove the newline
            if (line[slen - 2] == '\r') // ...and the preceding carriage return
                line[slen - 2] = '\0';
            return string(line);
        }
    }

    throw Error("I expected to find a MIME header, but got EOF instead.");
}

/**
 * This function and its companion read the next MIME header from an
 * input stream (or FILE*) and return it as a string. The stream
 * pointer is updated to the next line of input. It returns an empty
 * string when the blank line ending the MIME headers is found.
 *
 * @note FIXME: This function returns tainted data.
 *
 * @param in Read the next header from the input stream
 * @return Return the next header in a string; an empty string indicates
 * all headers have been returned.
 */
string get_next_mime_header(istream &in)
{
#if 0
    // Get the header line and strip \r\n. Some headers end with just \n.
    // If a blank line is found, return an empty string.
	char line[line_length];
	while (!in.eof()) {
		in.getline(line, line_length);
		if (strncmp(line, CRLF, 2) == 0 || line[0] == '\n') {
			return "";
		}
		else {
			size_t slen = min(strlen(line), line_length); // Never > line_length
			line[slen - 1] = '\0'; // remove the newline
			if (line[slen - 2] == '\r') // ...and the preceding carriage return
				line[slen - 2] = '\0';
			return string(line);
		}
	}
#endif
    // Get the header line and strip \r\n. Some headers end with just \n.
    // If a blank line is found, return an empty string.
	char raw_line[line_length];
	while (!in.eof()) {
		in.getline(raw_line, line_length); // strips the trailing newline; terminates with null
		string line = raw_line;
		if (line.find('\r') != string::npos)
			line = line.substr(0, line.size()-1);
		return line;
	}

	throw Error("I expected to find a MIME header, but got EOF instead.");
}

/** Given a string that contains a MIME header line, parse it into the
    the header (name) and its value. Both are downcased.

    @param header The input line, striped of the ending crlf pair
    @param name A value-result parameter that holds the header name
    @param value A value-result parameter that holds the header's value.
 */
void parse_mime_header(const string &header, string &name, string &value)
{
    istringstream iss(header);

    size_t length = header.length() + 1;
    vector<char> s(length);
    //char s[line_length];
    iss.getline(&s[0], length, ':');
    name = &s[0];

    iss.ignore(length, ' ');
    iss.getline(&s[0], length);
    value = &s[0];

    downcase(name);
    downcase(value);
}

/** Is this string the same as the MPM boundary value?

    @note Since fgets() is used to read into line, it is guaranteed to be
    null terminated.

    @param line The input to test
    @param boundary The complete boundary line to test for, excluding
    terminating characters.
    @return true is line is a MPM boundary
 */

bool is_boundary(const char *line, const string &boundary)
{
    if (strlen(line) < 2 || !(line[0] == '-' && line[1] == '-'))
		return false;
    else
		return strncmp(line, boundary.c_str(), boundary.length()) == 0;
}

/** Read the next line of input and test to see if it is a multipart MIME
    boundary line. If the value of boundary is the default (an empty string)
    then just test that the line starts with "--". In either case, return the
    value of boundary just read.

    @param boundary Value of the boundary to look for - optional
    @param in Read from this FILE*
    @return The value of teh boundary header read
    @exception Error if no boundary was found.
 */
string read_multipart_boundary(FILE *in, const string &boundary)
{
    string boundary_line = get_next_mime_header(in);
    // If the caller passed in a value for the boundary, test for that value,
    // else just see that this line starts with '--'.
    // The value of 'boundary_line' is returned by this function.
    if ((!boundary.empty() && is_boundary(boundary_line.c_str(), boundary))
	    || boundary_line.find("--") != 0)
	throw Error(internal_error, "The DAP4 data response document is broken - missing or malformed boundary.");

    return boundary_line;
}

string read_multipart_boundary(istream &in, const string &boundary)
{
    string boundary_line = get_next_mime_header(in);
    // If the caller passed in a value for the boundary, test for that value,
    // else just see that this line starts with '--'.
    // The value of 'boundary_line' is returned by this function.
    if ((!boundary.empty() && is_boundary(boundary_line.c_str(), boundary))
	    || boundary_line.find("--") != 0)
	throw Error(internal_error, "The DAP4 data response document is broken - missing or malformed boundary.");

    return boundary_line;
}

/** Consume the Multipart MIME headers that prefix the DDX in a DataDDX
    response. The stream pointer is advanced to the start of the DDX. It might
    seem odd that this function both takes the value of the MPM boundary as
    a parameter _and_ returns that value as a result, but this code can be
    used in two different situations. In one case, it is called on a partial
    document read from stdin and needs to return the value of boundary to the
    downstream DDX parser so that code can sense the end of the DDX. In the
    other case, this function is told the value of boundary and tests for it
    to ensure document correctness.

    @param in Read from this stream
    @param content_type The expected value of the Content-Type header
    @param object_type The expected value of the Content-Description header
    @param cid The expected value of the Content-Id header - optional.

    @return The value of the MIME boundary
    @exception Error if the boundary is not found or if any of the expected
    header values don't match. The optional values are tested only if they
    are given (the default values are not tested).
 */
void read_multipart_headers(FILE *in, const string &content_type, const ObjectType object_type, const string &cid)
{
	bool ct = false, cd = false, ci = false;

	// The function get_next_mime_header() returns tainted data. Fix this
	// if we are going to use this code. jhrg 4/18/17
	string header = get_next_mime_header(in);
	while (!header.empty()) {
		string name, value;
		parse_mime_header(header, name, value);

		if (name == "content-type") {
			ct = true;
			if (value.find(content_type) == string::npos)
				throw Error(internal_error, "Content-Type for this part of a DAP2 data ddx response must be " + content_type + ".");
		}
		else if (name == "content-description") {
			cd = true;
			if (get_description_type(value) != object_type)
				throw Error(internal_error, "Content-Description for this part of a DAP2 data ddx response must be dods-ddx or dods-data-ddx");
		}
		else if (name == "content-id") {
			ci = true;
			if (!cid.empty() && value != cid)
				throw Error("Content-Id mismatch. Expected: " + cid + ", but got: " + value);
		}

		header = get_next_mime_header(in);
	}

	if (!(ct && cd && ci)) throw Error(internal_error, "The DAP4 data response document is broken - missing header.");
}

void read_multipart_headers(istream &in, const string &content_type, const ObjectType object_type, const string &cid)
{
	bool ct = false, cd = false, ci = false;

	string header = get_next_mime_header(in);
	while (!header.empty()) {
		string name, value;
		parse_mime_header(header, name, value);

		if (name == "content-type") {
			ct = true;
			if (value.find(content_type) == string::npos)
				throw Error(internal_error, "Content-Type for this part of a DAP4 data response must be " + content_type + ".");
		}
		else if (name == "content-description") {
			cd = true;
			if (get_description_type(value) != object_type)
				throw Error("Content-Description '" + value + "' not the expected value (expected: " + descrip[object_type] + ").");
		}
		else if (name == "content-id") {
			ci = true;
			if (!cid.empty() && value != cid)
				throw Error("Content-Id mismatch. Expected: " + cid + ", but got: " + value);
		}

		header = get_next_mime_header(in);
	}

	if (!(ct && cd && ci)) throw Error(internal_error, "The DAP4 data response document is broken - missing header.");
}

/** Given a Content-Id read from the DDX, return the value to look for in a
    MPM Content-Id header. This function downcases the CID to match the value
    returned by parse_mime_header.

    @param cid The Content-Id read from the DDX
    @return The header value to look for.
    @exception Error if the CID does not start with the string "cid:"
 */
string cid_to_header_value(const string &cid)
{
    string::size_type offset = cid.find("cid:");
    if (offset != 0)
        throw Error(internal_error, "expected CID to start with 'cid:'");

    string value = "<";
    value.append(cid.substr(offset + 4));
    value.append(">");
    downcase(value);

    return value;
}

/** Generate an HTTP 1.0 response header for an Error object.

    @deprecated
    @param out Write the MIME header to this FILE pointer.
    @param code HTTP 1.0 response code. Should be 400, ... 500, ...
    @param reason Reason string of the HTTP 1.0 response header.
    @param version The version string; denotes the DAP spec and implementation
    version. */
void
set_mime_error(FILE *out, int code, const string &reason,
               const string &version)
{
    ostringstream oss;
    set_mime_error(oss, code, reason, version);
    fwrite(oss.str().data(), 1, oss.str().length(), out);
}

/** Generate an HTTP 1.0 response header for an Error object.

    @deprecated
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

    @deprecated
    @brief Send a `Not Modified' response.
    @param out Write the response to this FILE pointer. */
void
set_mime_not_modified(FILE *out)
{
    ostringstream oss;
    set_mime_not_modified(oss);
    fwrite(oss.str().data(), 1, oss.str().length(), out);
}

/** Use this function to create a response signaling that the target of a
    conditional get has not been modified relative to the condition given in
    the request. This will have to be a date until the servers support ETags.

    @deprecated
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

#if 0

// This was removed because it's not being used by our server.

/** Look for the override file by taking the dataset name and
    appending `.ovr' to it. If such a file exists, then read it in and
    store the contents in <tt>doc</tt>. Note that the file contents
    are not checked to see if they are valid HTML (which they must
    be).

    @deprecated
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
        tmp[255] = '\0';
        strncat(tmp, "\n", sizeof(tmp) - strlen(tmp) - 1);
        doc += tmp;
    }

	ifs.close();
    return true;
}
#endif

/** Read the input stream <tt>in</tt> and discard the MIME header. The MIME
    header is separated from the body of the document by a single blank line.
    If no MIME header is found, then the input stream is `emptied' and will
    contain nothing.

    @deprecated
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

/**
 * Used for test code; strip the leading MIME headers from a response.
 * @param in
 */
void
remove_mime_header(istream &in)
{
	while(!get_next_mime_header(in).empty()) ;
#if 0
	string header;
	do {
		header = get_next_mime_header(in);
	} while (!header.empty());
#endif
}

} // namespace libdap

