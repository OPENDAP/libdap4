
// Implementation for Array.
//
// jhrg 9/13/94

#ifdef __GNUG__
#pragma implementation
#endif

#include "Array.h"

Array::Array(const String &n, const String &t, BaseType *v) 
    : var_ptr(v)
{
    set_var_name(n);
    set_var_type(t);
}

// NAME defaults to NULL. It is present since the definition of this mfunc is
// inherited from CtorType, which declares it like this since some ctor types
// have several memebr variables.

BaseType *
Array::var(const String &name)
{
    return var_ptr;
}

// Add the BaseType pointer to this ctor type instance. Propagate the name of
// the BaseType instance to this instance This ensures that variables at any
// given level of the DDS table have unique names (i.e., that Arrays do not
// have there default name "").
// NB: Part p defaults to nil for this class

void
Array::add_var(BaseType *v, Part p)
{
    if (var_ptr) {
	error("Attempt to overwrite base type of an arry\n");
	return;
    }

    var_ptr = v;
    set_var_name(v->get_var_name());
}

void 
Array::append_dim(int dim)
{ 
    shape.append(dim); 
}

Pix 
Array::first_dim() 
{ 
    return shape.first();
}

void 
Array::next_dim(Pix &p) 
{ 
    shape.next(p); 
}

int 
Array::dim(Pix p) 
{ 
    return shape(p); 
}

int 
Array::dimensions() 
{ 
    return shape.length(); 
}


void
Array::print_decl(bool print_semi)
{
    var_ptr->print_decl(false);		// print it, but w/o semicolon
    if (!shape.empty()) 
	for (Pix p = shape.first(); p; shape.next(p))
	    cout << "[" << shape(p) << "]";
    if (print_semi)
	cout << ";" << endl;
}
