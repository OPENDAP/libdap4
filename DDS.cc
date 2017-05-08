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

//
// jhrg 9/7/94

#include "config.h"

#include <cstdio>
#include <cmath>
#include <sys/types.h>

#ifdef WIN32
#include <io.h>
#include <process.h>
#include <fstream>
#else
#include <unistd.h>    // for alarm and dup
#include <sys/wait.h>
#endif

#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>

// #define DODS_DEBUG
//#define DODS_DEBUG2

#include "GNURegex.h"

#include "DAS.h"
#include "Clause.h"
#include "Error.h"
#include "InternalErr.h"
#include "Keywords2.h"

#include "parser.h"
#include "debug.h"
#include "util.h"

#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "escaping.h"

/**
 * ############################################################################################
 * ############################################################################################
 * ############################################################################################
 * DapXmlNamespaces
 *
 * FIXME Replace all usages of the following variable with calls to DapXmlNamespaces
 * TODO  Replace all usages of the following variable with calls to DapXmlNamespaces
 *
 */
const string c_xml_xsi = "http://www.w3.org/2001/XMLSchema-instance";
const string c_xml_namespace = "http://www.w3.org/XML/1998/namespace";

const string grddl_transformation_dap32 = "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";

const string c_default_dap20_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
const string c_default_dap32_schema_location = "http://xml.opendap.org/dap/dap3.2.xsd";
const string c_default_dap40_schema_location = "http://xml.opendap.org/dap/dap4.0.xsd";

const string c_dap20_namespace = "http://xml.opendap.org/ns/DAP2";
const string c_dap32_namespace = "http://xml.opendap.org/ns/DAP/3.2#";
const string c_dap40_namespace = "http://xml.opendap.org/ns/DAP/4.0#";

const string c_dap_20_n_sl = c_dap20_namespace + " " + c_default_dap20_schema_location;
const string c_dap_32_n_sl = c_dap32_namespace + " " + c_default_dap32_schema_location;
const string c_dap_40_n_sl = c_dap40_namespace + " " + c_default_dap40_schema_location;
/**
 *
 * DapXmlNamespaces
 * ############################################################################################
 * ############################################################################################
 * ############################################################################################
 */



using namespace std;

int ddsparse(libdap::parser_arg *arg);

// Glue for the DDS parser defined in dds.lex
void dds_switch_to_buffer(void *new_buffer);
void dds_delete_buffer(void * buffer);
void *dds_buffer(FILE *fp);

namespace libdap {

void
DDS::duplicate(const DDS &dds)
{
    DBG(cerr << "Entering DDS::duplicate... " <<endl);
#if 0
    BaseTypeFactory *d_factory;

    string d_name;                // The dataset d_name
    string d_filename;          // File d_name (or other OS identifier) for
    string d_container_name;    // d_name of container structure
    Structure *d_container;     // current container for container d_name
                                // dataset or part of dataset.

    int d_dap_major;            // The protocol major version number
    int d_dap_minor;            // ... and minor version number
    string d_dap_version;       // String version of the protocol
    string d_request_xml_base;
    string d_namespace;

    AttrTable d_attr;           // Global attributes.

    vector<BaseType *> vars;    // Variables at the top level

    int d_timeout;              // alarm time in seconds. If greater than
                                // zero, raise the alarm signal if more than
                                // d_timeout seconds are spent reading data.
    Keywords d_keywords;        // Holds keywords parsed from the CE

    long d_max_response_size;   // In bytes
#endif

    d_factory = dds.d_factory;

    d_name = dds.d_name;
    d_filename = dds.d_filename;
    d_container_name = dds.d_container_name;
    d_container = dds.d_container;

    d_dap_major = dds.d_dap_major;
    d_dap_minor = dds.d_dap_minor;

    d_dap_version = dds.d_dap_version;       // String version of the protocol
    d_request_xml_base = dds.d_request_xml_base;
    d_namespace = dds.d_namespace;

    d_attr = dds.d_attr;

    DDS &dds_tmp = const_cast<DDS &>(dds);

    // copy the things pointed to by the list, not just the pointers
    for (Vars_iter i = dds_tmp.var_begin(); i != dds_tmp.var_end(); i++) {
        add_var(*i); // add_var() dups the BaseType.
    }

    d_timeout = dds.d_timeout;

    d_keywords = dds.d_keywords; // value copy; Keywords contains no pointers

    d_max_response_size = dds.d_max_response_size;
}

/**
 * Make a DDS which uses the given BaseTypeFactory to create variables.
 *
 * @note The default DAP version is 3.2 - this is really DAP2 with a handful
 * of enhancements that our WCS software relies on.
 *
 * @param factory The BaseTypeFactory to use when creating instances of
 * DAP variables. The caller must ensure the factory's lifetime is at least
 * that of the DDS instance.
 * @param name The name of the DDS - usually derived from the name of the
 * pathname or table name of the dataset.
 */
DDS::DDS(BaseTypeFactory *factory, const string &name)
        : d_factory(factory), d_name(name), d_container_name(""), d_container(0),
          d_request_xml_base(""),
          d_timeout(0), d_keywords(), d_max_response_size(0)
{
    DBG(cerr << "Building a DDS for the default version (2.0)" << endl);

    // This method sets a number of values, including those returned by
    // get_protocol_major(), ..., get_namespace().
    set_dap_version("2.0");
}

/**
 * Make a DDS with the DAP protocol set to a specific value. This method
 * provides an easy way to build DDS objects for use in a server or client
 * that will process DAP4, for example. It's roughly equivalent to calling
 * set_dap_version() after making an instance using
 * DDS::DDS(BaseTypeFactory *, const string &).
 *
 * @param factory The BaseTypeFactory to use when creating instances of
 * DAP variables. The caller must ensure the factory's lifetime is at least
 * that of the DDS instance.
 * @param name The name of the DDS - usually derived from the name of the
 * pathname or table name of the dataset.
 * @param version The DAP version to support. This sets the DAP version, as
 * well as a number of other dependent constants.
 */
DDS::DDS(BaseTypeFactory *factory, const string &name, const string &version)
        : d_factory(factory), d_name(name), d_container_name(""), d_container(0),
          d_request_xml_base(""),
          d_timeout(0), d_keywords(), d_max_response_size(0)
{
    DBG(cerr << "Building a DDS for version: " << version << endl);

    // This method sets a number of values, including those returned by
    // get_protocol_major(), ..., get_namespace().
    set_dap_version(version);
}

/** The DDS copy constructor. */
DDS::DDS(const DDS &rhs) : DapObj()
{
    DBG(cerr << "Entering DDS(const DDS &rhs) ..." << endl);
    duplicate(rhs);
    DBG(cerr << " bye." << endl);
}

DDS::~DDS()
{
    // delete all the variables in this DDS
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
        BaseType *btp = *i ;
        delete btp ; btp = 0;
    }
}

DDS &
DDS::operator=(const DDS &rhs)
{
    DBG(cerr << "Entering DDS::operator= ..." << endl);
    if (this == &rhs)
        return *this;

    duplicate(rhs);

    DBG(cerr << " bye." << endl);
    return *this;
}

/**
 * This is the main method used to transfer attributes from a DAS object into a
 * DDS. This uses the BaseType::transfer_attributes() method and the various
 * implementations found here (in the constructors classes) and in handlers.
 *
 * This method uses a deep copy to transfer the attributes, so it is safe to
 * delete the source DAS object passed to this method once it is done.
 *
 * @note To accommodate oddly built DAS objects produced by various handlers,
 * specialize the methods there.
 *
 * @param das Transfer (copy) attributes from this DAS object.
 */
void DDS::transfer_attributes(DAS *das)
{
    // If there is a container set in the DDS then check the container from
    // the DAS. If they are not the same container, then throw an exception
    // (should be working on the same container). If the container does not
    // exist in the DAS, then throw an exception
    if (d_container && das->container_name() != d_container_name)
        throw InternalErr(__FILE__, __LINE__,
                "Error transferring attributes: working on a container in dds, but not das");

    // Give each variable a chance to claim its attributes.
    AttrTable *top = das->get_top_level_attributes();

    for (DDS::Vars_iter i = var_begin(), e = var_end(); i != e; i++) {
        (*i)->transfer_attributes(top);
    }
#if 0
    Vars_iter var = var_begin();
    while (var != var_end()) {
        try {
            DBG(cerr << "Processing the attributes for: " << (*var)->d_name() << " a " << (*var)->type_name() << endl);
            (*var)->transfer_attributes(top);
            var++;
        }
        catch (Error &e) {
            DBG(cerr << "Got this exception: " << e.get_error_message() << endl);
            var++;
            throw e;
        }
    }
#endif
    // Now we transfer all of the attributes still marked as global to the
    // global container in the DDS.
    for (AttrTable::Attr_iter i = top->attr_begin(), e = top->attr_end(); i != e; ++i) {
        if ((*i)->type == Attr_container && (*i)->attributes->is_global_attribute()) {
            // copy the source container so that the DAS passed in can be
            // deleted after calling this method.
            AttrTable *at = new AttrTable(*(*i)->attributes);
            d_attr.append_container(at, at->get_name());
        }
    }
#if 0
    AttrTable::Attr_iter at_cont_p = top_level->attr_begin();
    while (at_cont_p != top_level->attr_end()) {
        // In truth, all of the top level attributes should be containers, but
        // this test handles the abnormal case where somehow someone makes a
        // top level attribute that is not a container by silently dropping it.
        if ((*at_cont_p)->type == Attr_container && (*at_cont_p)->attributes->is_global_attribute()) {
            DBG(cerr << (*at_cont_p)->d_name << " is a global attribute." << endl);
            // copy the source container so that the DAS passed in can be
            // deleted after calling this method.
            AttrTable *at = new AttrTable(*(*at_cont_p)->attributes);
            d_attr.append_container(at, at->get_name());
        }

        at_cont_p++;
    }
#endif
}

/** Get and set the dataset's d_name.  This is the d_name of the dataset
    itself, and is not to be confused with the d_name of the file or
    disk on which it is stored.

    @d_name Dataset Name Accessors */

//@{

/** Returns the dataset's d_name. */
string
DDS::get_dataset_name() const
{
    return d_name;
}

/** Sets the dataset d_name. */
void
DDS::set_dataset_name(const string &n)
{
    d_name = n;
}

//@}

/** Get the attribute table for the global attributes. */
AttrTable &
DDS::get_attr_table()
{
    return d_attr;
}

/** Get and set the dataset's filename. This is the physical
    location on a disk where the dataset exists.  The dataset d_name
    is simply a title.

    @d_name File Name Accessor
    @see Dataset Name Accessors */

//@{
/** Gets the dataset file d_name. */
string
DDS::filename() const
{
    return d_filename;
}

/** Set the dataset's filename. */
void
DDS::filename(const string &fn)
{
    d_filename = fn;
}
//@}

/**
 * @deprecated
 */
void
DDS::set_dap_major(int p)
{
    d_dap_major = p;

    // This works because regardless of the order set_dap_major and set_dap_minor
    // are called, once they both are called, the value in the string is
    // correct. I protect against negative numbers because that would be
    // nonsensical.
    if (d_dap_minor >= 0) {
	ostringstream oss;
	oss << d_dap_major << "." << d_dap_minor;
	d_dap_version = oss.str();
    }
}

/**
 * @deprecated
 */
void
DDS::set_dap_minor(int p)
{
    d_dap_minor = p;

    if (d_dap_major >= 0) {
	ostringstream oss;
	oss << d_dap_major << "." << d_dap_minor;
	d_dap_version = oss.str();
    }
}

/**
 * Given the DAP protocol version, parse that string and set the DDS fields.
 *
 * @param v The version string.
 */
void
DDS::set_dap_version(const string &v /* = "2.0" */)
{
    istringstream iss(v);

    int major = -1, minor = -1;
    char dot;
    if (!iss.eof() && !iss.fail())
        iss >> major;
    if (!iss.eof() && !iss.fail())
        iss >> dot;
    if (!iss.eof() && !iss.fail())
        iss >> minor;

    if (major == -1 || minor == -1 or dot != '.')
        throw InternalErr(__FILE__, __LINE__, "Could not parse dap version. Value given: " + v);

    d_dap_version = v;

    d_dap_major = major;
    d_dap_minor = minor;

    // Now set the related XML constants. These might be overwritten if
    // the DDS instance is being built from a document parse, but if it's
    // being constructed by a server the code to generate the XML document
    // needs these values to match the DAP version information.
    switch (d_dap_major) {
        case 2:
            d_namespace = c_dap20_namespace;
            break;
        case 3:
            d_namespace = c_dap32_namespace;
            break;
        case 4:
            d_namespace = c_dap40_namespace;
            break;
        default:
            throw InternalErr(__FILE__, __LINE__, "Unknown DAP version.");
    }
}

/** Old way to set the DAP version.
 *
 * @note Don't use this - two interfaces to set the version number is overkill
 *
 * @param d The protocol version requested by the client, as a double.
 * @deprecated
 */
void
DDS::set_dap_version(double d)
{
    int major = floor(d);
    int minor = (d-major)*10;

    DBG(cerr << "Major: " << major << ", Minor: " << minor << endl);

    ostringstream oss;
    oss << major << "." << minor;

    set_dap_version(oss.str());
}

/** Get and set the current container. If there are multiple files being
    used to build this DDS, using a container will set a virtual structure
    for the current container.

    @d_name Container Name Accessor
    @see Dataset Name Accessors */

//@{
/** Gets the dataset file d_name. */
string
DDS::container_name()
{
    return d_container_name;
}

/** Set the current container d_name and get or create a structure for that
 * d_name. */
void
DDS::container_name(const string &cn)
{
    // we want to search the DDS for the top level structure with the given
    // d_name. Set the container to null so that we don't search some previous
    // container.
    d_container = 0 ;
    if( !cn.empty() )
    {
	d_container = dynamic_cast<Structure *>( var( cn ) ) ;
	if( !d_container )
	{
	    // create a structure for this container. Calling add_var
	    // while_container is null will add the new structure to DDS and
	    // not some sub structure. Adding the new structure makes a copy
	    // of it.  So after adding it, go get it and set d_container.
	    Structure *s = new Structure( cn ) ;
	    add_var( s ) ;
	    delete s ;
	    s = 0 ;
	    d_container = dynamic_cast<Structure *>( var( cn ) ) ;
	}
    }
    d_container_name = cn;

}

/** Get the current container structure. */
Structure *
DDS::container()
{
    return d_container ;
}

//@}

/** Get the size of a response. This method looks at the variables in the DDS
 *  a computes the number of bytes in the response.
 *
 *  @note This version of the method does a poor job with Sequences. A better
 *  implementation would look at row-constraint-based limitations and use them
 *  for size computations. If a row-constraint is missing, return an error.
 *
 *  @param constrained Should the size of the whole DDS be used or should the
 *  current constraint be taken into account?
 */
int
DDS::get_request_size(bool constrained)
{
	int w = 0;
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
    	if (constrained) {
    		if ((*i)->send_p())
    			w += (*i)->width(constrained);
    	}
    	else {
    		w += (*i)->width(constrained);
    	}
    }

    return w;
}

/** @brief Adds a copy of the variable to the DDS.
    Using the ptr_duplicate() method, perform a deep copy on the variable
    \e bt and adds the result to this DDS.
    @note The copy will not copy data values.
    @param bt Source variable. */
void DDS::add_var(BaseType *bt) {
    if (!bt)
        throw InternalErr(__FILE__, __LINE__, "Trying to add a BaseType object with a NULL pointer.");
#if 0
    if (bt->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 DDS.");
#endif
    DBG2(cerr << "In DDS::add_var(), bt's address is: " << bt << endl);

    BaseType *btp = bt->ptr_duplicate();
    DBG2(cerr << "In DDS::add_var(), btp's address is: " << btp << endl);
    if (d_container) {
        // Mem leak fix [mjohnson nov 2009]
        // Structure::add_var() creates ANOTHER copy.
        d_container->add_var(bt);
        // So we need to delete btp or else it leaks
        delete btp;
        btp = 0;
    }
    else {
        vars.push_back(btp);
    }
}

/** @brief Adds the variable to the DDS.
    @param bt Source variable. */
void
DDS::add_var_nocopy(BaseType *bt)
{
    if (!bt)
        throw InternalErr(__FILE__, __LINE__, "Trying to add a BaseType object with a NULL pointer.");
#if 0
    //FIXME There's no longer a DAP2 and DAP4 DDS
    if (bt->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 DDS.");
#endif

    DBG2(cerr << "In DDS::add_var(), bt's address is: " << bt << endl);

    if (d_container) {
        d_container->add_var_nocopy(bt);
    }
    else {
        vars.push_back(bt);
    }
}


/** Remove the named variable from the DDS. This method is not smart about
    looking up names. The variable must exist at the top level of the DDS and
    must match \e exactly the d_name given.

    @note Invalidates any iterators that reference the contents of the DDS.
    @param n The d_name of the variable to remove. */
void
DDS::del_var(const string &n)
{
    if( d_container )
    {
	d_container->del_var( n ) ;
	return ;
    }

    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
        if ((*i)->name() == n) {
            BaseType *bt = *i ;
            vars.erase(i) ;
            delete bt ; bt = 0;
            return;
        }
    }
}

/** Remove the variable referenced by the iterator and free its storage.

    @note Invalidates any iterators that reference the contents of the DDS.
    @param i The Vars_iter which refers to the variable. */
void
DDS::del_var(Vars_iter i)
{
    if (i != vars.end()) {
        BaseType *bt = *i ;
        vars.erase(i) ;
        delete bt ; bt = 0;
    }
}

/** Remove the variables referenced by the range of iterators and free their
    storage.

    @note Invalidates any iterators that reference the contents of the DDS.
    @param i1 The start of the range.
    @param i2 The end of the range. */
void
DDS::del_var(Vars_iter i1, Vars_iter i2)
{
    for (Vars_iter i_tmp = i1; i_tmp != i2; i_tmp++) {
        BaseType *bt = *i_tmp ;
        delete bt ; bt = 0;
    }
    vars.erase(i1, i2) ;
}

/** Search for for variable <i>n</i> as above but record all
    compound type variables which ultimately contain <i>n</i> on
    <i>s</i>. This stack can then be used to mark the contained
    compound-type variables as part of the current projection.

    @return A BaseType pointer to the variable <i>n</i> or 0 if <i>n</i>
    could not be found. */
BaseType *
DDS::var(const string &n, BaseType::btp_stack &s)
{
    return var(n, &s);
}
/** @brief Find the variable with the given d_name.

    Returns a pointer to the named variable. If the d_name contains one or
    more field separators then the function looks for a variable whose
    name matches exactly. If the d_name contains no field separators then
    the function looks first in the top level and then in all subsequent
    levels and returns the first occurrence found. In general, this
    function searches constructor types in the order in which they appear
    in the DDS, but there is no requirement that it do so.

    @note If a dataset contains two constructor types which have field names
    that are the same (say point.x and pair.x) you should use fully qualified
    names to get each of those variables.

    @param n The name of the variable to find.
    @param s If given, this value-result parameter holds the path to the
    returned BaseType. Thus, this method can return the FQN for the variable
    \e n.
    @return A BaseType pointer to the variable or null if not found. */
BaseType *
DDS::var(const string &n, BaseType::btp_stack *s)
{
    string name = www2id(n);
    if( d_container )
        return d_container->var( name, false, s ) ;

    BaseType *v = exact_match(name, s);
    if (v)
        return v;

    return leaf_match(name, s);
}

BaseType *
DDS::leaf_match(const string &n, BaseType::btp_stack *s)
{
    DBG(cerr << "DDS::leaf_match: Looking for " << n << endl);

    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
        BaseType *btp = *i;
        DBG(cerr << "DDS::leaf_match: Looking for " << n << " in: " << btp->name() << endl);
        // Look for the d_name in the dataset's top-level
        if (btp->name() == n) {
            DBG(cerr << "Found " << n << " in: " << btp->name() << endl);
            return btp;
        }

        if (btp->is_constructor_type()) {
            BaseType *found = btp->var(n, false, s);
            if (found) {
                DBG(cerr << "Found " << n << " in: " << btp->name() << endl);
                return found;
            }
        }
#if STRUCTURE_ARRAY_SYNTAX_OLD
        if (btp->is_vector_type() && btp->var()->is_constructor_type()) {
            s->push(btp);
            BaseType *found = btp->var()->var(n, false, s);
            if (found) {
                DBG(cerr << "Found " << n << " in: " << btp->var()->d_name() << endl);
                return found;
            }
        }
#endif
    }

    return 0;   // It is not here.
}

BaseType *
DDS::exact_match(const string &name, BaseType::btp_stack *s)
{
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
        BaseType *btp = *i;
        DBG2(cerr << "Looking for " << d_name << " in: " << btp << endl);
        // Look for the d_name in the current ctor type or the top level
        if (btp->name() == name) {
            DBG2(cerr << "Found " << d_name << " in: " << btp << endl);
            return btp;
        }
    }

    string::size_type dot_pos = name.find(".");
    if (dot_pos != string::npos) {
        string aggregate = name.substr(0, dot_pos);
        string field = name.substr(dot_pos + 1);

        BaseType *agg_ptr = var(aggregate, s);
        if (agg_ptr) {
            DBG2(cerr << "Descending into " << agg_ptr->name() << endl);
            return agg_ptr->var(field, true, s);
        }
        else
            return 0;  // qualified names must be *fully* qualified
    }

    return 0;   // It is not here.
}


/** @brief Returns the first variable in the DDS. */

DDS::Vars_iter
DDS::var_begin()
{
    return vars.begin();
}

DDS::Vars_riter
DDS::var_rbegin()
{
    return vars.rbegin();
}

DDS::Vars_iter
DDS::var_end()
{
    return vars.end() ;
}

DDS::Vars_riter
DDS::var_rend()
{
    return vars.rend() ;
}

/** Return the iterator for the \e ith variable.
    @param i the index
    @return The corresponding  Vars_iter */
DDS::Vars_iter
DDS::get_vars_iter(int i)
{
    return vars.begin() + i;
}

/** Return the \e ith variable.
    @param i the index
    @return The corresponding variable */
BaseType *
DDS::get_var_index(int i)
{
    return *(vars.begin() + i);
}

/** Insert a copy of the BaseType before the position given.
 * @param i The iterator that marks the position
 * @param ptr The BaseType object to copy and insert
 */
void
DDS::insert_var(Vars_iter i, BaseType *ptr)
{
#if 0
    if (ptr->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 DDS.");
#endif
    vars.insert(i, ptr->ptr_duplicate());
}

/** Insert the BaseType before the position given.
 * @note Does not copy the BaseType object - that caller must not
 * free the inserted object's pointer. This object will, however,
 * delete the pointer when it is deleted.
 * @param i The iterator that marks the position
 * @param ptr The BaseType object to insert
 */
void
DDS::insert_var_nocopy(Vars_iter i, BaseType *ptr)
{
#if 0
    if (ptr->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 DDS.");
#endif
    vars.insert(i, ptr);
}

/** @brief Returns the number of variables in the DDS. */
int
DDS::num_var()
{
    return vars.size();
}

void
DDS::timeout_on()
{
#if USE_LOCAL_TIMEOUT_SCHEME
#ifndef WIN32
    alarm(d_timeout);
#endif
#endif
}

void
DDS::timeout_off()
{
#if USE_LOCAL_TIMEOUT_SCHEME
#ifndef WIN32
    // Old behavior commented out. I think it is an error to change the value
    // of d_timeout. The way this will likely be used is to set the timeout
    // value once and then 'turn on' or turn off' that timeout as the situation
    // dictates. The initeded use for the DDS timeout is so that timeouts for
    // data responses will include the CPU resources needed to build the response
    // but not the time spent transmitting the response. This may change when
    // more parallelism is added to the server... These methods are called from
    // BESDapResponseBuilder in bes/dap. jhrg 12/22/15

    // d_timeout = alarm(0);

    alarm(0);
#endif
#endif
}

void
DDS::set_timeout(int)
{
#if USE_LOCAL_TIMEOUT_SCHEME
    //  Has no effect under win32
    d_timeout = t;
#endif
}

int
DDS::get_timeout()
{
#if USE_LOCAL_TIMEOUT_SCHEME
    //  Has to effect under win32
    return d_timeout;
#endif
    return 0;
}

/** @brief Traverse DDS, set Sequence leaf nodes. */
void
DDS::tag_nested_sequences()
{
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
        if ((*i)->type() == dods_sequence_c)
            dynamic_cast<Sequence&>(**i).set_leaf_sequence();
        else if ((*i)->type() == dods_structure_c)
            dynamic_cast<Structure&>(**i).set_leaf_sequence();
    }
}

/** @brief Parse a DDS from a file with the given d_name. */
void
DDS::parse(string fname)
{
    FILE *in = fopen(fname.c_str(), "r");

    if (!in) {
        throw Error(cannot_read_file, "Could not open: " + fname);
    }

    try {
        parse(in);
        fclose(in);
    }
    catch (Error &e) {
        fclose(in);
        throw ;
    }
}


/** @brief Parse a DDS from a file indicated by the input file descriptor. */
void
DDS::parse(int fd)
{
#ifdef WIN32
    int new_fd = _dup(fd);
#else
    int new_fd = dup(fd);
#endif

    if (new_fd < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not access file.");
    FILE *in = fdopen(new_fd, "r");

    if (!in) {
        throw InternalErr(__FILE__, __LINE__, "Could not access file.");
    }

    try {
        parse(in);
        fclose(in);
    }
    catch (Error &e) {
        fclose(in);
        throw ;
    }
}

/** @brief Parse a DDS from a file indicated by the input file descriptor.
    Read the persistent representation of a DDS from the FILE *in, parse it
    and create a matching binary object.
    @param in Read the persistent DDS from this FILE*.
    @exception InternalErr Thrown if \c in is null
    @exception Error Thrown if the parse fails. */
void
DDS::parse(FILE *in)
{
    if (!in) {
        throw InternalErr(__FILE__, __LINE__, "Null input stream.");
    }

    void *buffer = dds_buffer(in);
    dds_switch_to_buffer(buffer);

    parser_arg arg(this);

    bool status = ddsparse(&arg) == 0;

    dds_delete_buffer(buffer);

    DBG2(cout << "Status from parser: " << status << endl);

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
        if (arg.error())
            throw *arg.error();
    }
}

/** @brief Print the entire DDS to the specified file. */
void
DDS::print(FILE *out)
{
    ostringstream oss;
    print(oss);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

/** @brief Print the entire DDS to the specified ostream. */
void
DDS::print(ostream &out)
{
    out << "Dataset {\n" ;

    for (Vars_citer i = vars.begin(); i != vars.end(); i++) {
        (*i)->print_decl(out) ;
    }

    out << "} " << id2www(d_name) << ";\n" ;

    return ;
}

/**
 * Print the DAP2 DAS object using attribute information recorded
 * this DDS object.
 *
 * @note Uses default indenting of four spaces and does not follow
 * (now deprecated) attribute aliases.
 *
 * @param out Write the DAS here.
 */
static string four_spaces = "    ";
void print_var_das(ostream &out, BaseType *bt, string indent=""){

    AttrTable attr_table = bt->get_attr_table();
    out << indent << add_space_encoding(bt->name()) << " {" << endl;
    attr_table.print(out, indent+four_spaces);
    Constructor *cnstrctr = dynamic_cast < Constructor * >(bt);
    if(cnstrctr) {
        Constructor::Vars_iter i = cnstrctr->var_begin();
        Constructor::Vars_iter e = cnstrctr->var_end();
        for (; i!=e; i++) {
            print_var_das(out,*i,indent+four_spaces);
        }

    }
    out << indent << "}" << endl;

}

void
DDS::print_das(ostream &out)
{
    string indent("    ");
    out << "Attributes {" << endl ;
    for (Vars_citer i = vars.begin(); i != vars.end(); i++) {
        print_var_das(out, *i, four_spaces);
    }
    // Print the global attributes at the end.
    d_attr.print(out,indent);
    out << "}" << endl ;
}

/** @brief Print a constrained DDS to the specified file.

    Print those parts (variables) of the DDS structure to OS that
    are marked to be sent after evaluating the constraint
    expression.

    @note This function only works for scalars at the top level.

    @returns true.
*/
void
DDS::print_constrained(FILE *out)
{
    ostringstream oss;
    print_constrained(oss);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

/** @brief Print a constrained DDS to the specified ostream.

    Print those parts (variables) of the DDS structure to OS that
    are marked to be sent after evaluating the constraint
    expression.

    \note This function only works for scalars at the top level.

    @returns true.
*/
void
DDS::print_constrained(ostream &out)
{
    out << "Dataset {\n" ;

    for (Vars_citer i = vars.begin(); i != vars.end(); i++) {
        // for each variable, indent with four spaces, print a trailing
        // semicolon, do not print debugging information, print only
        // variables in the current projection.
        (*i)->print_decl(out, "    ", true, false, true) ;
    }

    out << "} " << id2www(d_name) << ";\n" ;

    return;
}

/** Print an XML representation of this DDS. This method is used to generate
    the part of the DDX response. The \c Dataset tag is \e not written by
    this code. The caller of this method must handle writing that and
    including the \c dataBLOB tag.

    @param out Destination.
    @param constrained True if the output should be limited to just those
    variables that are in the projection of the current constraint
    expression.
    @param blob The dataBLOB href.
    @deprecated */
void
DDS::print_xml(FILE *out, bool constrained, const string &blob)
{
    ostringstream oss;
    print_xml_writer(oss, constrained, blob);
    fwrite(oss.str().data(), 1, oss.str().length(), out);
}

/** Print an XML representation of this DDS. This method is used to generate
    the part of the DDX response. The \c Dataset tag is \e not written by
    this code. The caller of this method must handle writing that and
    including the \c dataBLOB tag.

    @param out Destination ostream.
    @param constrained True if the output should be limited to just those
    variables that are in the projection of the current constraint
    expression.
    @param blob The dataBLOB href.
    @deprecated */
void
DDS::print_xml(ostream &out, bool constrained, const string &blob)
{
    print_xml_writer(out, constrained, blob);
}

class VariablePrintXMLWriter : public unary_function<BaseType *, void>
{
    XMLWriter &d_xml;
    bool d_constrained;
public:
    VariablePrintXMLWriter(XMLWriter &xml, bool constrained)
            : d_xml(xml), d_constrained(constrained)
    {}
    void operator()(BaseType *bt)
    {
        bt->print_xml_writer(d_xml, d_constrained);
    }
};

/**
 * Print the DDX. This code uses the libxml2 'TextWriter' interface; something
 * that seems to be a good compromise between doing it by hand (although more
 * verbose it is also more reliable) and DOM.
 *
 * @note This code handles several different versions of DAP in a fairly
 * crude way. I've broken it up into three different responses: DAP2, DAP3.2
 * and DAP4.
 *
 * @param out Write the XML to this output sink
 * @param constrained True if the only variables to print are those in the
 * current projection. If true, this will also suppress printing attributes.
 * @param blob This is an href (DAP2) or a cid (DAP3.4 and 4). The href
 * points to the binary data; the cid is the M-MIME separator for the binary
 * data.
 */
void
DDS::print_xml_writer(ostream &out, bool constrained, const string &blob)
{
    XMLWriter xml("    ");

    // Stamp and repeat for these sections; trying to economize is makes it
    // even more confusing
    if (get_dap_major() >= 4) {
        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Group") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write Group element");
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)d_name.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "dapVersion", (const xmlChar*)get_dap_version().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for dapVersion");

        if (!get_request_xml_base().empty()) {
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:xml", (const xmlChar*)c_xml_namespace.c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:xml");

            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xml:base", (const xmlChar*)get_request_xml_base().c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xml:base");
        }
        if (!get_namespace().empty()) {
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns", (const xmlChar*)get_namespace().c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns");
        }
    }
    else if (get_dap_major() == 3 && get_dap_minor() >= 2) {
        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Dataset") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write Dataset element");
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)d_name.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:xsi", (const xmlChar*)"http://www.w3.org/2001/XMLSchema-instance") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:xsi");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xsi:schemaLocation", (const xmlChar*)c_dap_32_n_sl.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:schemaLocation");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:grddl", (const xmlChar*)"http://www.w3.org/2003/g/data-view#") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:grddl");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "grddl:transformation", (const xmlChar*)grddl_transformation_dap32.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:transformation");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns", (const xmlChar*)c_dap32_namespace.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns");
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:dap", (const xmlChar*)c_dap32_namespace.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:dap");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "dapVersion", (const xmlChar*)"3.2") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for dapVersion");

        if (!get_request_xml_base().empty()) {
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:xml", (const xmlChar*)c_xml_namespace.c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:xml");

            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xml:base", (const xmlChar*)get_request_xml_base().c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xml:base");
        }
    }
    else { // dap2
        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Dataset") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write Dataset element");
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)d_name.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for d_name");
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:xsi", (const xmlChar*)"http://www.w3.org/2001/XMLSchema-instance") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:xsi");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns", (const xmlChar*)c_dap20_namespace.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xsi:schemaLocation", (const xmlChar*)c_dap_20_n_sl.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:schemaLocation");
    }

    // Print the global attributes
    d_attr.print_xml_writer(xml);

    // Print each variable
    for_each(var_begin(), var_end(), VariablePrintXMLWriter(xml, constrained));

    // For DAP 3.2 and greater, use the new syntax and value. The 'blob' is
    // the CID of the MIME part that holds the data. For DAP2 (which includes
    // 3.0 and 3.1), the blob is an href. For DAP4, only write the CID if it's
    // given.
    if (get_dap_major() >= 4) {
        if (!blob.empty()) {
            if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "blob") < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write blob element");
            string cid = "cid:" + blob;
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "href", (const xmlChar*) cid.c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for d_name");
            if (xmlTextWriterEndElement(xml.get_writer()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not end blob element");
        }
    }
    else if (get_dap_major() == 3 && get_dap_minor() >= 2) {
        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "blob") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write blob element");
        string cid = "cid:" + blob;
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "href", (const xmlChar*) cid.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for d_name");
        if (xmlTextWriterEndElement(xml.get_writer()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not end blob element");
    }
    else { // dap2
        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "dataBLOB") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write dataBLOB element");
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "href", (const xmlChar*) "") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for d_name");
        if (xmlTextWriterEndElement(xml.get_writer()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not end dataBLOB element");
    }

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end Dataset element");

    out << xml.get_doc();// << ends;// << endl;
}

/**
 * Print the DAP4 DMR object.
 * This method prints the DMR. If the dap version is not >= 4.0, it's an
 * error to call this method.
 *
 * @note Calling methods that print the DDS or DDX when get_dap_major()
 * returns a value >= 4 is undefined. Use this method to get the DAP4
 * metadata response.
 *
 * @param out Write the XML to this stream
 * @param constrained Should the DMR be subject to a constraint?
 */
void
DDS::print_dmr(ostream &out, bool constrained)
{
    if (get_dap_major() < 4)
        throw InternalErr(__FILE__, __LINE__, "Tried to print a DMR with DAP major version less than 4");

    XMLWriter xml("    ");

    // DAP4 wraps a dataset in a top-level Group element.
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Group") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Group element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:xml",
            (const xmlChar*) c_xml_namespace.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:xml");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:xsi", (const xmlChar*) c_xml_xsi.c_str())
            < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:xsi");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xsi:schemaLocation",
            (const xmlChar*) c_dap_40_n_sl.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:schemaLocation");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns",
            (const xmlChar*) get_namespace().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "dapVersion",
            (const xmlChar*) get_dap_version().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for dapVersion");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "dmrVersion", (const xmlChar*) get_dmr_version().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for dapVersion");

    if (!get_request_xml_base().empty()) {
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xml:base",
                (const xmlChar*) get_request_xml_base().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xml:base");
    }

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*) d_name.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    // Print the global attributes
    d_attr.print_xml_writer(xml);

    // Print each variable
    for_each(var_begin(), var_end(), VariablePrintXMLWriter(xml, constrained));

#if 0
    // For DAP 3.2 and greater, use the new syntax and value. The 'blob' is
    // the CID of the MIME part that holds the data. For DAP2 (which includes
    // 3.0 and 3.1), the blob is an href. For DAP4, only write the CID if it's
    // given.
    if (get_dap_major() >= 4) {
        if (!blob.empty()) {
            if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "blob") < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write blob element");
            string cid = "cid:" + blob;
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "href", (const xmlChar*) cid.c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for d_name");
            if (xmlTextWriterEndElement(xml.get_writer()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not end blob element");
        }
    }
#endif

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end the top-level Group element");

    out << xml.get_doc();
}

// Used by DDS::send() when returning data from a function call.
/** @brief Check the semantics of each of the variables represented in the
    DDS.

    Check the semantics of the DDS describing a complete dataset. If ALL is
    true, check not only the semantics of THIS->TABLE, but also recursively
    all ctor types in the THIS->TABLE. By default, ALL is false since parsing
    a DDS input file runs semantic checks on all variables (but not the
    dataset itself.

    @return TRUE if the conventions for the DDS are not violated, FALSE
    otherwise.
    @param all If true, recursively check the individual members of
    compound variables.
    @see BaseType::check_semantics */
bool
DDS::check_semantics(bool all)
{
    // The dataset must have a d_name
    if (d_name == "") {
        cerr << "A dataset must have a d_name" << endl;
        return false;
    }

    string msg;
    if (!unique_names(vars, d_name, "Dataset", msg))
        return false;

    if (all)
        for (Vars_iter i = vars.begin(); i != vars.end(); i++)
            if (!(*i)->check_semantics(msg, true))
                return false;

    return true;
}

/** @brief Mark the <tt>send_p</tt> flag of the named variable to
    <i>state</i>.

    Mark the named variable by setting its SEND_P flag to STATE (true
    indicates that it is to be sent). Names must be fully qualified.

    @note For aggregate types this sets each part to STATE when STATE is
    True. Thus, if State is True and N is `exp1.test', then both `exp1' and
    `test' have their SEND_P flag set to True. If STATE is False, then the
    SEND_P flag of the `test' is set to False, but `exp1' is left
    unchanged. This means that a single variable can be removed from the
    current projection without removing all the other children of its
    parent. See the mfunc set_send_p().

    @return True if the named variable was found, false otherwise.

    @todo This should throw an exception on error!!!

    @todo These methods that use the btp_stack to keep track of the path from
    the top of a dataset to a particular variable can be rewritten to use the
    parent field instead.

    @todo All the methods that use names to identify variables should have
    counterparts that take BaseType pointers.
*/
bool
DDS::mark(const string &n, bool state)
{
    // TODO use auto_ptr
    BaseType::btp_stack *s = new BaseType::btp_stack;

    DBG2(cerr << "DDS::mark: Looking for " << n << endl);

    BaseType *variable = var(n, s);
    if (!variable) {
        DBG2(cerr << "Could not find variable " << n << endl);
        delete s; s = 0;
        return false;
    }
    variable->set_send_p(state);

    DBG2(cerr << "DDS::mark: Set variable " << variable->d_name()
            << " (a " << variable->type_name() << ")" << endl);

    // Now check the btp_stack and run BaseType::set_send_p for every
    // BaseType pointer on the stack. Using BaseType::set_send_p() will
    // set the property for a Constructor but not its contained variables
    // which preserves the semantics of projecting just one field.
    while (!s->empty()) {
        s->top()->BaseType::set_send_p(state);

        DBG2(cerr << "DDS::mark: Set variable " << s->top()->d_name()
                << " (a " << s->top()->type_name() << ")" << endl);
        // FIXME get_parent() hosed?
#if 1
        string parent_name = (s->top()->get_parent()) ? s->top()->get_parent()->name(): "none";
        string parent_type = (s->top()->get_parent()) ? s->top()->get_parent()->type_name(): "none";
        DBG2(cerr << "DDS::mark: Parent variable " << parent_name << " (a " << parent_type << ")" << endl);
#endif
        s->pop();
    }

    delete s ; s = 0;

    return true;
}

/** Mark the member variable <tt>send_p</tt> flags to
    <i>state</i>.

    @return Void
*/
void
DDS::mark_all(bool state)
{
    for (Vars_iter i = vars.begin(); i != vars.end(); i++)
        (*i)->set_send_p(state);
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and then calls parent dump
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
DDS::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "DDS::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    strm << DapIndent::LMarg << "d_name: " << d_name << endl ;
    strm << DapIndent::LMarg << "filename: " << d_filename << endl ;
    strm << DapIndent::LMarg << "protocol major: " << d_dap_major << endl;
    strm << DapIndent::LMarg << "protocol minor: " << d_dap_minor << endl;
    strm << DapIndent::LMarg << "factory: " << (void *)d_factory << endl ;

    strm << DapIndent::LMarg << "global attributes:" << endl ;
    DapIndent::Indent() ;
    d_attr.dump(strm) ;
    DapIndent::UnIndent() ;

    if (vars.size()) {
        strm << DapIndent::LMarg << "vars:" << endl ;
        DapIndent::Indent() ;
        Vars_citer i = vars.begin() ;
        Vars_citer ie = vars.end() ;
        for (; i != ie; i++) {
            (*i)->dump(strm) ;
        }
        DapIndent::UnIndent() ;
    }
    else {
        strm << DapIndent::LMarg << "vars: none" << endl ;
    }

    DapIndent::UnIndent() ;
}

} // namespace libdap
