
// implementation for Grid.
//
// jhrg 9/15/94

// $Log: Grid.cc,v $
// Revision 1.2  1994/09/23 14:45:28  jimg
// Added mfunc check_semantics().
// Added sanity checking on the variable list (is it empty?).
//

#include "Grid.h"
#include "Array.h"		// for downcasts
#include "util.h"

Grid::Grid(const String &n, const String &t)
{
    set_var_name(n);
    set_var_type(t);
}

BaseType *
Grid::var(const String &name)
{
    if (array_var_->get_var_name() == name)
	return array_var_;

    for (Pix p = map_vars.first(); p; map_vars.next(p))
	if (map_vars(p)->get_var_name() == name)
	    return map_vars(p);

    return 0;
}    

void 
Grid::add_var(BaseType *bt, Part part)
{
    switch (part) {
      case array:
	array_var_ = bt;
	return;
      case maps:
	map_vars.append(bt);
	return;
      default:
	error("Unknown grid part (must be array or maps)\n");
	return;
    }
}    

BaseType *
Grid::array_var()
{
    return array_var_;
}

Pix 
Grid::first_map_var()
{
    return map_vars.first();
}

void 
Grid::next_map_var(Pix &p)
{
    if (!map_vars.empty() && p)
	map_vars.next(p);
}

BaseType *
Grid::map_var(Pix p)
{
    if (!map_vars.empty() && p)
	return map_vars(p);
}

void 
Grid::print_decl(bool print_semi)
{
    cout << get_var_type() << " {" << endl;

    cout << "ARRAY:" << endl;
    array_var_->print_decl();

    cout << "MAPS:" << endl;
    for (Pix p = map_vars.first(); p; map_vars.next(p))
	map_vars(p)->print_decl();

    cout << "} " << get_var_name();
    if (print_semi)
	cout << ";" << endl;
}

// Grids have ugly semantics.

bool
Grid::check_semantics(bool all)
{
    if (!BaseType::check_semantics())
	return false;

    if (!unique(map_vars, (const char *)get_var_name(),
		(const char *)get_var_type()))
	return false;

    if (!array_var_) {
	cerr << "Null grid base array in `" << get_var_name() << "'" << endl;
	return false;
    }
	
    // Is it an array?
    if (array_var_->get_var_type() != "Array") {
	cerr << "Grid `" << get_var_name() << "'s' member `"
	    << array_var_->get_var_name() << "' must be an array" << endl;
	return false;
    }
	    
    Array *av = (Array *)array_var_; // past test above, must be an array

    // engough maps?
    if (map_vars.length() != av->dimensions()) {
	cerr << "The number of map variables for grid `"
	     << this->get_var_name() 
	     << "' does not match the number of dimensions of `"
	    << av->get_var_name() << "'" << endl;
	return false;
    }

    const String &array_var_name = av->get_var_name();
    Pix p, ap;
    for (p = map_vars.first(), ap = av->first_dim();
	 p; map_vars.next(p), av->next_dim(ap)) {

	BaseType *mv = map_vars(p);

	// check names
	if (array_var_name == mv->get_var_name()) {
	    cerr << "Grid map variable `" << mv->get_var_name()
		<< "' conflicts with the grid array name in grid `"
		<< get_var_name() << "'" << endl;
	    return false;
	}
	// check types
	if (mv->get_var_type() != "Array") {
	    cerr << "Grid map variable  `" << mv->get_var_name()
		<< "' is not an array" << endl;
	    return false;
	}

	Array *mv_a = (Array *)mv; // downcast to (Array *)

	// check shape
	if (mv_a->dimensions() != 1) {// maps must have one dimension
	    cerr << "Grid map variable  `" << mv_a->get_var_name()
		<< "' must be only one dimension" << endl;
	    return false;
	}
	// size of map must match corresponding array dimension
	if (mv_a->dim(mv_a->first_dim()) != av->dim(ap)) {
	    cerr << "Grid map variable  `" << mv_a->get_var_name()
		<< "'s' size does not match the size of array variable '"
		<< array_var_->get_var_name() << "'s' cooresponding dimension"
		<< endl;
	    return false;
	}
    }

    if (all) {
	if (!array_var_->check_semantics(true))
	    return false;
	for (Pix p = map_vars.first(); p; map_vars.next(p))
	    if (!map_vars(p)->check_semantics(true))
		return false;
    }

    return true;
}
