
// -*- c++ -*-

// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      reza            Reza Nekovei (reza@intcomm.net)

// External definitions for functions defined in libnetio.a.
//
// 2/22/95 jhrg

char *name_path(const char *path);
char *fmakeword(FILE *f, const char stop, int *cl) ;
void ErrMsgT(const char *Msgt);
void set_mime_text(String description);
void set_mime_binary(String description);

extern "C" {
    FILE *NetConnect(const char *AbsAddress);
    FILE *NetExecute(const char *AbsAddress);
}

/* Make *SURE* that any change to VERSION_LENGTH here is also made in HTTP.c */

#define VERSION_LENGTH 		20    /* Number of chars in protocol version */
extern char DVersion[VERSION_LENGTH]; /* version string from DODS MIME docs */
