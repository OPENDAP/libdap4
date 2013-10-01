
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
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

//
// jhrg 9/19/97

#include "config.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "DataDDS.h"
#include "debug.h"

using namespace std;

namespace libdap {

// private

/** Parse the version string. A string that does not parse causes the
    version to default to 0.0. This is better than throwing an Error since
    this method is called from a constructor. */
void
DataDDS::m_version_string_to_numbers()
{
    string num = d_server_version.substr(d_server_version.find('/') + 1);

    if (!num.empty() && num.find('.') != string::npos) {
        istringstream iss(num);
        char c;

        iss >> d_server_version_major;
        iss >> c;               // This reads the `.' in the version string
        iss >> d_server_version_minor;

        // Did it parse?
        if (!(c == '.' && d_server_version_major > 0
              && d_server_version_minor > 0)) {

            d_server_version_major = 0;
            d_server_version_minor = 0;
        }
    }
    else {
        d_server_version_major = 0;
        d_server_version_minor = 0;
    }

    DBG(cerr << "Server version: " << d_server_version_major << "." \
        << d_server_version_minor << endl);
}

/** Parse the protocol string. A string that does not parse causes the
    version to default to 2.0. This is better than throwing an Error since
    this method is called from a constructor. */
void
DataDDS::m_protocol_string_to_numbers()
{

    if (!d_protocol_version.empty() && d_protocol_version.find('.')
        != string::npos) {
        istringstream iss(d_protocol_version);
        char c;

        iss >> d_server_protocol_major;
        iss >> c;               // This reads the `.' in the version string
        iss >> d_server_protocol_minor;

        // Did it parse?
        if (!(c == '.' && d_server_protocol_major > 0)) {
            d_server_protocol_major = 2;
            d_server_protocol_minor = 0;
        }
    }
    else {
        d_server_protocol_major = 2;
        d_server_protocol_minor = 0;
    }

    DBG(cerr << "Server version: " << d_server_version_major << "." \
        << d_server_version_minor << endl);
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and then calls parent dump
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
DataDDS::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "DataDDS::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    DDS::dump(strm) ;
    strm << DapIndent::LMarg << "server version: " << d_server_version
         << endl ;
    strm << DapIndent::LMarg << "version major: " << d_server_version_major
         << endl ;
    strm << DapIndent::LMarg << "version minor: " << d_server_version_minor
         << endl ;
    strm << DapIndent::LMarg << "protocol version: " << d_protocol_version
         << endl ;
    strm << DapIndent::LMarg << "protocol major: " << d_server_protocol_major
         << endl ;
    strm << DapIndent::LMarg << "protocol minor: " << d_server_protocol_minor
         << endl ;
    DapIndent::UnIndent() ;
}

// public

/** @brief Make an instance of DataDDS
    A DataDDS instance is a DDS with additional information about the version
    of the server from which the data came.
    @param factory Use this BaseTypeFactory to instantiate the variables.
    Caller must free; can also be set using the set_factory() method. Never
    delete until just before deleting the DDS itself unless you intend to
    replace the factory with a new instance.
    @param n The name of the dataset. Can also be set using the
    set_dataset_name() method.
    @param v The server version.
    @param p The protocol version. */

DataDDS::DataDDS(BaseTypeFactory *factory, const string &n, const string &v,
                 const string &p)
        : DDS(factory, n), d_server_version(v), d_protocol_version(p)
{
    m_version_string_to_numbers();
    m_protocol_string_to_numbers();
}

} // namespace libdap

