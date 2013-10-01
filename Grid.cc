
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

using namespace std;

namespace libdap {

void
Grid::m_duplicate(const Grid &s)
{
    // Clear out any spurious vars in Constructor::d_vars
    d_vars.clear(); // [mjohnson 10 Sep 2009]

    d_array_var = s.d_array_var->ptr_duplicate();
    d_array_var->set_parent(this);
    d_vars.push_back(d_array_var); // so the Constructor::Vars_Iter sees it [mjohnson 10 Sep 2009]

    Grid &cs = const_cast<Grid &>(s);

    for (Map_iter i = cs.d_map_vars.begin(); i != cs.d_map_vars.end(); i++) {
        BaseType *btp = (*i)->ptr_duplicate();
        btp->set_parent(this);
        d_map_vars.push_back(btp);
        d_vars.push_back(btp); // push all map vectors as weak refs into super::d_vars which won't delete them [mjohnson 10 Sep 2009]
    }

}

/** The Grid constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications.

    @param n A string containing the name of the variable to be
    created.

    @brief The Grid constructor.
*/
Grid::Grid(const string &n) : Constructor(n, dods_grid_c), d_array_var(0)
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
    : Constructor(n, d, dods_grid_c), d_array_var(0)
{}

/** @brief The Grid copy constructor. */
Grid::Grid(const Grid &rhs) : Constructor(rhs)
{
    m_duplicate(rhs);
}

Grid::~Grid()
{
    delete d_array_var; d_array_var = 0;

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        BaseType *btp = *i ;
        delete btp ; btp = 0;
    }
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

    delete d_array_var; d_array_var = 0;

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        BaseType *btp = *i ;
        delete btp ;
    }

    // this doesn't copy Constructor::d_vars so...
    dynamic_cast<Constructor &>(*this) = rhs;

    // we do it in here...
    m_duplicate(rhs);

    return *this;
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

int
Grid::element_count(bool leaves)
{
    if (!leaves)
        return d_map_vars.size() + 1;
    else {
        int i = 0;
        for (Map_iter j = d_map_vars.begin(); j != d_map_vars.end(); j++) {
            j += (*j)->element_count(leaves);
        }

		if (!get_array())
			throw InternalErr(__FILE__, __LINE__, "No Grid array!");

        i += get_array()->element_count(leaves);
        return i;
    }
}

void
Grid::set_send_p(bool state)
{
    d_array_var->set_send_p(state);

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        (*i)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Grid::set_read_p(bool state)
{
    d_array_var->set_read_p(state);

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        (*i)->set_read_p(state);
    }

    BaseType::set_read_p(state);
}

void
Grid::set_in_selection(bool state)
{
    d_array_var->set_in_selection(state);

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        (*i)->set_in_selection(state);
    }

    BaseType::set_in_selection(state);
}
#if 0
unsigned int
Grid::width(bool)
{
    unsigned int sz = d_array_var->width();

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        sz += (*i)->width();
    }

    return sz;
}
#endif
/** This version of width simply returns the same thing as width() for simple
    types and Arrays. For Structure it returns the total size if constrained
    is false, or the size of the elements in the current projection if true.

    @param constrained If true, return the size after applying a constraint.
    @return  The number of bytes used by the variable.
 */
unsigned int
Grid::width(bool constrained)
{
    unsigned int sz = 0;

    if (constrained) {
    	if (d_array_var->send_p())
    		sz = d_array_var->width(constrained);
    }
    else {
    	sz = d_array_var->width(constrained);
    }

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
    	if (constrained) {
    		if ((*i)->send_p())
    			sz += (*i)->width(constrained);
    	}
    	else {
    		sz += (*i)->width(constrained);
    	}
    }

    return sz;
}

void
Grid::intern_data(ConstraintEvaluator &eval, DDS &dds)
{
    dds.timeout_on();

    if (!read_p())
        read();  // read() throws Error and InternalErr

    dds.timeout_off();

    if (d_array_var->send_p())
        d_array_var->intern_data(eval, dds);

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        if ((*i)->send_p()) {
            (*i)->intern_data(eval, dds);
        }
    }
}

bool
Grid::serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval)
{
    DBG(cerr << "In Grid::serialize()" << endl);

    dds.timeout_on();

    // Re ticket 560: Get an object from eval that describes how to sample
    // and rearrange the data, then perform those actions. Alternative:
    // implement this as a selection function.
    DBG(cerr << "In Grid::serialize(), before read() - read_p() returned: " << read_p() << endl);

    if (!read_p())
        read();  // read() throws Error and InternalErr

    DBG(cerr << "In Grid::serialize(), past read() - read_p() returned: " << read_p() << endl);

    #if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#endif

    dds.timeout_off();

    if (d_array_var->send_p()) {
#ifdef CHECKSUMS
        XDRStreamMarshaller *sm = dynamic_cast<XDRStreamMarshaller*>(&m);
        if (sm && sm->checksums())
            sm->reset_checksum();

        d_array_var->serialize(eval, dds, m, false);

        if (sm && sm->checksums())
            sm->get_checksum();
#else
        DBG(cerr << "About to call Array::serialize() in Grid::serialize" << endl);
        d_array_var->serialize(eval, dds, m, false);
#endif
    }

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        if ((*i)->send_p()) {
#ifdef CHECKSUMS
            XDRStreamMarshaller *sm = dynamic_cast<XDRStreamMarshaller*>(&m);
            if (sm && sm->checksums())
                sm->reset_checksum();

            (*i)->serialize(eval, dds, m, false);

            if (sm && sm->checksums())
                sm->get_checksum();
#else
            (*i)->serialize(eval, dds, m, false);
#endif
        }
    }

    return true;
}

bool
Grid::deserialize(UnMarshaller &um, DDS *dds, bool reuse)
{
    d_array_var->deserialize(um, dds, reuse);

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        (*i)->deserialize(um, dds, reuse);
    }

    return false;
}

#if 0
/** @brief Returns the size of the Grid type.

    Use the <tt>val2buf()</tt>
    functions of the member elements to insert values into the Grid
    buffer.

    @return The size (in bytes) of the value's representation.  */
unsigned int
Grid::val2buf(void *, bool)
{
    return sizeof(Grid);
}

/** Returns the size of the Grid type.  Use the <tt>buf2val()</tt>
    functions of the member elements to read values from the Grid
    buffer. */
unsigned int
Grid::buf2val(void **)
{
    return sizeof(Grid);
}
#endif

BaseType *
Grid::var(const string &n, btp_stack &s)
{
    return var(n, true, &s);
}

/** Note the parameter <i>exact_match</i> is not used by this
    member function.

    @see BaseType */
BaseType *
Grid::var(const string &n, bool, btp_stack *s)
{
    string name = www2id(n);

    if (d_array_var->name() == name) {
        if (s)
            s->push(static_cast<BaseType *>(this));
        return d_array_var;
    }

    for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
        if ((*i)->name() == name) {
            if (s)
                s->push(static_cast<BaseType *>(this));
            return *i;
        }
    }

    return 0;
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
#if 0
    if (bt->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 Grid.");
#endif
    if (part == array && d_array_var) {
      // Avoid leaking memory...  Function is add, not set, so it is an error to call again for the array part.
      throw InternalErr(__FILE__, __LINE__, "Error: Grid::add_var called with part==Array, but the array was already set!");
    }

    // Set to the clone of bt if we get that far.
    BaseType* bt_clone = 0;

    switch (part) {

    case array: {
        // Refactored to use new set_array ([mjohnson 11 nov 2009])
        Array* p_arr = dynamic_cast<Array*>(bt);
        // avoid obvious broken semantics
        if (!p_arr) {
          throw InternalErr(__FILE__, __LINE__,
              "Grid::add_var(): with Part==array: object is not an Array!");
        }
        // Add it as a copy to preserve old semantics.  This sets parent too.
        bt_clone = p_arr->ptr_duplicate();
        set_array(static_cast<Array*>(bt_clone));
    }
    break;

    case maps: {
            bt_clone = bt->ptr_duplicate();
            bt_clone->set_parent(this);
            d_map_vars.push_back(bt_clone);
        }
    break;

    default: {
        if (!d_array_var) {
            // Refactored to use new set_array ([mjohnson 11 nov 2009])
            Array* p_arr = dynamic_cast<Array*>(bt);
            // avoid obvious broken semantics
            if (!p_arr) {
              throw InternalErr(__FILE__, __LINE__,
                  "Grid::add_var(): with Part==array: object is not an Array!");
            }
            // Add it as a copy to preserve old semantics.  This sets parent too.
            bt_clone = p_arr->ptr_duplicate();
            set_array(static_cast<Array*>(bt_clone));
        }
        else {
            bt_clone = bt->ptr_duplicate();
            bt_clone->set_parent(this);
            d_map_vars.push_back(bt_clone);
        }
    }
    break;
  }// switch

  // if we get ehre without exception, add the cloned object to the superclass variable iterator
  // mjohnson 10 Sep 2009
  // Add it to the superclass d_vars list so we can iterate on superclass vars
  if (bt_clone) {
    d_vars.push_back(bt_clone);
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
#if 0
    if (bt->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 Grid.");
#endif
    if (part == array && d_array_var) {
      // Avoid leaking memory...  Function is add, not set, so it is an error to call again for the array part.
      throw InternalErr(__FILE__, __LINE__, "Error: Grid::add_var called with part==Array, but the array was already set!");
    }

    bt->set_parent(this);

    switch (part) {

    case array: {
        // Refactored to use new set_array ([mjohnson 11 nov 2009])
        Array* p_arr = dynamic_cast<Array*>(bt);
        // avoid obvious broken semantics
        if (!p_arr) {
          throw InternalErr(__FILE__, __LINE__,
              "Grid::add_var(): with Part==array: object is not an Array!");
        }
        set_array(static_cast<Array*>(bt));
    }
    break;

    case maps: {
            //bt->set_parent(this);
            d_map_vars.push_back(bt);
        }
    break;

    default: {
        if (!d_array_var) {
            // Refactored to use new set_array ([mjohnson 11 nov 2009])
            Array* p_arr = dynamic_cast<Array*>(bt);
            // avoid obvious broken semantics
            if (!p_arr) {
              throw InternalErr(__FILE__, __LINE__,
                  "Grid::add_var(): with Part==array: object is not an Array!");
            }
            set_array(static_cast<Array*>(bt));
        }
        else {
            d_map_vars.push_back(bt);
        }
    }
    break;
  }// switch

  // if we get here without exception, add the cloned object to the superclass variable iterator
  // mjohnson 10 Sep 2009
  // Add it to the superclass d_vars list so we can iterate on superclass vars
  if (bt) {
    d_vars.push_back(bt);
  }
}

/**
 * Set the Array part of the Grid to point to the memory
 * p_new_arr.  Grid takes control of the memory (no copy
 * is made).
 * If there already exists an array portion, the old
 * one will be deleted to avoid leaks.
 * @param p_new_arr  the object to store as the array
 *                   part of the grid.
 */
void
Grid::set_array(Array* p_new_arr)
{
  if (!p_new_arr) {
    throw InternalErr(__FILE__, __LINE__,
        "Grid::set_array(): Cannot set to null!");
  }
  // Make sure not same memory, this would be evil.
  if (p_new_arr == d_array_var) {
      return;
   }
  // clean out any old array
  delete d_array_var; d_array_var = 0;
  // Set the new, with parent
  d_array_var = p_new_arr;
  d_array_var->set_parent(this);
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
#if 0
  if (p_new_map->is_dap4_only_type())
      throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 Grid.");
#endif
  if (add_as_copy)
    p_new_map = static_cast<Array*>(p_new_map->ptr_duplicate());

  p_new_map->set_parent(this);
  d_map_vars.push_back(p_new_map);
  d_vars.push_back(p_new_map); // allow superclass iter to work as well.

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
  d_map_vars.insert(d_map_vars.begin(), p_new_map);
  d_vars.insert(d_vars.begin(), p_new_map); // allow superclass iter to work as well.

   // return the one that got put into the Grid.
   return p_new_map;
}

/** @brief Returns the Grid Array.
    @deprecated
    @see get_array() */
BaseType *
Grid::array_var()
{
    return d_array_var;
}

/** @brief Returns the Grid Array.
    This method returns the array using an Array*, so no cast is required.
    @return A pointer to the Grid's (dependent) data array */
Array *
Grid::get_array()
{
    Array *a = dynamic_cast<Array*>(d_array_var);
    if (a)
        return a;
    else
        throw InternalErr(__FILE__, __LINE__, "bad Cast");
}

/** @brief Returns an iterator referencing the first Map vector. */
Grid::Map_iter
Grid::map_begin()
{
    return d_map_vars.begin() ;
}

/** Returns an iterator referencing the end of the list of Map vectors.
    It does not reference the last Map vector */
Grid::Map_iter
Grid::map_end()
{
    return d_map_vars.end() ;
}

/** @brief Returns an iterator referencing the first Map vector. */
Grid::Map_riter
Grid::map_rbegin()
{
    return d_map_vars.rbegin() ;
}

/** Returns an iterator referencing the end of the list of Map vectors.
    It does not reference the last Map vector */
Grid::Map_riter
Grid::map_rend()
{
    return d_map_vars.rend() ;
}

/** Return the iterator for the \e ith map.
    @param i the index
    @return The corresponding  Vars_iter */
Grid::Map_iter
Grid::get_map_iter(int i)
{
    return d_map_vars.begin() + i;
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
        comp = d_array_var->send_p() ? 1 : 0;

        for (Map_iter i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
            if ((*i)->send_p()) {
                comp++;
            }
        }
    }
    else {
        comp = 1 + d_map_vars.size();
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
		    get_attr_table().append_container(new AttrTable(
			    *at->get_attr_table(at_p)), at->get_name(at_p));
		else
		    get_attr_table().append_attr(at->get_name(at_p),
			    at->get_type(at_p), at->get_attr_vector(at_p));
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
    Array *a = (Array *)d_array_var;

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
    dynamic_cast<Array&>(*d_array_var).clear_constraint();
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

        d_array_var->print_decl(out, space + "    ", true, constraint_info,
                               constrained);

        for (Map_citer i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
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
        d_array_var->print_decl(out, space + "    ", true, constraint_info,
                               constrained);

	out << space << "  Maps:\n" ;
        for (Map_citer i = d_map_vars.begin(); i != d_map_vars.end(); i++) {
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

void
Grid::print_val(ostream &out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
	out << " = " ;
    }

    // If we are printing a value on the client-side, projection_yields_grid
    // should not be called since we don't *have* a projection without a
    // Constraint. I think that if we are here and send_p() is not true, then
    // the value of this function should be ignored. 4/6/2000 jhrg
    bool pyg = projection_yields_grid(); // hack 12/1/99 jhrg
    if (pyg || !send_p())
	out << "{  Array: " ;
    else
	out << "{" ;
    d_array_var->print_val(out, "", false);
    if (pyg || !send_p())
	out << "  Maps: " ;
    for (Map_citer i = d_map_vars.begin(); i != d_map_vars.end();
         i++, (void)(i != d_map_vars.end() && out << ", ")) {
        (*i)->print_val(out, "", false);
    }
    out << " }" ;

    if (print_decl_p)
	out << ";\n" ;
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

    if (!d_array_var) {
        msg += "Null grid base array in `" + name() + "'\n";
        return false;
    }

    // Is it an array?
    if (d_array_var->type() != dods_array_c) {
        msg += "Grid `" + name() + "'s' member `" + d_array_var->name() + "' must be an array\n";
        return false;
    }

    Array *av = (Array *)d_array_var; // past test above, must be an array

    // Array must be of a simple_type.
    if (!av->var()->is_simple_type()) {
        msg += "The field variable `" + this->name() + "' must be an array of simple type elements (e.g., int32, String)\n";
        return false;
    }

    // enough maps?
    if ((unsigned)d_map_vars.size() != av->dimensions()) {
        msg += "The number of map variables for grid `" + this->name() + "' does not match the number of dimensions of `";
        msg += av->name() + "'\n";
        return false;
    }

    const string array_var_name = av->name();
    Array::Dim_iter asi = av->dim_begin() ;
    for (Map_iter mvi = d_map_vars.begin();
         mvi != d_map_vars.end(); mvi++, asi++) {

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
            msg += d_array_var->name() + "'s' cooresponding dimension\n";
            return false;
        }
    }

    if (all) {
        if (!d_array_var->check_semantics(msg, true))
            return false;
        for (Map_iter mvi = d_map_vars.begin(); mvi != d_map_vars.end(); mvi++) {
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
    if (d_array_var) {
        strm << DapIndent::LMarg << "array var: " << endl ;
        DapIndent::Indent() ;
        d_array_var->dump(strm) ;
        DapIndent::UnIndent() ;
    }
    else {
        strm << DapIndent::LMarg << "array var: null" << endl ;
    }
    strm << DapIndent::LMarg << "map var: " << endl ;
    DapIndent::Indent() ;
    Map_citer i = d_map_vars.begin() ;
    Map_citer ie = d_map_vars.end() ;
    for (; i != ie; i++) {
        (*i)->dump(strm) ;
    }
    DapIndent::UnIndent() ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

