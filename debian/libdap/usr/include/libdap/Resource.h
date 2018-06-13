
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
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

#ifndef resource_h
#define resource_h

#include <string>
#include <iostream>

#ifndef _error_h
#include "Error.h"
#endif

using namespace std;

namespace libdap
{

/** Bind an ancillary resource with the rule that should be used when
    combining it with a primary resource. Ancillary resources are always
    specified using URLs. If an ancillary resource is a local file, use
    <code>file://</code> URLs.

    Note that operator<< is defined for Resource as a function.

    @brief Associate a rule with an ancillary resource.
    @author James Gallagher <jgallagher@opendap.org> */
class Resource
{
public:

    /** The AIS uses this enumeration to describe how a given ancillary should
    be merged into a primary data source.

    <ul>
    <li>overwrite: Attributes in the ancillary source overwrite those in
    the primary source. New values are added.</li>
    <li>replace: The ancillary source replaces the primary. All of the
    Attributes in the primary are removed.</li>
    <li>fallback: The ancillary resource provides a set of fallback values
    if the primary data source lacks any attributes. Note that this does
    not apply to individual attributes, but to an entire set. The fallback
    attributes are used only if the original data source lacks attributes
    altogether. 
    </ul>

    @brief How are ancillary resources used.
    @author James Gallagher <jgallagher@opendap.org> */
    enum rule { overwrite, replace, fallback };

    /** Build a Resource with a null URL and set the combination rule to the
    default. */
    Resource() : d_url(""), d_rule(overwrite)
    {}

    /** Build a resource. Set the combination rule to the default value,
    which is overwrite. 
    @param u The ancillary resource URL. */
    Resource(const string &u) : d_url(u), d_rule(overwrite)
    {}

    /** Build a Resource.
    @param u The ancillary resource URL.
    @param r The combination rule. */
    Resource(const string &u, const rule &r) : d_url(u), d_rule(r)
    {}

    /** Build a Resource.

        Note: If this is used in a callback, make sure to check the value of
    <code>r</code> before calling this constructor. Exceptions thrown
    within callbacks are not portable. Valid values are "overwrite",
    "replace" and "fallback". The constructor accepts "default" as a
    synonym for "overwrite".

    @param u The ancillary resource URL.
    @param r The name of the combination rule. */
    Resource(const string &u, const string &r) throw(Error) : d_url(u)
    {
        if (r == "replace")
            d_rule = replace;
        else if (r == "fallback")
            d_rule = fallback;
        else if (r == "overwrite" || r == "default")
            d_rule = overwrite;
        else
            throw Error(string("An AIS Resource object was created with an unknown rule type '") + r);
    }

    virtual ~Resource()
{}

    /** Return the resource URL. */
    virtual string get_url() const
    {
        return d_url;
    }

    /** Set the resource URL.
    @param u The resource's URL. */
    virtual void set_url(const string &u)
    {
        d_url = u;
    }

    /** Return combination rule for this resource. */
    virtual Resource::rule get_rule() const
    {
        return d_rule;
    }

    /** Set the resource's combination rule.
    @param r The combination rule. */
    virtual void set_rule(const Resource::rule &r)
    {
        d_rule = r;
    }

    /** Write the XML for this resource. This function is defined in
    AISResoruces. 
    @param os Write to this ostream.
    @paran r The Resource to write. */
    friend ostream &operator<<(ostream &os, const Resource &r);


private:

    string d_url;
    Resource::rule d_rule;
};

} // namespace libdap

#endif // resource_h
