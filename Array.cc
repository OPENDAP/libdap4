
/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

// Implementation for Array.
//
// jhrg 9/13/94

// $Log: Array.cc,v $
// Revision 1.22  1995/08/22 23:45:33  jimg
// Removed DBMALLOC code.
// Added set_vec/vec mfuncs so that non-Array software can access the BaseType *
// vector.
// Changed names read_val and store_val to buf2val and val2buf. The old names
// remain.
// removed the card member function: used the new _type enum with a switch in
// its place.
//
// Revision 1.21  1995/07/09  21:28:48  jimg
// Added copyright notice.
//
// Revision 1.20  1995/05/10  15:33:52  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.19  1995/05/10  13:45:01  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.18  1995/04/28  19:53:46  reza
// First try at adding constraints capability.
// Enforce a new size calculated from constraint expression.
//
// Revision 1.17  1995/03/16  17:22:58  jimg
// Added include of config_dap.h before all other includes.
// Fixed deletes of buffers in read_val().
// Added initialization of _buf in ctor.
//
// Revision 1.16  1995/03/04  14:34:40  jimg
// Major modifications to the transmission and representation of values:
// 	Added card() virtual function which is true for classes that
// 	contain cardinal types (byte, int float, string).
// 	Changed the representation of Str from the C rep to a C++
// 	class represenation.
// 	Chnaged read_val and store_val so that they take and return
// 	types that are stored by the object (e.g., inthe case of Str
// 	an URL, read_val returns a C++ String object).
// 	Modified Array representations so that arrays of card()
// 	objects are just that - no more storing strings, ... as
// 	C would store them.
// 	Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// 	an array of a structure is represented as an array of Structure
// 	objects).
//
// Revision 1.15  1995/02/10  02:22:54  jimg
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
// Revision 1.14  1995/01/19  20:05:21  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.13  1995/01/11  15:54:39  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.12  1994/12/19  20:52:45  jimg
// Minor modifications to the print_val mfunc.
//
// Revision 1.11  1994/12/16  20:13:31  dan
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

#include "config_dap.h"

#include <assert.h>

#include "Array.h"
#include "util.h"
#include "errmsg.h"
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void
Array::_duplicate(const Array *a)
{
    set_name(a->name());
    set_type(a->type());

    _const_length = a->_const_length;
    _shape = a->_shape;
    _var = a->_var->ptr_duplicate();
    _vec = a->_vec;
    
    _buf = 0;    
    // copy the buffer's contents if there are any
    if (a->_buf) {
	val2buf(a->_buf);
    }
}

// Construct an instance of Array. The (BaseType *) is assumed to be
// allocated using new -- The dtor for Array will delete this object.

Array::Array(const String &n, BaseType *v)
     : BaseType(n, array_t), _var(v), _buf(0), _const_length(-1)
{
}

Array::Array(const Array &rhs)
{
    _duplicate(&rhs);
}

Array::~Array()
{
    delete _var;

    if (_buf)
	delete[] _buf;
}

const Array &
Array::operator=(const Array &rhs)
{
    if (this == &rhs)
	return *this;
    
    _duplicate(&rhs);

    return *this;
}

#ifdef NEVER
bool
Array::card()
{
    return false;
}
#endif

// set the length (number of elements) after constraint evaluation

void
Array::const_length(long const_len)
{
    _const_length = const_len;
}

#ifdef NEVER
// Returns: The number of bytes required to store the array's value.

unsigned int
Array::size()			// deprecated
{
    return width();
}
#endif

// Return: The number of bytes required to store the array `in a C
// program'. For an array of cardinal types this is the same as the storage
// used by _buf. For anything else, it is the product of length() and the
// element width(). It turns out that both values can be computed the same
// way. 

unsigned int
Array::width()
{
    return length() * _var->width();
}

// Returns: The number of elements in the array.

unsigned int
Array::length()
{
    assert(_var);

    unsigned int sz = 1;

    if(_const_length >= 0) {
      sz = (unsigned int) _const_length;
    }
    else {
      for (Pix p = first_dim(); p; next_dim(p)) 
	sz *= dimension_size(p); 
    }

    return sz;
}

// Serialize an array. This uses the BaseType member XDR_CODER to encode each
// element of the array. See Sun's XDR manual. For Arrays of Str and Url
// types, send the element count over as a prefix to the data so that
// deserialize will know how many elements to read.
//
// NB: The array must already be in BUF (in the local machine's
// representation) *before* this call is made.

bool
Array::serialize(bool flush)
{
    bool status;
    unsigned int num = length();

    switch (_var->type()) {
      case byte_t:
      case int32_t:
      case float64_t:

	assert(_buf);
	if (_var->type() == byte_t)
	    status = (bool)xdr_bytes(_xdrout, (char **)&_buf, &num,
				     DODS_MAX_ARRAY); 
	else
	    status = (bool)xdr_array(_xdrout, (char **)&_buf, &num,
				     DODS_MAX_ARRAY, _var->width(),
				     _var->xdr_coder()); 
	break;

      case str_t:
      case url_t:
      case array_t:
      case list_t:
      case structure_t:
      case sequence_t:
      case function_t:
      case grid_t:

	assert(_vec.capacity());
	status = (bool)xdr_int(_xdrout, &num); // send length

	for (int i = 0; status && i < num; ++i)	// test status in loop
	    status = _vec[i]->serialize();

	break;

      default:
	cerr << "Array::serialize: Unknown type\n";
	status = false;
    }

    if (status && flush)
	status = expunge();

    return status;
}

// Read an object from the network and internalize it. For an array this is
// handled differently for an array of a `cardinal' type. Cardinal arrays are
// stored using the `C' representations because these object often are used
// to build huge arrays (e.g., an array of 1024 by 1024 bytes). However,
// arrays of non-cardinal types are stored as Vectors of the C++ objects or
// DODS objects (Str and Url are vectors of the String class, Structure, ...,
// Grid are vectors of the DODS Structure, ... classes). 

bool
Array::deserialize(bool reuse)
{
    bool status;
    unsigned int num;

    switch (_var->type()) {
      case byte_t:
      case int32_t:
      case float64_t:

	if (_buf && !reuse) {
	    delete[] _buf;
	    _buf = 0;
	}

	if (!_buf) {
	    _buf = new char[width()]; // we always do the allocation!
	    DBG(cerr << "List::deserialize: allocating " \
		<< width() << " bytes for an array of " \
		<< length() << " " << _var->type_name() << endl);
	}

	if (_var->type() == byte_t)
	    status = (bool)xdr_bytes(_xdrin, (char **)&_buf, &num,
				     DODS_MAX_ARRAY); 
	else
	    status = (bool)xdr_array(_xdrin, (char **)&_buf, &num,
				     DODS_MAX_ARRAY, _var->width(), 
				     _var->xdr_coder());
	DBG(cerr << "List::deserialize: read " << num <<  " elements\n");

	break;

      case str_t:
      case url_t:
      case array_t:
      case list_t:
      case structure_t:
      case sequence_t:
      case function_t:
      case grid_t:

	status = (bool)xdr_int(_xdrin, &num);
	_vec.resize(num);
	for (int i = 0; status && i < num; ++i) {
	    _vec[i] = _var;	// init to empty object of correct class
	    _vec[i]->deserialize();
	}
	
	break;

      default:
	cerr << "Array::deserialize: Unknown type\n";
	status = false;
    }

    return status;
}

// copy contents of VAL to the internal buffer (val2buf)

// Assume that VAL points to memory which contains, in row major order,
// enough elements of the correct type to fill the array. For an array of a
// cardinal type they are memcpy'd into _buf, otherwise store_val is called
// length() times on each successive _var->width() piece of VAL.

unsigned int
Array::store_val(void *val, bool reuse)
{
    return val2buf(val, reuse);
}

unsigned int
Array::val2buf(void *val, bool reuse)
{
    assert(val);

    switch (_var->type()) {
      case byte_t:
      case int32_t:
      case float64_t:

	unsigned int array_wid = width();

	if (_buf && !reuse) {
	    delete[] _buf;
	    _buf = 0;
	}

	if (!_buf) {		// First time or no reuse (free'd above)
	    _buf = new char[array_wid];
	    memcpy(_buf, val, array_wid);
	}
	else { 
	    memcpy(_buf, val, array_wid);
	}

	break;

      case str_t:
      case url_t:

	unsigned int elem_wid = _var->width();
	unsigned int len = length();

	_vec.resize(len);

	for (int i = 0; i < len; ++i) {
	    _vec[i] = _var;	// init with empty instance of correct class
	    _vec[i]->val2buf(val + i * elem_wid, reuse);
	}

	break;

      default:
	cerr << "Array::val2buf: Can be called for arrays of Byte, Int32, \n"
	  << "Float64, String and Url only.\n";
	return 0;
    }

    return width();
}

// copy contents of the internal buffer to VAL (buf2val).
//
// In the case of a cardinal type, assume that val points to an array large
// enough to hold N instances of the `C' representation of the element type.
// In the case of a non-cardinal type, assume that val points to an array
// loarge enough to hold N instances of the DODS class used to represent that
// type. 
 
unsigned int
Array::read_val(void **val)
{
    return buf2val(val);
}

unsigned int
Array::buf2val(void **val)
{
    assert(val);

    unsigned int wid = width();

    switch (_var->type()) {
      case byte_t:
      case int32_t:
      case float64_t:

	if (!*val)
	    *val = new char[wid];

	(void)memcpy(*val, _buf, wid);

	break;

      case str_t:
      case url_t:

	unsigned int elem_wid = _var->width();
	unsigned int len = length();

	for (int i = 0; i < len; ++i) {
	    void *val_elem = *val + i * elem_wid;
	    _vec[i]->buf2val(&val_elem);
	}

	break;

      default:
	cerr << "Array::buf2val: Can be called for arrays of Byte, Int32, \n"
	  << "Float64, String and Url only.\n";
	return 0;
    }

    return wid;
}

// Given an index I into the _vec BaseType * vector, set the Ith element to
// VAL. 
//
// Returns: void

void 
Array::set_vec(int i, BaseType *val)
{
    assert(i > -1);
    assert(val);

    if (i >= _vec.capacity())
	_vec.resize(i + 10);

    _vec.elem(i) = val;
}
 
// Given the the index I, return the Ith element of the BaseType * vector
// _vec.
//
// Returns: the Ith element of _vec if the index is within range, otherwise
// causes an error

BaseType *
Array::vec(int i)
{
    return _vec[i];
}

// NAME defaults to NULL. It is present since the definition of this mfunc is
// inherited from BaseType, which declares it like this since some ctor types
// have several member variables.

BaseType *
Array::var(const String &name)
{
    return _var;
}

// Add the BaseType pointer to this ctor type instance. Propagate the name of
// the BaseType instance to this instance. This ensures that variables at any
// given level of the DDS table have unique names (i.e., that Arrays do not
// have their default name "").
// NB: Part p defaults to nil for this class

void
Array::add_var(BaseType *v, Part p)
{
    if (_var)
	err_quit("Array::add_var: Attempt to overwrite base type of an array");

    _var = v;
    set_name(v->name());

    DBG(cerr << "Array::add_var: Added variable " << v << " (" \
	 << v->name() << " " << v->type_name() << ")" << endl);
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
    _shape.append(d); 
}

Pix 
Array::first_dim() 
{ 
    return _shape.first();
}

void 
Array::next_dim(Pix &p) 
{ 
    if (!_shape.empty() && p)
	_shape.next(p); 
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
    if (!_shape.empty() && p)
	return _shape(p).size; 
}

// Return the name of the array dimension referred to by P.

String
Array::dimension_name(Pix p) 
{ 
    if (!_shape.empty() && p)
	return _shape(p).name; 
}

void
Array::print_decl(ostream &os, String space, bool print_semi)
{
    _var->print_decl(os, space, false); // print it, but w/o semicolon

    for (Pix p = _shape.first(); p; _shape.next(p)) {
	os << "[";
	if (_shape(p).name != "")
	    os << _shape(p).name << " = ";
	os << _shape(p).size << "]";
    }

    if (print_semi)
	os << ";" << endl;
}

// Print the values of a array to OS. The array must be of a cardinal type
// (i.e., the card() mfunc returns true). In order to print each element,
// temporarily store that element's value in the instance _VAR. Then call
// print_val() on _VAR. This ensures that the proper printed representation
// will be used without knowing the type of array's elements. Given that
// there can be an infinite number of types (using Structure, ...) this is
// important!
//
// OS is the C++ stream for writing.
// VAR is a pointer to an instance of this array's element type (e.g., Int32)
// ARRAY is a pointer to the array contents (stored as `C' would store them)
// DIMS is the number of dimentsions
// SHAPE is an array of the dimension sizes (for a 3-d array SHAPE[0] is the
// size of the first dimension, ..., SHAPE[2] is the size fo the third dim.
//
// Returns: the number of (cardinal) elements written to OS.

static unsigned int
print_card_array(ostream &os, BaseType *var, void *array, unsigned int dims, 
		 unsigned int shape[])
{
    unsigned int iarray = (unsigned int)array;

    if (dims == 1) {
	os << "{";
	for (int i = 0; i < shape[0]-1; ++i) {
	    array += var->val2buf(array);
	    var->print_val(os, "", false);
	    os << ", ";
	}
	array += var->val2buf(array);
	var->print_val(os, "", false);
	os << "}";
	return (unsigned int)array - iarray;
    }
    else {
	os << "{";
	for (int i = 0; i < shape[dims-1]-1; ++i) {
	    array += print_card_array(os, var, array, dims - 1, shape + 1);
	    os << "},";
	}
	array += print_card_array(os, var, array, dims - 1, shape + 1);
	os << "}";
	return (unsigned int)array - iarray;
    }
}

// Print an array. The array must be of a non-cardinal type (e.g., String,
// Structure, ...).
//
// OS is the stream used for writing
// VEC is the vector used to represent the values.
// INDEX is the index of VEC to start printing
// DIMS is the number of dimensions in the array
// SHAPE is the sixe of the dimensions of the array.
//
// Returns: the number of elements written.

static unsigned int
print_vec_array(ostream &os, BaseTypePtrVec &vec, unsigned int index,
		unsigned int dims, unsigned int shape[])
{
    if (dims == 1) {
	os << "{";
	for (int i = 0; i < shape[0]-1; ++i) {
	    vec[index++]->print_val(os, "", false);
	    os << ", ";
	}
	vec[index++]->print_val(os, "", false);
	os << "}";

	return index;
    }
    else {
	os << "{";
	for (int i = 0; i < shape[dims-1]-1; ++i) {
	    index += print_vec_array(os, vec, index, dims - 1, shape + 1);
	    os << "},";
	}
	index += print_vec_array(os, vec, index, dims - 1, shape + 1);
	os << "}";

	return index;
    }
}

void 
Array::print_val(ostream &os, String space, bool print_decl_p)
{
    // print the declaration if print decl is true.
    // for each dimension,
    //   for each element, 
    //     call store_val and then call print_val with PRINT_DECL false.
    // Add the `;'
    
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    unsigned int dims = dimensions();
    unsigned int shape[dims];
    unsigned int i = 0;
    for (Pix p = first_dim(); p; next_dim(p))
	shape[i++] = dimension_size(p);

    switch (_var->type()) {
      case byte_t:
      case int32_t:
      case float64_t:
	print_card_array(os, _var, _buf, dims, shape);
	break;

      case str_t:
      case url_t:
      case array_t:
      case list_t:
      case structure_t:
      case sequence_t:
      case function_t:
      case grid_t:
	print_vec_array(os, _vec, 0, dims, shape);
	break;

      default:
	cerr<< "Array::print_val: Unrecognized type" << endl;
    }

    if (print_decl_p) {
	os << ";" << endl;
    }
}

bool
Array::check_semantics(bool all)
{
    bool sem = BaseType::check_semantics() && !_shape.empty();

    if (!sem)
	cerr << "An array variable must have dimensions" << endl;

    return sem;
}
