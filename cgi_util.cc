
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      reza            Reza Nekovei (reza@intcomm.net)

// A few useful routines which are used in CGI programs.
//
// ReZa 9/30/94 

// $Log: cgi_util.cc,v $
// Revision 1.33  1999/03/17 23:05:46  jimg
// Added to find_ancillary_file() so that <pathname>.ext will also be checked.
// This ensures that ancillary DAS files such as 1998-6-avhrr.dat.das will be
// used properly.
//
// Revision 1.32  1998/12/16 19:10:53  jimg
// Added support for XDODS-Server MIME header. This fixes a problem where our use of Server clashed with Java
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
// Added __unused__ to definition of char rcsid[].
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

static char rcsid[] __unused__ = {"$Id: cgi_util.cc,v 1.33 1999/03/17 23:05:46 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream.h>
#include <stdiostream.h>
#include <String.h>

#include "cgi_util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifndef FILE_DELIMITER		// default to unix
#define FILE_DELIMITER '/'
#endif

static const int TimLen = 26;	// length of string from asctime()
static const int CLUMP_SIZE = 1024; // size of clumps to new in fmakeword()

// The next two functions are helpers used to simplify creating the *_dods
// filter programs.

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
			(String)"\"DODS internal error; missing parameter.\"");
    set_mime_text(cout, dods_error);
    ErrorObj->print(cout);
}

// Note that the filter program must define `find_dataset_version()' for this
// function to work.

bool
do_version(const String &script_ver, const String &dataset_ver)
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

// This function has been superseded by the DODSFilter::send_data() mfunc.
// 02/13/98 jhrg
bool
do_data_transfer(bool compression, FILE *data_stream, DDS &dds,
		 const String &dataset, const String &constraint)
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

String
find_ancillary_file(String pathname, String ext, String dir, String file)
{
    int slash = pathname.index("/", -1) + 1;
    String directory = pathname.at(0, slash);
    String basename = pathname.at(slash, pathname.index(".", -1)-slash);
    
    ext = "." + ext;

    String name = directory + basename + ext;
    if (access((const char *)name, F_OK) == 0)
	return name;

    name = dir + basename + ext;
    if (access((const char *)name, F_OK) == 0)
	return name;

    name = pathname + ext;
    if (access((const char *)name, F_OK) == 0)
	return name;

    name = directory + file + ext;
    if (access((const char *)name, F_OK) == 0)
	return name;

    name = dir + file + ext;
    if (access((const char *)name, F_OK) == 0)
	return name;

    return "";
}

// If external descriptor file exists, read it... This file must be in the
// same directory that the netCDF file is located. Its name must be the
// netCDF file name with the addition of .dds

bool
read_ancillary_dds(DDS &dds, String dataset, String dir = "", 
		   String file = "")
{
    String name = find_ancillary_file(dataset, "dds", dir, file);
    FILE *in = fopen((const char *)name, "r");
 
    if (in) {
	int status = dds.parse(in);
	fclose(in);
    
	if(!status) {
	    String msg = "Parse error in external file " + dataset + ".dds";

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
read_ancillary_das(DAS &das, String dataset, String dir = "", 
		   String file = "")
{
    String name = find_ancillary_file(dataset, "das", dir, file);
    FILE *in = fopen((const char *)name, "r");
 
    if (in) {
	int status = das.parse(in);
	fclose(in);
    
	if(!status) {
	    String msg = "Parse error in external file " + dataset + ".das";

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
ErrMsgT(const char *Msgt)
{
    time_t TimBin;
    char TimStr[TimLen];

    if (time(&TimBin) == (time_t)-1)
	strcpy(TimStr, "time() error           ");
    else {
	strcpy(TimStr, ctime(&TimBin));
	TimStr[TimLen - 2] = '\0'; // overwrite the \n 
    }

    char *host_or_addr = getenv("REMOTE_HOST") ? getenv("REMOTE_HOST") :
	getenv("REMOTE_ADDR");
    char *script = getenv("SCRIPT_NAME") ? getenv("SCRIPT_NAME") : 
	"DODS server"; 

    cerr << "[" << TimStr << "] CGI: " << script << " failed for " 
	 << host_or_addr << ": "<< Msgt << endl;
}

// Given a pathname, return just the filename component with any extension
// removed. The new string resides in newly allocated memory; the caller must
// delete it when done using the filename.
// Originally from the netcdf distribution (ver 2.3.2).
// 
// *** Change to String class argument and return type. jhrg
//
// Returns: A filename, with path and extension information removed. If
// memory for the new name cannot be allocated, does not return!

char *
name_path(const char *path)
{
    if (!path)
	return NULL;

    char *cp = strrchr(path, FILE_DELIMITER);
    if (cp == 0)                // no delimiter
	cp = (char *)path;
    else                        // skip delimeter
	cp++;

    char *newp = new char[strlen(cp)+1]; 

    (void) strcpy(newp, cp);	// copy last component of path
    if ((cp = strrchr(newp, '.')) != NULL)
      *cp = '\0';               /* strip off any extension */

    return newp;
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
set_mime_text(FILE *out, ObjectType type = unknown_type, EncodingType enc = x_plain)
{
    ostdiostream os(out);
    set_mime_text(os, type, enc);
}

void
set_mime_text(ostream &os, ObjectType type = unknown_type, EncodingType enc = x_plain)
{
    os << "HTTP/1.0 200 OK" << endl;
    os << "XDODS-Server: " << DVR << endl;
    os << "Content-type: text/plain" << endl; 
    os << "Content-Description: " << descrip[type] << endl;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
	os << "Content-Encoding: " << encoding[enc] << endl;
    os << endl;
}

void
set_mime_binary(FILE *out, ObjectType type = unknown_type, EncodingType enc = x_plain)
{
    ostdiostream os(out);
    set_mime_binary(os, type, enc);
}

void
set_mime_binary(ostream &os, ObjectType type = unknown_type, EncodingType enc = x_plain)
{
    os << "HTTP/1.0 200 OK" << endl;
    os << "XDODS-Server: " << DVR << endl;
    os << "Content-type: application/octet-stream" << endl; 
    os << "Content-Description: " << descrip[type] << endl;
    if (enc != x_plain)
	os << "Content-Encoding: " << encoding[enc] << endl;
    os << endl;
}

void 
set_mime_error(FILE *out, int code = HTERR_NOT_FOUND, 
	       const char *reason = "Dataset not found")
{
    ostdiostream os(out);
    set_mime_error(os, code, reason);
}

void 
set_mime_error(ostream &os, int code = HTERR_NOT_FOUND, 
	       const char *reason = "Dataset not found")
{
    os << "HTTP/1.0 " << code << " " << reason << endl;
    os << "XDODS-Server: " << DVR << endl;
    os << endl;
}

#ifdef TEST_CGI_UTIL

int
main(int argc, char *argv[])
{
    // test ErrMsgT
    ErrMsgT("Error");
    String smsg = "String Error";
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

