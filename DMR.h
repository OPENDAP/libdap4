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


#ifndef _dmr_h
#define _dmr_h 1

#include <iostream>
#include <string>
#include <vector>

#include "D4Group.h"
#include "XMLWriter.h"
#include "DapObj.h"

namespace libdap
{

class D4BaseTypeFactory;

/** The DAP2 Data Descriptor Object (DDS) is a data structure used by
    the DAP2 software to describe datasets and subsets of those
    datasets.  The DDS may be thought of as the declarations for the
    data structures that will hold data requested by some DAP2 client.
    Part of the job of a DAP2 server is to build a suitable DDS for a
    specific dataset and to send it to the client.  Depending on the
    data access API in use, this may involve reading part of the
    dataset and inferring the DDS.  Other APIs may require the server
    simply to read some ancillary data file with the DDS in it.

    On the server side, in addition to the data declarations, the DDS
    holds the clauses of any constraint expression that may have
    accompanied the data request from the DAP2 client.  The DDS object
    includes methods for modifying the DDS according to the given
    constraint expression.  It also has methods for directly modifying
    a DDS, and for transmitting it from a server to a client.

    For the client, the DDS object includes methods for reading the
    persistent form of the object sent from a server. This includes parsing
    the ASCII representation of the object and, possibly, reading data
    received from a server into a data object.

    Note that the class DDS is used to instantiate both DDS and DataDDS
    objects. A DDS that is empty (contains no actual data) is used by servers
    to send structural information to the client. The same DDS can becomes a
    DataDDS when data values are bound to the variables it defines.

    For a complete description of the DDS layout and protocol, please
    refer to <i>The OPeNDAP User Guide</i>.

    The DDS has an ASCII representation, which is what is transmitted
    from a DAP2 server to a client.  Here is the DDS representation of
    an entire dataset containing a time series of worldwide grids of
    sea surface temperatures:

    <pre>
    Dataset {
        Float64 lat[lat = 180];
        Float64 lon[lon = 360];
        Float64 time[time = 404];
        Grid {
         ARRAY:
            Int32 sst[time = 404][lat = 180][lon = 360];
         MAPS:
            Float64 time[time = 404];
            Float64 lat[lat = 180];
            Float64 lon[lon = 360];
        } sst;
    } weekly;
    </pre>

    If the data request to this dataset includes a constraint
    expression, the corresponding DDS might be different.  For
    example, if the request was only for northern hemisphere data
    at a specific time, the above DDS might be modified to appear like
    this:

    <pre>
    Dataset {
        Grid {
         ARRAY:
            Int32 sst[time = 1][lat = 90][lon = 360];
         MAPS:
            Float64 time[time = 1];
            Float64 lat[lat = 90];
            Float64 lon[lon = 360];
        } sst;
    } weekly;
    </pre>

    Since the constraint has narrowed the area of interest, the range
    of latitude values has been halved, and there is only one time
    value in the returned array.  Note that the simple arrays (<tt>lat</tt>,
    <tt>lon</tt>, and <tt>time</tt>) described in the dataset are also
    part of the <tt>sst</tt> Grid object.  They can be requested by
    themselves or as part of that larger object.

    See the <i>The OPeNDAP User Guide</i>, or the documentation of the
    BaseType class for descriptions of the DAP2 data types.

    @note Make sure to pass a valid pointer to the DDS constructor or use
    the set_factory() method before actually using the DDS. Also make sure
    that the Factory's lifetime thereafter is the same as the DDS's. Never
    delete the factory until you're done using the DDS.

    @note Update: I removed the DEFAULT_BASETYPE_FACTORY switch because it
    caused more confusion than it avoided. See Trac #130. jhrg

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

    @see BaseType
    @see DAS */

class DMR : public DapObj
{
private:
    D4BaseTypeFactory *d_factory;

    /// The name of the dataset. This should not be the pathname to a file
    string d_name;
    /// The pathname or other system identifier for the dataset
    string d_filename;

    /// DAP protocol major version number. Should be '4'
    int d_dap_major;
    /// DAP protocol minor version number.
    int d_dap_minor;
    /// String version of the DAP protocol number
    string d_dap_version;

    /// The version of the DMR document
    string d_dmr_version;

    /// The URL for the request base
    string d_request_xml_base;

    /// The namespace to use when printing the XML serialization
    string d_namespace;

    /// When sending data, timeout after this many seconds
    int d_timeout;

    /// The maximum response size (in Kilo bytes)
    long d_max_response_size;

    /// The root group; holds dimensions, enums, variables, groups, ...
    D4Group *d_root;

    friend class DMRTest;

protected:
    void m_duplicate(const DMR &dmr);

public:
    DMR();
    DMR(const DMR &dds);
    DMR(D4BaseTypeFactory *factory, const string &name = "");

    virtual ~DMR();

    DMR &operator=(const DMR &rhs);

    /** Get and set the DMR's name. This is effectively the 'dataset' name.
     * It should not be used to reference the dataset's data store
     * (e.g., it should not be a pathname to a file). This will be used in
     * error messages.
     */
    //@{
    string name() const { return d_name; }
    void set_name(const string &n) { d_name = n; }
    //@}

    /** Get/set the factory which makes instances of the variables.
        Specialize D4BaseTypeFactory so that a DMR will be
        populated with your client or server's specialized types.*/
    //@{
    virtual D4BaseTypeFactory *factory() const { return d_factory; }
    virtual void set_factory(D4BaseTypeFactory *f) { d_factory = f; }
    //@}

    /** get/set the dataset's 'filename.' The filename is a string that can
     * be used to access the dataset's actual data store (it's usually a
     * pathname to a file, but it might be a database key.
     *
     * @todo Move this to Group?
     */
    //@{
    string filename() const { return d_filename; }
    void set_filename(const string &fn) { d_filename = fn;}
    //@}

    // TODO Add documentation
    string dap_version() const { return d_dap_version; }
    int dap_major() const { return d_dap_major; }
    int dap_minor() const { return d_dap_minor; }
    void set_dap_version(const string &version_string);

    string dmr_version() const { return d_dmr_version; }
    void set_dmr_version(const string &v) { d_dmr_version = v; }

    /// Get the URL that will return this DMR/DDX/DataThing
    string request_xml_base() const { return d_request_xml_base; }

    /// @see get_request_xml_base
    void set_request_xml_base(const string &xb) { d_request_xml_base = xb; }

    /// Get the namespace associated with the DDS - likely set only by DDX responses
    string get_namespace() const { return d_namespace; }

    /// Set the namespace for this DDS/DDX object/response
    void set_namespace(const string &ns) { d_namespace = ns; }

    /// Get the maximum response size, in KB. Zero indicates no limit.
    long response_limit() { return d_max_response_size; }

    /** Set the maximum response size. Zero is the default value. The size
        is given in kilobytes.
        @param size The maximum size of the response in kilobytes. */
    void set_response_limit(long size) { d_max_response_size = size; }

    /// Get the estimated response size, in kilo bytes
    long request_size(bool constrained);

    void timeout_on();
    void timeout_off();

    int get_timeout() const { return d_timeout; }
    void set_timeout(int t) { d_timeout = t; }

    D4Group *root() {
        if (!d_root) d_root = new D4Group("");
        return d_root;
    }

    void print_dap4(XMLWriter &xml, bool constrained = false);

    virtual void dump(ostream &strm) const ;
};

} // namespace libdap

#endif // _dmr_h
