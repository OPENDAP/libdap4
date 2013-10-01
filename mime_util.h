
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
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

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>
//      reza            Reza Nekovei <reza@intcomm.net>

// External definitions for utility functions used by servers.
//
// 2/22/95 jhrg

#ifndef _mime_util_h
#define _mime_util_h

#ifndef _dds_h
#include "DDS.h"
#endif

#ifndef _object_type_h
#include "ObjectType.h"
#endif

#ifndef _encoding_type_h
#include "EncodingType.h"
#endif

namespace libdap
{

/** The CGI utilities include a variety of functions useful to
    programmers developing OPeNDAP CGI filter programs. However, before jumping
    in and using these, look at the class ResponseBuilder. Always choose to use
    that class over these functions if you can. Many of these functions were used
    by the CGI programs that made up the first DAP server; all of those are
    deprecated and the ResponseBuilder class should be used instead. Some of
    the other functions are used by ResponseBuilder and the client-side parsing
    code that needs to identify MIME headers, boundaries, etc.

    @name MIME Utilities
    @brief A collection of useful functions for writing MIME headers for
    OPeNDAP servers.
    @see ResponseBuilder, Connect, DDXParserSAX2
    */

//@{
string rfc822_date(const time_t t);
time_t last_modified_time(const string &name);
ObjectType get_description_type(const string &value);
bool is_boundary(const char *line, const string &boundary);
string cid_to_header_value(const string &cid);

string read_multipart_boundary(istream &in, const string &boundary = "");

void parse_mime_header(const string &header, string &name, string &value);
string name_path(const string &path);
string get_next_mime_header(istream &in);

void read_multipart_headers(istream &in, const string &content_type,
	const ObjectType object_type, const string &cid = "");

// All of these are deprecated
string read_multipart_boundary(FILE *in, const string &boundary = "");
void read_multipart_headers(FILE *in, const string &content_type,
	const ObjectType object_type, const string &cid = "");
bool do_version(const string &script_ver, const string &dataset_ver);
void ErrMsgT(const string &Msgt);
ObjectType get_type(const string &value);
bool remove_mime_header(FILE *in);
string get_next_mime_header(FILE *in);

#if 0
bool found_override(string name, string &doc);
#endif
//@}

/** These functions are used to create the MIME headers for a message
    from a server to a client. They are public but should not be called
    directly unless absolutely necessary. Use DODSFilter instead.

    NB: These functions actually write both the response status line
    <i>and</i> the header.

    @deprecated Use ResponseBuilder instead.
    @name MIME utility functions
    @see DODSFilter
*/
//@{
void set_mime_text(FILE *out, ObjectType type = unknown_type,
                   const string &version = "", EncodingType enc = x_plain,
                   const time_t last_modified = 0);
void set_mime_text(ostream &out, ObjectType type = unknown_type,
                   const string &version = "", EncodingType enc = x_plain,
                   const time_t last_modified = 0);
void set_mime_text(std::ostream &out, ObjectType type = unknown_type,
                   EncodingType enc = x_plain,
                   const time_t last_modified = 0,
                   const std::string &protocol = "");

void set_mime_html(FILE *out, ObjectType type = unknown_type,
                   const string &version = "", EncodingType enc = x_plain,
                   const time_t last_modified = 0);
void set_mime_html(ostream &out, ObjectType type = unknown_type,
                   const string &version = "", EncodingType enc = x_plain,
                   const time_t last_modified = 0);
void set_mime_html(std::ostream &out, ObjectType type = unknown_type,
                   EncodingType enc = x_plain,
                   const time_t last_modified = 0,
                   const std::string &protocol = "");

void set_mime_binary(FILE *out, ObjectType type = unknown_type,
                     const string &version = "", EncodingType enc = x_plain,
                     const time_t last_modified = 0);
void set_mime_binary(ostream &out, ObjectType type = unknown_type,
                     const string &version = "", EncodingType enc = x_plain,
                     const time_t last_modified = 0);
void set_mime_binary(std::ostream &out, ObjectType type = unknown_type,
                     EncodingType enc = x_plain,
                     const time_t last_modified = 0,
                     const std::string &protocol = "");

void set_mime_multipart(ostream &out, const string &boundary,
	const string &start, ObjectType type = unknown_type,
        const string &version = "", EncodingType enc = x_plain,
        const time_t last_modified = 0);

void set_mime_multipart(std::ostream &out, const std::string &boundary,
	const std::string &start, ObjectType type = unknown_type, EncodingType enc = x_plain,
	const time_t last_modified = 0, const std::string &protocol = "",
	const std::string &url = "");

void set_mime_ddx_boundary(ostream &out, const string &boundary,
	const string &start, ObjectType type = unknown_type,
        EncodingType enc = x_plain);

void set_mime_data_boundary(ostream &out, const string &boundary,
	const string &cid, ObjectType type = unknown_type,
        EncodingType enc = x_plain);

void set_mime_error(FILE *out, int code = 404,
                    const string &reason = "Dataset not found",
                    const string &version = "");
void set_mime_error(ostream &out, int code = 404,
                    const string &reason = "Dataset not found",
                    const string &version = "");

void set_mime_not_modified(FILE *out);
void set_mime_not_modified(ostream &out);


//@}

} // namespace libdap

#endif // _mime_util_h
