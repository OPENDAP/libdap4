
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

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>


#include "config.h"

#include <string>
#include <algorithm>
#include <functional>

//#define DODS_DEBUG

#include "Constructor.h"
#include "Grid.h"

#include "debug.h"
#include "escaping.h"
#include "Error.h"
#include "InternalErr.h"


using namespace std;

namespace libdap {

// Private member functions

void
Constructor::_duplicate(const Constructor &c)
{
    Constructor &cc = const_cast<Constructor &> (c);

    DBG(cerr << "Copying constructor: " << name() << endl);

    for (Vars_iter i = cc._vars.begin(); i != cc._vars.end(); i++) {
	DBG(cerr << "Copying field: " << (*i)->name() << endl);
	BaseType *btp = (*i)->ptr_duplicate();
	btp->set_parent(this);
	_vars.push_back(btp);
    }
}

// Public member functions

Constructor::Constructor(const string &n, const Type &t)
        : BaseType(n, t)
{}

/** Server-side constructor that takes the name of the variable to be
 * created, the dataset name from which this variable is being created, and
 * the type of data being stored in the Constructor. This is a protected
 * constructor, available only to derived classes of Constructor
 *
 * @param n string containing the name of the variable to be created
 * @param d string containing the name of the dataset from which this
 * variable is being created
 * @param t type of data being stored
 */
Constructor::Constructor(const string &n, const string &d, const Type &t)
        : BaseType(n, d, t)
{}

Constructor::Constructor(const Constructor &rhs) : BaseType(rhs)
{}

Constructor::~Constructor()
{}

Constructor &
Constructor::operator=(const Constructor &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<BaseType &>(*this) = rhs; // run BaseType=

    _duplicate(rhs);

    return *this;
}

/** Returns an iterator referencing the first structure element. */
Constructor::Vars_iter
Constructor::var_begin()
{
    return _vars.begin() ;
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
Constructor::find_hdf4_dimension_attribute_home(AttrTable::entry *source)
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

/** Given an attribute container from a table, find or make a destination
    for its contents in the current constructor variable. */
AttrTable *
Constructor::find_matching_container(AttrTable::entry *source,
                                     BaseType **dest_variable)
{
    // The attribute entry 'source' must be a container
    if (source->type != Attr_container)
        throw InternalErr(__FILE__, __LINE__, "Constructor::find_matching_container");

    // Use the name of the attribute container 'source' to figure out where
    // to put its contents.
    BaseType *btp;
    if ((btp = var(source->name))) {
        // ... matches a variable name? Use var's table
        *dest_variable = btp;
        return &btp->get_attr_table();
    }
    // As more special-case attribute containers come to light, add clauses
    // here.
    else if ((btp = find_hdf4_dimension_attribute_home(source))) {
        // ... hdf4 dimension attribute? Make a sub table and use that.
        // btp can only be an Array or a Grid Map (which is an array)
        if (btp->get_parent()->type() == dods_grid_c) {
            DBG(cerr << "Found a Grid" << endl);
            *dest_variable = btp;
            return &btp->get_attr_table();
        }
        else { // must be a plain Array
            string::size_type i = source->name.find("_dim_");
            string ext = source->name.substr(i + 1);
            *dest_variable = btp;
            return btp->get_attr_table().append_container(ext);
        }
    }
    else {
        // ... otherwise assume it's a global attribute.
        AttrTable *at = get_attr_table().find_container(source->name);
        if (!at) {
            at = new AttrTable();       // Make a new global table if needed
            get_attr_table().append_container(at, source->name);
        }

        *dest_variable = 0;
        return at;
    }
}

/** Given an Attribute entry, scavenge attributes from it and load them into
    this object and the variables it contains. Assume that the caller has
    determined the table holds attributes pertinent to only this variable.

    @note This method is technically \e unnecessary because a server (or
    client) can easily add attributes directly using the DDS::get_attr_table
    or BaseType::get_attr_table methods and then poke values in using any
    of the methods AttrTable provides. This method exists to ease the
    transition to DDS objects which contain attribute information for the
    existing servers (Since they all make DAS objects separately from the
    DDS). They could be modified to use the same AttrTable methods but
    operate on the AttrTable instances in a DDS/BaseType instead of those in
    a DAS.

    @param entry Get attribute information from this Attribute table. Note
    that even though the type of the argument is an AttrTable::entry, the
    entry \e must be an attribute container.*/
void
Constructor::transfer_attributes(AttrTable::entry * entry)
{
    DBG(cerr << "Constructor::transfer_attributes, variable: " << name() <<
        endl);
    DBG(cerr << "Working on the '" << entry->
        name << "' container." << endl);
    if (entry->type != Attr_container)
        throw InternalErr(__FILE__, __LINE__,
                          "Constructor::transfer_attributes");

    AttrTable *source = entry->attributes;
    BaseType *dest_variable = 0;
    AttrTable *dest = find_matching_container(entry, &dest_variable);

    // foreach source attribute in the das_i container
    AttrTable::Attr_iter source_p = source->attr_begin();
    while (source_p != source->attr_end()) {
        DBG(cerr << "Working on the '" << (*source_p)->
            name << "' attribute" << endl);

        if ((*source_p)->type == Attr_container) {
            if (dest_variable && dest_variable->is_constructor_type()) {
                dynamic_cast <Constructor & >(*dest_variable).transfer_attributes(*source_p);
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
}

/** Returns an iterator referencing the end of the list of structure
    elements. Does not reference the last structure element. */
Constructor::Vars_iter
Constructor::var_end()
{
    return _vars.end() ;
}

/** Return a reverse iterator that references the last element. */
Constructor::Vars_riter
Constructor::var_rbegin()
{
    return _vars.rbegin();
}

/** Return a reverse iterator that references a point 'before' the first
    element. */
Constructor::Vars_riter
Constructor::var_rend()
{
    return _vars.rend();
}

/** Return the iterator for the \e ith variable.
    @param i the index
    @return The corresponding  Vars_iter */
Constructor::Vars_iter
Constructor::get_vars_iter(int i)
{
    return _vars.begin() + i;
}

/** Return the BaseType pointer for the \e ith variable.
    @param i This index
    @return The corresponding BaseType*. */
BaseType *
Constructor::get_var_index(int i)
{
    return *(_vars.begin() + i);
}

#if FILE_METHODS
void
Constructor::print_decl(FILE *out, string space, bool print_semi,
                        bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
        return;

    fprintf(out, "%s%s {\n", space.c_str(), type_name().c_str()) ;
    for (Vars_citer i = _vars.begin(); i != _vars.end(); i++) {
        (*i)->print_decl(out, space + "    ", true,
                         constraint_info, constrained);
    }
    fprintf(out, "%s} %s", space.c_str(), id2www(name()).c_str()) ;

    if (constraint_info) { // Used by test drivers only.
        if (send_p())
            cout << ": Send True";
        else
            cout << ": Send False";
    }

    if (print_semi)
        fprintf(out, ";\n") ;
}
#endif

void
Constructor::print_decl(ostream &out, string space, bool print_semi,
                        bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
        return;

    out << space << type_name() << " {\n" ;
    for (Vars_citer i = _vars.begin(); i != _vars.end(); i++) {
        (*i)->print_decl(out, space + "    ", true,
                         constraint_info, constrained);
    }
    out << space << "} " << id2www(name()) ;

    if (constraint_info) { // Used by test drivers only.
        if (send_p())
            out << ": Send True";
        else
            out << ": Send False";
    }

    if (print_semi)
	out << ";\n" ;
}

#if FILE_METHODS
class PrintField : public unary_function<BaseType *, void>
{
    FILE *d_out;
    string d_space;
    bool d_constrained;
public:
    PrintField(FILE *o, string s, bool c)
            : d_out(o), d_space(s), d_constrained(c)
    {}

    void operator()(BaseType *btp)
    {
        btp->print_xml(d_out, d_space, d_constrained);
    }
};

void
Constructor::print_xml(FILE *out, string space, bool constrained)
{
    if (constrained && !send_p())
        return;

    bool has_attributes = false; // *** fix me
    bool has_variables = (var_begin() != var_end());

    fprintf(out, "%s<%s", space.c_str(), type_name().c_str());
    if (!name().empty())
        fprintf(out, " name=\"%s\"", id2xml(name()).c_str());

    if (has_attributes || has_variables) {
        fprintf(out, ">\n");

        get_attr_table().print_xml(out, space + "    ", constrained);

        for_each(var_begin(), var_end(),
                 PrintField(out, space + "    ", constrained));

        fprintf(out, "%s</%s>\n", space.c_str(), type_name().c_str());
    }
    else {
        fprintf(out, "/>\n");
    }
}
#endif

class PrintFieldStrm : public unary_function<BaseType *, void>
{
    ostream &d_out;
    string d_space;
    bool d_constrained;
public:
    PrintFieldStrm(ostream &o, string s, bool c)
            : d_out(o), d_space(s), d_constrained(c)
    {}

    void operator()(BaseType *btp)
    {
        btp->print_xml(d_out, d_space, d_constrained);
    }
};

void
Constructor::print_xml(ostream &out, string space, bool constrained)
{
    if (constrained && !send_p())
        return;

    bool has_attributes = get_attr_table().get_size() > 0;
    bool has_variables = (var_begin() != var_end());

    out << space << "<" << type_name() ;
    if (!name().empty())
	out << " name=\"" << id2xml(name()) << "\"" ;

    if (has_attributes || has_variables) {
	out << ">\n" ;

        get_attr_table().print_xml(out, space + "    ", constrained);

        for_each(var_begin(), var_end(),
                 PrintFieldStrm(out, space + "    ", constrained));

	out << space << "</" << type_name() << ">\n" ;
    }
    else {
	out << "/>\n" ;
    }
}

/** True if the instance can be flattened and printed as a single table
    of values. For Arrays and Grids this is always false. For Structures
    and Sequences the conditions are more complex. The implementation
    provided by this class always returns false. Other classes should
    override this implementation.

    @todo Change the name to is_flattenable or something like that. 05/16/03
    jhrg

    @brief Check to see whether this variable can be printed simply.
    @return True if the instance can be printed as a single table of
    values, false otherwise. */
bool
Constructor::is_linear()
{
    return false;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and information about this
 * instance.
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
Constructor::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Constructor::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    BaseType::dump(strm) ;
    strm << DapIndent::LMarg << "vars: " << endl ;
    DapIndent::Indent() ;
    Vars_citer i = _vars.begin() ;
    Vars_citer ie = _vars.end() ;
    for (; i != ie; i++) {
        (*i)->dump(strm) ;
    }
    DapIndent::UnIndent() ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

