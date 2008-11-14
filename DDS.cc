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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

static char rcsid[] not_used =
    {"$Id$"
    };

#include <cstdio>
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

//#define DODS_DEBUG
//#define DODS_DEBUG2

#include "GNURegex.h"

#include "DAS.h"
#include "Clause.h"
#include "Error.h"
#include "InternalErr.h"

#include "parser.h"
#include "debug.h"
#include "util.h"
#include "escaping.h"

const string c_default_dap20_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
const string c_default_dap32_schema_location = "http://xml.opendap.org/dap/dap/3.2.xsd";

const string c_dap20_namespace = "http://xml.opendap.org/ns/DAP2";
const string c_dap32_namespace = "http://xml.opendap.org/ns/DAP/3.2#";

const string grddl_transformation_dap32 = "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";

const string c_xml_namespace = "http://www.w3.org/XML/1998/namespace";

using namespace std;

void ddsrestart(FILE *yyin); // Defined in dds.tab.c
int ddsparse(void *arg);

// Glue for the DDS parser defined in dds.lex
void dds_switch_to_buffer(void *new_buffer);
void dds_delete_buffer(void * buffer);
void *dds_buffer(FILE *fp);

namespace libdap {

void
DDS::duplicate(const DDS &dds)
{
    DBG(cerr << "Entering DDS::duplicate... " <<endl);
    name = dds.name;
    d_factory = dds.d_factory;
    d_container = dds.d_container;

    d_client_dap_major = dds.d_client_dap_major;
    d_client_dap_minor = dds.d_client_dap_minor;


#if 0
    //fields to copy
    string _filename;       // File name (or other OS identifier) for
    string _container_name; // name of container structure
    int d_protocol_major;       // The protocol major version number
    int d_protocol_minor;       // ... and minor version number

    // These hold the major and minor versions of DAP that the client send in
    // the XDAP-Accept header. If the header is not sent, these default to 2.0
    int d_client_dap_major;
    int d_client_dap_minor;

    AttrTable d_attr;           // Global attributes.
    int d_timeout;              // alarm time in seconds. If greater than

#endif

    DDS &dds_tmp = const_cast<DDS &>(dds);

    // copy the things pointed to by the list, not just the pointers
    for (Vars_iter i = dds_tmp.var_begin(); i != dds_tmp.var_end(); i++) {
        add_var(*i); // add_var() dups the BaseType.
    }
}

/** Make a DDS which uses the given BaseTypeFactory to create variables.
    @param n The name of the dataset. Can also be set using the
    set_dataset_name() method.
    @param factory BaseTypeFactory which instantiates Byte, ..., Grid. The
    caller is responsible for freeing the object \e after deleting this DDS.
    Can also be set using set_factory(). Never delete until just before
    deleting the DDS itself unless you intend to replace the factory with a
    new instance.
    @param n The name of the data set. Can also be set using
    set_dataset_name(). */
DDS::DDS(BaseTypeFactory *factory, const string &n)

        : d_factory(factory), name(n), d_container(0), d_dap_major(2),
        d_dap_minor(0), d_client_dap_major(2), d_client_dap_minor(0),
        d_request_xml_base(""), d_timeout(0)
{
    DBG(cerr << "Building a DDS with client major/minor: "
            << d_client_dap_major << "." << d_client_dap_minor << endl);
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

/** @brief Look for the parent of an HDF4 dimension attribute

    If this attribute container's name ends in the '_dim_?' suffix, look
    for the variable to which it's attributes should be bound: For an array,
    they should be held in a sub-table of the array; for a Structure or
    Sequence, I don't think the HDF4 handler ever makes these (since those
    types don't have 'dimension' in hdf-land);  and for a Grid, the attributes
    belong with the map variables.

    @note This method does check that the \e source really is an hdf4 dimension
    attribute.

    @param source The attribute container, an AttrTable::entry instance.
    @return the BaseType to which these attributes belong or null if none
    was found. */
BaseType *
DDS::find_hdf4_dimension_attribute_home(AttrTable::entry *source)
{
    BaseType *btp;
    string::size_type i = source->name.find("_dim_");
    if (i != string::npos && (btp = var(source->name.substr(0, i)))) {
        if (btp->is_vector_type()) {
            return btp;
        }
        else if (btp->type() == dods_grid_c) {
            // For a Grid, the hdf4 handler uses _dim_n for the n-th Map
            // i+5 points to the character holding 'n'
            int n = atoi(source->name.substr(i + 5).c_str());
            DBG(cerr << "Found a Grid (" << btp->name() << ") and "
                << source->name.substr(i) << ", extracted n: " << n << endl);
            return *(dynamic_cast<Grid&>(*btp).map_begin() + n);
        }
    }

    return 0;
}

/** Given an attribute container from a DAS, find or make a destination
    for its contents in the DDS.
    @param source Attribute table
    @param dest_variable result param that holds the BaseType
    @return Destination Attribute Table */
AttrTable *
DDS::find_matching_container(AttrTable::entry *source, BaseType **dest_variable)
{
    // The attribute entry 'source' must be a container
    if (source->type != Attr_container)
        throw InternalErr(__FILE__, __LINE__, "DDS::find_matching_container");

    // Use the name of the attribute container 'source' to figure out where
    // to put its contents.
    BaseType *btp;
    if ((btp = var(source->name))) {
        // ... matches a variable name? Use var's table
        *dest_variable = btp;
        return &btp->get_attr_table();
    }
    else if ((btp = find_hdf4_dimension_attribute_home(source))) {
        // ... hdf4 dimension attribute? Make a sub table and use that.
        // btp can only be an Array or a Grid Map (which is an array)
        if (btp->get_parent() && btp->get_parent()->type() == dods_grid_c) {
            DBG(cerr << "Found a Grid, assigning to the map" << endl);
            *dest_variable = btp;
            return &btp->get_attr_table();
        }
        else { // must ba a plain Array
            string::size_type i = source->name.find("_dim_");
            string ext = source->name.substr(i + 1);
            *dest_variable = btp;
            return btp->get_attr_table().append_container(ext);
        }
    }
    else {
        // ... otherwise assume it's a global attribute.
        AttrTable *at = d_attr.find_container(source->name);
        if (!at) {
            at = new AttrTable();       // Make a new global table if needed
            d_attr.append_container(at, source->name);
        }

        *dest_variable = 0;
        return at;
    }
}

/** Given a DAS object, scavenge attributes from it and load them into this
    object and the variables it contains.

    If a DAS contans attributes from the current (8/2006) HDF4 server with
    names like var_dim_0, var_dim_1, then make those attribute tables
    sub tables of the \e var table.

    @todo Generalize the code that treats the _dim_? attributes or make
    is obsolete by fixing the HDF4 server.

    @note This method is technically \e unnecessary because a server (or
    client) can easily add attributes directly using the DDS::get_attr_table
    or BaseType::get_attr_table methods and then poke values in using any
    of the methods AttrTable provides. This method exists to ease the
    transition to DDS objects which contain attribute information for the
    existing servers (Since they all make DAS objects separately from the
    DDS). They could be modified to use the same AttrTable methods but
    operate on the AttrTable instances in a DDS/BaseType instead of those in
    a DAS.

    @param das Get attribute information from this DAS. */
void
DDS::transfer_attributes(DAS *das)
{
    // If there is a container set in the DDS then get the container from
    // the DAS. If they are not the same container, then throw an exception
    // (should be working on the same container). If the container does not
    // exist in the DAS, then throw an exception
    if( d_container )
    {
	if( das->container_name() != _container_name )
	{
	    string err = (string)"Error transferring attributes: "
			 + "working on container in dds, but not das" ;
	    throw InternalErr(__FILE__, __LINE__, err ) ;
	}
    }
    AttrTable *top_level = das->get_top_level_attributes() ;

    // foreach container at the outer level
    AttrTable::Attr_iter das_i = top_level->attr_begin();
    AttrTable::Attr_iter das_e = top_level->attr_end();
    while (das_i != das_e) {
        DBG(cerr << "Working on the '" << (*das_i)->name << "' container."
            << endl);

        AttrTable *source = (*das_i)->attributes;
        // Variable that holds 'dest'; null for a global attribute.
        BaseType *dest_variable = 0;
        AttrTable *dest = find_matching_container(*das_i, &dest_variable);

        // foreach source attribute in the das_i container
        AttrTable::Attr_iter source_p = source->attr_begin();
        while (source_p != source->attr_end()) {
            DBG(cerr << "Working on the '" << (*source_p)->name << "' attribute"
                << endl);

            // If this is container, we must have a container (this one) within
            // a container (the 'source'). Look and see if the variable is a
            // Constructor. If so, pass that container into
            // Constructor::transfer_attributes()
            if ((*source_p)->type == Attr_container) {
                if (dest_variable && dest_variable->is_constructor_type()) {
                    dynamic_cast<Constructor&>(*dest_variable).transfer_attributes(*source_p);
                }
                else {
                    dest->append_container(new AttrTable(*(*source_p)->attributes),
                                           (*source_p)->name);
                }
            }
            else {
                dest->append_attr(source->get_name(source_p),
                                  source->get_type(source_p),
                                  source->get_attr_vector(source_p));
            }

            ++source_p;
        }

        ++das_i;
    }
}

/** Get and set the dataset's name.  This is the name of the dataset
    itself, and is not to be confused with the name of the file or
    disk on which it is stored.

    @name Dataset Name Accessors */

//@{

/** Returns the dataset's name. */
string
DDS::get_dataset_name() const
{
    return name;
}

/** Sets the dataset name. */
void
DDS::set_dataset_name(const string &n)
{
    name = n;
}

//@}

/** Get the attribute table for the global attributes. */
AttrTable &
DDS::get_attr_table()
{
    return d_attr;
}

/** Get and set the dataset's filename. This is the physical
    location on a disk where the dataset exists.  The dataset name
    is simply a title.

    @name File Name Accessor
    @see Dataset Name Accessors */

//@{
/** Gets the dataset file name. */
string
DDS::filename()
{
    return _filename;
}

/** Set the dataset's filename. */
void
DDS::filename(const string &fn)
{
    _filename = fn;
}
//@}

/** Given the dap protocol version either from a mime header or from within
    the DDX Dataset element, parse that string and set the DDS fields.
    @see set_dap_client_version()
    @param version_string The version string from the MIME of XML docuemt.
 */
void
DDS::set_dap_version(const string &version_string)
{
    istringstream iss(version_string);

    int major = -1, minor = -1;
    char dot;
    iss >> major;
    iss >> dot;
    iss >> minor;

    DBG(cerr << "Major: " << major << ", dot: " << dot <<", Minor: " << minor << endl);

    if (major == -1 || minor == -1)
        throw Error("Could not parse the client dap (XDAP-Accept header) value");

    set_dap_major(major);
    set_dap_minor(minor);
}

/** Given a version string passed to a server from a client in the XDAP-Accept
    MIME header, parse that string and record the major and minor protocol
    version numbers. This method differs from set_dap_version() in that it is
    storing the version that the client would _like_ the server to use. The
    actual protocol version to which this DDS/DDX conforms is found using the
    get_dap_major() and get_dap_minor() methods.
    @param version_string
 */
void
DDS::set_client_dap_version(const string &version_string)
{
    istringstream iss(version_string);

    int major = -1, minor = -1;
    char dot;
    iss >> major;
    iss >> dot;
    iss >> minor;

    DBG(cerr << "Major: " << major << ", dot: " << dot <<", Minor: " << minor << endl);

    if (major == -1 || minor == -1)
        throw Error("Could not parse the client dap (XDAP-Accept header) value");

    set_client_dap_major(major);
    set_client_dap_minor(minor);
}

/** Get and set the current container. If there are multiple files being
    used to build this DDS, using a container will set a virtual structure
    for the current container.

    @name Container Name Accessor
    @see Dataset Name Accessors */

//@{
/** Gets the dataset file name. */
string
DDS::container_name()
{
    return _container_name;
}

/** Set the current container name and get or create a structure for that
 * name. */
void
DDS::container_name(const string &cn)
{
    // we want to search the DDS for the top level structure with the given
    // name. Set the container to null so that we don't search some previous
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
    _container_name = cn;

}

/** Get the current container structure. */
Structure *
DDS::container()
{
    return d_container ;
}

//@}

/** @brief Adds a copy of the variable to the DDS.
    Using the ptr_duplicate() method, perform a deep copy on the variable
    \e bt and adds the result to this DDS.
    @note The copy will not copy data values.
    @param bt Source variable. */
void
DDS::add_var(BaseType *bt)
{
    if (!bt)
        throw InternalErr(__FILE__, __LINE__,
                          "Trying to add a BaseType object with a NULL pointer.");

    DBG2(cerr << "In DDS::add_var(), bt's address is: " << bt << endl);

    BaseType *btp = bt->ptr_duplicate();
    DBG2(cerr << "In DDS::add_var(), btp's address is: " << btp << endl);
    if( d_container )
    {
	d_container->add_var( bt ) ;
    }
    else
    {
	vars.push_back(btp);
    }
}

/** Remove the named variable from the DDS. This method is not smart about
    looking up names. The variable must exist at the top level of the DDS and
    must match \e exactly the name given.

    @note Invalidates any iterators that reference the contents of the DDS.
    @param n The name of the variable to remove. */
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
/** @brief Find the variable with the given name.

    Returns a pointer to the named variable. If the name contains one or
    more field separators then the function looks for a variable whose
    name matches exactly. If the name contains no field separators then
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
        // Look for the name in the dataset's top-level
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
                DBG(cerr << "Found " << n << " in: " << btp->var()->name() << endl);
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
        DBG2(cerr << "Looking for " << name << " in: " << btp << endl);
        // Look for the name in the current ctor type or the top level
        if (btp->name() == name) {
            DBG2(cerr << "Found " << name << " in: " << btp << endl);
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

/** @brief Returns the number of variables in the DDS. */
int
DDS::num_var()
{
    return vars.size();
}

void
DDS::timeout_on()
{
#ifndef WIN32
    alarm(d_timeout);
#endif
}

void
DDS::timeout_off()
{
#ifndef WIN32
    d_timeout = alarm(0);
#endif
}

void
DDS::set_timeout(int t)
{
    //  Has no effect under win32
    d_timeout = t;
}

int
DDS::get_timeout()
{
    //  Has to effect under win32
    return d_timeout;
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

/** @brief Parse a DDS from a file with the given name. */
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
        throw e;
    }
}


/** @brief Parse a DDS from a file indicated by the input file descriptor. */
void
DDS::parse(int fd)
{
#ifdef WIN32
    FILE *in = fdopen(_dup(fd), "r");
#else
    FILE *in = fdopen(dup(fd), "r");
#endif

    if (!in) {
        throw InternalErr(__FILE__, __LINE__, "Could not access file.");
    }

    try {
        parse(in);
        fclose(in);
    }
    catch (Error &e) {
        fclose(in);
        throw e;
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

    bool status = ddsparse((void *) & arg) == 0;

    dds_delete_buffer(buffer);

    DBG2(cout << "Status from parser: " << status << endl);

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
        if (arg.error())
            throw *arg.error();
    }
}
#if FILE_METHODS
/** @brief Print the entire DDS to the specified file. */
void
DDS::print(FILE *out)
{
    fprintf(out, "Dataset {\n") ;

    for (Vars_citer i = vars.begin(); i != vars.end(); i++) {
        (*i)->print_decl(out) ;
    }

    fprintf(out, "} %s;\n", id2www(name).c_str()) ;

    return ;
}
#endif
/** @brief Print the entire DDS to the specified ostream. */
void
DDS::print(ostream &out)
{
    out << "Dataset {\n" ;

    for (Vars_citer i = vars.begin(); i != vars.end(); i++) {
        (*i)->print_decl(out) ;
    }

    out << "} " << id2www(name) << ";\n" ;

    return ;
}
#if FILE_METHODS
/** @brief Print a constrained DDS to the specified file.

    Print those parts (variables) of the DDS structure to OS that
    are marked to be sent after evaluating the constraint
    expression.

    \note This function only works for scalars at the top level.

    @returns true.
*/
void
DDS::print_constrained(FILE *out)
{
    fprintf(out, "Dataset {\n") ;

    for (Vars_citer i = vars.begin(); i != vars.end(); i++) {
        // for each variable, indent with four spaces, print a trailing
        // semi-colon, do not print debugging information, print only
        // variables in the current projection.
        (*i)->print_decl(out, "    ", true, false, true) ;
    }

    fprintf(out, "} %s;\n", id2www(name).c_str()) ;

    return;
}
#endif
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
        // semi-colon, do not print debugging information, print only
        // variables in the current projection.
        (*i)->print_decl(out, "    ", true, false, true) ;
    }

    out << "} " << id2www(name) << ";\n" ;

    return;
}
#if FILE_METHODS
class VariablePrintXML : public unary_function<BaseType *, void>
{
    FILE *d_out;
    bool d_constrained;
public:
    VariablePrintXML(FILE *out, bool constrained)
            : d_out(out), d_constrained(constrained)
    {}
    void operator()(BaseType *bt)
    {
        bt->print_xml(d_out, "    ", d_constrained);
    }
};
#endif
#if FILE_METHODS
/** Print an XML representation of this DDS. This method is used to generate
    the part of the DDX response. The \c Dataset tag is \e not written by
    this code. The caller of this method must handle writing that and
    including the \c dataBLOB tag.

    @param out Destination.
    @param constrained True if the output should be limited to just those
    variables that are in the projection of the current constraint
    expression.
    @param blob The dataBLOB href. */
void
DDS::print_xml(FILE *out, bool constrained, const string &)
{
    fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    fprintf(out, "<Dataset name=\"%s\"\n", id2xml(name).c_str());

    fprintf(out, "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");

    fprintf(out,"method=\"FILE*\"\n");
    fprintf(out, "dap_major=\"%d\"\n", get_client_dap_major());
    fprintf(out, "dap_minor=\"%d\"\n", get_client_dap_minor());

    // Are we responding to a 3.2 or 2.0 client? We will have to improve on
    // this at some point... jhrg
    if (get_client_dap_major() == 3 && get_client_dap_minor() == 2) {
    fprintf(out, "xmlns=\"%s\"\n", c_dap32_namespace.c_str());

    fprintf(out, "xsi:schemaLocation=\"%s  %s\">\n\n",
            c_dap32_namespace.c_str(), c_default_dap32_schema_location.c_str());
    }
    else {
        fprintf(out, "xmlns=\"%s\"\n", c_dap20_namespace.c_str());
        fprintf(out, "xsi:schemaLocation=\"%s  %s\">\n\n",
                c_dap20_namespace.c_str(), c_default_dap20_schema_location.c_str());
    }


    d_attr.print_xml(out, "    ", constrained);

    fprintf(out, "\n");

    for_each(var_begin(), var_end(), VariablePrintXML(out, constrained));

    fprintf(out, "\n");

    // Only print this for the 2.0, 3.0 and 3.1 versions - which are essentially
    // the same. jhrg
    if (get_client_dap_major() == 2 && get_client_dap_minor() == 0) {
        fprintf(out, "    <dataBLOB href=\"\"/>\n");
    }

    fprintf(out, "</Dataset>\n");
}
#endif

class VariablePrintXMLStrm : public unary_function<BaseType *, void>
{
    ostream &d_out;
    bool d_constrained;
public:
    VariablePrintXMLStrm(ostream &out, bool constrained)
            : d_out(out), d_constrained(constrained)
    {}
    void operator()(BaseType *bt)
    {
        bt->print_xml(d_out, "    ", d_constrained);
    }
};

/** Print an XML representation of this DDS. This method is used to generate
    the part of the DDX response. The \c Dataset tag is \e not written by
    this code. The caller of this method must handle writing that and
    including the \c dataBLOB tag.

    @param out Destination ostream.
    @param constrained True if the output should be limited to just those
    variables that are in the projection of the current constraint
    expression.
    @param blob The dataBLOB href. */
void
DDS::print_xml(ostream &out, bool constrained, const string &)
{
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" ;

    out << "<Dataset name=\"" << id2xml(name) << "\"\n" ;

    out << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n" ;

    // Are we responding to a 3.2 or 2.0 client? We will have to improve on
    // this at some point... jhrg
    if (get_client_dap_major() == 3 && get_client_dap_minor() == 2) {
        out << "xsi:schemaLocation=\"" << c_dap32_namespace
            << "  " << c_default_dap32_schema_location << "\"\n" ;

        out << "xmlns:grddl=\"http://www.w3.org/2003/g/data-view#\"\n";
        out << "grddl:transformation=\"" << grddl_transformation_dap32 <<"\"\n";

        out << "xmlns=\"" << c_dap32_namespace << "\"\n" ;
        out << "xmlns:dap=\"" << c_dap32_namespace << "\"\n" ;

        out << "dap_version=\"" << get_client_dap_major() << "."
            << get_client_dap_minor() << "\"\n";

        if (!get_request_xml_base().empty()) {
            out << "xmlns:xml=\"" << c_xml_namespace << "\"\n";
            out << "xml:base=\"" << get_request_xml_base() << "\"\n";
        }

        // Close the Dataset element
        out << ">\n";
    }
    else {
        out << "xmlns=\"" << c_dap20_namespace << "\"\n" ;
        out << "xsi:schemaLocation=\"" << c_dap20_namespace
            << "  " << c_default_dap20_schema_location << "\">\n\n" ;
    }

    d_attr.print_xml(out, "    ", constrained);

    out << "\n" ;

    for_each(var_begin(), var_end(), VariablePrintXMLStrm(out, constrained));

    out << "\n" ;

    // Only print this for the 2.0, 3.0 and 3.1 versions - which are essentially
    // the same. jhrg
    if (get_client_dap_major() == 2 && get_client_dap_minor() == 0) {
        out << "    <dataBLOB href=\"\"/>\n" ;
    }

    out << "</Dataset>\n" ;
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
    // The dataset must have a name
    if (name == "") {
        cerr << "A dataset must have a name" << endl;
        return false;
    }

    string msg;
    if (!unique_names(vars, name, "Dataset", msg))
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
    BaseType::btp_stack *s = new BaseType::btp_stack;

    DBG2(cerr << "DDS::mark: Looking for " << n << endl);

    BaseType *variable = var(n, s);
    if (!variable) {
        DBG2(cerr << "Could not find variable " << n << endl);
        delete s; s = 0;
        return false;
    }
    variable->set_send_p(state);

    DBG2(cerr << "DDS::mark: Set variable " << variable->name()
            << " (a " << variable->type_name() << ")" << endl);

    // Now check the btp_stack and run BaseType::set_send_p for every
    // BaseType pointer on the stack. Using BaseType::set_send_p() will
    // set the property for a Constructor but not its contained variables
    // which preserves the semantics of projecting just one field.
    while (!s->empty()) {
        s->top()->BaseType::set_send_p(state);

        DBG2(cerr << "DDS::mark: Set variable " << s->top()->name()
                << " (a " << s->top()->type_name() << ")" << endl);
        string parent_name = (s->top()->get_parent()) ? s->top()->get_parent()->name(): "none";
        string parent_type = (s->top()->get_parent()) ? s->top()->get_parent()->type_name(): "none";
        DBG2(cerr << "DDS::mark: Parent variable " << parent_name << " (a " << parent_type << ")" << endl);

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
    strm << DapIndent::LMarg << "name: " << name << endl ;
    strm << DapIndent::LMarg << "filename: " << _filename << endl ;
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
