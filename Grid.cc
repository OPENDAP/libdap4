
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

// implementation for Grid.
//
// jhrg 9/15/94

#include "config.h"

// #define DODS_DEBUG

#include <sstream>
#include <functional>
#include <algorithm>

#include "Grid.h"
#include "DDS.h"
#include "Array.h"  // for downcasts
#include "util.h"
#include "InternalErr.h"
#include "escaping.h"
#include "XDRStreamMarshaller.h"
#include "debug.h"

#include "XMLWriter.h"
#include "DMR.h"
#include "D4Group.h"
#include "D4Maps.h"
#include "D4Attributes.h"

using namespace std;

namespace libdap {

void
Grid::m_duplicate(const Grid &s)
{
    // TODO revisit this code once/if the class is switched from using it's
    // own vars to those in Constructor. jhrg 4/3/13

	// copy the weak pointer - Constructor will take care of copying
	// the 'strong' pointers.
	//d_array_var = s.d_array_var;
	d_is_array_set = s.d_is_array_set;
}

/** The Grid constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications.

    @param n A string containing the name of the variable to be
    created.

    @brief The Grid constructor.
*/
Grid::Grid(const string &n) : Constructor(n, dods_grid_c), d_is_array_set(false)
{}

/** The Grid server-side constructor requires the name of the variable
    to be created and the dataset name from which this variable is created.
    Used when creating variables on the server side.

    @param n A string containing the name of the variable to be
    created.
    @param d A string containing the name of the dataset from which this
    variable is being created.

    @brief The Grid constructor.
*/
Grid::Grid(const string &n, const string &d)
    : Constructor(n, d, dods_grid_c), d_is_array_set(false)
{}

/** @brief The Grid copy constructor. */
Grid::Grid(const Grid &rhs) : Constructor(rhs)
{
    m_duplicate(rhs);
}

Grid::~Grid()
{
	//d_array_var = 0;	// Weak pointer; object will be freed by Constructor
}

BaseType *
Grid::ptr_duplicate()
{
    return new Grid(*this);
}

Grid &
Grid::operator=(const Grid &rhs)
{
    if (this == &rhs)
        return *this;

    // Removed this; it makes this operator= work differently than the rest
#if 0
    delete d_array_var; d_array_var = 0;

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        BaseType *btp = *i ;
        delete btp ;
    }
#endif

    dynamic_cast<Constructor &>(*this) = rhs;

    m_duplicate(rhs);

    return *this;
}

/**
 *
 */
void
Grid::transform_to_dap4(D4Group *root, Constructor *container)
{
    DBG(cerr << __func__ << "() - BEGIN (gird:"<< name() << ")" << endl;);

    vector<Array*> d4_map_arrays;

    // We do the Map Arrays first because some people expect to see them
    // delclared prior to the coverage array the utilizes them - even though that
    // is not a requirement of DAP4 I did it here to make people happier.
	// We add the maps arrays to the current container if needed and make a
    // a vector of them so we can add D4Map objects to our Precious down
    // below.
	for (Map_iter i = map_begin(), e = map_end(); i != e; ++i) {
	    DBG(cerr << __func__ << "() - Processing Map Array:  '"<< (*i)->name() << "' ("<< (void *)(*i)<< ")" << endl;);
        // Only add the map/array if it's not already present in the target DAP2 container.
	    // Given the scoping rules for DAP2 and the assumption the DDS is valid, testing for
	    // the same name is good enough. The point here is to be sure to only use the
	    // existing maps. This is an important issue when there are multiple Grids in the same
	    // dataset that utilize the same Map arrays data.
        Array *the_map_array;;
        Array *container_map_array = static_cast<Array*>(container->var((*i)->name()));
        if(!container_map_array){
            DBG(cerr << __func__ << "() - No Map Array '" << (*i)->name() << "' present in the current DAP4 container ("<<container->name()<< ":"<<(void*)container<< "). Let's fix that..." << endl;);
            // Not in the container, so we check root group
            Array *root_map_array = static_cast<Array*>(root->var((*i)->name()));
            if (!root_map_array) {
                // Not in the root group so we transform a new array and add it to container.
                DBG(cerr << __func__ << "() - No Map Array '" << (*i)->name() << "' present in the root Group ("<<root->name()<< ":"<<(void*)root<< "). Let's fix that..." << endl;);
                // transform it and add it to the container
                (*i)->transform_to_dap4(root, container);
                // Recover the new dap4 version from the container.
                the_map_array = static_cast<Array*>(container->var((*i)->name()));
                DBG(cerr << __func__ << "() - Transformed array '"<< the_map_array->name() <<
                    "' to DAP4 Array (" << (void *) the_map_array << ") added to container: '"<<
                    container.name() <<"'" << endl;);
            }
            else {
                the_map_array = root_map_array;
                DBG(cerr << __func__ << "() - Located Map Array '" << the_map_array->name() << "' (" <<
                    (void *) the_map_array << ") present in the root group ("<<root->name()<< ":"<<(void*)root <<
                    "). Let's fix that..." << endl;);
            }
        }
        else {
            the_map_array = container_map_array;
            DBG(cerr << __func__ << "() - Located Map Array '" << the_map_array->name() << "' (" <<
                (void *) the_map_array << ") present in the current DAP4 container ("<<container->name( )<< ":"<<
                (void*)container<< "). Let's fix that..." << endl;);
        }
        // We'll use these (below) to make D4Map objects for the coverage
        d4_map_arrays.push_back(the_map_array);
	}

	// Adds the coverage array to the container.
    array_var()->transform_to_dap4(root, container);
    // Get the new coverage array
    BaseType *btp = container->var(array_var()->name());
    Array *coverage = static_cast<Array*>(btp);
    DBG(cerr << __func__ << "() - Transformed and added DAP4 coverage Array '"<< coverage->name() <<
        "' to parent container: '" << container->name() << "'" << endl;);

    // Add the D4Maps
    vector<Array*>::iterator d4aItr=d4_map_arrays.begin();
    vector<Array*>::iterator end=d4_map_arrays.end();
    for( ; d4aItr!=end ; d4aItr++){
        Array *the_map_array = *d4aItr;
        // Here we use the Map Array that we saved the Map
        // name and Map Array reference for our map.
        D4Map *d4_map = new D4Map(the_map_array->FQN(), the_map_array, coverage); // bind the 'map' to the coverage
        coverage->maps()->add_map(d4_map);    // bind the coverage to the map
        // Clear the vector entry to ensure that ~Array doesn't
        // get called when the (stack declared) vector goes out of scope.
        *d4aItr = 0;
        DBG(cerr << __func__ << "() - Added DAP4 Map Array:  '"<< d4_map->name() <<
            "' (" << (void *) d4_map->array() << ") to coverage: '" << coverage->name() << "'" << endl;);

    }
    DBG(cerr << __func__ << "() - END (grid:" << name() << ")" << endl;);
}


/**
 * Grid can only be used for DAP2.
 * @note This might change depending on just how complex DAP4Array becomes,
 * for example.
 */
bool
Grid::is_dap2_only_type()
{
    return true;
}

/** Add an array or map to the Grid.

    The original version of this method required that the \c part parameter
    be present. However, this complicates using the class from a parser
    (e.g., the schema-based XML parser). I have modified the method so that
    if \c part is nil (the default), then the first variable added is the
    array and subsequent variables are maps. This matches the behavior in the
    Java DAP implementation.

    @param bt Array or Map variable
    @param part is this an array or a map. If not present, first \c bt is the
    array and subsequent <tt>bt</tt>s are maps. */
void
Grid::add_var(BaseType *bt, Part part)
{
    if (!bt)
        throw InternalErr(__FILE__, __LINE__, "Passing NULL pointer as variable to be added.");

    if (part == array && d_is_array_set/*get_array()*/) {
      // Avoid leaking memory...  Function is add, not set, so it is an error to call again for the array part.
      throw InternalErr(__FILE__, __LINE__, "Error: Grid::add_var called with part==Array, but the array was already set!");
    }

    // avoid obvious broken semantics
    if (!dynamic_cast<Array*>(bt)) {
      throw InternalErr(__FILE__, __LINE__, "Grid::add_var(): object is not an Array!");
    }

    // Set to the clone of bt if we get that far.
    BaseType* bt_clone = 0;

    switch (part) {

    case array: {
        // Add it as a copy to preserve old semantics.  This sets parent too.
        bt_clone = bt->ptr_duplicate();
        set_array(static_cast<Array*>(bt_clone));
    }
    break;

    case maps: {
            bt_clone = bt->ptr_duplicate();
            bt_clone->set_parent(this);
            d_vars.push_back(bt_clone);
        }
    break;

    default: {
        if (!d_is_array_set/*!d_array_var*/) {
            // Add it as a copy to preserve old semantics.  This sets parent too.
            bt_clone = bt->ptr_duplicate();
            set_array(static_cast<Array*>(bt_clone));
        }
        else {
            bt_clone = bt->ptr_duplicate();
            bt_clone->set_parent(this);
            d_vars.push_back(bt_clone);
        }
    }
    break;
  }
}

/** Add an array or map to the Grid.

    @note The original version of this method required that the \c part parameter
    be present. However, this complicates using the class from a parser
    (e.g., the schema-based XML parser). I have modified the method so that
    if \c part is nil (the default), then the first variable added is the
    array and subsequent variables are maps. This matches the behavior in the
    Java DAP implementation.

    @note This version of the method does not the BaseType before adding it.
    The caller must not free the BaseType object.

    @param bt Array or Map variable
    @param part is this an array or a map. If not present, first \c bt is the
    array and subsequent <tt>bt</tt>s are maps. */
void
Grid::add_var_nocopy(BaseType *bt, Part part)
{
    if (!bt)
        throw InternalErr(__FILE__, __LINE__, "Passing NULL pointer as variable to be added.");

    if (part == array && d_is_array_set/*get_array()*/) {
      // Avoid leaking memory...  Function is add, not set, so it is an error to call again for the array part.
      throw InternalErr(__FILE__, __LINE__, "Error: Grid::add_var called with part==Array, but the array was already set!");
    }

    // avoid obvious broken semantics
    if (!dynamic_cast<Array*>(bt)) {
      throw InternalErr(__FILE__, __LINE__, "Grid::add_var(): object is not an Array!");
    }

    bt->set_parent(this);

    switch (part) {

    case array: {
        // Refactored to use new set_array ([mjohnson 11 nov 2009])
        set_array(static_cast<Array*>(bt));
    }
    break;

    case maps: {
    	// FIXME Why is this commented out?
            //bt->set_parent(this);
            d_vars.push_back(bt);
        }
    break;

    default: {
        if (!d_is_array_set/*!get_array()*/) {
            // Refactored to use new set_array ([mjohnson 11 nov 2009])
            // avoid obvious broken semantics
            set_array(static_cast<Array*>(bt));
        }
        else {
            d_vars.push_back(bt);
        }
    }
    break;
  }
}

/**
 * Set the Array part of the Grid to point to the memory
 * p_new_arr.  Grid takes control of the memory (no copy
 * is made).
 * If there already exists an array portion, the old
 * one will be deleted to avoid leaks.
 *
 * @note This code has been modified to use a new storage for
 * the Grid's variables (the storage defined by Constructor).
 *
 * @param p_new_arr  the object to store as the array
 *                   part of the grid.
 */
void Grid::set_array(Array* p_new_arr)
{
	if (!p_new_arr) {
		throw InternalErr(__FILE__, __LINE__, "Grid::set_array(): Cannot set to null!");
	}

	// Make sure not same memory, this would be evil.
	if (p_new_arr == get_array()) {
		return;
	}

	p_new_arr->set_parent(this);

	// Three cases: 1. There are no variables set for this grid at all
	// 2. There are maps but no array
	// 3. There is already an array set (and maybe maps).
	// NB: d_array_var is a weak pointer to the Grid's Array
	if (d_vars.size() == 0) {
		d_vars.push_back(p_new_arr);
	}
	else if (!d_is_array_set/*!d_array_var*/) {
		d_vars.insert(d_vars.begin(), p_new_arr);
	}
	else {
		// clean out old array
		delete get_array();
		d_vars[0] = p_new_arr;
	}

	d_is_array_set = true;
#if 0
	// store the array pointer locally
	d_array_var = p_new_arr;

	// Set the  parent
	d_array_var->set_parent(this);
#endif
}

/**
 * Add the given array p_new_map as a new map
 * vector for the Grid.
 *
 * If add_as_copy, p_new_map will be cloned
 * and the copy added, leaving p_new_map
 * in the control of the caller.
 *
 * If !add_as_copy, p_new_map will be explicitly
 * added as the new map vector.
 *
 * The actual Array* in the Grid will be returned,
 * either the address of the COPY if add_as_copy,
 * else p_new_map itself if !add_as_copy.
 *
 * It is an exception for p_new_map to be null.
 *
 * @param p_new_map  the map we want to add
 * @param add_as_copy whether to add p_new_map
 *             explicitly and take onwership of memory
 *             or to add a clone of it and leave control
 *             to caller.
 * @return the actual object stored in the Grid, whether
 *         p_new_map, or the address of the copy.
 *
 */
Array*
Grid::add_map(Array* p_new_map, bool add_as_copy)
{
  if (!p_new_map)
    throw InternalErr(__FILE__, __LINE__, "Grid::add_map(): cannot have p_new_map null!");

  if (add_as_copy)
    p_new_map = static_cast<Array*>(p_new_map->ptr_duplicate());

  p_new_map->set_parent(this);

  d_vars.push_back(p_new_map);

  // return the one that got put into the Grid.
  return p_new_map;
}

/**
 * Add pMap (or a clone if addAsCopy) to the
 * FRONT of the maps list.  This is needed if
 * we are preserving Grid semantics but want to
 * add a new OUTER dimension, whereas add_map
 * appends to the end making a new INNER dimension.
 * @param p_new_map the map to add or copy and add
 * @param add_copy if true, copy pMap and add the copy.
 * @return The actual memory stored in the Grid,
 *      either pMap (if !add_copy) or the ptr to
 *      the clone (if add_copy).
 */
Array*
Grid::prepend_map(Array* p_new_map, bool add_copy)
{
  if (add_copy)
    {
      p_new_map = static_cast<Array*>(p_new_map->ptr_duplicate());
    }

  p_new_map->set_parent(this);
  d_vars.insert(map_begin(), p_new_map);

  return p_new_map;
}

/** @brief Returns the Grid Array.
    @deprecated
    @see get_array() */
BaseType *
Grid::array_var()
{
    //return d_array_var;
	// FIXME Should really test that the array has not be set; maps might be added first. jhrg 5/9/13
#if 0
	if (d_array_var)
		cerr << "In array_var(), d_array_var holds a " << d_array_var->type_name() << endl;
	else
		cerr << "In array_var(), d_array_var is null" << endl;
#endif
	return d_is_array_set /*d_vars.size() > 0*/ ? *d_vars.begin() : 0;
}

/** @brief Returns the Grid Array.
    This method returns the array using an Array*, so no cast is required.
    @return A pointer to the Grid's (dependent) data array */
Array *
Grid::get_array()
{
    return dynamic_cast<Array*>(array_var());
}

/** @brief Returns an iterator referencing the first Map vector. */
Grid::Map_iter
Grid::map_begin()
{
    // The maps are stored in the second and subsequent elements of the
    // d_var vector<BaseType*> of Constructor _unless_ the Array part
    // has yet to be set. In the latter case, there are only maps in
    // d_vars
    return d_is_array_set/*(d_array_var != 0)*/ ? d_vars.begin() + 1: d_vars.begin();
}

/** Returns an iterator referencing the end of the list of Map vectors.
    It does not reference the last Map vector */
Grid::Map_iter
Grid::map_end()
{
    return d_vars.end();
}

/** @brief Returns an iterator referencing the first Map vector. */
Grid::Map_riter
Grid::map_rbegin()
{
    // see above
    // return d_is_array_set/*(d_array_var != 0)*/ ? d_vars.rbegin() + 1: d_vars.rbegin();
    return d_vars.rbegin();
}

/** Returns an iterator referencing the end of the list of Map vectors.
    It does not reference the last Map vector */
Grid::Map_riter
Grid::map_rend()
{
    return d_is_array_set ? d_vars.rend() - 1: d_vars.rend();
}

/** Return the iterator for the \e ith map.
    @param i the index
    @return The corresponding  Vars_iter */
Grid::Map_iter
Grid::get_map_iter(int i)
{
    // return map_begin() + i;
    return d_is_array_set ? map_begin() + 1 + i : map_begin() + i;
}

/** Returns the number of components in the Grid object.  This is
    equal to one plus the number of Map vectors.  If there is a
    constraint expression in effect, the number of dimensions needed
    may be smaller than the actual number in the stored data.  (Or
    the Array might not even be requested.) In this case, a user can
    request the smaller number with the <i>constrained</i> flag.

    @brief Returns the number of components in the Grid object.
    @return The number of components in the Grid object.
    @param constrained If TRUE, the function returns the number of
    components contained in the constrained Grid.  Since a
    constraint expression might well eliminate one or more of the
    Grid dimensions, this number can be lower than the actual number
    of components.  If this parameter is FALSE (the default), the
    actual number of components will be returned.  */
int
Grid::components(bool constrained)
{
    int comp;

    if (constrained) {
        comp = get_array()->send_p() ? 1 : 0;

        for (Map_iter i = map_begin(); i != map_end(); i++) {
            if ((*i)->send_p()) {
                comp++;
            }
        }
    }
    else {
        comp = d_vars.size();
    }

    return comp;
}

void Grid::transfer_attributes(AttrTable *at_container)
{
	AttrTable *at = at_container->get_attr_table(name());

	if (at) {
		at->set_is_global_attribute(false);

		array_var()->transfer_attributes(at);

		Map_iter map = map_begin();
		while (map != map_end()) {
			(*map)->transfer_attributes(at);
			map++;
		}

		// Trick: If an attribute that's within the container 'at' still has its
		// is_global_attribute property set, then it's not really a global attr
		// but instead an attribute that belongs to this Grid.
		AttrTable::Attr_iter at_p = at->attr_begin();
		while (at_p != at->attr_end()) {
			if (at->is_global_attribute(at_p)) {
				if (at->get_attr_type(at_p) == Attr_container)
					get_attr_table().append_container(new AttrTable(*at->get_attr_table(at_p)), at->get_name(at_p));
				else
					get_attr_table().append_attr(at->get_name(at_p), at->get_type(at_p), at->get_attr_vector(at_p));
			}

			at_p++;
		}
	}
}

// When projected (using whatever the current constraint provides in the way
// of a projection), is the object still a Grid?

/** Returns TRUE if the current projection will yield a Grid that
    will pass the <tt>check_semantics()</tt> function. A Grid that, when
    projected, will not pass the <tt>check_semantics()</tt> function must
    be sent as either a Structure of Arrays or a single Array
    depending on the projection.

    The function first checks to see whether the Array is present.
    Then, for each dimension in the Array part, the function checks
    the corresponding Map vector to make sure it is present in the
    projected Grid. If for each projected dimension in the Array
    component, there is a matching Map vector, then the Grid is
    valid.

    @return TRUE if the projected grid is still a Grid.  FALSE
    otherwise.
*/
bool
Grid::projection_yields_grid()
{
    // For each dimension in the Array part, check the corresponding Map
    // vector to make sure it is present in the projected Grid. If for each
    // projected dimension in the Array component, there is a matching Map
    // vector, then the Grid is valid.
    bool valid = true;
    Array *a = get_array();

    // Don't bother checking if the Array component is not included.
    if (!a->send_p())
        return false;

    // If only one part is being sent, it's clearly not a grid (it must be
    // the array part of the Grid that's being sent (given that the above
    // test passed and the array is being sent).
    if (components(true) == 1)
    	return false;

    Array::Dim_iter d = a->dim_begin() ;
    Map_iter m = map_begin() ;

    while (valid && d != a->dim_end() && m != map_end()) {
	Array &map = dynamic_cast<Array&>(**m);
        if (a->dimension_size(d, true) && map.send_p()) {
            // Check the matching Map vector; the Map projection must equal
            // the Array dimension projection
            Array::Dim_iter fd = map.dim_begin(); // Maps have only one dim!
            valid = map.dimension_start(fd, true) == a->dimension_start(d, true)
                    && map.dimension_stop(fd, true) == a->dimension_stop(d, true)
                    && map.dimension_stride(fd, true) == a->dimension_stride(d, true);
        }
        else {
           valid = false;
        }

	d++, m++;
    }

    return valid;
}

/** For each of the Array and Maps in this Grid, call clear_constraint(). */
void
Grid::clear_constraint()
{
    get_array()->clear_constraint();
    for (Map_iter m = map_begin(); m != map_end(); ++m)
        dynamic_cast<Array&>(*(*m)).clear_constraint();
}

void
Grid::print_decl(FILE *out, string space, bool print_semi,
                 bool constraint_info, bool constrained)
{
    ostringstream oss;
    print_decl(oss, space, print_semi, constraint_info, constrained);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void
Grid::print_decl(ostream &out, string space, bool print_semi,
                 bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
        return;

    // See comment for the FILE* version of this method.
    if (constrained && !projection_yields_grid()) {
	out << space << "Structure {\n" ;

        get_array()->print_decl(out, space + "    ", true, constraint_info,
                               constrained);

        for (Map_citer i = map_begin(); i != map_end(); i++) {
            (*i)->print_decl(out, space + "    ", true,
                             constraint_info, constrained);
        }

	out << space << "} " << id2www(name()) ;
    }
    else {
        // The number of elements in the (projected) Grid must be such that
        // we have a valid Grid object; send it as such.
	out << space << type_name() << " {\n" ;

	out << space << "  Array:\n" ;
        get_array()->print_decl(out, space + "    ", true, constraint_info,
                               constrained);

	out << space << "  Maps:\n" ;
        for (Map_citer i = map_begin(); i != map_end(); i++) {
            (*i)->print_decl(out, space + "    ", true,
                             constraint_info, constrained);
        }

	out << space << "} " << id2www(name()) ;
    }

    if (constraint_info) {
        if (send_p())
            out << ": Send True";
        else
            out << ": Send False";
    }

    if (print_semi)
	out << ";\n" ;

    return;
}

/**
 * @deprecated
 */
void
Grid::print_xml(FILE *out, string space, bool constrained)
{
    XMLWriter xml(space);
    print_xml_writer(xml, constrained);
    fwrite(xml.get_doc(), sizeof(char), xml.get_doc_size(), out);
}

/**
 * @deprecated
 */
void
Grid::print_xml(ostream &out, string space, bool constrained)
{
    XMLWriter xml(space);
    print_xml_writer(xml, constrained);
    out << xml.get_doc();
}


class PrintGridFieldXMLWriter : public unary_function<BaseType *, void>
{
    XMLWriter &d_xml;
    bool d_constrained;
    string d_tag;
public:
    PrintGridFieldXMLWriter(XMLWriter &x, bool c, const string &t = "Map")
            : d_xml(x), d_constrained(c), d_tag(t)
    {}

    void operator()(BaseType *btp)
    {
        Array *a = dynamic_cast<Array*>(btp);
        if (!a)
            throw InternalErr(__FILE__, __LINE__, "Expected an Array.");
        a->print_xml_writer_core(d_xml, d_constrained, d_tag);
    }
};

void
Grid::print_xml_writer(XMLWriter &xml, bool constrained)
{
    if (constrained && !send_p())
        return;

    if (constrained && !projection_yields_grid()) {
        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"Structure") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write Structure element");

        if (!name().empty())
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)name().c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

        get_attr_table().print_xml_writer(xml);

        get_array()->print_xml_writer(xml, constrained);

        for_each(map_begin(), map_end(),
                 PrintGridFieldXMLWriter(xml, constrained, "Array"));

        if (xmlTextWriterEndElement(xml.get_writer()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not end Structure element");
    }
    else {
        // The number of elements in the (projected) Grid must be such that
        // we have a valid Grid object; send it as such.
        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"Grid") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write Grid element");

        if (!name().empty())
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)name().c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

        get_attr_table().print_xml_writer(xml);

        get_array()->print_xml_writer(xml, constrained);

        for_each(map_begin(), map_end(),
                 PrintGridFieldXMLWriter(xml, constrained, "Map"));

        if (xmlTextWriterEndElement(xml.get_writer()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not end Grid element");
    }
}

void
Grid::print_val(FILE *out, string space, bool print_decl_p)
{
    ostringstream oss;
    print_val(oss, space, print_decl_p);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void Grid::print_val(ostream &out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
        out << " = ";
    }

    // If we are printing a value on the client-side, projection_yields_grid
    // should not be called since we don't *have* a projection without a
    // Constraint. I think that if we are here and send_p() is not true, then
    // the value of this function should be ignored. 4/6/2000 jhrg
    bool pyg = projection_yields_grid(); // hack 12/1/99 jhrg
    if (pyg || !send_p())
        out << "{  Array: ";
    else
        out << "{";

    get_array()->print_val(out, "", false);

    if (pyg || !send_p()) out << "  Maps: ";

    for (Map_citer i = map_begin(); i != map_end(); i++, (void) (i != map_end() && out << ", ")) {
        (*i)->print_val(out, "", false);
    }

    out << " }";

    if (print_decl_p) out << ";\n";
}

// Grids have ugly semantics.

/** @brief Return true if this Grid is well formed.

    The array dimensions and number of map vectors must match and
    both the array and maps must be of simple-type elements. */
bool
Grid::check_semantics(string &msg, bool all)
{
    if (!BaseType::check_semantics(msg))
        return false;

    msg = "";

    if (!get_array()) {
        msg += "Null grid base array in `" + name() + "'\n";
        return false;
    }

    // Is it an array?
    if (get_array()->type() != dods_array_c) {
        msg += "Grid `" + name() + "'s' member `" + get_array()->name() + "' must be an array\n";
        return false;
    }

    Array *av = (Array *)get_array(); // past test above, must be an array

    // Array must be of a simple_type.
    if (!av->var()->is_simple_type()) {
        msg += "The field variable `" + this->name() + "' must be an array of simple type elements (e.g., int32, String)\n";
        return false;
    }

    // enough maps?
    if ((unsigned)d_vars.size()-1 != av->dimensions()) {
        msg += "The number of map variables for grid `" + this->name() + "' does not match the number of dimensions of `";
        msg += av->name() + "'\n";
        return false;
    }

    const string array_var_name = av->name();
    Array::Dim_iter asi = av->dim_begin() ;
    for (Map_iter mvi = map_begin(); mvi != map_end(); mvi++, asi++) {

        BaseType *mv = *mvi;

        // check names
        if (array_var_name == mv->name()) {
            msg += "Grid map variable `" + mv->name() + "' conflicts with the grid array name in grid `" + name() + "'\n";
            return false;
        }
        // check types
        if (mv->type() != dods_array_c) {
            msg += "Grid map variable  `" + mv->name() + "' is not an array\n";
            return false;
        }

        Array *mv_a = (Array *)mv; // downcast to (Array *)

        // Array must be of a simple_type.
        if (!mv_a->var()->is_simple_type()) {
            msg += "The field variable `" + this->name() + "' must be an array of simple type elements (e.g., int32, String)\n";
            return false;
        }

        // check shape
        if (mv_a->dimensions() != 1) {// maps must have one dimension
            msg += "Grid map variable  `" + mv_a->name() + "' must be only one dimension\n";
            return false;
        }
        // size of map must match corresponding array dimension
        Array::Dim_iter mv_asi = mv_a->dim_begin() ;
        int mv_a_size = mv_a->dimension_size(mv_asi) ;
        int av_size = av->dimension_size(asi) ;
        if (mv_a_size != av_size) {
            msg += "Grid map variable  `" + mv_a->name() + "'s' size does not match the size of array variable '";
            msg += get_array()->name() + "'s' cooresponding dimension\n";
            return false;
        }
    }

    if (all) {
        if (!get_array()->check_semantics(msg, true))
            return false;
        for (Map_iter mvi = map_begin(); mvi != map_end(); mvi++) {
            if (!(*mvi)->check_semantics(msg, true)) {
                return false;
            }
        }
    }

    return true;
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
Grid::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Grid::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    Constructor::dump(strm) ;

    DapIndent::UnIndent() ;
}

} // namespace libdap

