
// implementation for Grid.
//
// jhrg 9/15/94

// $Log: Grid.cc,v $
// Revision 1.7  1995/02/10 02:23:07  jimg
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

#include <assert.h>

#include "Grid.h"
#include "Array.h"		// for downcasts
#include "util.h"
#include "errmsg.h"

void
Grid::_duplicate(const Grid &s)
{
    set_name(s.name());
    
    _array_var = s._array_var->ptr_duplicate();

    Grid &cs = (Grid)s;		// cast away const;

    for (Pix p = cs._map_vars.first(); p; cs._map_vars.next(p))
	_map_vars.append(cs._map_vars(p)->ptr_duplicate());
}

Grid::Grid(const String &n)
     : BaseType( n, "Grid", (xdrproc_t)NULL)
{
    set_name(n);
}

Grid::Grid(const Grid &rhs)
{
    _duplicate(rhs);
}

Grid::~Grid()
{
    delete _array_var;

    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	delete _map_vars(p);
}

const Grid &
Grid::operator=(const Grid &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

unsigned int
Grid::size()
{
    unsigned int sz = _array_var->size();
  
    for (Pix p = _map_vars.first(); p; _map_vars.next(p)) 
	sz += _map_vars(p)->size();
  
    return sz;
}

bool
Grid::serialize(bool flush)
{
    bool status;

    if (!(status = _array_var->serialize(false))) 
	return false;

    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	if  (!(status = _map_vars(p)->serialize(false)) ) 
	    break;
	
    if (status && flush)
	status = expunge();

    return status;
}

unsigned int
Grid::deserialize(bool reuse)
{
    unsigned int num, sz = 0;
    
    sz += num = _array_var->deserialize(reuse);
    if (num == 0) 
	return (unsigned int)false;

    for(Pix p = _map_vars.first(); p; _map_vars.next(p)) {
	sz += num = _map_vars(p)->deserialize(reuse);
	if (num == 0) 
	    return (unsigned int)false;
    }

    return sz;
}

unsigned int
Grid::store_val(void *val, bool reuse)
{
    assert(val);

    unsigned int pos = 0;
    pos += _array_var->store_val(val, reuse);

    for(Pix p = _map_vars.first(); p; _map_vars.next(p))
	pos += _map_vars(p)->store_val(val + pos, reuse);

    return pos;
}

unsigned int
Grid::read_val(void **val)
{
    assert(val);

    if (!*val)
	*val = new char[size()];

    unsigned int pos = 0;
    pos += _array_var->read_val((void **)*val);

    for(Pix p = _map_vars.first(); p; _map_vars.next(p))
	pos += _map_vars(p)->read_val((void **)*val + pos);

    return pos;
}

BaseType *
Grid::var(const String &name)
{
    if (_array_var->name() == name)
	return _array_var;

    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	if (_map_vars(p)->name() == name)
	    return _map_vars(p);

    return 0;
}    

void 
Grid::add_var(BaseType *bt, Part part)
{
    switch (part) {
      case array:
	_array_var = bt;
	return;
      case maps:
	_map_vars.append(bt);
	return;
      default:
	err_quit("Grid::add_var:Unknown grid part (must be array or maps)");
	return;
    }
}    

BaseType *
Grid::array_var()
{
    return _array_var;
}

Pix 
Grid::first_map_var()
{
    return _map_vars.first();
}

void 
Grid::next_map_var(Pix &p)
{
    if (!_map_vars.empty() && p)
	_map_vars.next(p);
}

BaseType *
Grid::map_var(Pix p)
{
    if (!_map_vars.empty() && p)
	return _map_vars(p);
}

void 
Grid::print_decl(ostream &os, String space, bool print_semi)
{
    os << space << type() << " {" << endl;

    os << space << " ARRAY:" << endl;
    _array_var->print_decl(os, space + "    ");

    os << space << " MAPS:" << endl;
    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	_map_vars(p)->print_decl(os, space + "    ");

    os << space << "} " << name();
    if (print_semi)
	os << ";" << endl;
}

void 
Grid::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ARRAY: ";
    _array_var->print_val(os, "", false);
    os << " MAPS: ";
    for (Pix p = _map_vars.first(); p; _map_vars.next(p), p && os << ", ")
	_map_vars(p)->print_val(os, "", false);
    os << " }";

    if (print_decl_p)
	os << ";";
}

// Grids have ugly semantics.

bool
Grid::check_semantics(bool all)
{
    if (!BaseType::check_semantics())
	return false;

    if (!unique(_map_vars, (const char *)name(), (const char *)type()))
	return false;

    if (!_array_var) {
	cerr << "Null grid base array in `" << name() << "'" << endl;
	return false;
    }
	
    // Is it an array?
    if (_array_var->type() != "Array") {
	cerr << "Grid `" << name() << "'s' member `"
	    << _array_var->name() << "' must be an array" << endl;
	return false;
    }
	    
    Array *av = (Array *)_array_var; // past test above, must be an array

    // enough maps?
    if (_map_vars.length() != av->dimensions()) {
	cerr << "The number of map variables for grid `"
	     << this->name() 
	     << "' does not match the number of dimensions of `"
	    << av->name() << "'" << endl;
	return false;
    }

    const String &array_var_name = av->name();
    Pix p, ap;
    for (p = _map_vars.first(), ap = av->first_dim();
	 p; _map_vars.next(p), av->next_dim(ap)) {

	BaseType *mv = _map_vars(p);

	// check names
	if (array_var_name == mv->name()) {
	    cerr << "Grid map variable `" << mv->name()
		<< "' conflicts with the grid array name in grid `"
		<< name() << "'" << endl;
	    return false;
	}
	// check types
	if (mv->type() != "Array") {
	    cerr << "Grid map variable  `" << mv->name()
		<< "' is not an array" << endl;
	    return false;
	}

	Array *mv_a = (Array *)mv; // downcast to (Array *)

	// check shape
	if (mv_a->dimensions() != 1) {// maps must have one dimension
	    cerr << "Grid map variable  `" << mv_a->name()
		<< "' must be only one dimension" << endl;
	    return false;
	}
	// size of map must match corresponding array dimension
	if (mv_a->dimension_size(mv_a->first_dim()) != av->dimension_size(ap)) {
	    cerr << "Grid map variable  `" << mv_a->name()
		<< "'s' size does not match the size of array variable '"
		<< _array_var->name() << "'s' cooresponding dimension"
		<< endl;
	    return false;
	}
    }

    if (all) {
	if (!_array_var->check_semantics(true))
	    return false;
	for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	    if (!_map_vars(p)->check_semantics(true))
		return false;
    }

    return true;
}

