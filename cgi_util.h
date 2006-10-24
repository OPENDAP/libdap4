
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#ifndef _cgi_util_h
#define _cgi_util_h

#ifndef _dds_h
#include "DDS.h"
#endif

#include "ObjectType.h"
#include "EncodingType.h"

/** The CGI utilities include a variety of functions useful to
    programmers developing OPeNDAP CGI filter programs. However, before jumping
    in and using these, look at the class DODSFilter. Always choose to use
    that class over these functions if you can.

    @name CGI Utilities
    @brief A collection of useful functions for writing OPeNDAP servers.
    @see DODSFilter
    */

//@{

bool do_version(const string &script_ver, const string &dataset_ver);

bool do_data_transfer(bool compression, FILE *data_stream, DDS &dds,
		      const string &dataset, const string &constraint);

string find_ancillary_file(const string &pathname, const string &ext, 
                           const string &dir, const string &file);

string find_group_ancillary_file(const string &pathname, const string &ext);

void ErrMsgT(const string &Msgt);

string name_path(const string &path);

string rfc822_date(const time_t t);

time_t last_modified_time(const string &name);
//@}

/** These functions are used to create the MIME headers for a message
    from a server to a client. They are public but should not be called
    directly unless absolutely necessary. Use DODSFilter instead.

    NB: These functions actually write both the response status line
    <i>and</i> the header.

    @name MIME utility functions
    @see DODSFilter
*/
//@{
void set_mime_text(FILE *out, ObjectType type = unknown_type, 
		   const string &version = "", EncodingType enc = x_plain,
		   const time_t last_modified = 0);
void set_mime_html(FILE *out, ObjectType type = unknown_type, 
		   const string &version = "", EncodingType enc = x_plain,
		   const time_t last_modified = 0);
void set_mime_binary(FILE *out, ObjectType type = unknown_type, 
		     const string &version = "", EncodingType enc = x_plain,
		     const time_t last_modified = 0);
void set_mime_error(FILE *out, int code = 404, 
		    const string &reason = "Dataset not found",
		    const string &version = "");
void set_mime_not_modified(FILE *out);

//@}

string get_user_supplied_docs(string name, string cgi);

bool remove_mime_header(FILE *in);

bool found_override(string name, string &doc);

#endif // _cgi_util_h
