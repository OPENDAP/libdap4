
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

// Specialize DDS for returned data. This currently means adding version
// information about the source of the data. Was it from a version 1, 2 or
// later server?
//
// jhrg 9/19/97

#ifndef _datadds_h
#define _datadds_h 1

#include <iostream>
#include <string>

#ifndef _dds_h
#include "DDS.h"
#endif

namespace libdap
{

/** This class adds some useful state information to the DDS
    structure.  It is for use on the client side of the connection.

    @note Make sure to pass a valid pointer to the DDS constructor or use
    the set_factory() method before actually using the DDS. Also make sure
    that the Factory's lifetime thereafter is the same as the DDS's. Never
    delete the factory until you're done using the DDS.

    @note Update: I removed the DEFAULT_BASETYPE_FACTORY switch because it
    caused more confusion than it avoided. See Trac #130.

    @note The compile-time symbol DEFAULT_BASETYPE_FACTORY controls whether
    the old (3.4 and earlier) DDS and DataDDS constructors are supported.
    These constructors now use a default factory class (BaseTypeFactory,
    implemented by this library) to instantiate Byte, ..., Grid variables. To
    use the default ctor in your code you must also define this symbol. If
    you \e do choose to define this and fail to provide a specialization of
    BaseTypeFactory when your software needs one, you code may not link or
    may fail at run time. In addition to the older ctors for DDS and DataDDS,
    defining the symbol also makes some of the older methods in Connect
    available (because those methods require the older DDS and DataDDS ctors.

    @brief Holds a DAP2 DDS.
    @see Connect
    */

class DataDDS : public DDS
{
private:
    string d_server_version;
    int d_server_version_major;
    int d_server_version_minor;

    string d_protocol_version;
    int d_server_protocol_major;
    int d_server_protocol_minor;

    void m_version_string_to_numbers();
    void m_protocol_string_to_numbers();

public:
    DataDDS(BaseTypeFactory *factory, const string &n = "",
            const string &v = "", const string &p = "");
    // #ifdef DEFAULT_BASETYPE_FACTORY
    // DataDDS(const string &n = "", const string &v = "");
    // #endif
    virtual ~DataDDS()
    {}

    /** Sets the version string.  This typically looks something like:
    <tt>DODS/2.15</tt>, where ``2'' is the major version number, and ``15''
    the minor number. */
    void set_version(const string &v)
    {
        d_server_version = v;
        m_version_string_to_numbers();
    }
    /** @brief Get the server version string, unparsed. */
    string get_version() const
    {
        return d_server_version;
    }
    /** @brief Returns the major version number. */
    int get_version_major() const
    {
        return d_server_version_major;
    }
    /** @brief Returns the minor version number. */
    int get_version_minor() const
    {
        return d_server_version_minor;
    }

    void set_protocol(const string &p)
    {
        d_protocol_version = p;
        m_protocol_string_to_numbers();
    }
    string get_protocol() const
    {
        return d_protocol_version;
    }
    int get_protocol_major() const
    {
        return d_server_protocol_major;
    }
    int get_protocol_minor() const
    {
        return d_server_protocol_minor;
    }

    virtual void dump(ostream &strm) const ;
};

} // namespace libdap

#endif // _datadds_h
