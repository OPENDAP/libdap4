
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// implementation for Grid.
//
// jhrg 9/15/94

// $Log: Grid.cc,v $
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

#include "config_dap.h"

#include <assert.h>

#include "Grid.h"
#include "DDS.h"
#include "Array.h"		// for downcasts
#include "util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void
Grid::_duplicate(const Grid &s)
{
    BaseType::_duplicate(s);

    _array_var = s._array_var->ptr_duplicate();

    Grid &cs = (Grid &)s;		// cast away const;

    for (Pix p = cs._map_vars.first(); p; cs._map_vars.next(p))
	_map_vars.append(cs._map_vars(p)->ptr_duplicate());
}

Grid::Grid(const String &n) : BaseType(n, dods_grid_c)
{
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

void
Grid::set_send_p(bool state)
{
    _array_var->set_send_p(state);

    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	_map_vars(p)->set_send_p(state);

    BaseType::set_send_p(state);
}

void
Grid::set_read_p(bool state)
{
    _array_var->set_read_p(state);

    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	_map_vars(p)->set_read_p(state);

    BaseType::set_read_p(state);
}

unsigned int
Grid::width()
{
    unsigned int sz = _array_var->width();
  
    for (Pix p = _map_vars.first(); p; _map_vars.next(p)) 
	sz += _map_vars(p)->width();
  
    return sz;
}

bool
Grid::serialize(const String &dataset, DDS &dds, bool ce_eval, bool flush)
{
    bool status = true;
    int error = 0;

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (_array_var->send_p() 
	&& !(status = _array_var->serialize(dataset, dds, false, false))) 
	return false;

    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	if  (_map_vars(p)->send_p() 
	     && !(status = _map_vars(p)->serialize(dataset, dds, false, 
						   false))) 
	    break;

    // flush the stream *even* if status is false, but preserve the value of
    // status if it's false.
    if (flush)
	status = status && expunge();

    return status;
}

bool
Grid::deserialize(bool reuse)
{
    bool status;
    
    status = _array_var->deserialize(reuse);
    if (!status) 
	return false;

    for(Pix p = _map_vars.first(); p; _map_vars.next(p)) {
	status = _map_vars(p)->deserialize(reuse);
	if (!status) 
	    break;
    }

    return status;
}

unsigned int
Grid::val2buf(void *, bool)
{
    return sizeof(Grid);
}

unsigned int
Grid::buf2val(void **)
{
    return sizeof(Grid);
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
	assert(false);
	cerr << "Grid::add_var:Unknown grid part (must be array or maps)" 
	     << endl;
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
    else
	return 0;
}

void 
Grid::print_decl(ostream &os, String space, bool print_semi,
		 bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    os << space << type_name() << " {" << endl;

    os << space << " ARRAY:" << endl;
    _array_var->print_decl(os, space + "    ", true, constraint_info,
			   constrained);

    os << space << " MAPS:" << endl;
    for (Pix p = _map_vars.first(); p; _map_vars.next(p))
	_map_vars(p)->print_decl(os, space + "    ", true, constraint_info,
				 constrained);

    os << space << "} " << name();

    if (constraint_info) {
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

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
	os << ";" << endl;
}

// Grids have ugly semantics.

bool
Grid::check_semantics(bool all)
{
    if (!BaseType::check_semantics())
	return false;

    if (!unique(_map_vars, (const char *)name(), (const char *)type_name()))
	return false;

    if (!_array_var) {
	cerr << "Null grid base array in `" << name() << "'" << endl;
	return false;
    }
	
    // Is it an array?
    if (_array_var->type() != dods_array_c) {
	cerr << "Grid `" << name() << "'s' member `"
	    << _array_var->name() << "' must be an array" << endl;
	return false;
    }
	    
    Array *av = (Array *)_array_var; // past test above, must be an array

    // enough maps?
    if ((unsigned)_map_vars.length() != av->dimensions()) {
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
	if (mv->type() != dods_array_c) {
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
	if (mv_a->dimension_size(mv_a->first_dim()) 
	    != av->dimension_size(ap)) {
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
	for (p = _map_vars.first(); p; _map_vars.next(p))
	    if (!_map_vars(p)->check_semantics(true))
		return false;
    }

    return true;
}

