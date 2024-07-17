
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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

#ifndef _d4response_builder_h
#define _d4response_builder_h

#include <string>
#include <set>

#if 0
#ifndef _das_h
#include "DAS.h"
#endif

#ifndef _dds_h
#include "DDS.h"
#endif
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

#include "escaping.h"

namespace libdap
{

class DAPCache3;

/**
 * This class is used to build responses for/by the BES. This class replaces
 * DODSFilter (although DODSFilter is still included in the library, its use
 * is deprecated). and it does not have a provision for command line arguments.
 * @author jhrg 1/28/2011
 */

class D4ResponseBuilder
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

    void initialize();

public:

    /** Make an empty instance. Use the set_*() methods to load with needed
        values. You must call at least set_dataset_name() or be requesting
        version information. */
    D4ResponseBuilder() {
        initialize();
    }

    virtual ~D4ResponseBuilder();

    /** Get the complete constraint expression. */
    virtual string ce() const { return d_ce; }
    /** Set the constraint expression to use for this response. */
    virtual void set_ce(const string &ce) { d_ce = www2id(ce, "%", "%20"); }

    virtual string get_btp_func_ce() const { return d_btp_func_ce; }
    virtual void set_btp_func_ce(string _ce) { d_btp_func_ce = _ce; }

    /** Get the dataset name - often a pathname to a file. */
    virtual string dataset_name() const { return d_dataset; }
    /** Set the dataset name, removing any escaping except spaces */
    virtual void set_dataset_name(const string &ds) { d_dataset = www2id(ds, "%", "%20"); }

    /** Set the time out for requests in seconds. Zero means no timeout */
    void set_timeout(int t = 0) { d_timeout = t; }
    /** Get the current request timeout value in seconds. */
    int timeout() const { return d_timeout; }

    virtual void establish_timeout(ostream &stream) const;
    virtual void remove_timeout() const;

    virtual void split_ce(ConstraintEvaluator &eval, const string &expr = "");
    virtual bool is_valid(const string &cache_file_name);
#if 0
    virtual void send_das(ostream &out, DAS &das, bool with_mime_headers = true) const;
    virtual void send_das(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                          bool constrained = false, bool with_mime_headers = true);
#endif
    virtual void send_dds(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                          bool constrained = false,  bool with_mime_headers = true);

    virtual void dataset_constraint(ostream &out, DDS &dds, ConstraintEvaluator &eval, bool ce_eval = true);
    virtual void send_data(ostream &data_stream, DDS &dds, ConstraintEvaluator &eval, bool with_mime_headers = true);

    virtual void send_ddx(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                          bool with_mime_headers = true);

    virtual void dataset_constraint_ddx(ostream &out, DDS & dds, ConstraintEvaluator & eval,
                                   const string &boundary, const string &start,
                                   bool ce_eval = true);

    virtual void send_data_ddx(ostream &data_stream, DDS &dds, ConstraintEvaluator &eval,
                           const string &start, const string &boundary,
                           bool with_mime_headers = true);

#ifdef DAP4
    // DAP4 responses - but do not send the response MIME headers, just the
    // response body.
    virtual void send_dmr(ostream &out, DDS &dds, ConstraintEvaluator &eval);
#endif // DAP4

    virtual void cache_data_ddx(const string &cache_file_name, DDS &dds);
    virtual void read_data_from_cache(FILE *data, DDS *fdds);
    virtual DDS *get_cached_data_ddx(const string &cache_file_name, BaseTypeFactory *factory);

    // This method is uses the above three and is used by send_das(), send_dds(), and send_data().
    virtual DDS *read_cached_dataset(DDS &dds, ConstraintEvaluator & eval, string &cache_token);

#ifdef DAP4
    // These functions are used both by the methods above and by other code

    virtual void send_dap4_data(ostream &data_stream, DDS &dds, ConstraintEvaluator &eval);
#endif // DAP4

    void set_mime_ddx_boundary(ostream &out, const string &boundary,
        const string &start) const;

    void set_mime_data_boundary(ostream &out, const string &boundary,
    const string &cid, const string &endian, unsigned long long len) const;

    // These functions are used both by the methods above and by other code.
    // However, Hyrax uses the OLFS to send the HTTP headers, so these functions
    // are never used in Hyrax. The BES may uses these in other contexts.

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
    	const string &start, ObjectType type = unknown_type, EncodingType enc = x_plain,
    	const time_t last_modified = 0, const string &protocol = "",
    	const string &url = "") const;

    void set_mime_error(ostream &out, int code = 404,
                        const string &reason = "Dataset not found",
                        const string &protocol = "") const;
};

} // namespace libdap

#endif // _response_builder_h
