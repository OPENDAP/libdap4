/*
  Copyright 1994-1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

// A few useful routines which are used in CGI programs.
//
// ReZa 9/30/94 

// $Log: cgi_util.cc,v $
// Revision 1.8  1995/07/09 21:14:43  jimg
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

static char rcsid[]={"$Id: cgi_util.cc,v 1.8 1995/07/09 21:14:43 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <iostream.h>
#include <String.h>

#include "config_netio.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifndef FILE_DELIMITER		// default to unix
#define FILE_DELIMITER '/'
#endif

#define TimLen 26		// length of string from asctime()
#define CLUMP_SIZE 1024		// size of clumps to new in fmakeword()



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

    if(getenv("REMOTE_HOST"))
	cerr << "[" << TimStr << "] CGI: " << getenv("SCRIPT_NAME") 
	     << " faild for " 
	     << getenv("REMOTE_HOST") 
	     << " reason: "<< Msgt << endl;
    else
	cerr << "[" << TimStr << "] CGI: " << getenv("SCRIPT_NAME") 
	     << " faild for " 
	     << getenv("REMOTE_ADDR")
	     << " reason: "<< Msgt << endl;
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
name_path(char *path)
{
    if (!path)
	return NULL;

    char *cp = strrchr(path, FILE_DELIMITER);
    if (cp == 0)                // no delimiter
	cp = path;
    else                        // skip delimeter
	cp++;

    char *newp = new char[strlen(cp)+1]; 
    if (newp == 0) {
        ErrMsgT("name_path: out of memory.");
        exit(-1);
    }

    (void) strcpy(newp, cp);	// copy last component of path
    if ((cp = strrchr(newp, '.')) != NULL)
      *cp = '\0';               /* strip off any extension */

    return newp;
}

// Send string to set the transfer (mime) type and server version
//
void
set_mime_text()
{
  cout << "Status: 200 " << DVR << endl;  /* send server version */
  cout << "Content-type: text/plain\n" << endl; 
}

void
set_mime_binary()
{
  cout << "Status: 200 " << DVR << endl;
  cout << "Content-type: application/octet-stream\n" << endl; 
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
    FILE *in = fopen("./fmakeword.input", "r");
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
    in = fopen("./fmakeword2.input", "r");
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
}

#endif

