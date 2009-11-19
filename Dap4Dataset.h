// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2009 OPeNDAP, Inc.
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

#ifndef dap4_dataset_h
#define dap4_dataset_h 1

#ifndef dap4_goup_h
#include "Dap4Group.h"
#endif

#ifndef base_type_factory_h
#include "BaseTypeFactory.h"
#endif

#ifndef _dds_h
#include "DDS.h"
#endif

using namespace std;

namespace libdap
{

const string c_default_dap20_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
const string c_default_dap32_schema_location = "http://xml.opendap.org/dap/dap3.2.xsd";
const string c_default_dap33_schema_location = "http://xml.opendap.org/dap/dap3.3.1.xsd";
const string c_default_dap_schema_location = c_default_dap33_schema_location;

const string c_dap20_namespace = "http://xml.opendap.org/ns/DAP2";
const string c_dap32_namespace = "http://xml.opendap.org/ns/DAP/3.2#";
const string c_dap33_namespace = "http://xml.opendap.org/ns/DAP/3.3#";
const string c_dap_namespace = c_dap33_namespace;

const string c_grddl_transformation_dap32 = "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";
const string c_xml_namespace = "http://www.w3.org/XML/1998/namespace";

class Dap4Dataset : public DDS
{
private:
    void m_clone(const Dap4Dataset &dds);
    bool m_groups_only();

    // Dap4Group *d_default_group;
    bool d_no_default_group;

    friend class Dap4DatasetTest;

protected:

public:
    Dap4Dataset(BaseTypeFactory *factory, const string &name);
    Dap4Dataset(const Dap4Dataset &rhs);
    virtual ~Dap4Dataset();

    Dap4Dataset &operator=(const Dap4Dataset &rhs);

#if 0
    // These parsing methods use the flex/bison parsers for the DAP2 curly
    // brace representations
    virtual void parse(string fname);
    virtual void parse(int fd);
    virtual void parse(FILE *in = stdin);
#endif

    virtual void add_var(BaseType *btp);
    virtual void print_xml(ostream &out, bool constrained,
	    const string &blob = "");
    virtual bool check_semantics(bool all = false);

    virtual void dump(ostream &strm) const;
};

} // namespace libdap

#endif // _dap4_dataset_h
