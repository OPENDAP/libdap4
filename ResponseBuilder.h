
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2011 OPeNDAP, Inc.
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

#ifndef _response_builder_h
#define _response_builder_h

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

#ifndef _object_type_h
#include "ObjectType.h"
#endif

#ifndef _encodingtype_h
#include "EncodingType.h"
#endif

class DAPCache3;

namespace libdap
{

/**

 @brief Build responses for Hyrax server modules/handlers.
 @author jhrg 1/28/2011 */

class ResponseBuilder
{
public:
    friend class ResponseBuilderTest;

protected:
    string d_dataset;  		/// Name of the dataset/database
    string d_ce;  		    /// Constraint expression
    string d_btp_func_ce;   /// The BTP functions, extracted from the CE
    int d_timeout;  		/// Response timeout after N seconds
    string d_default_protocol;	/// Version string for the library's default protocol version

    DAPCache3 *d_cache;

#if 0	// Keyword support moved to Keywords class
    set<string> d_keywords; 	/// Holds all of the keywords passed in the CE
    set<string> d_known_keywords; /// Holds all of the keywords libdap understands.
#endif
    void initialize();

public:

    /** Make an empty instance. Use the set_*() methods to load with needed
        values. You must call at least set_dataset_name() or be requesting
        version information. */
    ResponseBuilder() {
        initialize();
    }

    virtual ~ResponseBuilder();
#if 0
    virtual void add_keyword(const string &kw);
    virtual bool is_keyword(const string &kw) const;
    virtual list<string> get_keywords() const;
    // This method holds all of the keywords that this version of libdap groks
    virtual bool is_known_keyword(const string &w) const;
#endif

    virtual string get_ce() const;
    virtual void set_ce(string _ce);

    virtual string get_btp_func_ce() const { return d_btp_func_ce; }
    virtual void set_btp_func_ce(string _ce) { d_btp_func_ce = _ce; }

    virtual string get_dataset_name() const;
    virtual void set_dataset_name(const string _dataset);

    void set_timeout(int timeout = 0);
    int get_timeout() const;

    virtual void establish_timeout(ostream &stream) const;

    virtual void split_ce(ConstraintEvaluator &eval, const string &expr = "");
    virtual bool is_valid(const string &cache_file_name);

    virtual void send_das(ostream &out, DAS &das, bool with_mime_headers = true);
    virtual void send_das(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                          bool constrained = false, bool with_mime_headers = true);

    virtual void send_dds(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                          bool constrained = false,  bool with_mime_headers = true);

    virtual void dataset_constraint(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                                    bool ce_eval = true) const;
    virtual void dataset_constraint_ddx(ostream &out, DDS & dds, ConstraintEvaluator & eval,
                                   const string &boundary, const string &start,
                                   bool ce_eval = true) const;

    virtual void send_data(ostream &data_stream, DDS &dds, ConstraintEvaluator &eval, bool with_mime_headers = true);

    virtual void send_ddx(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                          bool with_mime_headers = true);

    virtual void send_data_ddx(ostream &data_stream, DDS &dds, ConstraintEvaluator &eval,
                           const string &start, const string &boundary,
                           bool with_mime_headers = true);

    virtual void cache_data_ddx(const string &cache_file_name, DDS &dds);
    virtual void read_data_from_cache(FILE *data, DDS *fdds);
    virtual DDS *get_cached_data_ddx(const string &cache_file_name, BaseTypeFactory *factory);

    // These functions are used both by the methods above and by other code

    void set_mime_text(ostream &out, ObjectType type = unknown_type,
                       EncodingType enc = x_plain,
                       const time_t last_modified = 0,
                       const string &protocol = "") const;

    void set_mime_html(ostream &out, ObjectType type = unknown_type,
                       EncodingType enc = x_plain,
                       const time_t last_modified = 0,
                       const string &protocol = "") const;

    void set_mime_binary(ostream &out, ObjectType type = unknown_type,
                         EncodingType enc = x_plain,
                         const time_t last_modified = 0,
                         const string &protocol = "") const;

    void set_mime_multipart(ostream &out, const string &boundary,
    	const string &start, ObjectType type = unknown_type,
            EncodingType enc = x_plain,
            const time_t last_modified = 0,
            const string &protocol = "") const;

    void set_mime_ddx_boundary(ostream &out, const string &boundary,
    	const string &start, ObjectType type = unknown_type,
            EncodingType enc = x_plain) const;

    void set_mime_data_boundary(ostream &out, const string &boundary,
    	const string &cid, ObjectType type = unknown_type,
            EncodingType enc = x_plain) const;

    void set_mime_error(ostream &out, int code = 404,
                        const string &reason = "Dataset not found",
                        const string &protocol = "") const;
};

} // namespace libdap

#endif // _response_builder_h
