
// -*- c++ -*-

// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      reza            Reza Nekovei (reza@intcomm.net)

// External definitions for utility functions used by servers.
//
// 2/22/95 jhrg

#include "Connect.h"		// For ObjectType and EncodingType defs 

char *name_path(const char *path);
char *fmakeword(FILE *f, const char stop, int *cl) ;
void ErrMsgT(const char *Msgt);
void set_mime_text(ObjectType type = unknown_type, 
		   EncodingType enc = x_plain);
void set_mime_binary(ObjectType type = unknown_type, 
		     EncodingType enc = x_plain);
FILE *compress_stdout();
FILE *decompress_stdin();
