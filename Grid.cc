
// implementation for Grid.
//
// jhrg 9/15/94

// $Log: Grid.cc,v $
// Revision 1.4  1994/12/14 20:56:57  dan
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

#include "Grid.h"
#include "Array.h"		// for downcasts
#include "util.h"
#include "errmsg.h"

void
Grid::duplicate(const Grid &s)
{
    set_var_name(s.get_var_name());
    
    array_var_ = s.array_var_->ptr_duplicate();

    Grid &cs = (Grid)s;		// cast away const;

    for (Pix p = cs.map_vars.first(); p; cs.map_vars.next(p))
	map_vars.append(cs.map_vars(p)->ptr_duplicate());
}

// protected

BaseType *
Grid::ptr_duplicate()
{
    return new Grid(*this);
}

Grid::Grid(const String &n, FILE *in, FILE *out)
     : BaseType( n, "Grid", (xdrproc_t)NULL, in, out)
{
    set_var_name(n);
}

Grid::Grid(const Grid &rhs)
{
    duplicate(rhs);
}

Grid::~Grid()
{
    delete array_var_;

    for (Pix p = map_vars.first(); p; map_vars.next(p))
	delete map_vars(p);
}

const Grid &
Grid::operator=(const Grid &rhs)
{
    if (this == &rhs)
	return *this;

    duplicate(rhs);

    return *this;
}

unsigned int
Grid::size()
{
    unsigned int sz = array_var_->size();
  
    for( Pix p = map_vars.first(); p; map_vars.next(p)) 
      sz += map_vars(p)->size();
  
    return sz;
}

bool
Grid::serialize(bool flush, unsigned int num)
{
    bool status;

    if( !(status = array_var_->serialize(false,0))) 
      return (bool)FALSE;

    for( Pix p = map_vars.first(); p; map_vars.next(p))
      if ( !(status = map_vars(p)->serialize(false)) ) break;
	
    if ( status && flush )
      status = expunge();

    return status;
}

unsigned int
Grid::deserialize()
{
    unsigned int num, sz = 0;

    if ((num = array_var_->deserialize()) == 0) return (unsigned int)FALSE;
    sz += num;

    for( Pix p = map_vars.first(); p; map_vars.next(p)) 
      {
	if ((num = map_vars(p)->deserialize()) == 0) break;
	sz += num;
      }
    return num ? sz : (unsigned int)FALSE;
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
	err_quit("Grid::add_var:Unknown grid part (must be array or maps)");
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
Grid::print_decl(ostream &os, String space, bool print_semi)
{
    os << space << get_var_type() << " {" << endl;

    os << space << " ARRAY:" << endl;
    array_var_->print_decl(os, space + "    ");

    os << space << " MAPS:" << endl;
    for (Pix p = map_vars.first(); p; map_vars.next(p))
	map_vars(p)->print_decl(os, space + "    ");

    os << space << "} " << get_var_name();
    if (print_semi)
	os << ";" << endl;
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

    // enough maps?
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
	if (mv_a->dimension_size(mv_a->first_dim()) != av->dimension_size(ap)) {
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

void 
Grid::print_val(ostream &os, String space)
{
    print_decl(os, "", false);
    //os << " = " << buf << ";" << endl;
}
