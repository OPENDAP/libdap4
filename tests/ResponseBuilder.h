
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef _response_builder_h
#define _response_builder_h

#include <string>

namespace libdap {

class ConstraintEvaluator;
class DDS;

/**
 * Used for testing only. This duplicates code in the bes/dap module.
 * jhrg 6/11/13
 */

class ResponseBuilder
{
public:
    friend class ResponseBuilderTest;

protected:
    std::string d_dataset;  		/// Name of the dataset/database
    std::string d_ce;  		    /// Constraint expression
    std::string d_btp_func_ce;   /// The BTP functions, extracted from the CE
    int d_timeout;  		/// Response timeout after N seconds
    std::string d_default_protocol;	/// Version std::string for the library's default protocol version

    void initialize();

public:

    /** Make an empty instance. Use the set_*() methods to load with needed
        values. You must call at least set_dataset_name() or be requesting
        version information. */
    ResponseBuilder() {
        initialize();
    }

    virtual ~ResponseBuilder();

    virtual std::string get_ce() const;
    virtual void set_ce(std::string _ce);

    void split_ce(ConstraintEvaluator &eval, const string &expr = "");

    virtual std::string get_dataset_name() const;
    virtual void set_dataset_name(const std::string _dataset);

    virtual void dataset_constraint(std::ostream &out, libdap::DDS &dds, libdap::ConstraintEvaluator &eval, bool ce_eval = true);
    virtual void send_data(std::ostream &data_stream, libdap::DDS &dds, libdap::ConstraintEvaluator &eval, bool with_mime_headers = true);
};

}
#endif // _response_builder_h
