
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

static char rcsid[] __unused__ = {"$Id: cgi_util.cc,v 1.24 1997/03/12 01:07:23 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream.h>
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
	 << host_or_addr << " reason: "<< Msgt << endl;
}

// Given an open FILE *IN, a separator character (in STOP) and the number of
// characters in the thing referenced by IN, return characters upto the
// STOP. The STOP character itself is discarded. Memory for the new word is
// dynamically allocated using C++'s new facility. In addition, the count of
// characters in the input source (CL; content length) is decremented. 
//
// Once CL is zero, do not continue calling this function!
//
// Returns: a newly allocated string.

char *
fmakeword(FILE *f, const char stop, int *cl) 
{
    assert(f && stop && *cl);

    int wsize = CLUMP_SIZE;
    int ll = 0;
    char *word = new char[wsize + 1];

    while(1) {
	assert(ll <= wsize);

        word[ll] = (char)fgetc(f);

	// If the word size is exceeded, allocate more space. What a kluge.
        if(ll == wsize) {
            wsize += CLUMP_SIZE;
	    char *tmp_word_buf = new char[wsize + 1];
	    memcpy(tmp_word_buf, word, wsize - (CLUMP_SIZE - 1));
	    delete word;
	    word = tmp_word_buf;
	}

        --(*cl);

        if((word[ll] == stop) || (feof(f)) || (!(*cl))) {
            if(word[ll] != stop) 
		ll++;
	    assert(ll <= wsize);
            word[ll] = '\0';
            return word;
	}

        ++ll;
    }
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
static char *encoding[]={"unknown", "x-plain", "x-gzip"};

void
set_mime_text(ObjectType type = unknown_type, EncodingType enc = x_plain)
{
    // Remove this until our servers can support HTTP/1.1 and other versions.
    // jhrg 3/10/97 
#if 0
    char *protocol = getenv("SERVER_PROTOCOL");
    if (!protocol)
	protocol = "HTTP/1.0";
#endif

    cout << "HTTP/1.0 200 OK" << endl;
    cout << "Server: " << DVR << endl;
    cout << "Content-type: text/plain" << endl; 
    cout << "Content-Description: " << descrip[type] << endl;
    cout << "Content-Encoding: " << encoding[enc] << endl;
    cout << endl;
}

void
set_mime_binary(ObjectType type = unknown_type, EncodingType enc = x_plain)
{
#if 0
    char *protocol = getenv("SERVER_PROTOCOL");
    if (!protocol)
	protocol = "HTTP/1.0";
#endif

    cout << "HTTP/1.0 200 OK" << endl;
    cout << "Server: " << DVR << endl;
    cout << "Content-type: application/octet-stream" << endl; 
    cout << "Content-Description: " << descrip[type] << endl;
    cout << "Content-Encoding: " << encoding[enc] << endl;
    cout << endl;
}

void 
set_mime_error(int code = HTERR_NOT_FOUND, 
	       const char *reason = "Dataset not found")
{
#if 0
    char *protocol = getenv("SERVER_PROTOCOL");
    if (!protocol)
	protocol = "HTTP/1.0";
#endif

    cout << "HTTP/1.0 " << code << " " << reason << endl;
    cout << "Server: " << DVR << endl;
    cout << endl;
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

