
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
 
// (c) COPYRIGHT URI/MIT 1997-1999
// Please first read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//	jhrg,jimg	James Gallagher <jgallagher@gso.uri.edu>

#ifndef _dodsfilter_h
#define _dodsfilter_h

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>

#ifndef _das_h
#include "DAS.h"
#endif

#ifndef _dds_h
#include "DDS.h"
#endif


/** When a DODS server receives a request from a DODS client, the
    server CGI script dispatches the request to one of several
    ``filter'' programs.  Each filter is responsible for returning a
    different aspect of the dataset information: one is for data, one
    is for the dataset DDS, one is for the dataset DAS, and a fourth
    is for a usage message describing the server itself.  Some
    installations may have additional optional filters.

    The filter program receives a data request from the dispatch
    script. It receives its operating parameters from the command
    line, like any UNIX command, and it returns its output to standard
    output, which the httpd server packages up into a reply to the
    client. 

    This class contains some common functions for the filter programs
    used to make up the DODS data servers. The filter programs do not
    <i>have</i> to be called by a CGI program, but that is the normal
    mechanism by which they are invoked.
  
    @brief Common functions for DODS server filter programs.
    @author jhrg 8/26/97 */

class DODSFilter {
public:
    /** Types of responses DODSFilter know about. */
    enum Response {
	Unknown_Response,
	DAS_Response,
	DDS_Response,
	DataDDS_Response,
	Version_Response
    };

protected:
    bool d_comp;		// True if the output should be compressed.
    bool d_bad_options;		// True if the options (argc,argv) are bad.
    bool d_conditional_request;

    string d_program_name;	// Name of the filter program
    string d_dataset;		// Name of the dataset/database 
    string d_ce;		// Constraint expression 
    string d_cgi_ver;		// Version of CGI script (caller)
    string d_anc_dir;		// Look here for ancillary files
    string d_anc_file;		// Use this for ancillary file name
    string d_cache_dir;		// Use this for cache files

    Response d_response;	// Name of the response to generate

    time_t d_anc_das_lmt;	// Last modified time of the anc. DAS.
    time_t d_anc_dds_lmt;	// Last modified time of the anc. DDS.
    time_t d_if_modified_since;	// Time from a conditional request.

    DODSFilter() {}		// Private default ctor.

    void initialize(int argc, char *argv[]) throw(Error);

    virtual int process_options(int argc, char *argv[]) throw(Error);

public:
    DODSFilter(int argc, char *argv[]) throw(Error);

    virtual ~DODSFilter();

    virtual bool is_conditional();

    virtual string get_cgi_version();
    virtual void set_cgi_version(string version);

    virtual string get_ce();
    virtual void set_ce(string _ce);

    virtual string get_dataset_name();
    virtual void set_dataset_name(const string _dataset);

    virtual string get_dataset_version();

    virtual Response get_response();
    virtual void set_response(const string &r) throw(Error);

    virtual time_t get_dataset_last_modified_time();

    virtual time_t get_das_last_modified_time(const string &anc_location="");

    virtual time_t get_dds_last_modified_time(const string &anc_location="");

    virtual time_t get_data_last_modified_time(const string &anc_location="");

    virtual time_t get_request_if_modified_since();

    virtual string get_cache_dir();

    virtual void read_ancillary_das(DAS &das, string anc_location = "");

    virtual void read_ancillary_dds(DDS &dds, string anc_location = "");

    virtual void print_usage() throw(Error);

    virtual void send_version_info();

    virtual void send_das(DAS &das, const string &anc_location = "");

    virtual void send_das(ostream &os, DAS &das,
			  const string &anc_location="");

    virtual void send_das(FILE *out, DAS &das, const string &anc_location="");

    virtual void send_dds(DDS &dds, bool constrained = false,
			  const string &anc_location = "");

    virtual void send_dds(ostream &os, DDS &dds, bool constrained = false,
			  const string &anc_location = "");

    virtual void send_dds(FILE *out, DDS &dds, bool constrained = false,
			  const string &anc_location = "");

    virtual void send_data(DDS &dds, FILE *data_stream,
			   const string &anc_location = "");
};

// $Log: DODSFilter.h,v $
// Revision 1.27  2003/05/13 22:10:58  jimg
// MOdified DODSFilter so that it takes a -o switch which names the type
// of response to generate. This can be used to build a single hander
// which can return all of the responses.
//
// Revision 1.26  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.25  2003/03/13 23:57:04  jimg
// Added process_options() and initialize() methods. These facilitate
// specializing this class.
//
// Revision 1.24  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.23.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.23  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.22  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.18.2.5  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.21  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.20  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.18.2.4  2001/08/21 14:54:43  dan
// Added a set_dataset_name method to provide a mechanism to change the
// dataset name in the DODSFilter class, which currently can only be set
// by running the constructor.   This method was required for a modification
// to the jg-dods server which now support relative pathnames as part of
// the object name.
//
// Revision 1.19  2001/06/15 23:49:02  jimg
// Merged with release-3-2-4.
//
// Revision 1.18.2.3  2001/06/14 21:32:04  jimg
// Added a method to set the cgi_version property without relying on the ctor.
//
// Revision 1.18.2.2  2001/05/03 19:10:35  jimg
// Added the d_conditional_request and d_if_modified_since fields. These are
// used to indicate that the request from the client is a conditional GET
// request. DODS currently only supports conditional requests based on the Last
// Modified time included by a server in a response. The ctor takes a -l switch
// which expects the time given with the If-Modified-Since header. This provides
// an easy way for servers to handle the conditional request since they can
// simply pass the switches and parameters they receive directly to DODSFilter
// (as if they were opaque objects).
//
// Revision 1.18.2.1  2001/04/23 22:34:46  jimg
// Added support for the Last-Modified MIME header in server responses.`
//
// Revision 1.18  2000/10/30 17:21:27  jimg
// Added support for proxy servers (from cjm).
//
// Revision 1.17  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.16  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.15  2000/07/09 21:57:09  rmorris
// Mods's to increase portability, minimuze ifdef's in win32 and account
// for differences between the Standard C++ Library - most notably, the
// iostream's.
//
// Revision 1.14  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.13  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.12.10.1  2000/06/02 18:16:48  rmorris
// Mod's for port to Win32.
//
// Revision 1.11.2.3  2000/05/18 20:45:27  jimg
// added set_ce(). Maybe add more set methods?
//
// Revision 1.12.4.1  2000/02/07 21:11:36  jgarcia
// modified prototypes and implementations to use exceeption handling
//
// Revision 1.11.2.2  1999/09/08 22:36:03  jimg
// Fixed the -V comment.
//
// Revision 1.12  1999/09/03 22:07:44  jimg
// Merged changes from release-3-1-1
//
// Revision 1.11.2.1  1999/08/28 06:43:04  jimg
// Fixed the implementation/interface pragmas and misc comments
//
// Revision 1.11  1999/05/25 21:57:12  dan
// Added an optional second argument to read_ancillary_dds to support JGOFS
// usage.
//
// Revision 1.10  1999/05/25 21:54:50  dan
// Added an optional second argument to read_ancillary_das to support
// JGOFS usage.
//
// Revision 1.9  1999/05/05 00:48:07  jimg
// Added the get_cgi_version() member function.
// Added documentation about get_cgi_version() and the -V option (new).
//
// Revision 1.8  1999/05/04 19:47:21  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.7  1999/04/29 02:29:28  jimg
// Merge of no-gnu branch
//
// Revision 1.6  1999/02/22 22:58:02  jimg
// Added the get_accept_types() accessor. Also added to the ctor so that the -t
// option will be parsed properly and used to set the value of accept_types.
//
// Revision 1.5  1999/01/21 20:42:01  tom
// Fixed comment formatting problems for doc++
//
// Revision 1.4.2.1  1999/02/02 21:56:57  jimg
// String to string version
//
// Revision 1.4  1998/08/06 16:11:47  jimg
// Added cache_dir member (from jeh).
//
// Revision 1.3  1998/02/04 14:55:32  tom
// Another draft of documentation.
//
// Revision 1.2  1997/09/22 23:04:59  jimg
// Added doc++ style comments.
//
// Revision 1.1  1997/08/28 20:39:02  jimg
// Created
//

#endif // _dodsfilter_h
