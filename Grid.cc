
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

#include <functional>
#include <algorithm>

#include "Grid.h"
#include "DDS.h"
#include "Array.h"		// for downcasts
#include "util.h"
#include "InternalErr.h"
#include "escaping.h"
//#include "BTIterAdapter.h"


using namespace std;

void
Grid::_duplicate(const Grid &s)
{
    _array_var = s._array_var->ptr_duplicate();
    _array_var->set_parent(this);

    Grid &cs = const_cast<Grid &>(s);

    for (Map_iter i = cs._map_vars.begin(); i != cs._map_vars.end(); i++)
    {
	BaseType *btp = (*i)->ptr_duplicate();
	btp->set_parent(this);
	_map_vars.push_back(btp);
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
{
}

/** @brief The Grid copy constructor. */
Grid::Grid(const Grid &rhs) : Constructor(rhs)
{
    _duplicate(rhs);
}

Grid::~Grid()
{
    delete _array_var; _array_var = 0;

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
    {
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

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
    {
	BaseType *btp = *i ;
	delete btp ;
    }

    dynamic_cast<Constructor &>(*this) = rhs;

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
	for (Map_iter j = _map_vars.begin(); j != _map_vars.end(); j++)
	{
	    j += (*j)->element_count(leaves);
	}

	i += array_var()->element_count(leaves);
	return i;
    }
}

void
Grid::set_send_p(bool state)
{
    _array_var->set_send_p(state);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
    {
	(*i)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Grid::set_read_p(bool state)
{
    _array_var->set_read_p(state);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
    {
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
  
    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
    {
	sz += (*i)->width();
    }
  
    return sz;
}

bool
Grid::serialize(const string &dataset, ConstraintEvaluator &eval, DDS &dds,
                XDR *sink, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
	read(dataset);		// read() throws Error and InternalErr

    if (ce_eval && !eval.eval_selection(dds, dataset))
	return true;

    dds.timeout_off();

    if (_array_var->send_p())
	_array_var->serialize(dataset, eval, dds, sink, false);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++) {
	if ((*i)->send_p()) {
	    (*i)->serialize(dataset, eval, dds, sink, false);
	}
    }

    return true;
}

bool
Grid::deserialize(XDR *source, DDS *dds, bool reuse)
{
    _array_var->deserialize(source, dds, reuse);

    for(Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
    {
	(*i)->deserialize(source, dds, reuse);
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

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
    {
	if ((*i)->name() == name)
	{
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
    if(!bt)
	throw InternalErr(__FILE__, __LINE__, 
			  "Passing NULL pointer as variable to be added.");
    
    // Jose Garcia
    // Now we get a copy of the maps or of the array
    // so the owner of bt which is external to libdap++
    // is free to deallocate its object.
    switch (part) {
      case array:
	_array_var = bt->ptr_duplicate();
	_array_var->set_parent(this);
	return;
      case maps: {
	  BaseType *btp = bt->ptr_duplicate();
	  btp->set_parent(this);
	  _map_vars.push_back(btp);
	  return;
      }
      default:
	if (!_array_var) {
	    _array_var = bt->ptr_duplicate();
	    _array_var->set_parent(this);
	}
	else {
	    BaseType *btp = bt->ptr_duplicate();
	    btp->set_parent(this);
	    _map_vars.push_back(btp);
	}
	return;
    }
}    

/** @brief Returns the Grid Array. */
BaseType *
Grid::array_var()
{
    return _array_var;
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

    if (constrained)
    {
	comp = _array_var->send_p() ? 1: 0;

	for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
	{
	    if ((*i)->send_p())
	    {
		comp++;
	    }
	}
    }
    else
    {
	comp = 1 + _map_vars.size();
    }

    return comp;
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

    Array::Dim_iter i = a->dim_begin() ;
    Map_iter m = map_begin() ;
    for( ; valid && i != a->dim_end() && m != map_end(); i++, m++)
    {
	if (a->dimension_size(i, true)) {
	    // Check the matching Map vector; the Map projection must equal
	    // the Array dimension projection
	    Array *map = (Array *)(*m);
	    Array::Dim_iter fd = map->dim_begin(); // Maps have only one dim!
	    valid = map->dimension_start(fd, true) 
		    == a->dimension_start(i, true)
		&& map->dimension_stop(fd, true) 
                   == a->dimension_stop(i, true)
		&& map->dimension_stride(fd, true) 
                   == a->dimension_stride(i, true);
	}
	else {
	    // Corresponding Map vector must be excluded from the projection.
	    Array *map = (Array *)(*m);
	    valid = !map->send_p();
	}
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

void 
Grid::print_decl(FILE *out, string space, bool print_semi,
		 bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    // If we are printing the declaration of a constrained Grid then check for
    // the case where the projection removes all but one component; the
    // resulting object is a simple array.
    int projection = components(true);
    if (constrained && projection == 1) {
	_array_var->print_decl(out, space, true, constraint_info,
			       constrained);
	for (Map_citer i = _map_vars.begin(); i != _map_vars.end(); i++)
	{
	    (*i)->print_decl(out, space, true, constraint_info, constrained);
	}
	goto exit;		// Skip end material.
    }
    // If there are M (< N) componets (Array and Maps combined) in a N
    // component Grid, send the M components as elements of a Struture.
    // This will preserve the grouping without violating the rules for a
    // Grid. 
    else if (constrained && !projection_yields_grid()) {
	fprintf( out, "%sStructure {\n", space.c_str() ) ;

	_array_var->print_decl(out, space + "    ", true, constraint_info,
			       constrained);

	for (Map_citer i = _map_vars.begin(); i != _map_vars.end(); i++)
	{
	    (*i)->print_decl(out, space + "    ", true,
			     constraint_info, constrained);
	}

	fprintf( out, "%s} %s", space.c_str(), id2www( name() ).c_str() ) ;
    }
    else {
	// The number of elements in the (projected) Grid must be such that
	// we have a valid Grid object; send it as such.
	fprintf( out, "%s%s {\n", space.c_str(), type_name().c_str() ) ;

	fprintf( out, "%s  Array:\n", space.c_str() ) ;
	_array_var->print_decl(out, space + "    ", true, constraint_info,
			       constrained);

	fprintf( out, "%s  Maps:\n", space.c_str() ) ;
	for (Map_citer i = _map_vars.begin(); i != _map_vars.end(); i++)
	{
	    (*i)->print_decl(out, space + "    ", true,
			     constraint_info, constrained);
	}

	fprintf( out, "%s} %s", space.c_str(), id2www( name() ).c_str() ) ;
    }

    if (constraint_info) {
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	fprintf( out, ";\n" ) ;

    // If sending just one comp, skip sending the terminal semicolon, etc.
exit:
    return;
}

class PrintMapField : public unary_function<BaseType *, void> {
    FILE *d_out;
    string d_space;
    bool d_constrained;
public:
    PrintMapField(FILE *o, string s, bool c) 
	: d_out(o), d_space(s), d_constrained(c) {}

    void operator()(BaseType *btp) {
	Array *a = dynamic_cast<Array*>(btp);
	if (!a)
	    throw InternalErr(__FILE__, __LINE__, "Expected an Array.");
	a->print_as_map_xml(d_out, d_space, d_constrained);
    }
};

void
Grid::print_xml(FILE *out, string space, bool constrained)
{
    if (constrained && !send_p())
        return;

    fprintf(out, "%s<Grid", space.c_str());
    if (!name().empty())
	fprintf(out, " name=\"%s\"", id2xml(name()).c_str());
    
    fprintf(out, ">\n");

    get_attr_table().print_xml(out, space + "    ", constrained);

    array_var()->print_xml(out, space + "    ", constrained);

    for_each(map_begin(), map_end(),
	     PrintMapField(out, space + "    ", constrained));
	
    fprintf(out, "%s</Grid>\n", space.c_str());
}

void 
Grid::print_val(FILE *out, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(out, space, false);
	fprintf( out, " = " ) ;
    }

    // If we are printing a value on the client-side, projection_yields_grid
    // should not be called since we don't *have* a projection without a
    // contraint. I think that if we are here and send_p() is not true, then
    // the value of this function should be ignored. 4/6/2000 jhrg
    bool pyg = projection_yields_grid(); // hack 12/1/99 jhrg
    if (pyg || !send_p())
	fprintf( out, "{  Array: " ) ;
    else
	fprintf( out, "{" ) ;
    _array_var->print_val(out, "", false);
    if (pyg || !send_p())
	fprintf( out, "  Maps: " ) ;
    for( Map_citer i = _map_vars.begin(); i != _map_vars.end(); 
	 i++, (void)(i != _map_vars.end() && fprintf( out, ", " ) ) )
    {
	(*i)->print_val(out, "", false);
    }
    fprintf( out, " }" ) ;

    if (print_decl_p)
	fprintf( out, ";\n" ) ;
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
#if 0
    // Actually, the spec doesn't say this. jhrg 2/13/06
    if (!unique_names(_map_vars, name(), type_name(), msg))
	return false;
#endif

    msg="";

    if (!_array_var) {
        msg+="Null grid base array in `"+name()+"'\n";
	return false;
    }
	
    // Is it an array?
    if (_array_var->type() != dods_array_c) {
	msg+= "Grid `"+ name()+"'s' member `"+ _array_var->name()+"' must be an array\n";
	return false;
    }
	    
    Array *av = (Array *)_array_var; // past test above, must be an array

    // Array must be of a simple_type.
    if (!av->var()->is_simple_type()) {
        msg+="The field variable `"+this->name()+"' must be an array of simple type elements (e.g., int32, String)\n";
	return false;
    }

    // enough maps?
    if ((unsigned)_map_vars.size() != av->dimensions()) {
        msg+="The number of map variables for grid `"+this->name()+ "' does not match the number of dimensions of `";
	msg+=av->name()+ "'\n";
	return false;
    }

    const string array_var_name = av->name();
    Array::Dim_iter asi = av->dim_begin() ;
    for (Map_iter mvi = _map_vars.begin();
	 mvi != _map_vars.end(); mvi++, asi++)
    {

	BaseType *mv = *mvi;

	// check names
	if (array_var_name == mv->name()) {
	    msg+= "Grid map variable `" + mv->name()+"' conflicts with the grid array name in grid `"+ name()+"'\n";
	    return false;
	}
	// check types
	if (mv->type() != dods_array_c) {
	    msg+= "Grid map variable  `"+ mv->name()+ "' is not an array\n";
	    return false;
	}

	Array *mv_a = (Array *)mv; // downcast to (Array *)

	// Array must be of a simple_type.
	if (!mv_a->var()->is_simple_type()) {
	    msg+= "The field variable `"+ this->name()+ "' must be an array of simple type elements (e.g., int32, String)\n";
	    return false;
	}

	// check shape
	if (mv_a->dimensions() != 1) {// maps must have one dimension
	    msg+="Grid map variable  `"+mv_a->name()+"' must be only one dimension\n";
	    return false;
	}
	// size of map must match corresponding array dimension
	Array::Dim_iter mv_asi = mv_a->dim_begin() ;
	int mv_a_size = mv_a->dimension_size(mv_asi) ;
	int av_size = av->dimension_size(asi) ;
	if (mv_a_size != av_size)
	{
	    msg+="Grid map variable  `" +mv_a->name()+"'s' size does not match the size of array variable '";
	    msg+=_array_var->name()+"'s' cooresponding dimension\n";
	    return false;
	}
    }

    if (all) {
	if (!_array_var->check_semantics(msg, true))
	    return false;
	for (Map_iter mvi = _map_vars.begin(); mvi != _map_vars.end(); mvi++)
	{
	    if (!(*mvi)->check_semantics(msg, true))
	    {
		return false;
	    }
	}
    }

    return true;
}
