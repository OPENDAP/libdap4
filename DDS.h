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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Provide access to the DDS. This class is used to parse DDS text files, to
// produce a printed representation of the in-memory variable table, and to
// update the table on a per-variable basis.
//
// jhrg 9/8/94

#ifndef _dds_h
#define _dds_h 1

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _constructor_h
#include "Constructor.h"
#endif

#ifndef base_type_factory_h
#include "BaseTypeFactory.h"
#endif

#ifndef _das_h
#include "DAS.h"
#endif

#ifndef A_DapObj_h
#include "DapObj.h"
#endif

#if 0
#ifndef KEYWORDS_H_
#include "Keywords2.h"
#endif
#endif

#ifndef XMLWRITER_H_
#include "XMLWriter.h"
#endif

namespace libdap
{

bool has_dap2_attributes(BaseType *btp);
bool has_dap2_attributes(AttrTable &a);

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

class DDS : public DapObj
{
private:
    BaseTypeFactory *d_factory;

    string d_name;                // The dataset d_name
    string d_filename;		    // File d_name (or other OS identifier) for
    string d_container_name;	// d_name of container structure
    Structure *d_container; 	// current container for container d_name
				                // dataset or part of dataset.

    int d_dap_major;       	    // The protocol major version number
    int d_dap_minor;       	    // ... and minor version number
    string d_dap_version; 	    // String version of the protocol
    string d_request_xml_base;
    string d_namespace;

    AttrTable d_attr;           // Global attributes.

    vector<BaseType *> vars;    // Variables at the top level

    int d_timeout;              // alarm time in seconds. If greater than
                                // zero, raise the alarm signal if more than
                                // d_timeout seconds are spent reading data.
#if 0
    Keywords d_keywords;	    // Holds keywords parsed from the CE
#endif

    long d_max_response_size;   // In bytes...

    friend class DDSTest;

protected:
    void duplicate(const DDS &dds);
    BaseType *leaf_match(const string &name, BaseType::btp_stack *s = 0);
    BaseType *exact_match(const string &name, BaseType::btp_stack *s = 0);

public:
    typedef std::vector<BaseType *>::const_iterator Vars_citer ;
    typedef std::vector<BaseType *>::iterator Vars_iter ;
    typedef std::vector<BaseType *>::reverse_iterator Vars_riter ;

    DDS(BaseTypeFactory *factory, const string &name = "");
    DDS(BaseTypeFactory *factory, const string &name, const string &version);
    DDS(const DDS &dds);

    virtual ~DDS();

    DDS & operator=(const DDS &rhs);

    virtual void transfer_attributes(DAS *das);

    string get_dataset_name() const;
    void set_dataset_name(const string &n);

    /** Return the factory which makes instances of the Byte, ..., Grid
        type classes. Specialize BaseTypeFactory so that a DDS will be
        populated with your client or server's specialized types.
        @return An instance of BaseTypeFactory. */
    BaseTypeFactory *get_factory() const
    {
        return d_factory;
    }

    /** Set the factory class used to instantiate variables during the
        parse of a DDS.
        @param factory The factory this DDS should use. Caller must free
        factory when done with this DDS.
        @return The old factory.
        @see BaseTypeFactory */
    BaseTypeFactory *set_factory(BaseTypeFactory *factory)
    {
        BaseTypeFactory *t = d_factory;
        d_factory = factory;
        return t;
    }

    virtual AttrTable &get_attr_table();

    string filename() const;
    void filename(const string &fn);

    /// Get the DAP major version as sent by the client
    int get_dap_major() const { return d_dap_major; }
    /// Get the DAP minor version as sent by the client
    int get_dap_minor() const { return d_dap_minor; }

    void set_dap_version(const string &version_string = "2.0");
    string get_dap_version() const { return d_dap_version; }
    string get_dmr_version() const { return "1.0"; }

    /// @deprecated
    void set_dap_major(int p);
    /// @deprecated
    void set_dap_minor(int p);
    /// @deprecated
    void set_dap_version(double d);

#if 0
    Keywords &get_keywords() {return d_keywords;}
#endif

    /// Get the URL that will return this DDS/DDX/DataThing
    string get_request_xml_base() const { return d_request_xml_base; }

    /// @see get_request_xml_base
    void set_request_xml_base(const string &xb) { d_request_xml_base = xb; }

    /// Get the namespace associated with the DDS - likely set only by DDX responses
    string get_namespace() const { return d_namespace; }

    /// Set the namespace for this DDS/DDX object/response
    void set_namespace(const string &ns) { d_namespace = ns; }

    /// Get the maximum response size, in Bytes. Zero indicates no limit.
    long get_response_limit() { return d_max_response_size; }

    /** Set the maximum response size. Zero is the default value. The size
        is given in kilobytes (but stored internally as the number of bytes).
        @param size The maximum size of the response in kilobytes. */
    void set_response_limit(long size) { d_max_response_size = size * 1024; }

    /// Get the estimated response size.
    int get_request_size(bool constrained);

    string container_name() ;
    void container_name( const string &cn ) ;
    Structure *container() ;

    void add_var(BaseType *bt);
    void add_var_nocopy(BaseType *bt);

    /// Removes a variable from the DDS.
    void del_var(const string &n);

    BaseType *var(const string &n, BaseType::btp_stack &s);
    BaseType *var(const string &n, BaseType::btp_stack *s = 0);
    int num_var();

    /// Return an iterator to the first variable
    Vars_iter var_begin();
#if 0
    /// Return a const iterator.
    Vars_citer var_cbegin() const { return vars.cbegin(); }
#endif
    /// Return a reverse iterator
    Vars_riter var_rbegin();
    /// Return an iterator
    Vars_iter var_end();
#if 0
    /// Return a const iterator
    Vars_citer var_cend() const { return vars.cend(); }
#endif
    /// Return a reverse iterator
    Vars_riter var_rend();
    /// Get an iterator
    Vars_iter get_vars_iter(int i);
    /// Get a variable
    BaseType *get_var_index(int i);
    /// Insert a variable before the referenced element
    void insert_var(Vars_iter i, BaseType *ptr);
    void insert_var_nocopy(Vars_iter i, BaseType *ptr);
    /// Removes a variable from the DDS.
    void del_var(Vars_iter i);
    /// Removes a range of variables from the DDS.
    void del_var(Vars_iter i1, Vars_iter i2);

    /** @name DDS_timeout
     *  Old deprecated DDS timeout code
     *  @deprecated
     */
    ///@{
    void timeout_on();
    void timeout_off();
    void set_timeout(int t);
    int get_timeout();
    ///@}

    // These parse the DAP2 curly-brace document and make a C++ object.
    void parse(string fname);
    void parse(int fd);
    void parse(FILE *in = stdin);

    // These print the Binary object in either the curly-brace or XML reps
    void print(FILE *out);
    void print_constrained(FILE *out);
    void print_xml(FILE *out, bool constrained, const string &blob = "");

    // Same as above, but using C++ i/o streams
    void print(ostream &out);
    void print_constrained(ostream &out);
    void print_xml(ostream &out, bool constrained, const string &blob = "");

    // Print the XML using libxml2; the other print_xml methods use this impl.
    void print_xml_writer(ostream &out, bool constrained, const string &blob = "");

    // Print the DAP4 DMR 'object'
    void print_dmr(ostream &out, bool constrained);

    void print_das(ostream &out);
    DAS *get_das();
    void get_das(DAS *das);

    void mark_all(bool state);
    bool mark(const string &name, bool state);
    bool check_semantics(bool all = false);

    void tag_nested_sequences();

    virtual void dump(ostream &strm) const ;
};

} // namespace libdap

#endif // _dds_h
