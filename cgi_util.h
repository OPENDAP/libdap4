
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

void usage(const char *name);
bool do_version(const String &script_ver, const String &dataset_ver);
bool do_data_transfer(bool compression, FILE *data_stream, DDS &dds,
		      const String &datsset, const String &constraint);
String find_ancillary_file(String pathname, String ext, String dir, 
			   String file);
bool read_ancillary_dds(DDS &dds, String dataset, String dir, String file);
bool read_ancillary_das(DAS &das, String dataset, String dir, String file);
void ErrMsgT(const char *Msgt);

#if 0
char *fmakeword(FILE *f, const char stop, int *cl) ;
#endif
char *name_path(const char *path);

void set_mime_text(ObjectType type = unknown_type, 
		   EncodingType enc = x_plain);
void set_mime_binary(ObjectType type = unknown_type, 
		     EncodingType enc = x_plain);
void set_mime_error(int code = HTERR_NOT_FOUND, 
		    const char *reason = "Dataset not found");
