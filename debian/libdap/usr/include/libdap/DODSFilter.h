
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

// (c) COPYRIGHT URI/MIT 1997-1999
// Please first read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
// jhrg,jimg James Gallagher <jgallagher@gso.uri.edu>

#ifndef _dodsfilter_h
#define _dodsfilter_h

#include <string>

#ifndef _das_h
#include "DAS.h"
#endif

#ifndef _dds_h
#include "DDS.h"
#endif

#ifndef constraint_evaluator_h
#include "ConstraintEvaluator.h"
#endif

namespace libdap
{

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

    @todo Add a test to make sure that the required arguments are given.
    @todo We need to rethink the ancillary file/directory stuff. I don't
    think it's ever been used...

    @brief Common functions for DODS server filter programs.
    @author jhrg 8/26/97 */

class DODSFilter
{
public:
    /** Types of responses DODSFilter know about. */
    enum Response {
        Unknown_Response,
        DAS_Response,
        DDS_Response,
        DataDDS_Response,
        DDX_Response,
        DataDDX_Response,
        BLOB_Response,
        Version_Response
    };

protected:
    bool d_comp;  // True if the output should be compressed.
    bool d_bad_options;  // True if the options (argc,argv) are bad.
    bool d_conditional_request;

    string d_program_name; // Name of the filter program
    string d_dataset;  // Name of the dataset/database
    string d_dap2ce;  // DAP2 Constraint expression
    string d_cgi_ver;  // Version of CGI script (caller)
    string d_anc_dir;  // Look here for ancillary files
    string d_anc_file;  // Use this for ancillary file name
    string d_cache_dir;  // Use this for cache files
    string d_url;  // URL minus CE.

    Response d_response; // enum name of the response to generate
    string d_action;  // string name of the response to generate

    int d_timeout;  // Server timeout after N seconds

    time_t d_anc_das_lmt; // Last modified time of the anc. DAS.
    time_t d_anc_dds_lmt; // Last modified time of the anc. DDS.
    time_t d_if_modified_since; // Time from a conditional request.

    void initialize();
    void initialize(int argc, char *argv[]);

    virtual int process_options(int argc, char *argv[]);

public:
    /** Make an empty instance. Use the set_*() methods to load with needed
        values. You must call at least set_dataset_name() or be requesting
        version information.

        @todo Add methods to provide a way to set all of the parameters
        this class contains. They can currently only be set using the
        argc/argv command line parameters. */
    DODSFilter()
    {
        initialize();
    }
    DODSFilter(int argc, char *argv[]) throw(Error);

    virtual ~DODSFilter();

    virtual bool is_conditional() const;

    virtual string get_cgi_version() const;
    virtual void set_cgi_version(string version);

    virtual string get_ce() const;
    virtual void set_ce(string _ce);

    virtual string get_dataset_name() const;
    virtual void set_dataset_name(const string _dataset);

    virtual string get_URL() const;
    virtual void set_URL(const string &url);

    virtual string get_dataset_version() const;

    virtual Response get_response() const;
    virtual string get_action() const;
    virtual void set_response(const string &r);

    virtual time_t get_dataset_last_modified_time() const;

    virtual time_t get_das_last_modified_time(const string &anc_location = "") const;

    virtual time_t get_dds_last_modified_time(const string &anc_location = "") const;

    virtual time_t get_data_last_modified_time(const string &anc_location = "") const;

    virtual time_t get_request_if_modified_since() const;

    virtual string get_cache_dir() const;

    void set_timeout(int timeout = 0);

    int get_timeout() const;

    virtual void establish_timeout(ostream &stream) const;

    virtual void print_usage() const;

    virtual void send_version_info() const;

    virtual void send_das(DAS &das, const string &anc_location = "",
                          bool with_mime_headers = true) const;
    virtual void send_das(ostream &out, DAS &das, const string &anc_location = "",
                          bool with_mime_headers = true) const;

    virtual void send_dds(DDS &dds, ConstraintEvaluator &eval,
                          bool constrained = false,
                          const string &anc_location = "",
                          bool with_mime_headers = true) const;
    virtual void send_dds(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                          bool constrained = false,
                          const string &anc_location = "",
                          bool with_mime_headers = true) const;
    // deprecated
    virtual void functional_constraint(BaseType &var, DDS &dds,
                                       ConstraintEvaluator &eval, ostream &out) const;

    virtual void dataset_constraint(DDS &dds, ConstraintEvaluator &eval,
                                    ostream &out, bool ce_eval = true) const;
    virtual void dataset_constraint_ddx(DDS & dds, ConstraintEvaluator & eval,
                                   ostream &out, const string &boundary,
                                   const string &start,
                                   bool ce_eval = true) const;

    virtual void send_data(DDS &dds, ConstraintEvaluator &eval,
                           ostream &data_stream,
                           const string &anc_location = "",
                           bool with_mime_headers = true) const;
    virtual void send_ddx(DDS &dds, ConstraintEvaluator &eval, ostream &out,
                          bool with_mime_headers = true) const;
    virtual void send_data_ddx(DDS &dds, ConstraintEvaluator &eval,
                           ostream &data_stream, const string &start,
                           const string &boundary,
                           const string &anc_location = "",
                           bool with_mime_headers = true) const;

    virtual void establish_timeout(FILE *stream) const;
    virtual void send_das(FILE *out, DAS &das, const string &anc_location = "",
                          bool with_mime_headers = true) const;
    virtual void send_dds(FILE *out, DDS &dds, ConstraintEvaluator &eval,
                          bool constrained = false,
                          const string &anc_location = "",
                          bool with_mime_headers = true) const;
    // deprecated
    virtual void functional_constraint(BaseType &var, DDS &dds,
                                       ConstraintEvaluator &eval, FILE *out) const;

    virtual void dataset_constraint(DDS &dds, ConstraintEvaluator &eval,
                                    FILE *out, bool ce_eval = true) const;
    virtual void send_data(DDS &dds, ConstraintEvaluator &eval,
                           FILE *data_stream,
                           const string &anc_location = "",
                           bool with_mime_headers = true) const;
    virtual void send_ddx(DDS &dds, ConstraintEvaluator &eval, FILE *out,
                          bool with_mime_headers = true) const;
};

} // namespace libdap

#endif // _dodsfilter_h
