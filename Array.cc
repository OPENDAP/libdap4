
// Implementation for Array.
//
// jhrg 9/13/94

// $Log: Array.cc,v $
// Revision 1.11  1994/12/16 20:13:31  dan
// Fixed serialize() and deserialize() for arrays of strings.
//
// Revision 1.10  1994/12/14  20:35:36  dan
// Added dimensions() member function to return number of dimensions
// contained in the array.
// Removed alloc_buf() and free_buf() member functions and placed them
// in util.cc.
//
// Revision 1.9  1994/12/14  17:50:34  dan
// Modified serialize() and deserialize() member functions to special
// case BaseTypes 'Str' and 'Url'.  These special cases do not call
// xdr_array, but iterate through the arrays using calls to XDR_STR.
// Modified print_val() member function to handle arrays of different
// BaseTypes.
// Modified append_dim() member function for initializing its dimension
// components.
// Removed dim() member function.
//
// Revision 1.7  1994/12/09  21:36:33  jimg
// Added support for named array dimensions.
//
// Revision 1.6  1994/12/08  15:51:41  dan
// Modified size() member to return cumulative size of all dimensions
// given the variable basetype.
// Modified serialize() and deserialize() member functions for data
// transmission using XDR.
//
// Revision 1.5  1994/11/22  20:47:45  dan
// Modified size() to return total number of elements.
// Fixed errors in deserialize (multiple returns).
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
#include "util.h"
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

Array::Array(const String &n, FILE *in, FILE *out, BaseType *v)
     : BaseType( n, "Array", xdr_array, in, out), var_ptr(v)
{
    set_var_name(n);
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
	err_quit("Array::add_var:Attempt to overwrite base type of an array");

    var_ptr = v;
    set_var_name(v->get_var_name());

    DBG(cerr << "Array::add_var: Added variable " << v << " (" \
	 << v->get_var_name() << " " << v->get_var_type() << ")" << endl);
}

// When you want to allocate a buffer big enough to hold the entire array,
// in the local representation, allocate size() bytes.

unsigned int
Array::size()
{
  assert(var_ptr);

  unsigned int sz = 1;
  for (Pix p = first_dim(); p; next_dim(p)) 
      sz *= dimension_size(p); 

  return (sz * var_ptr->size());
}

// Serialize an array. This uses the BaseType member XDR_CODER to encode each
// element of the array. See Sun's XDR manual. 
//
// NB: The array must already be in BUF (in the local machine's
// representation) *before* this call is made.
//
// NB: For arrays of strings or urls, serialize will interate through the
// array and call xdr_str for each array element member instead of calling
// xdr_array with xdr_str function element.  

bool
Array::serialize(bool flush, unsigned int num)
{
    bool status = FALSE;
    int i;

    assert(buf);

    if (num == 0)  
      num = (size() / var_ptr->size());

    if ( var_ptr->get_var_type() == "Str" || var_ptr->get_var_type() == "Url" )
      {
	char **strBuf = (char **)buf;
	for ( i = 0; i < num; ++i )
	  {
	    status = (bool)xdr_str(xdrout, strBuf+i);
	    if ( status == FALSE ) break;
	  }
      }
    else
      {
	status = (bool)xdr_array(xdrout, (char **)&buf, &num, DODS_MAX_ARRAY, 
				  var_ptr->size(), var_ptr->xdr_coder());
      }

    if (status && flush)
	status = expunge();

    return status;

}

// NB: If you do not allocate any memory to BUF *and* ensure that BUF ==
// NULL, then deserialize will allocate the memory for you. However, it will
// do so using malloc so YOU MUST USE FREE, NOT DELETE, TO RELEASE IT. 
// You can avoid all this hassle by using the mfunc alloc_buf and free_buf
// in CtorType. You can use free_buf to free the contents of BUF when they
// were allocated by alloc_buf *or* deserialize (but *not* new).

unsigned int
Array::deserialize()
{
    bool status = FALSE;
    unsigned int num = 0;

    if ( var_ptr->get_var_type() == "Str" || var_ptr->get_var_type() == "Url" )
      {
	char **strBuf = (char **)buf;
	for ( int i = 0; i < (size()/var_ptr->size()); ++i )
	  {
	    //cout << "buf[" << i << "] = " << *(strBuf+i) << endl;
	    status = (bool)xdr_str(xdrin, strBuf+i);
	    if ( status == FALSE ) break;
	    else num++;
	  }
      }
    else
      {
	status = (bool)xdr_array(xdrin, (char **)&buf, &num, DODS_MAX_ARRAY, 
				 var_ptr->size(), var_ptr->xdr_coder());
      }

    return (status ? (num * var_ptr->size()) : (unsigned int)FALSE);
}

// Add the dimension DIM to the list of dimensions for this array. If NAME is
// given, set it to the name of this dimension. NAME defaults to "".
//
// Returns: void

void 
Array::append_dim(int size, String name)
{ 
    dimension d;
    d.size = size;
    d.name = name;
    shape.append(d); 
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

// Return the number of dimensions contained in the array.

unsigned int
Array::dimensions()
{
  unsigned int dim = 0;
  for( Pix p = first_dim(); p; next_dim(p)) dim ++;

  return dim;
}

// Return the size of the array dimension referred to by P.

int 
Array::dimension_size(Pix p) 
{ 
    if (!shape.empty() && p)
	return shape(p).size; 
}

// Return the name of the array dimension referred to by P.

String
Array::dimension_name(Pix p) 
{ 
    if (!shape.empty() && p)
	return shape(p).name; 
}

void
Array::print_decl(ostream &os, String space, bool print_semi)
{
    var_ptr->print_decl(os, space, false); // print it, but w/o semicolon

    for (Pix p = shape.first(); p; shape.next(p)) {
	os << "[";
	if (shape(p).name != "")
	    os << shape(p).name << "=";
	os << shape(p).size << "]";
    }

    if (print_semi)
	os << ";" << endl;
}

void 
Array::print_val(ostream &os, String space)
{
  int i, arrLength = size()/var_ptr->size();

  if ( var_ptr->get_var_type() == "Str" || var_ptr->get_var_type() == "Url" )
    {
      char **strBuf = (char **)buf;
      for( i = 0; i < arrLength; ++i ) 
	os << " = " << *(strBuf+i) << ";" << endl;
    }
  else if ( var_ptr->get_var_type() == "Int32" )
    {
      for( i = 0; i < arrLength; ++i )
	os << " = " << *(int *)(buf+i) << ";" << endl;
    }
  else if ( var_ptr->get_var_type() == "Float64" )
    {
      for( i = 0; i < arrLength; ++i )
	os << " = " << *(double *)(buf+(var_ptr->size()*i)) << ";" << endl;
    }
  else if ( var_ptr->get_var_type() == "Byte" )
    {
      for( i = 0; i < arrLength; ++i )
	os << " = " << *(char *)(buf+(var_ptr->size()*i)) << ";" << endl;
    }
}

bool
Array::check_semantics(bool all)
{
    bool sem = BaseType::check_semantics() && !shape.empty();

    if (!sem)
	cerr << "An array variable must have dimensions" << endl;

    return sem;
}








