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

#include <cassert>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "BaseType.h"
#include "DapObj.h"

namespace libdap {

const string c_dap40_namespace = "http://xml.opendap.org/ns/DAP/4.0#";

class D4Group;
class D4BaseTypeFactory;
class XMLWriter;

class DDS;

/** DMR is root object for a DAP4 dataset. It holds a D4Group and other
 * information about the dataset (DAP protocol number, DMR version, etc.).
 *
 * @note This class holds the dataset name and filename (which might
 * actually be a database name, but it's usually a filename). The variables
 * of a DAP4 dataset are held by the D4Group instance (which is a child
 * of Constructor).
 */
class DMR : public DapObj {
private:
    D4BaseTypeFactory *d_factory = nullptr;

    /// The name of the dataset. This should not be the pathname to a file
    std::string d_name;
    /// The pathname or other system identifier for the dataset
    std::string d_filename;

    /// DAP protocol major version number. Should be '4'
    int d_dap_major = 4;
    /// DAP protocol minor version number.
    int d_dap_minor = 0;
    /// String version of the DAP protocol number
    std::string d_dap_version = "4.0";

    /// The version of the DMR document
    std::string d_dmr_version = "1.0";

    /// The URL for the request base
    std::string d_request_xml_base;

    /// The namespace to use when printing the XML serialization
    std::string d_namespace = c_dap40_namespace;

    /// The maximum response size (in Kilo bytes)
    uint64_t d_max_response_size_kb = 0;

    /// Whether transferring the whole DMR(the expression constraint is empty)
    bool d_ce_empty = false;

    /// The root group; holds dimensions, enums, variables, groups, ...
    D4Group *d_root = nullptr;

    /// A global flag to indicate if we need to use direct IO
    bool global_dio_flag = false;

    bool utf8_xml_encoding = false;

    bool d_use_dap4_checksums = false;

    friend class DMRTest;
    friend class MockDMR;

protected:
    void m_duplicate(const DMR &dmr);

public:
    DMR() = default;
    DMR(const DMR &dmr);
    explicit DMR(D4BaseTypeFactory *factory, const std::string &name = "");

    DMR(D4BaseTypeFactory *factory, DDS &dds);

    ~DMR() override;

    DMR &operator=(const DMR &rhs);

    virtual void build_using_dds(DDS &dds);

    /**
     * Class invariant. If true, any method can be used.
     * @return True if the instance is OK to use, false otherwise.
     */
    bool OK() const { return (d_factory && d_root && !d_dap_version.empty()); }

    /** Get and set the DMR's name. This is effectively the 'dataset' name.
     * It should not be used to reference the dataset's data store
     * (e.g., it should not be a pathname to a file). This will be used in
     * error messages.
     */
    //@{
    std::string name() const { return d_name; }
    void set_name(const std::string &n) { d_name = n; }
    //@}

    /** Get/set the factory which makes instances of the variables.
        Specialize D4BaseTypeFactory so that a DMR will be
        populated with your client's or server's specialized types.*/
    //@{
    virtual D4BaseTypeFactory *factory() { return d_factory; }
    virtual void set_factory(D4BaseTypeFactory *f) { d_factory = f; }
    //@}

    /** get/set the dataset's 'filename.' The filename is a string that can
     * be used to access the dataset via some data store (it's usually a
     * pathname to a file, but it might be a database key).
     */
    //@{
    std::string filename() const { return d_filename; }
    void set_filename(const std::string &fn) { d_filename = fn; }
    //@}

    std::string dap_version() const { return d_dap_version; }
    void set_dap_version(const std::string &version_string);
    int dap_major() const { return d_dap_major; }
    int dap_minor() const { return d_dap_minor; }

    std::string dmr_version() const { return d_dmr_version; }
    void set_dmr_version(const std::string &v) { d_dmr_version = v; }

    /// Get the URL that will return this DMR
    std::string request_xml_base() const { return d_request_xml_base; }

    /// @see get_request_xml_base
    void set_request_xml_base(const std::string &xb) { d_request_xml_base = xb; }

    /// Get the namespace associated with the DMR
    std::string get_namespace() const { return d_namespace; }

    /// Set the namespace for this DMR
    void set_namespace(const std::string &ns) { d_namespace = ns; }

    /**
     * @brief Get the maximum response size, in KB. Zero indicates no limit.
     * @return The maximum allowable response size. A value of 0 means there is no
     * limit (default).
     */
    long response_limit() const { return (long)d_max_response_size_kb; }

    /**
     * @brief Get the maximum response size, in KB. Zero indicates no limit.
     * @return The maximum allowable response size. A value of 0 means there is no
     * limit (default).
     */
    uint64_t response_limit_kb() const { return d_max_response_size_kb; }

    /**
     * Set the maximum response size. Zero is the default value. The size
     * is given in kilobytes.
     * @param size The maximum size of the response in kilobytes.
     */
    void set_response_limit(long size) { d_max_response_size_kb = size; }

    /**
     * Set the maximum response size. Zero is the default value and indicates there is no limit.
     * The size is given in kilobytes.
     * @param size The maximum size of the response in kilobytes.
     */
    void set_response_limit_kb(const uint64_t &size) { d_max_response_size_kb = size; }

    /// Get the estimated response size, in kilobytes
    long request_size(bool constrained);

    /**
     * @brief Compute the estimated response size, in kilobytes.
     * @param constrained
     * @return The estimated response size, in kilobytes.
     */
    uint64_t request_size_kb(bool constrained);

    /**
     * @return Returns true if the total data bytes requested exceeds the set limit, false otherwise.
     */
    bool too_big() { return d_max_response_size_kb != 0 && request_size_kb(true) > d_max_response_size_kb; }

    /// Set the flag that marks the expression constraint as empty.
    void set_ce_empty(bool ce_empty) { d_ce_empty = ce_empty; }

    /// Get the flag that marks the expression constraint as empty.
    bool get_ce_empty() const { return d_ce_empty; }

    /** Return the root group of this Dataset. If no root group has been
     * set, use the D4BaseType factory to make it.
     * @return The root group of the dataset.
     */
    D4Group *root();

    virtual DDS *getDDS();

    virtual bool is_dap4_projected(std::vector<string> &inventory);

    void print_dap4(XMLWriter &xml, bool constrained = false);

    void dump(std::ostream &strm) const override;

    // The following methods are for direct IO optimization.
    bool get_global_dio_flag() const { return global_dio_flag; }
    void set_global_dio_flag(bool dio_flag_value = true) { global_dio_flag = dio_flag_value; }

    // The following methods are for utf8_encoding.
    bool get_utf8_xml_encoding() const { return utf8_xml_encoding; }
    void set_utf8_xml_encoding(bool encoding_value = true) { utf8_xml_encoding = encoding_value; }

    bool use_checksums() const { return d_use_dap4_checksums; }
    void use_checksums(bool value) { d_use_dap4_checksums = value; }
};

} // namespace libdap

#endif // _dmr_h
