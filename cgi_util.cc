// Few usefull routines which are used in CGI programs.
//
// ReZa 9/30/94 

// $Log: cgi_util.cc,v $
// Revision 1.1  1995/01/10 16:23:01  jimg
// Created new `common code' library for the net I/O stuff.
//
// Revision 1.1  1994/10/28  14:34:01  reza
// First version
//

static char rcsid[]={"$Id: cgi_util.cc,v 1.1 1995/01/10 16:23:01 jimg Exp $"};

#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <stdlib.h>

#define TimLen 26


// An error handling routine to append the error messege from CGI programs, 
// a time stamp, and the client host name (or address) to HTTPD error-log.

void ErrMsgT(char *Msgt)
{
  char TimStr[TimLen];
  char *TimStrPtr = TimStr;
  time_t TimBin;
  struct tm *TimStruct;

  (void)setlocale(LC_ALL, "");

  if (time(&TimBin) == (time_t) - 1)
    cerr <<"ErrMsg: Error getting time of day from time()\n" << endl;

  TimStruct = localtime(&TimBin);
  TimStrPtr = asctime(TimStruct);
  TimStr[24] = ']';

  if(getenv("REMOTE_HOST"))
    cerr << "[" << TimStr << " CGI: " << getenv("SCRIPT_NAME") 
      << " faild for " 
      << getenv("REMOTE_HOST") 
      << " reason: "<< Msgt << endl;
  else
    cerr << "[" << TimStr << " CGI: " << getenv("SCRIPT_NAME") 
      << " faild for " 
      << getenv("REMOTE_ADDR")
      << " reason: "<< Msgt << endl;
}

// from util.c in cgi-src distribution of HTTPD, 
// Seperates a line to words.

char *fmakeword(FILE *f, char stop, int *cl) {
    int wsize;
    char *word;
    int ll;

    wsize = 102400;
    ll=0;
    word = (char *) malloc(sizeof(char) * (wsize + 1));

    while(1) {
        word[ll] = (char)fgetc(f);
        if(ll==wsize) {
            word[ll+1] = '\0';
            wsize+=102400;
            word = (char *)realloc(word,sizeof(char)*(wsize+1));
	  }
        --(*cl);
        if((word[ll] == stop) || (feof(f)) || (!(*cl))) {
            if(word[ll] != stop) ll++;
            word[ll] = '\0';
            return word;
	  }
        ++ll;
      }
  }

// Original from the netcdf standard distribution (ver 2.3.2) ncdump.c
//
char *
name_path(char *path)
{
    char *cp, *newp;
    static char Msgt[255];

#ifndef FILE_DELIMITER /* default to unix */
#define FILE_DELIMITER '/'
#endif
    cp = strrchr(path, FILE_DELIMITER);
    if (cp == 0)                /* no delimiter */
      cp = path;
    else                        /* skip delimeter */
      cp++;
    newp = (char *) malloc((unsigned) (strlen(cp)+1));
    if (newp == 0) {
        sprintf (Msgt,"Could not get name, out of memory! (name_path)");
        ErrMsgT(Msgt);
        exit(-1);
      }
    (void) strcpy(newp, cp);     /* copy last component of path */
    if ((cp = strrchr(newp, '.')) != NULL)
      *cp = '\0';               /* strip off any extension */
    return newp;
  }
