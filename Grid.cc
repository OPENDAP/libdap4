
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

#include "config_dap.h"

#include <strstream>

#include "Grid.h"
#include "DDS.h"
#include "Array.h"		// for downcasts
#include "util.h"
#include "InternalErr.h"
#include "escaping.h"
#include "BTIterAdapter.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using std::cerr;
using std::endl;
using std::ends;
using std::ostrstream;

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
Grid::Grid(const string &n) : Constructor(n, dods_grid_c)
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
	delete btp ;
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
Grid::serialize(const string &dataset, DDS &dds, XDR *sink, 
		bool ce_eval)
{
    if (!read_p())
	read(dataset);		// read() throws Error and InternalErr

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (_array_var->send_p())
	_array_var->serialize(dataset, dds, sink, false);

    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
    {
	if ((*i)->send_p())
	{
	    (*i)->serialize(dataset, dds, sink, false);
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
Grid::var(const string &name, btp_stack &s)
{
    return var(name, true, &s);
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

#if 0
/** This probably shouldn't be here.

    @deprecated */
BaseType *
Grid::var(const string &name, bool)
{
    if (_array_var->name() == name)
	return _array_var;

    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	if (_map_vars(p)->name() == name)
	    return _map_vars(p);

    return 0;
}    
#endif

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
	throw InternalErr(__FILE__, __LINE__, 
			  "Unknown grid part (must be Array or Maps).");
	return;
    }
}    

/** @brief Returns the Grid Array. */
BaseType *
Grid::array_var()
{
    return _array_var;
}

/** @brief Returns the index of the first Map vector. */
Pix 
Grid::first_map_var()
{
    if (_map_vars.empty())
	return 0;

    BTIterAdapter *i = new BTIterAdapter( _map_vars ) ;
    i->first() ;
    return i ;
}

Grid::Map_iter
Grid::map_begin()
{
    return _map_vars.begin() ;
}

Grid::Map_iter
Grid::map_end()
{
    return _map_vars.end() ;
}

/** Return the iterator for the \i ith map.
    @param i the index
    @return The corresponding  Vars_iter */
Grid::Map_iter
Grid::get_map_iter(int i)
{
    return _map_vars.begin() + i;
}

/** @brief Increments the Map vector index. */
void 
Grid::next_map_var(Pix p)
{
    p.next() ;
}

/** @brief Given an index, returns the corresponding Map vector. */
BaseType *
Grid::map_var(Pix p)
{
    BTIterAdapter *i = (BTIterAdapter *)p.getIterator() ;
    if( !i )
	return 0 ;

    return i->entry() ;
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

/** This mfunc has been modified so that if the user projects a Grid
    such that the number of components in the Grid no longer match
    up properly (the Array dimension no longer matches the number of
    Map vectors) whatever is in the projection is sent not as a Grid
    but as a simple array or Structure of Arrays, whichever is
    appropriate.


    @brief Prints the Grid declaration only if a valid Grid.
    @see Array
    @see Structure */

void 
Grid::print_decl(ostream &os, string space, bool print_semi,
		 bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    // If we are printing the declaration of a constrained Grid then check for
    // the case where the projection removes all but one component; the
    // resulting object is a simple array.
    int projection = components(true);
    if (constrained && projection == 1) {
	_array_var->print_decl(os, space, true, constraint_info,
			       constrained);
	for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
	{
	    (*i)->print_decl(os, space, true, constraint_info, constrained);
	}
	goto exit;		// Skip end material.
    }
    // If there are M (< N) componets (Array and Maps combined) in a N
    // component Grid, send the M components as elements of a Struture.
    // This will preserve the grouping without violating the rules for a
    // Grid. 
    else if (constrained && !projection_yields_grid()) {
	os << space << "Structure {" << endl;

	_array_var->print_decl(os, space + "    ", true, constraint_info,
			       constrained);

	for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
	{
	    (*i)->print_decl(os, space + "    ", true,
			     constraint_info, constrained);
	}

	os << space << "} " << id2www(name());
    }
    else {
	// The number of elements in the (projected) Grid must be such that
	// we have a valid Grid object; send it as such.
	os << space << type_name() << " {" << endl;

	os << space << " ARRAY:" << endl;
	_array_var->print_decl(os, space + "    ", true, constraint_info,
			       constrained);

	os << space << " MAPS:" << endl;
	for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); i++)
	{
	    (*i)->print_decl(os, space + "    ", true,
			     constraint_info, constrained);
	}

	os << space << "} " << id2www(name());
    }

    if (constraint_info) {
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	os << ";" << endl;

    // If sending just one comp, skip sending the terminal semicolon, etc.
exit:
    return;
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

	fprintf( out, "%s ARRAY:\n", space.c_str() ) ;
	_array_var->print_decl(out, space + "    ", true, constraint_info,
			       constrained);

	fprintf( out, "%s MAPS:\n", space.c_str() ) ;
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

void 
Grid::print_val(ostream &os, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    // If we are printing a value on the client-side, projection_yields_grid
    // should not be called since we don't *have* a projection without a
    // contraint. I think that if we are here and send_p() is not true, then
    // the value of this function should be ignored. 4/6/2000 jhrg
    bool pyg = projection_yields_grid(); // hack 12/1/99 jhrg
    if (pyg || !send_p())
	os << "{ ARRAY: ";
    else
	os << "{";
    _array_var->print_val(os, "", false);
    if (pyg || !send_p())
	os << " MAPS: ";
    for (Map_iter i = _map_vars.begin(); i != _map_vars.end(); 
	 i++, (void)(i != _map_vars.end() && os << ", "))
    {
	(*i)->print_val(os, "", false);
    }
    os << " }";

    if (print_decl_p)
	os << ";" << endl;
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
	fprintf( out, "{ ARRAY: " ) ;
    else
	fprintf( out, "{" ) ;
    _array_var->print_val(out, "", false);
    if (pyg || !send_p())
	fprintf( out, " MAPS: " ) ;
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

    if (!unique_names(_map_vars, name(), type_name(), msg))
	return false;
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

// $Log: Grid.cc,v $
// Revision 1.57  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.55.2.2  2003/04/15 01:17:12  jimg
// Added a method to get the iterator for a variable (or map) given its
// index. To get the iterator for the ith variable/map, call
// get_vars_iter(i).
//
// Revision 1.56  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.55.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.55  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.54  2003/01/15 19:24:39  pwest
// Removing IteratorAdapterT and replacing with non-templated versions.
//
// Revision 1.53  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.46.4.15  2002/12/31 16:43:20  rmorris
// Patches to handle some of the fancier template code under VC++ 6.0.
//
// Revision 1.46.4.14  2002/12/27 19:34:42  jimg
// Modified the var() methods so that www2id() is called before looking
// up identifier names. See bug 563.
//
// Revision 1.46.4.13  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.46.4.12  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.46.4.11  2002/09/22 14:31:08  rmorris
// VC++ doesn't consider x in 'for(int x,...)' to be only for the block
// associated with the loop.  Multiple of these therefore case a error
// because VC++ sees multiple definitions.  Changed to use different vars names
// in each such block.
//
// Revision 1.46.4.10  2002/09/12 22:49:57  pwest
// Corrected signature changes made with Pix to IteratorAdapter changes. Rather
// than taking a reference to a Pix, taking a Pix value.
//
// Revision 1.46.4.9  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.46.4.8  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.52  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.51  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.46.4.7  2002/05/22 16:57:51  jimg
// I modified the `data type classes' so that they do not need to be
// subclassed for clients. It might be the case that, for a complex client,
// subclassing is still the best way to go, but you're not required to do
// it anymore.
//
// Revision 1.46.4.6  2002/03/01 21:03:08  jimg
// Significant changes to the var(...) methods. These now take a btp_stack
// pointer and are used by DDS::mark(...). The exact_match methods have also
// been updated so that leaf variables which contain dots in their names
// will be found. Note that constructor variables with dots in their names
// will break the lookup routines unless the ctor is the last field in the
// constraint expression. These changes were made to fix bug 330.
//
// Revision 1.46.4.5  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.50  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.46.4.4  2001/10/02 17:01:52  jimg
// Made the behavior of serialize and deserialize uniform. Both methods now
// use Error exceptions to signal problems with network I/O and InternalErr
// exceptions to signal other problems. The return codes, always true for
// serialize and always false for deserialize, are now meaningless. However,
// by always returning a code that means OK, old code should continue to work.
//
// Revision 1.49  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.46.4.3  2001/09/07 00:38:35  jimg
// Sequence::deserialize(...) now reads all the sequence values at once.
// Its call semantics are the same as the other classes' versions. Values
// are stored in the Sequence object using a vector<BaseType *> for each
// row (those are themselves held in a vector). Three new accessor methods
// have been added to Sequence (row_value() and two versions of var_value()).
// BaseType::deserialize(...) now always returns true. This matches with the
// expectations of most client code (the seqeunce version returned false
// when it was done reading, but all the calls for sequences must be changed
// anyway). If an XDR error is found, deserialize throws InternalErr.
//
// Revision 1.48  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.46.4.2  2001/07/28 00:29:22  jimg
// Added to operator= code which deletes dynamically allocated memory held by
// the instance. This prevents a memory leak when the assignment operator is
// used.
//
// Revision 1.47  2001/06/15 23:49:02  jimg
// Merged with release-3-2-4.
//
// Revision 1.46.4.1  2001/06/05 06:49:19  jimg
// Added the Constructor class which is to Structures, Sequences and Grids
// what Vector is to Arrays and Lists. This should be used in future
// refactorings (I thought it was going to be used for the back pointers).
// Introduced back pointers so children can refer to their parents in
// hierarchies of variables.
// Added to Sequence methods to tell if a child sequence is done
// deserializing its data.
// Fixed the operator=() and copy ctors; removed redundency from
// _duplicate().
// Changed the way serialize and deserialize work for sequences. Now SOI and
// EOS markers are written for every `level' of a nested Sequence. This
// should fixed nested Sequences. There is still considerable work to do
// for these to work in all cases.
//
// Revision 1.46  2000/10/06 01:26:05  jimg
// Changed the way serialize() calls read(). The status from read() is
// returned by the Structure and Sequence serialize() methods; ignored by
// all others. Any exceptions thrown by read() are caught and discarded.
// serialize() returns false if read() throws an exception. This should
// be fixed once all the servers build using the new read() definition.
//
// Revision 1.45  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.44  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.43  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.42  2000/06/16 18:14:59  jimg
// Merged with 3.1.7
//
// Revision 1.38.6.1  2000/06/07 23:05:31  jimg
// The first_*() methods return 0 if there are no variables
//
// Revision 1.41  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.40.4.1  2000/06/02 18:21:27  rmorris
// Mod's for port to Win32.
//
// Revision 1.40  2000/04/07 00:16:29  jimg
// Fixed an error in print_val() where the method worked OK for the
// server-side but not the client-side because it demoted any Grid whose
// send_p flag was not set to a Structure. Since the client-side does not have
// CEs, there are no projections and send_p will never be set.
//
// Revision 1.39.2.3  2000/03/08 00:09:04  jgarcia
// replace ostrstream with string;added functions to convert from double and long to string
//
// Revision 1.39.2.2  2000/02/17 05:03:13  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.39.2.1  2000/01/28 22:14:05  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.39  1999/12/02 00:24:32  jimg
// Fixed print_val for Grids that decay to Structures.
//
// Revision 1.38  1999/04/29 02:29:30  jimg
// Merge of no-gnu branch
//
// Revision 1.37  1998/09/17 17:20:00  jimg
// Changes for the new variable lookup scheme. Fields of ctor types no longer
// need to be fully qualified. my.thing.f1 can now be named `f1' in a CE. Note
// that if there are two `f1's in a dataset, the first will be silently used;
// There's no warning about the situation. The new code in the var member
// function passes a stack of BaseType pointers so that the projection
// information (send_p field) can be set properly.
//
// Revision 1.36  1998/08/31 21:46:09  jimg
// Changed the check_semantics member function so that the array and map
// vectors must be composed of simple-type elements.
//
// Revision 1.35.2.1  1999/02/02 21:56:59  jimg
// String to string version
//
// Revision 1.35  1998/08/06 16:19:54  jimg
// Fixed misuse of read member function in serialize. Test for a read(...)
// error by checking the value of the `error' parameter, not the return value
// of the read(...) member function. (from jeh)
//
// Revision 1.34  1998/03/17 17:32:00  jimg
// Added an implementation of element_count().
//
// Revision 1.33  1997/09/22 23:02:10  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.32  1997/06/05 22:50:46  jimg
// Added two mfuncs: components() and projection_yields_grid(). These aid in
// sending Grids that have been projected in various ways.
// Fixed print_decl() so that a Grid that has some components projected either
// by explicitly listing them or by listing only some of the Grids dimensions
// will be sent properly. This means that some Grid objects `decay' to either
// Structures of Arrays or simple Arrays depending on the projection.
//
// Revision 1.31  1997/03/08 19:02:02  jimg
// Changed default param to check_semantics() from  to String()
// and removed the default from the argument list in the mfunc definition
//
// Revision 1.30  1997/02/28 01:27:54  jimg
// Changed check_semantics() so that it now returns error messages in a String
// object (passed by reference).
//
// Revision 1.29  1997/02/10 02:32:40  jimg
// Added assert statements for pointers
//
// Revision 1.28  1996/09/16 18:08:40  jimg
// Fixed var(const String name) so that it would correctly descend names of the
// form <base>.<name> where <name> may itself contain `dots'.
//
// Revision 1.27  1996/08/12 21:52:41  jimg
// Fixed a bug in check_semantics where the array name was confused with the
// dimension names - this may be a bug in the libg++2.7.1 implementation of the
// class String or a bug in our usage of the same. The problem did not show up
// on the SunOS 4.1.3 platform.
//
// Revision 1.26  1996/06/04 21:33:31  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.25  1996/05/31 23:29:46  jimg
// Updated copyright notice.
//
// Revision 1.24  1996/05/22 18:05:12  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.23  1996/05/16 22:49:47  jimg
// Dan's changes for version 2.0. Added a parameter to read that returns
// an error code so that EOF can be distinguished from an actual error when
// reading sequences. This *may* be replaced by an error member function
// in the future.
//
// Revision 1.22  1996/05/14 15:38:26  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.21  1996/04/05 00:21:33  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.20  1996/04/04 18:26:50  jimg
// Merged changes from version 1.1.1.
//
// Revision 1.19  1996/03/05 18:10:14  jimg
// Fixed serialize bug where Maps might not be sent.
// Added ce_eval to serailize member function.
//
// Revision 1.18  1996/02/02 00:31:05  jimg
// Merge changes for DODS-1.1.0 into DODS-2.x
//
// Revision 1.17  1995/12/09  01:06:46  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.16  1995/12/06  21:56:24  jimg
// Added `constrained' flag to print_decl.
// Removed third parameter of read.
// Modified print_decl() to print only those parts of a dataset that are
// selected when `constrained' is true.
//
// Revision 1.15  1995/10/23  23:20:55  jimg
// Added _send_p and _read_p fields (and their accessors) along with the
// virtual mfuncs set_send_p() and set_read_p().
//
// Revision 1.14  1995/08/26  00:31:33  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.13  1995/08/23  00:11:12  jimg
// Changed old, deprecated member functions to new ones.
// Switched from String representation of type to enum.
//
// Revision 1.12.2.2  1996/03/01 00:06:12  jimg
// Removed bad attempt at multiple connect implementation.
//
// Revision 1.12.2.1  1995/09/14  20:58:16  jimg
// Moved some loop index variables out of the loop statement.
//
// Revision 1.12  1995/07/09  21:28:59  jimg
// Added copyright notice.
//
// Revision 1.11  1995/05/10  15:34:00  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.10  1995/05/10  13:45:18  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.9  1995/03/16  17:29:10  jimg
// Added include config_dap.h to top of include list.
// Added TRACE_NEW switched dbnew includes.
// Fixed bug in read_val() where **val was passed incorrectly to
// subordinate read_val() calls.
//
// Revision 1.8  1995/03/04  14:34:45  jimg
// Major modifications to the transmission and representation of values:
// Added card() virtual function which is true for classes that
// contain cardinal types (byte, int float, string).
// Changed the representation of Str from the C rep to a C++
// class represenation.
// Chnaged read_val and store_val so that they take and return
// types that are stored by the object (e.g., inthe case of Str
// an URL, read_val returns a C++ String object).
// Modified Array representations so that arrays of card()
// objects are just that - no more storing strings, ... as
// C would store them.
// Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// an array of a structure is represented as an array of Structure
// objects).
//
// Revision 1.7  1995/02/10  02:23:07  jimg
// Added DBMALLOC includes and switch to code which uses malloc/free.
// Private and protected symbols now start with `_'.
// Added new accessors for name and type fields of BaseType; the old ones
// will be removed in a future release.
// Added the store_val() mfunc. It stores the given value in the object's
// internal buffer.
// Made both List and Str handle their values via pointers to memory.
// Fixed read_val().
// Made serialize/deserialize handle all malloc/free calls (even in those
// cases where xdr initiates the allocation).
// Fixed print_val().
//
// Revision 1.6  1995/01/19  20:05:27  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.5  1995/01/11  15:54:46  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.4  1994/12/14  20:56:57  dan
// Fixed deserialize() to return correct size count.
// Fixed check_semantics() to use new Array dimension member functions.
//
// Revision 1.3  1994/10/17  23:34:53  jimg
// Added code to print_decl so that variable declarations are pretty
// printed.
// Added private mfunc duplicate().
// Added ptr_duplicate().
// Added Copy ctor, dtor and operator=.
//
// Revision 1.2  1994/09/23  14:45:28  jimg
// Added mfunc check_semantics().
// Added sanity checking on the variable list (is it empty?).
//

