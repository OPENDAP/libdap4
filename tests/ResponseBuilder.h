
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

class libdap::ConstraintEvaluator;
class libdap::DDS;
class libdap::DMR;

/**
 * Used for testing only. This duplicates code in the bes/dap module.
 * jhrg 6/11/13
 *
 * This has now been extended with DAP4 DMR and Data response code.
 * jhrg 9/5/13
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
    ResponseBuilder() { initialize();  }

    virtual ~ResponseBuilder();

    /** Return the entire constraint expression in a string.  This
     includes both the projection and selection clauses, but not the
     question mark.

     @brief Get the constraint expression.
     @return A string object that contains the constraint expression. */
    virtual std::string get_ce() const { return d_ce; }
    virtual void set_ce(std::string _ce);

    /** The dataset name is the filename or other string that the
     filter program will use to access the data. In some cases this
     will indicate a disk file containing the data.  In others, it
     may represent a database query or some other exotic data
     access method.

     @brief Get the dataset name.
     @return A string object that contains the name of the dataset. */
    virtual std::string get_dataset_name() const { return d_dataset; }
    virtual void set_dataset_name(const std::string _dataset);

    // These are used for DAP2 testing by expr-test.
    virtual void dataset_constraint(std::ostream &out, libdap::DDS &dds, libdap::ConstraintEvaluator &eval, bool ce_eval = true);
    virtual void send_data(std::ostream &data_stream, libdap::DDS &dds, libdap::ConstraintEvaluator &eval, bool with_mime_headers = true);

    // These are used for DAP4 testing by dmr-test.
    virtual void establish_timeout(ostream &stream) const;
    virtual void remove_timeout() const;

    virtual void send_dmr(std::ostream &out, libdap::DMR &dmr, libdap::ConstraintEvaluator &eval, bool with_mime_headers);

    virtual void dataset_constraint_dmr_multipart(std::ostream &out, libdap::DMR &dmr, libdap::ConstraintEvaluator &eval,
            const string &start, const string &boundary, bool filter);

    virtual void send_data_dmr_multipart(std::ostream &out, libdap::DMR &dmr, libdap::ConstraintEvaluator &eval,
    		const string &start, const string &boundary, bool with_mime_headers);

    virtual void dataset_constraint_dmr(std::ostream &out, libdap::DMR &dmr, libdap::ConstraintEvaluator &eval,
            bool filter);

    virtual void send_data_dmr(std::ostream &out, libdap::DMR &dmr, libdap::ConstraintEvaluator &eval,
    		bool with_mime_headers);
};

#endif // _response_builder_h
