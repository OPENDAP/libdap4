
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

#ifndef _d4_response_builder_h
#define _d4_response_builder_h

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

class D4ResponseBuilder
{

protected:
    std::string d_dataset;  		/// Name of the dataset/database
    int d_timeout;  		/// Response timeout after N seconds
    std::string d_default_protocol;	/// Version std::string for the library's default protocol version

    void initialize();

public:
    /** Make an empty instance. Use the set_*() methods to load with needed
        values. You must call at least set_dataset_name() or be requesting
        version information. */
    D4ResponseBuilder() { initialize();  }

    virtual ~D4ResponseBuilder();

    /** The dataset name is the filename or other string that the
     filter program will use to access the data. In some cases this
     will indicate a disk file containing the data.  In others, it
     may represent a database query or some other exotic data
     access method.

     @brief Get the dataset name.
     @return A string object that contains the name of the dataset. */
    virtual std::string get_dataset_name() const { return d_dataset; }
    virtual void set_dataset_name(const std::string _dataset);

    // These are used for DAP4 testing by dmr-test.
    virtual void establish_timeout(ostream &stream) const;
    virtual void remove_timeout() const;

    virtual void send_dmr(std::ostream &out, libdap::DMR &dmr, bool with_mime_headers, bool constrained);
    virtual void send_data_dmr(std::ostream &out, libdap::DMR &dmr, bool with_mime_headers, bool constrained);
};

#endif // _response_builder_h
