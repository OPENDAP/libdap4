
// A few useful routines which are used in CGI programs.
//
// ReZa 9/30/94 

// $Log: cgi_util.cc,v $
// Revision 1.2  1995/02/22 19:53:32  jimg
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

static char rcsid[]={"$Id: cgi_util.cc,v 1.2 1995/02/22 19:53:32 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <iostream.h>
#include <String.h>

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
#ifdef NEVER
    // main() should call setlocale(); don't override its call. jhrg
    (void)setlocale(LC_ALL, "");
#endif

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
	*cp = '\0';		// strip off any extension

    return newp;
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
    char *name = "stuff";
    cout << name << ": " << name_path(name) << endl;

    name = "stuff.Z";
    cout << name << ": " << name_path(name) << endl;

    name = "/usr/local/src/stuff.Z";
    cout << name << ": " << name_path(name) << endl;

    name = "/usr/local/src/stuff.tar.Z";
    cout << name << ": " << name_path(name) << endl;

    name = "/usr/local/src/stuff";
    cout << name << ": " << name_path(name) << endl;

    name = "";
    cout << name << ": " << name_path(name) << endl;

    name = 0;
    cout << name << ": " << name_path(name) << endl;
}

#endif
