
// -*- c++ -*-

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      reza            Reza Nekovei (reza@intcomm.net)

// External definitions for utility functions used by servers.
//
// 2/22/95 jhrg

#ifndef _cgi_util_h
#define _cgi_util_h

#ifndef _dds_h
#include "DDS.h"
#endif

#include "ObjectType.h"
#include "EncodingType.h"

/** The CGI utilities include a variety of functions useful to
    programmers developing DODS CGI filter programs. However, before jumping
    in and using these, look at the class DODSFilter. Always choose to use
    that class over these functions if you can.

    @name CGI Utilities
    @brief A collection of useful functions for writing DODS servers.
    @see DODSFilter
    */

//@{

bool do_version(const string &script_ver, const string &dataset_ver);

bool do_data_transfer(bool compression, FILE *data_stream, DDS &dds,
		      const string &dataset, const string &constraint);

string find_ancillary_file(string pathname, string ext, string dir, 
			   string file);

string find_group_ancillary_file(string pathname, string ext);

void ErrMsgT(const string &Msgt);

string name_path(const string &path);

/** Given a constant pointer to a time_t, return a RFC 822/1123 style date.
    This function returns the RFC 822 date with the exception that the RFC
    1123 modification for four-digit years is implemented.

    @return The RFC 822/1123 style date in C++ string.
    @param t A const time_t pointer. */
string rfc822_date(const time_t t);

time_t last_modified_time(string name);
//@}

/** These functions are used to create the MIME header for a message
    from a server to a client. They are public but should not be called
    directly unless necessary. Use DODSFilter instead.

    NB: These functions actually write both the response status line
    <i>and</i> the header.

    @brief Functions to create a MIME header.
    @name MIME utility functions
    @see DODSFilter
*/

//@{
void set_mime_text(ostream &os, ObjectType type = unknown_type, 
		   const string &version = "", EncodingType enc = x_plain,
		   const time_t last_modified = 0);
void set_mime_text(FILE *out, ObjectType type = unknown_type, 
		   const string &version = "", EncodingType enc = x_plain,
		   const time_t last_modified = 0);


void set_mime_binary(ostream &os, ObjectType type = unknown_type, 
		     const string &version = "", EncodingType enc = x_plain,
		     const time_t last_modified = 0);
void set_mime_binary(FILE *out, ObjectType type = unknown_type, 
		     const string &version = "", EncodingType enc = x_plain,
		     const time_t last_modified = 0);

void set_mime_error(ostream &os, int code = 404, 
		    const string &reason = "Dataset not found",
		    const string &version = "");
void set_mime_error(FILE *out, int code = 404, 
		    const string &reason = "Dataset not found",
		    const string &version = "");

void set_mime_not_modified(ostream &os);
void set_mime_not_modified(FILE *out);

//@}

string get_user_supplied_docs(string name, string cgi);

bool remove_mime_header(FILE *in);

bool found_override(string name, string &doc);

#endif // _cgi_util_h
