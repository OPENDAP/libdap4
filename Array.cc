
// Implementation for Array.
//
// jhrg 9/13/94

// $Log: Array.cc,v $
// Revision 1.5  1994/11/22 20:47:45  dan
// 11/22/94 Modified dimension() to return total number of elements.
//          Modified deserialize to return (bool)status = num | 0.
//
// Revision 1.4  1994/11/22  14:05:19  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.3  1994/10/17  23:34:42  jimg
// Added code to print_decl so that variable declarations are pretty
// printed.
// Added private mfunc duplicate().
// Added ptr_duplicate().
// Added Copy ctor, dtor and operator=.
//
// Revision 1.2  1994/09/23  14:31:36  jimg
// Added check_semantics mfunc.
// Added sanity checking for access to shape list (is it empty?).
// Added cvs log listing to Array.cc.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "Array.h"
#include "errmsg.h"
#include "debug.h"

void
Array::duplicate(const Array &a)
{
    set_var_name(a.get_var_name());
    set_var_type(a.get_var_type());

    shape = a.shape;
    var_ptr = a.var_ptr->ptr_duplicate();
}

BaseType *
Array::ptr_duplicate()
{
    return new Array(*this);
}

// Construct an instance of Array. The (BaseType *) is assumed to be
// allocated using new -- The dtor for Array will delete this object.

Array::Array(const String &n, const String &t, BaseType *v) : var_ptr(v)
{
    set_var_name(n);
    set_var_type(t);
}

Array::Array(const Array &rhs)
{
    duplicate(rhs);
}

Array::~Array()
{
    delete var_ptr;
}

const Array &
Array::operator=(const Array &rhs)
{
    if (this == &rhs)
	return *this;
    
    duplicate(rhs);

    return *this;
}

// When you want to allocate a buffer big enough to hold the entire array,
// in the local representation, allocate size() bytes.

unsigned int
Array::size()
{
  int tSize = 1;
  for (Pix p = first_dim(); p; next_dim(p)) tSize *= dim(p); 
    return (tSize);
}

// Serialize an array. This uses the BaseType member XDR_CODER to encode each
// element of the array. See Sun's XDR manual. 
//
// NB: The array must already be in BUF (in the local machine's
// representation) *before* this call is made.

bool
Array::serialize(unsigned int num)
{
    assert(buf);

    if (num == 0)		// the default
	num = size();

    return (bool)xdr_array(xdrout, (char **)&buf, &num, DODS_MAX_ARRAY, 
			   var_ptr->size(), var_ptr->xdr_coder());
}

// NB: If you do not allocate any memory to BUF *and* ensure that BUF ==
// NULL, then deserialize will alloacte the memory for you. However, it will
// do so using malloc so YOU MUST USE FREE, NOT DELETE, TO RELEASE IT. 
// You can avoid all this hassle by using the mfunc alloc_buf and free_buf
// in CtorType. You can use free_buf to free the contents of BUF when they
// were allocated by alloc_buf *or* deserialize (but *not* new).

unsigned int
Array::deserialize()
{
    unsigned int num;
    bool status = (bool)xdr_array(xdrin, (char **)&buf, &num, DODS_MAX_ARRAY,
			   var_ptr->size(), var_ptr->xdr_coder());

    return status ? num : (unsigned int)FALSE;
}

// NAME defaults to NULL. It is present since the definition of this mfunc is
// inherited from CtorType, which declares it like this since some ctor types
// have several member variables.

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
    if (var_ptr)
	err_quit("Array::add_var:Attempt to overwrite base type of an arry");

    var_ptr = v;
    set_var_name(v->get_var_name());

    DBG(cerr << "Array::add_var: Added variable " << v << " (" \
	 << v->get_var_name() << " " << v->get_var_type() << ")" << endl);
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
    if (!shape.empty() && p)
	shape.next(p); 
}

int 
Array::dim(Pix p) 
{ 
    if (!shape.empty() && p)
	return shape(p); 
}

int 
Array::dimensions() 
{ 
    return shape.length(); 
}


void
Array::print_decl(ostream &os, String space, bool print_semi)
{
    var_ptr->print_decl(os, space, false); // print it, but w/o semicolon
    for (Pix p = shape.first(); p; shape.next(p))
	os << "[" << shape(p) << "]";
    if (print_semi)
	os << ";" << endl;
}

bool
Array::check_semantics(bool all)
{
    bool sem = BaseType::check_semantics() && !shape.empty();

    if (!sem)
	cerr << "An array variable must have dimensions" << endl;

    return sem;
}
