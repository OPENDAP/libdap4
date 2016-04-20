
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

// Using the DASVHMap class, build a parser for the DAS and add functions
// that provide access to the variables, their attributes and values.
//
// jhrg 7/25/94

#ifndef _das_h
#define _das_h 1


#include <cstdio>
#include <string>
#include <iostream>

#ifndef _attrtable_h
#include "AttrTable.h"
#endif

using std::cout;

namespace libdap
{

/** @brief Hold attribute data for a DAP2 dataset.

    The Data Attribute Structure is a set of name-value pairs used to
    describe the data in a particular dataset. The name-value pairs are
    called the ``attributes''. The values may be of any of the DAP2 simple
    data types (Byte, Int16, UInt16, Int32, UInt32, Float32, Float64, String
    and URL), and may be scalar or vector. Note that all values are actually
    stored as String data, making the easy to read/check using a web browser.

    A value may also consist of a set of other name-value pairs.  This
    makes it possible to nest collections of attributes, giving rise
    to a hierarchy of attributes.  DAP2 uses this structure to provide
    information about variables in a dataset.  For example, consider
    the dataset used in the DDS example earlier.

    In the following example of a DAS, several of the attribute
    collections have names corresponding to the names of variables in
    the DDS example.  The attributes in that collection are said to
    belong to that variable.  For example, the <tt>lat</tt> variable has an
    attribute ``units'' of ``degrees_north''.

    <pre>
    Attributes {
        GLOBAL {
            String title "Reynolds Optimum Interpolation (OI) SST";
        }
        lat {
            String units "degrees_north";
            String long_name "Latitude";
            Float64 actual_range 89.5, -89.5;
        }
        lon {
            String units "degrees_east";
            String long_name "Longitude";
            Float64 actual_range 0.5, 359.5;
        }
        time {
            String units "days since 1-1-1 00:00:00";
            String long_name "Time";
            Float64 actual_range 726468., 729289.;
            String delta_t "0000-00-07 00:00:00";
        }
        sst {
            String long_name "Weekly Means of Sea Surface Temperature";
            Float64 actual_range -1.8, 35.09;
            String units "degC";
            Float64 add_offset 0.;
            Float64 scale_factor 0.0099999998;
            Int32 missing_value 32767;
        }
    }
    </pre>

    Attributes may have arbitrary names, although in most datasets it
    is important to choose these names so a reader will know what they
    describe.  In the above example, the ``GLOBAL'' attribute provides
    information about the entire dataset.

    Data attribute information is an important part of the the data
    provided to a DAP2 client by a server, and the DAS is how this
    data is packaged for sending (and how it is received).

    The DAS class is simply a sequence of attribute tables and names.
    It may be thought of as the top level of the attribute hierarchy.

    @see DDS
    @see AttrTable */
class DAS : public DapObj
{
private:
    // The DAS support the notion of a current attribute table for a given
    // container. Containers are used by the BES to support datasets that
    // are built using several files (but not exactly the same way as
    // NCML builds them.
    AttrTable *d_container ;
    string d_container_name ;

    // A DAS is a shell around an attribute table. Since tables can be nested,
    // there is one top-level table and the attribute tables for individual
    // variables are its children.
    AttrTable d_attrs ;

    void duplicate(const DAS &src);

public:
    DAS() : DapObj(), d_container( 0 ) { }
    DAS(const DAS &das) { duplicate(das); }

    virtual ~DAS() { }

    DAS & operator=(const DAS &rhs);

    /** @brief Returns the name of the current attribute container when multiple
     * files used to build this DAS
     */
    virtual string container_name() const {return d_container_name; }

    virtual void container_name( const string &cn ) ;

    /** @brief Returns the current attribute container when multiple files
     * used to build this DAS.
     *
     * @return current attribute table for current container
     */
    virtual AttrTable *container() { return d_container; }

    /** @brief Returns the top most set of attributes
     *
     * This could be the top most variable attribute tables, or it could be
     * the top most dataset container attribute tables, if we have multiple
     * datasets being used to construct this DAS
     */
    virtual AttrTable *get_top_level_attributes() {
		if (d_container)
			return d_container;
		return &d_attrs;
	}

    virtual void erase() ;

    virtual unsigned int get_size() const ;

    AttrTable::Attr_iter var_begin() ;
    AttrTable::Attr_iter var_end() ;

    string get_name(AttrTable::Attr_iter &i);
    AttrTable *get_table(AttrTable::Attr_iter &i);

    virtual AttrTable *get_table(const string &name);

    virtual AttrTable *add_table(const string &name, AttrTable *at);

    /** Read a DAS by parsing the specified file*/
    virtual void parse(string fname);
    virtual void parse(int fd);
    virtual void parse(FILE *in = stdin);

    /** Print the DAS */
    virtual void print(FILE *out, bool dereference = false);
    virtual void print(ostream &out, bool dereference = false);

    virtual void dump(ostream &strm) const ;
};

} // namespace libdap

#endif // _das_h
