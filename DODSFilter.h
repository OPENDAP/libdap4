
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
// jhrg,jimg James Gallagher <jgallagher@gso.uri.edu>

#ifndef _dodsfilter_h
#define _dodsfilter_h

#include <string>
#include <set>

#ifndef _das_h
#include "DAS.h"
#endif

#ifndef _dds_h
#include "DDS.h"
#endif

#ifndef constraint_evaluator_h
#include "ConstraintEvaluator.h"
#endif

//#define FILE_METHODS 1
#undef FILE_METHODS

namespace libdap
{

/**
 Originally, this class was intended to simplify processing command line
 options that were passed to various 'handlers' used by the CGI version
 of our data server. That's long gone. This class is now used to trigger
 building responses using libdap by the BES. Since much of the class was
 originally used to parse command line options, there are many parts that
 are deprecated.

 The original use of the code was responsible for three things that are
 now handled by other parts of the Hyrax server: processing ancillary
 metadata (now done using hte NCML module); handling conditional HTTP GET
 requests (now done by the OLFS with some interactions with the BES to get
 information from the file system); and building compressed responses (now
 done by the olfs using a filter). Also note that the 'cache dir' information
 is specific to the HDF4 handler and is handled using a BES parameter (so
 that makes fur things...)

 Now this class is a place where the filename, ce, etc can be processed and
 passed off to the DDS class when a response object is needed.

 @note most (all?) of the old and unused methods are marked as deprecated.

 @brief Common functions for Hyrax server modules/handlers.
 @author jhrg 8/26/97 */

class DODSFilter
{
public:
    friend class DODSFilterTest;

    /** Types of responses DODSFilter knows about. */
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
    // Note that just about everything except the dataset name, ce and timeout
    // are deprecated. I'm adding support for keywords.

    bool d_comp;  /// @deprecated True if the output should be compressed.
    bool d_bad_options;  /// @deprecated True if the options (argc,argv) are bad.
    bool d_conditional_request; /// @deprecated True for a HTTP conditional-get

    string d_program_name; /// @deprecated Name of the filter program
    string d_dataset;  /// Name of the dataset/database
    string d_ce;  /// Constraint expression
    string d_cgi_ver;  /// @deprecated Version of CGI script (caller)
    string d_anc_dir;  /// @deprecated Look here for ancillary files
    string d_anc_file;  /// @deprecated Use this for ancillary file name
    string d_cache_dir;  /// @deprecated Use this for cache files
    string d_url;  /// @deprecated URL minus CE.

    Response d_response; /// @deprecated enum name of the response to generate
    string d_action;  /// @deprecated string name of the response to generate

    int d_timeout;  // Response timeout after N seconds

    time_t d_anc_das_lmt; /// @deprecated Last modified time of the anc. DAS.
    time_t d_anc_dds_lmt; /// @deprecated Last modified time of the anc. DDS.
    time_t d_if_modified_since; /// @deprecated Time from a conditional request.
#if 0
    set<string> d_keywords; /// Holds all of the keywords passed in the CE
    set<string> d_known_keywords; /// Holds all of the keywords libdap understands.
#endif
    void initialize();
    void initialize(int argc, char *argv[]);

    virtual int process_options(int argc, char *argv[]);

public:

    /** Make an empty instance. Use the set_*() methods to load with needed
        values. You must call at least set_dataset_name() or be requesting
        version information. */
    DODSFilter() {
        initialize();
    }
    DODSFilter(int argc, char *argv[]) throw(Error);

    virtual ~DODSFilter();
#if 0
    virtual void add_keyword(const string &kw);
    virtual bool is_keyword(const string &kw) const;
    virtual list<string> get_keywords() const;
    // This method holds all of the keywords that this version of libdap groks
    virtual bool is_known_keyword(const string &w) const;
#endif
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

#if FILE_METHODS
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
#endif
};

} // namespace libdap

#endif // _dodsfilter_h
