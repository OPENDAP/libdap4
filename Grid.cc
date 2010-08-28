
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

// implementation for Grid.
//
// jhrg 9/15/94

#include "config.h"

// #define DODS_DEBUG

#include <functional>
#include <algorithm>

#include "Grid.h"
#include "DDS.h"
#include "Array.h"  // for downcasts
#include "util.h"
#include "InternalErr.h"
#include "escaping.h"

#include "debug.h"

using namespace std;

namespace libdap {

void
Grid::_duplicate(const Grid &s)
{
    // Clear out any spurious vars in Constructor::_vars
    _vars.clear(); // [mjohnson 10 Sep 2009]

    _array_var = s._array_var->ptr_duplicate();
    _array_var->set_parent(this);
    _vars.push_back(_array_var); // so the Constructor::Vars_Iter sees it [mjohnson 10 Sep 2009]

    Grid &cs = const_cast<Grid &>(s);

    for (Map_iter i = cs._map_vars.begin(); i != cs._map_vars.end(); i++) {
        BaseType *btp = (*i)->ptr_duplicate();
        btp->set_parent(this);
        _map_vars.push_back(btp);
        _vars.push_back(btp); // push all map vectors as weak refs into super::_vars which won't delete them [mjohnson 10 Sep 2009]
    }

}

/** The Grid constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications.

    @param n A string containing the name of the variable to be
    created.

    @brief The Grid constructor.
*/
Grid::Grid(const string &n) : Constructor(n, dods_grid_c), _array_var(0)
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
    : Constructor(n, d, dods_grid_c), _array_var(0)
{}

/** @brief The Grid copy constructor. */
Grid::Grid(const Grid &rhs) : Constructor(rhs)
{
    _duplicate(rhs);
}

Grid::~Grid()
{
    delete _array_var; _array_var = 0;

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
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

    delete _array_var; _array_var = 0;

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
        BaseType *btp = *i ;
        delete btp ;
    }

    // this doesn't copy Constructor::_vars so...
    dynamic_cast<Constructor &>(*this) = rhs;

    // we do it in here...
    _duplicate(rhs);

    return *this;
}

int
Grid::element_count(bool leaves)
{
    if (!leaves)
        return _map_vars.size() + 1;
    else {
        int i = 0;
        for (Map_iter j = _map_vars.begin(); j != _map_vars.end(); j++) {
            j += (*j)->element_count(leaves);
        }

		if (!get_array())
			throw InternalErr(__FILE__, __LINE__, "No Grid arry!");

        i += get_array()->element_count(leaves);
        return i;
    }
}

void
Grid::set_send_p(bool state)
{
    _array_var->set_send_p(state);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
        (*i)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Grid::set_read_p(bool state)
{
    _array_var->set_read_p(state);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
        (*i)->set_read_p(state);
    }

    BaseType::set_read_p(state);
}

void
Grid::set_in_selection(bool state)
{
    _array_var->set_in_selection(state);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
        (*i)->set_in_selection(state);
    }

    BaseType::set_in_selection(state);
}

unsigned int
Grid::width()
{
    unsigned int sz = _array_var->width();

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
        sz += (*i)->width();
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

    if (_array_var->send_p())
        _array_var->intern_data(eval, dds);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
        if ((*i)->send_p()) {
            (*i)->intern_data(eval, dds);
        }
    }
}

bool
Grid::serialize(ConstraintEvaluator &eval, DDS &dds,
                Marshaller &m, bool ce_eval)
{
    dds.timeout_on();

    // Re ticket 560: Get an object from eval that describes how to sample
    // and rearrange the data, then perform those actions. Alternative:
    // implement this as a selection function.

    if (!read_p())
        read();  // read() throws Error and InternalErr

#if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#endif

    dds.timeout_off();

    if (_array_var->send_p())
        _array_var->serialize(eval, dds, m, false);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
        if ((*i)->send_p()) {
            (*i)->serialize(eval, dds, m, false);
        }
    }

    return true;
}

bool
Grid::deserialize(UnMarshaller &um, DDS *dds, bool reuse)
{
    _array_var->deserialize(um, dds, reuse);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
        (*i)->deserialize(um, dds, reuse);
    }

    return false;
}

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

    if (_array_var->name() == name) {
        if (s)
            s->push(static_cast<BaseType *>(this));
        return _array_var;
    }

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
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
    if (!bt) {
        throw InternalErr(__FILE__, __LINE__,
                          "Passing NULL pointer as variable to be added.");
    }

    if (part == array && _array_var) {
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
            _map_vars.push_back(bt_clone);
        }
    break;

    default: {
        if (!_array_var) {
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
            _map_vars.push_back(bt_clone);
        }
    }
    break;
  }// switch

  // if we get ehre without exception, add the cloned object to the superclass variable iterator
  // mjohnson 10 Sep 2009
  // Add it to the superclass _vars list so we can iterate on superclass vars
  if (bt_clone) {
    _vars.push_back(bt_clone);
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
  if (p_new_arr == _array_var) {
      return;
   }
  // clean out any old array
  delete _array_var; _array_var = 0;
  // Set the new, with parent
  _array_var = p_new_arr;
  _array_var->set_parent(this);
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
  if (!p_new_map) {
    throw InternalErr(__FILE__, __LINE__,
        "Grid::add_map(): cannot have p_new_map null!");
  }

  if (add_as_copy) {
    p_new_map = static_cast<Array*>(p_new_map->ptr_duplicate());
  }

  p_new_map->set_parent(this);
  _map_vars.push_back(p_new_map);
  _vars.push_back(p_new_map); // allow superclass iter to work as well.

  // return the one that got put into the Grid.
  return p_new_map;
}

/**
 * Add pMap (or a clone if addAsCopy) to the
 * FRONT of the maps list.  This is needed if
 * we are preserving Grid semantics but want to
 * add a new OUTER dimension, whereas add_map
 * appends to the end making a new INNER dimension.
 * @param pMap the map to add or copy and add
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
  _map_vars.insert(_map_vars.begin(), p_new_map);
  _vars.insert(_vars.begin(), p_new_map); // allow superclass iter to work as well.

   // return the one that got put into the Grid.
   return p_new_map;
}

/** @brief Returns the Grid Array.
    @deprecated
    @see get_array() */
BaseType *
Grid::array_var()
{
    return _array_var;
}

/** @brief Returns the Grid Array.
    This method returns the array using an Array*, so no cast is required.
    @return A pointer to the Grid's (dependent) data array */
Array *
Grid::get_array()
{
    Array *a = dynamic_cast<Array*>(_array_var);
    if (a)
        return a;
    else
        throw InternalErr(__FILE__, __LINE__, "bad Cast");
}

/** @brief Returns an iterator referencing the first Map vector. */
Grid::Map_iter
Grid::map_begin()
{
    return _map_vars.begin() ;
}

/** Returns an iterator referencing the end of the list of Map vectors.
    It does not reference the last Map vector */
Grid::Map_iter
Grid::map_end()
{
    return _map_vars.end() ;
}

/** @brief Returns an iterator referencing the first Map vector. */
Grid::Map_riter
Grid::map_rbegin()
{
    return _map_vars.rbegin() ;
}

/** Returns an iterator referencing the end of the list of Map vectors.
    It does not reference the last Map vector */
Grid::Map_riter
Grid::map_rend()
{
    return _map_vars.rend() ;
}

/** Return the iterator for the \e ith map.
    @param i the index
    @return The corresponding  Vars_iter */
Grid::Map_iter
Grid::get_map_iter(int i)
{
    return _map_vars.begin() + i;
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
        comp = _array_var->send_p() ? 1 : 0;

        for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
            if ((*i)->send_p()) {
                comp++;
            }
        }
    }
    else {
        comp = 1 + _map_vars.size();
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
    Array *a = (Array *)_array_var;

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
    dynamic_cast<Array&>(*_array_var).clear_constraint();
    for (Map_iter m = map_begin(); m != map_end(); ++m)
        dynamic_cast<Array&>(*(*m)).clear_constraint();
}

#if FILE_METHODS
void
Grid::print_decl(FILE *out, string space, bool print_semi,
                 bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
        return;

    // The problem with the above is that if two Grids are projected and each
    // contain one variable, say a map, and it happens to have the same name
    // in each Grid, then without the enclosing Structures, the returned dataset
    // has two variables with the same name at the same lexical level. So I'm
    // removing the code above.
    if (constrained && !projection_yields_grid()) {
        fprintf(out, "%sStructure {\n", space.c_str()) ;

        _array_var->print_decl(out, space + "    ", true, constraint_info,
                               constrained);

        for (Map_citer i = _map_vars.begin(); i != _map_vars.end(); i++) {
            (*i)->print_decl(out, space + "    ", true,
                             constraint_info, constrained);
        }

        fprintf(out, "%s} %s", space.c_str(), id2www(name()).c_str()) ;
    }
    else {
        // The number of elements in the (projected) Grid must be such that
        // we have a valid Grid object; send it as such.
        fprintf(out, "%s%s {\n", space.c_str(), type_name().c_str()) ;

        fprintf(out, "%s  Array:\n", space.c_str()) ;
        _array_var->print_decl(out, space + "    ", true, constraint_info,
                               constrained);

        fprintf(out, "%s  Maps:\n", space.c_str()) ;
        for (Map_citer i = _map_vars.begin(); i != _map_vars.end(); i++) {
            (*i)->print_decl(out, space + "    ", true,
                             constraint_info, constrained);
        }

        fprintf(out, "%s} %s", space.c_str(), id2www(name()).c_str()) ;
    }

    if (constraint_info) {
        if (send_p())
            fprintf( out, ": Send True");
        else
            fprintf( out, ": Send False");
    }

    if (print_semi)
        fprintf(out, ";\n") ;

    return;
}
#endif

void
Grid::print_decl(ostream &out, string space, bool print_semi,
                 bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
        return;

    // See comment for the FILE* version of this method.
    if (constrained && !projection_yields_grid()) {
	out << space << "Structure {\n" ;

        _array_var->print_decl(out, space + "    ", true, constraint_info,
                               constrained);

        for (Map_citer i = _map_vars.begin(); i != _map_vars.end(); i++) {
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
        _array_var->print_decl(out, space + "    ", true, constraint_info,
                               constrained);

	out << space << "  Maps:\n" ;
        for (Map_citer i = _map_vars.begin(); i != _map_vars.end(); i++) {
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

#if FILE_METHODS
class PrintMapField : public unary_function<BaseType *, void>
{
    FILE *d_out;
    string d_space;
    bool d_constrained;
    string d_tag;
public:
    PrintMapField(FILE *o, string s, bool c, const string &t = "Map")
            : d_out(o), d_space(s), d_constrained(c), d_tag(t)
    {}

    void operator()(BaseType *btp)
    {
        Array *a = dynamic_cast<Array*>(btp);
        if (!a)
            throw InternalErr(__FILE__, __LINE__, "Expected an Array.");
        a->print_xml_core(d_out, d_space, d_constrained, d_tag);
    }
};

void
Grid::print_xml(FILE *out, string space, bool constrained)
{
    if (constrained && !send_p())
         return;

     if (constrained && !projection_yields_grid()) {
         fprintf(out, "%s<Structure", space.c_str());
         if (!name().empty())
             fprintf(out, " name=\"%s\"", id2xml(name()).c_str());

         fprintf(out, ">\n");

         get_attr_table().print_xml(out, space + "    ", constrained);

         get_array()->print_xml(out, space + "    ", constrained);

         for_each(map_begin(), map_end(),
                  PrintMapField(out, space + "    ", constrained, "Array"));

         fprintf(out, "%s</Structure>\n", space.c_str());
     }
     else {
         // The number of elements in the (projected) Grid must be such that
         // we have a valid Grid object; send it as such.
         fprintf(out, "%s<Grid", space.c_str());
         if (!name().empty())
             fprintf(out, " name=\"%s\"", id2xml(name()).c_str());

         fprintf(out, ">\n");

         get_attr_table().print_xml(out, space + "    ", constrained);

         get_array()->print_xml(out, space + "    ", constrained);

         for_each(map_begin(), map_end(),
                  PrintMapField(out, space + "    ", constrained));

         fprintf(out, "%s</Grid>\n", space.c_str());
     }
}
#endif

class PrintMapFieldStrm : public unary_function<BaseType *, void>
{
    ostream &d_out;
    string d_space;
    bool d_constrained;
    string d_tag;
public:
    PrintMapFieldStrm(ostream &o, string s, bool c, const string &t = "Map")
            : d_out(o), d_space(s), d_constrained(c), d_tag(t)
    {}

    void operator()(BaseType *btp)
    {
        Array *a = dynamic_cast<Array*>(btp);
        if (!a)
            throw InternalErr(__FILE__, __LINE__, "Expected an Array.");
        a->print_xml_core(d_out, d_space, d_constrained, d_tag);
    }
};

void
Grid::print_xml(ostream &out, string space, bool constrained)
{
    if (constrained && !send_p())
        return;

    if (constrained && !projection_yields_grid()) {
        out << space << "<Structure" ;
        if (!name().empty())
            out << " name=\"" << id2xml(name()) << "\"" ;

        out << ">\n" ;

        get_attr_table().print_xml(out, space + "    ", constrained);

        get_array()->print_xml(out, space + "    ", constrained);

        for_each(map_begin(), map_end(),
                 PrintMapFieldStrm(out, space + "    ", constrained, "Array"));

        out << space << "</Structure>\n" ;
    }
    else {
        // The number of elements in the (projected) Grid must be such that
        // we have a valid Grid object; send it as such.
        out << space << "<Grid" ;
        if (!name().empty())
            out << " name=\"" << id2xml(name()) << "\"" ;

        out << ">\n" ;

        get_attr_table().print_xml(out, space + "    ", constrained);

        get_array()->print_xml(out, space + "    ", constrained);

        for_each(map_begin(), map_end(),
                 PrintMapFieldStrm(out, space + "    ", constrained));

        out << space << "</Grid>\n" ;
    }
}

#if FILE_METHODS
void
Grid::print_val(FILE *out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
        fprintf(out, " = ") ;
    }

    // If we are printing a value on the client-side, projection_yields_grid
    // should not be called since we don't *have* a projection without a
    // contraint. I think that if we are here and send_p() is not true, then
    // the value of this function should be ignored. 4/6/2000 jhrg
    bool pyg = projection_yields_grid(); // hack 12/1/99 jhrg
    if (pyg || !send_p())
        fprintf(out, "{  Array: ") ;
    else
        fprintf(out, "{") ;
    _array_var->print_val(out, "", false);
    if (pyg || !send_p())
        fprintf(out, "  Maps: ") ;
    for (Map_citer i = _map_vars.begin(); i != _map_vars.end();
         i++, (void)(i != _map_vars.end() && fprintf(out, ", "))) {
        (*i)->print_val(out, "", false);
    }
    fprintf(out, " }") ;

    if (print_decl_p)
        fprintf(out, ";\n") ;
}
#endif

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
    _array_var->print_val(out, "", false);
    if (pyg || !send_p())
	out << "  Maps: " ;
    for (Map_citer i = _map_vars.begin(); i != _map_vars.end();
         i++, (void)(i != _map_vars.end() && out << ", ")) {
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

    if (!_array_var) {
        msg += "Null grid base array in `" + name() + "'\n";
        return false;
    }

    // Is it an array?
    if (_array_var->type() != dods_array_c) {
        msg += "Grid `" + name() + "'s' member `" + _array_var->name() + "' must be an array\n";
        return false;
    }

    Array *av = (Array *)_array_var; // past test above, must be an array

    // Array must be of a simple_type.
    if (!av->var()->is_simple_type()) {
        msg += "The field variable `" + this->name() + "' must be an array of simple type elements (e.g., int32, String)\n";
        return false;
    }

    // enough maps?
    if ((unsigned)_map_vars.size() != av->dimensions()) {
        msg += "The number of map variables for grid `" + this->name() + "' does not match the number of dimensions of `";
        msg += av->name() + "'\n";
        return false;
    }

    const string array_var_name = av->name();
    Array::Dim_iter asi = av->dim_begin() ;
    for (Map_iter mvi = _map_vars.begin();
         mvi != _map_vars.end(); mvi++, asi++) {

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
            msg += _array_var->name() + "'s' cooresponding dimension\n";
            return false;
        }
    }

    if (all) {
        if (!_array_var->check_semantics(msg, true))
            return false;
        for (Map_iter mvi = _map_vars.begin(); mvi != _map_vars.end(); mvi++) {
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
    if (_array_var) {
        strm << DapIndent::LMarg << "array var: " << endl ;
        DapIndent::Indent() ;
        _array_var->dump(strm) ;
        DapIndent::UnIndent() ;
    }
    else {
        strm << DapIndent::LMarg << "array var: null" << endl ;
    }
    strm << DapIndent::LMarg << "map var: " << endl ;
    DapIndent::Indent() ;
    Map_citer i = _map_vars.begin() ;
    Map_citer ie = _map_vars.end() ;
    for (; i != ie; i++) {
        (*i)->dump(strm) ;
    }
    DapIndent::UnIndent() ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

