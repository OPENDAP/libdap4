
// implementation for Grid.
//
// jhrg 9/15/94

#include "Grid.h"

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

    for (Pix p = map_vars_.first(); p; map_vars_.next(p))
	if (map_vars_(p)->get_var_name() == name)
	    return map_vars_(p);

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
	map_vars_.append(bt);
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
    return map_vars_.first();
}

void 
Grid::next_map_var(Pix &p)
{
    map_vars_.next(p);
}

BaseType *
Grid::map_var(Pix p)
{
    return map_vars_(p);
}

void 
Grid::print_decl(bool print_semi)
{
    cout << get_var_type() << " {" << endl;

    cout << "ARRAY:" << endl;
    array_var_->print_decl();

    cout << "MAPS:" << endl;
    for (Pix p = map_vars_.first(); p; map_vars_.next(p))
	map_vars_(p)->print_decl();

    cout << "} " << get_var_name();
    if (print_semi)
	cout << ";" << endl;
}
    
