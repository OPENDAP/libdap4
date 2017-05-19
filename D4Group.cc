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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

//#define DODS_DEBUG

#include <iostream>
#include <sstream>
#include <iomanip>

#include <stdint.h>

#include "crc.h"

#include "BaseType.h"
#include "Array.h"

#include "XMLWriter.h"
#include "D4Attributes.h"
#include "D4Dimensions.h"
#include "D4Group.h"
#include "D4Enum.h"

#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"

#include "debug.h"

/**
 * Define this symbol iff we decide to include information about the
 * byte order of the response (as sent from the server) so that the
 * client can determine the correct CRC32 hash code. jhrg 1/4/16
 */
#undef INCLUDE_SOURCE_BYTE_ORDER

namespace libdap {

void D4Group::m_duplicate(const D4Group &g)
{
	DBG(cerr << "In D4Group::m_duplicate for " << g.name() << endl);

	// dims; deep copy, this is the parent
	if (g.d_dims) {
		d_dims = new D4Dimensions(*(g.d_dims));
		d_dims->set_parent(this);

	    // Update all of the D4Dimension weak pointers in the Array objects.
	    // This is a hack - we know that Constructor::m_duplicate() has been
	    // called at this point and any Array instances have dimension pointers
	    // that reference the 'old' dimensions (g.d_dims) and not the 'new'
	    // dimensions made above. Scan every array and re-wire the weak pointers.
	    // jhrg 8/15/14
	    Vars_citer vi = d_vars.begin();
	    while (vi != d_vars.end()) {
	        if ((*vi)->type() == dods_array_c)
	            static_cast<Array*>(*vi)->update_dimension_pointers(g.d_dims, d_dims);
	        ++vi;
	    }
	}

#if 0
	// Moved this block up inside the if because g.d_dims might be false. jhrg 9/14/15
	Vars_citer vi = d_vars.begin();
	while (vi != d_vars.end()) {
		if ((*vi)->type() == dods_array_c)
			static_cast<Array*>(*vi)->update_dimension_pointers(g.d_dims, d_dims);
		++vi;
	}
#endif

	// enums; deep copy
	if (g.d_enum_defs) d_enum_defs = new D4EnumDefs(*g.d_enum_defs);

    // groups
    groupsCIter i = g.d_groups.begin();
    while(i != g.d_groups.end()) {
        // Only D4Groups are in the d_groups container.
        D4Group *g = static_cast<D4Group*>((*i++)->ptr_duplicate());
        add_group_nocopy(g);
    }

    DBG(cerr << "Exiting D4Group::m_duplicate" << endl);
}

/** The D4Group constructor requires only the name of the variable
    to be created. The name may be omitted, which will create a
    nameless variable. This may be adequate for some applications.

    @note This type is available in DAP4 only.
    See http://docs.opendap.org/index.php/DAP4:_Specification_Volume_1#Groups


    @param n A string containing the name of the variable.
*/
D4Group::D4Group(const string &name)
    : Constructor(name, dods_group_c, /*is_dap4*/true), d_dims(0), d_enum_defs(0)
{}

/** The D4Group server-side constructor requires the name of the variable
    to be created and the dataset name from which this variable is being
    created. Used on server-side handlers.

    @note This type is available in DAP4 only.
    See http://docs.opendap.org/index.php/DAP4:_Specification_Volume_1#Groups

    @param n A string containing the name of the variable.
    @param d A string containing the name of the dataset.
*/
D4Group::D4Group(const string &name, const string &dataset)
    : Constructor(name, dataset, dods_group_c, /*is_dap4*/true), d_dims(0), d_enum_defs(0)
{}

/** The D4Group copy constructor. */
D4Group::D4Group(const D4Group &rhs) : Constructor(rhs), d_dims(0), d_enum_defs(0)
{
	DBG(cerr << "In D4Group::copy_ctor for " << rhs.name() << endl);
    m_duplicate(rhs);
}

D4Group::~D4Group()
{
    delete d_dims;
    delete d_enum_defs;

    groupsIter i = d_groups.begin();
    while(i != d_groups.end())
        delete *i++;
}

#if 0
D4Group *

// I think this was a mistake. jhrg 11/17/16
#endif
BaseType *
D4Group::ptr_duplicate()
{
    return new D4Group(*this);
}

D4Group &
D4Group::operator=(const D4Group &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<Constructor &>(*this) = rhs; // run Constructor=

    m_duplicate(rhs);

    return *this;
}

/**
 * Get the Fully Qualified Name for this Group, including the Group. This
 * uses the name representation described in the DAP4 specification.
 *
 * @return The FQN in a string
 */
string
D4Group::FQN() const
{
	// The root group is named "/" (always)
	return (name() == "/") ? "/" : static_cast<D4Group*>(get_parent())->FQN() + name() + "/";
}

// Note that in order for this to work the second argument must not be a reference.
// jhrg 8/20/13
static bool
name_eq(D4Group *g, const string name)
{
	return g->name() == name;
}

D4Group *
D4Group::find_child_grp(const string &grp_name)
{
	groupsIter g = find_if(grp_begin(), grp_end(), bind2nd(ptr_fun(name_eq), grp_name));
	return (g == grp_end()) ? 0: *g;
}

// TODO Add constraint param? jhrg 11/17/13
BaseType *
D4Group::find_first_var_that_uses_dimension(D4Dimension *dim)
{
    // for each group, starting with the root group
    //    for each variable in the group that is marked to send and is an array
    //        return the btp if it uses the D4Dimension
    //    if it contains child groups, search those
    //        return the btp if it uses the D4Dimension
    // return null

    // exhaustive breadth-first search for 'dim

    // root group
    for (Vars_iter i = var_begin(), e = var_end(); i != e; ++i) {
        if ((*i)->send_p() && (*i)->type() == dods_array_c) {
            Array *a = static_cast<Array*>(*i);
            for (Array::Dim_iter di = a->dim_begin(), de = a->dim_end(); di != de; ++di) {
                if (a->dimension_D4dim(di) == dim)
                    return a;
            }
        }
    }

    for (groupsIter i = grp_begin(), e = grp_end(); i != e; ++i) {
        BaseType *btp = (*i)->find_first_var_that_uses_dimension(dim);
        if (btp) return btp;
    }

    return 0;
}

BaseType *
D4Group::find_first_var_that_uses_enumeration(D4EnumDef *enum_def)
{
    // for each group, starting with the root group
    //    for each variable in the group that is marked to send and is an array
    //        return the btp if it uses the D4EnumDef
    //    if it contains child groups, search those
    //        return the btp if it uses the D4EnumDef
    // return null

    // exhaustive breadth-first search for 'dim

    // root group
    for (Vars_iter i = var_begin(), e = var_end(); i != e; ++i) {
        if ((*i)->send_p() && (*i)->type() == dods_enum_c) {
            D4Enum *e = static_cast<D4Enum*>(*i);
            if (e->enumeration() == enum_def)
                return e;
        }
    }

    for (groupsIter i = grp_begin(), e = grp_end(); i != e; ++i) {
        BaseType *btp = (*i)->find_first_var_that_uses_enumeration(enum_def);
        if (btp) return btp;
    }

    return 0;
}

/**
 * @brief Find the dimension using a path.
 * Using the DAP4 name syntax, lookup a dimension. The dimension must
 * be defined before it is used. The \c path argument may be either an
 * absolute path or a relative path. Note that the name syntax does not
 * provide for paths to contain an 'up one level' symbol.
 * @param path The path to the dimension
 * @return A pointer to the D4Dimension object.
 */
D4Dimension *
D4Group::find_dim(const string &path)
{
	string lpath = path;		// get a mutable copy

	// special-case for the root group
	if (lpath[0] == '/') {
		if (name() != "/")
			throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
		else
			lpath = lpath.substr(1);
	}

	string::size_type pos = lpath.find('/');
	if (pos == string::npos) {
		// name looks like 'bar'
		return dims()->find_dim(lpath);
	}

	// name looks like foo/bar/baz where foo and bar must be groups
	string grp_name = lpath.substr(0, pos);
	lpath = lpath.substr(pos + 1);

	D4Group *grp = find_child_grp(grp_name);
	return (grp == 0) ? 0: grp->find_dim(lpath);
}

Array *
D4Group::find_map_source(const string &path)
{
	BaseType *map_source = m_find_map_source_helper(path);

	// TODO more complete semantic checking jhrg 10/16/13
	if (map_source && map_source->type() == dods_array_c) return static_cast<Array*>(map_source);

	return 0;
}

BaseType *
D4Group::m_find_map_source_helper(const string &path)
{
	string lpath = path;		// get a mutable copy

	// special-case for the root group
	if (lpath[0] == '/') {
		if (name() != "/")
			throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
		else
			lpath = lpath.substr(1);
	}

	string::size_type pos = lpath.find('/');
	if (pos == string::npos) {
		// name looks like 'bar'
		return var(lpath);
	}

	// name looks like foo/bar/baz where foo an bar must be groups
	string grp_name = lpath.substr(0, pos);
	lpath = lpath.substr(pos + 1);

	D4Group *grp = find_child_grp(grp_name);
	return (grp == 0) ? 0: grp->var(lpath);
}

D4EnumDef *
D4Group::find_enum_def(const string &path)
{
    string lpath = path;        // get a mutable copy

    // special-case for the root group
    if (lpath[0] == '/') {
        if (name() != "/")
            throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
        else
            lpath = lpath.substr(1);
    }

    string::size_type pos = lpath.find('/');
    if (pos == string::npos) {
        // name looks like 'bar'
        return enum_defs()->find_enum_def(lpath);
    }

    // name looks like foo/bar/baz where foo and bar must be groups
    string grp_name = lpath.substr(0, pos);
    lpath = lpath.substr(pos + 1);

    D4Group *grp = find_child_grp(grp_name);
    return (grp == 0) ? 0: grp->enum_defs()->find_enum_def(lpath);
}

/**
 * Find a variable using it's FUlly Qualified Name (FQN). The leading '/' is optional.
 *
 * @param path The FQN to the variable
 * @return A BaseType* to the variable of null if it was not found
 * @see BaseType::FQN()
 */
BaseType *
D4Group::find_var(const string &path)
{
    string lpath = path;        // get a mutable copy

    // special-case for the root group
    if (lpath[0] == '/') {
        if (name() != "/")
            throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
        else
            lpath = lpath.substr(1);
    }

    string::size_type pos = lpath.find('/');
    if (pos == string::npos) {
        // name looks like 'bar' or bar.baz; lookup in the Constructor that's part of the Group
    	return var(lpath);
    }

    // name looks like foo/bar/baz where foo and bar must be groups
    string grp_name = lpath.substr(0, pos);
    lpath = lpath.substr(pos + 1);

    D4Group *grp = find_child_grp(grp_name);
    return (grp == 0) ? 0 : grp->find_var(lpath);
}

/** Compute the size of all of the variables in this group and it's children,
 * in kilobytes
 *
 * @param constrained Should the current constraint be taken into account?
 * @return The size in kilobytes
 */
long
D4Group::request_size(bool constrained)
{
    long long size = 0;
    // variables
    Constructor::Vars_iter v = var_begin();
    while (v != var_end()) {
        if (constrained) {
            if ((*v)->send_p())
                size += (*v)->width(constrained);
        }
        else {
            size += (*v)->width(constrained);
        }

        ++v;
    }

    // groups
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        size += (*g++)->request_size(constrained);

    return size / 1024;
}

void
D4Group::set_read_p(bool state)
{
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->set_read_p(state);

    Constructor::set_read_p(state);
}

void
D4Group::set_send_p(bool state)
{
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->set_send_p(state);

    Constructor::set_send_p(state);
}

void
D4Group::intern_data(/*Crc32 &checksum, DMR &dmr, ConstraintEvaluator &eval*/)
{
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->intern_data(/*checksum, dmr, eval*/);

    // Specialize how the top-level variables in any Group are sent; include
    // a checksum for them. A subset operation might make an interior set of
    // variables, but the parent structure will still be present and the checksum
    // will be computed for that structure. In other words, DAP4 does not try
    // to sort out which variables are the 'real' top-level variables and instead
    // simply computes the CRC for whatever appears as a variable in the root
    // group.
	for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
		// Only send the stuff in the current subset.
		if ((*i)->send_p()) {
#if 0
		    checksum.Reset();
#endif
			(*i)->intern_data(/*checksum, dmr, eval*/);
#if 0
			D4Attribute *a = new D4Attribute("DAP4_Checksum_CRC32", attr_str_c);

			ostringstream oss;
		    oss.setf(ios::hex, ios::basefield);
		    oss << setfill('0') << setw(8) << checksum.GetCrc32();
            a->add_value(oss.str());
#if INCLUDE_SOURCE_BYTE_ORDER
	        if (um.is_source_big_endian())
	            a->add_value("source:big-endian");
	        else
	            a->add_value("source:little-endian");
#endif
	        (*i)->attributes()->add_attribute_nocopy(a);
			DBG(cerr << "CRC32: " << oss.str() << " for " << (*i)->name() << endl);
#endif
		}
	}
}

/**
 * @brief Serialize a Group
 * @param m The DAP4 Stream Marshaller. This object serializes the data values and
 * writes checksums (using CRC32) for the top level variables in every Group for which
 * one or more variables are sent. The DAP4 Marshaller object can be made so that only
 * the checksums are written.
 * @param dmr Unused
 * @param eval Unused
 * @param filter Unused
 * @exception Error is thrown if the value needs to be read and that operation fails.
 */
void
D4Group::serialize(D4StreamMarshaller &m, DMR &dmr, /*ConstraintEvaluator &eval,*/ bool filter)
{
#if 0
    // This will call Constructor read which will, for everything but a Sequence,
    // read all of the data in one shot. However, the serialize() methods for the
    // Arrays, Structures, etc., also have read() calls in them and those can be
    // used to control how long the data are in memory, e.g., limiting the lifetime
    // of a large array and avoiding having overlapping arrays when they are not
    // needed. For a sequence read() has different semantics. It is called once
    // for every instance and the read_p flag is not used.
    if (!read_p())
        read();  // read() throws Error
#endif

    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->serialize(m, dmr, filter);

    // Specialize how the top-level variables in any Group are sent; include
    // a checksum for them. A subset operation might make an interior set of
    // variables, but the parent structure will still be present and the checksum
    // will be computed for that structure. In other words, DAP4 does not try
    // to sort out which variables are the 'real' top-level variables and instead
    // simply computes the CRC for whatever appears as a variable in the root
    // group.
	for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
		// Only send the stuff in the current subset.
		if ((*i)->send_p()) {
			m.reset_checksum();

	        DBG(cerr << "Serializing variable " << (*i)->type_name() << " " << (*i)->name() << endl);
			(*i)->serialize(m, dmr, filter);

			DBG(cerr << "Wrote CRC32: " << m.get_checksum() << " for " << (*i)->name() << endl);
			m.put_checksum();
		}
	}
}

void D4Group::deserialize(D4StreamUnMarshaller &um, DMR &dmr)
{
	groupsIter g = d_groups.begin();
	while (g != d_groups.end()) {
        DBG(cerr << "Deserializing group " << (*g)->name() << endl);
		(*g++)->deserialize(um, dmr);
	}
	// Specialize how the top-level variables in any Group are received; read
	// their checksum and store the value in a magic attribute of the variable
	for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        DBG(cerr << "Deserializing variable " << (*i)->type_name() << " " << (*i)->name() << endl);
		(*i)->deserialize(um, dmr);

		D4Attribute *a = new D4Attribute("DAP4_Checksum_CRC32", attr_str_c);
		string crc = um.get_checksum_str();
		a->add_value(crc);
#if INCLUDE_SOURCE_BYTE_ORDER
		if (um.is_source_big_endian())
		    a->add_value("source:big-endian");
		else
		    a->add_value("source:little-endian");
#endif
		DBG(cerr << "Read CRC32: " << crc << " for " << (*i)->name() << endl);
		(*i)->attributes()->add_attribute_nocopy(a);
	}
}

void
D4Group::print_dap4(XMLWriter &xml, bool constrained)
{
    if (!name().empty() && name() != "/") {
        // For named groups, if constrained is true only print if this group
        // has variables that are marked for transmission. For the root group
        // this test is not made.
        if (constrained && !send_p())
            return;

        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) type_name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write " + type_name() + " element");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*) name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
    }

    // dims
    if (!dims()->empty())
        dims()->print_dap4(xml, constrained);

    // enums
    if (!enum_defs()->empty())
        enum_defs()->print_dap4(xml, constrained);

    // variables
    Constructor::Vars_iter v = var_begin();
    while (v != var_end())
        (*v++)->print_dap4(xml, constrained);

    // attributes
    attributes()->print_dap4(xml);

    // groups
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->print_dap4(xml, constrained);

    if (!name().empty() && name() != "/") {
        if (xmlTextWriterEndElement(xml.get_writer()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not end " + type_name() + " element");
    }
}


/** @brief DAP4 to DAP2 transform
 *
 * D4Group objects, with the exception of the root group, "disappear"
 * into the names of their member variables. Specifically the Group
 * name is add as a prefix followed by a "/" separator to the names
 * of all of the Group's member groups variables. The Group attributes
 * (metadata) are transfered to the parent_attr_table. The Group
 * members are collected returned in vector.
 *
 *
 * @param  The AttrTable pointer parent_attr_table is used by Groups, which disappear
 * from the DAP2 representation. Their children are returned in the the BAseType vector
 * their attributes are added to parent_attr_table;
 * @return A pointer to a vector of BaseType pointers (right?). In this D4Group case the
 * vector will contain DAP2 versions of all of the member variables of the D4Group instance.
 * (ex: UInt64) the will return a NULL pointer and so this must be tested!
 */
vector<BaseType *> *
D4Group::transform_to_dap2(AttrTable *parent_attr_table){
    return transform_to_dap2(parent_attr_table,false);
}
vector<BaseType *> *
D4Group::transform_to_dap2(AttrTable *parent_attr_table, bool is_root)
{
    DBG( cerr << __func__ << "() - BEGIN ("<< name() << " is_root: "<< (is_root?"true":"false") << ")" << endl;);
    vector<BaseType *> *results = new vector<BaseType *>();
    vector<BaseType *> dropped_vars;

    AttrTable *group_attrs;
    group_attrs = attributes()->get_AttrTable(name());

    /**
     * If this is the root group then we handle the attributes differently.
     */
    if(is_root){
        DBG( cerr << __func__ << "() - Promoting group attributes to parent" << endl;);
       // If it's a root group we copy all the stuff up into the parent attr table
        for (AttrTable::Attr_iter i = group_attrs->attr_begin(), e = group_attrs->attr_end(); i != e; ++i) {
            if ((*i)->type == Attr_container) {
                // copy the source container so that the DAS passed in can be
                // deleted after calling this method.
                AttrTable *at = new AttrTable(*(*i)->attributes);
                parent_attr_table->append_container(at, at->get_name());
            }
            else {
                parent_attr_table->append_attr(
                    (*i)->name,
                    AttrType_to_String((*i)->type),
                    (*i)->attr);
            }
        }
        delete group_attrs;
        group_attrs = parent_attr_table;
    }

    /**
     * Now we process the child variables of this group
     */
    for (D4Group::Vars_citer varIter = var_begin(), e = var_end(); varIter != e; ++varIter) {
        DBG( cerr << __func__ << "() - Processing member variable '" << (*varIter)->name() <<
            "' root: " << (is_root?"true":"false") << endl;);
        vector<BaseType *> *new_vars = (*varIter)->transform_to_dap2(group_attrs);
        if (new_vars) {  // Might be un-mappable
            // It's not so game on..
            vector<BaseType*>::iterator vIter = new_vars->begin();
            vector<BaseType*>::iterator end = new_vars->end();
            for( ; vIter!=end ; vIter++ ){
                BaseType *new_var = (*vIter);

                string new_name = (is_root?"":FQN()) + new_var->name();
                new_var->set_name(new_name);
                results->push_back(new_var);
                (*vIter) = NULL;
                DBG( cerr << __func__ << "() - Added member variable '" << (*varIter)->name() << "' " <<
                    "to results vector. root: "<< (is_root?"true":"false") << endl;);
            }
            delete new_vars;
        }
        else {
            DBG( cerr << __func__ << "() - Dropping member variable " << (*varIter)->name() <<
            " root: " << (is_root?"true":"false") << endl;);
            // Got back a NULL, so we are dropping this var.
            dropped_vars.push_back(*varIter);
        }
    }
    // Process dropped DAP4 vars
    DBG( cerr << __func__ << "() - Processing " << dropped_vars.size() << " Dropped Variable(s)" << endl;);
    AttrTable *dv_attr_table = make_dropped_vars_attr_table(&dropped_vars);
    if(dv_attr_table){
        DBG( cerr << __func__ << "() - Adding Dropped Variables AttrTable" << endl;);
        group_attrs->append_container(dv_attr_table,dv_attr_table->get_name());
    }
    else {
        DBG( cerr << __func__ << "() - No Dropped Variables AttrTable returned." << endl;);

    }

    /**
     *  Get all the child groups.
     */
    D4Group::groupsIter gIter = grp_begin();
    D4Group::groupsIter gEnd = grp_end();
    for( ; gIter!=gEnd ; gIter++){
        D4Group *grp = *gIter;
        DBG( cerr << __func__ << "() - Processing D4Group " << grp->name() << endl;);
        vector<BaseType *> *d2_vars = grp->transform_to_dap2(group_attrs);
        if(d2_vars){
            DBG( cerr << __func__ << "() - Processing " << grp->name() << " Member Variables." << endl;);
            vector<BaseType *>::iterator vIter = d2_vars->begin();
            vector<BaseType *>::iterator vEnd = d2_vars->end();
            for( ; vIter!=vEnd; vIter++){
                DBG( cerr << __func__ << "() - Processing " << grp->name() << " Member Variable: " << (*vIter)->name() << endl;);
                results->push_back(*vIter);
            }
        }



    }

    if(!is_root){
        group_attrs->set_name(name());
        parent_attr_table->append_container(group_attrs,group_attrs->get_name());
    }
    DBG( cerr << __func__ << "() - END" << endl;);
    return results;
}


} /* namespace libdap */
