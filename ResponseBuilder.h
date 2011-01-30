
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

namespace libdap
{

/**

 @brief Build responses for Hyrax server modules/handlers.
 @author jhrg 1/28/2011 */

class ResponseBuilder
{
public:
    friend class ResponseBuilderTest;

    /** Types of responses ResponseBuilder knows about. */
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
    string d_dataset;  /// Name of the dataset/database
    string d_ce;  /// Constraint expression
    int d_timeout;  // Response timeout after N seconds

    set<string> d_keywords; /// Holds all of the keywords passed in the CE
    set<string> d_known_keywords; /// Holds all of the keywords libdap understands.

    void initialize();

public:

    /** Make an empty instance. Use the set_*() methods to load with needed
        values. You must call at least set_dataset_name() or be requesting
        version information. */
    ResponseBuilder() {
        initialize();
    }
    virtual ~ResponseBuilder();

    virtual void add_keyword(const string &kw);
    virtual bool is_keyword(const string &kw) const;
    virtual list<string> get_keywords() const;
    // This method holds all of the keywords that this version of libdap groks
    virtual bool is_known_keyword(const string &w) const;

    virtual string get_ce() const;
    virtual void set_ce(string _ce);

    virtual string get_dataset_name() const;
    virtual void set_dataset_name(const string _dataset);

    void set_timeout(int timeout = 0);
    int get_timeout() const;

    virtual void establish_timeout(ostream &stream) const;

    virtual void send_das(ostream &out, DAS &das, const string &anc_location = "",
                          bool with_mime_headers = true) const;
    virtual void send_dds(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                          bool constrained = false,
                          const string &anc_location = "",
                          bool with_mime_headers = true) const;

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

};

} // namespace libdap

#endif // _dodsfilter_h
