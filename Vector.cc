
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

// Implementation for class Vector. This class is the basis for all the
// vector-type classes in DODS (Array, List). 
//
// 11/21/95 jhrg

// $Log: Vector.cc,v $
// Revision 1.4  1996/02/01 17:43:14  jimg
// Added support for lists as operands in constraint expressions.
//
// Revision 1.3  1995/12/09  01:07:33  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.2  1995/12/06  19:52:26  jimg
// Modified print_decl() so that the declaration is printed only if the variable
// is selected.
//
// Revision 1.1  1995/11/22  22:30:18  jimg
// Created.
//

static char rcsid[]= {"$Id: Vector.cc,v 1.4 1996/02/01 17:43:14 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "Vector.h"
#include "DDS.h"
#include "util.h"
#include "errmsg.h"
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void
Vector::_duplicate(const Vector &v)
{
    BaseType::_duplicate(v);

    _length = v._length;
    _var = v._var->ptr_duplicate(); // use ptr_duplicate() 

    _vec = v._vec;		// use BaseTypeVec's copy.

    _buf = 0;			// init to null
    if (v._buf)			// only copy if data present
	val2buf(v._buf);	// store this's value in v's _BUF.
}

Vector::Vector(const String &n, BaseType *v, const Type &t) 
    :BaseType(n, t), _length(-1), _var(v), _buf(0)
{
}

Vector::Vector(const Vector &rhs)
{
    _duplicate(rhs);
}

Vector::~Vector()
{
    delete _var;

    if (_buf)
	delete[] _buf;
}

const Vector &
Vector::operator=(const Vector &rhs)
{
    if (this == &rhs)
	return *this;
    
    _duplicate(rhs);

    return *this;
}

// These mfuncs set the _send_p and _read_p fields of BaseType. They differ
// from BaseType's version in that they set both the Vector object's copy of
// _send_p (_read_p) but also _VAR's copy. This does not matter much when _VAR
// is a scalar, but does matter when it is an aggregate.

void
Vector::set_send_p(bool state)
{
    _var->set_send_p(state);
    BaseType::set_send_p(state);
}

void 
Vector::set_read_p(bool state)
{
    _var->set_read_p(state);
    BaseType::set_read_p(state);
}

// Return a pointer the the BaseType object for element I. If the Vector is
// of a cardinal type, store the Ith element's value in the BaseType
// object. If it is a Vector of a non-cardinal type, then this mfunc returns
// _vec[i]. 
//
// NB: I defaults to zero.
//
// Returns: A BaseType pointer to the Ith element of the Vector.

BaseType *
Vector::var(unsigned int i)
{
    
    switch (_var->type()) {
      case byte_t:
      case int32_t:
      case float64_t: {
	unsigned int sz = _var->width();
	_var->val2buf(_buf + (i * sz));
	return _var;
	break;
      }

      case str_t:
      case url_t:
      case array_t:
      case list_t:
      case structure_t:
      case sequence_t:
      case function_t:
      case grid_t:
	return _vec[i];
	break;

      default:
	cerr << "Vector::var: Unrecognized type" << endl;
    }
}

// Return: The number of bytes required to store the vector `in a C
// program'. For an array of cardinal types this is the same as the storage
// used by _BUF. For anything else, it is the product of length() and the
// element width(). It turns out that both values can be computed the same
// way. 
//
// Returns: The number of bytes used to store the vector.

unsigned int
Vector::width()
{
    assert(_var);

    return length() * _var->width();
}

// Returns: the number of elements in the vector. 

unsigned int
Vector::length()
{
    return _length;
}

// set the number of elements in the vector.
//
// Returns: void

void
Vector::set_length(int l)
{
    _length = l;
}

// Serialize a Vector. This uses the BaseType member XDR_CODER to encode each
// element of a cardinal array. See Sun's XDR manual. For Arrays of Str and
// Url types, send the element count over as a prefix to the data so that
// deserialize will know how many elements to read.
//
// The boolean parameter FLUSH determines whether the data stream is flushed
// after this call or not. If FLUSH is false, then the stream is flushed when
// its local buffer fills. if it is true, then it is flushed at the end of
// this call.
//
// NB: Arrays of cardinal types must already be in BUF (in the local machine's
// representation) *before* this call is made.
//
// Returns: true if the data was successfully writen, false otherwise.

bool
Vector::serialize(const String &dataset, DDS &dds, bool flush)
{
    bool status = true;

    if (!read_p())		// only read if not read already
	status = read(dataset);

    if (status && !dds.eval_constraint()) // if the constraint is false, return
	return status;

    if (!status)
	return false;

    unsigned int num = length();

    switch (_var->type()) {
      case byte_t:
      case int32_t:
      case float64_t:
	assert(_buf);

	status = (bool)xdr_int(xdrout(), &num); // send length
	if (!status)
	    return status;

	if (_var->type() == byte_t)
	    status = (bool)xdr_bytes(xdrout(), (char **)&_buf, &num,
				     DODS_MAX_ARRAY); 
	else
	    status = (bool)xdr_array(xdrout(), (char **)&_buf, &num,
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

	status = (bool)xdr_int(xdrout(), &num); // send length
	if (!status)
	    return status;

	for (int i = 0; status && i < num; ++i)	// test status in loop
	    status = _vec[i]->serialize(dataset, dds, false);
	
	break;

      default:
	cerr << "Vector::serialize: Unknow type\n";
	status = false;
	break;
    }

    if (status && flush)
	status = expunge();

    return status;
}

// Read an object from the network and internalize it. For a Vector this is
// handled differently for a `cardinal' type. Vectors of Cardinals are
// stored using the `C' representations because these object often are used
// to build huge arrays (e.g., an array of 1024 by 1024 bytes). However,
// arrays of non-cardinal types are stored as Vectors of the C++ objects or
// DODS objects (Str and Url are vectors of the String class, Structure, ...,
// Grid are vectors of the DODS Structure, ... classes). 
//
// The boolean parameter REUSE determines whether internal storage is reused
// or not. If true, the _buf member is assumed to be large enough to hold the
// incoming cardinal data and is *not* reallocated. If false, new storage is
// allocated. If the internal buffer has not yet been allocated, then this
// parameter has no effect (i.e., storage is allocated). This parameter
// effects storage for cardinal data only.
//
// Returns: True is successful, false otherwise.

bool
Vector::deserialize(bool reuse)
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

	status = (bool)xdr_int(xdrin(), &num);
	if (!status)
	    return status;
	    
	set_length(num);	// set the length for this instance of the list

	if (!_buf) {
	    _buf = new char[width()]; // we always do the allocation!
	    DBG(cerr << "List::deserialize: allocating " \
		<< width() << " bytes for an array of " \
		<< length() << " " << _var->type_name() << endl);
	}

	if (_var->type() == byte_t)
	    status = (bool)xdr_bytes(xdrin(), (char **)&_buf, &num,
				     DODS_MAX_ARRAY); 
	else
	    status = (bool)xdr_array(xdrin(), (char **)&_buf, &num,
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
	status = (bool)xdr_int(xdrin(), &num);
	if (!status)
	    return status;

	_vec.resize(num);
	set_length(num);

	for (int i = 0; status && i < num; ++i) {
	    _vec[i] = _var;	// init to empty object of correct class
	    _vec[i]->deserialize();
	}

	break;

      default:
	cerr << "Vector::deserialize: Unknow type\n";
	status = false;
	break;
    }

    return status;
}

// copy contents of VAL to the internal buffer (val to buf)
//
// Assume that VAL points to memory which contains, in row major order,
// enough elements of the correct type to fill the array. For an array of a
// cardinal type they are memcpy'd into _buf, otherwise val2buf is called
// length() times on each successive _var->width() piece of VAL.
//
// Returns: The number of bytes used by the array

unsigned int
Vector::val2buf(void *val, bool reuse)
{
    assert(val);

    switch (_var->type()) {
      case byte_t:
      case int32_t:
      case float64_t: {
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
      }

      case str_t:
      case url_t: {
	unsigned int elem_wid = _var->width();
	unsigned int len = length();

	_vec.resize(len);

	for (int i = 0; i < len; ++i) {
	    _vec[i] = _var;	// init with empty instance of correct class
	    _vec[i]->val2buf(val + i * elem_wid, reuse);
	}

	break;
      }

      default:
	cerr << "Array::val2buf: Can be called for arrays of Byte, Int32, \n"
	  << "Float64, String and Url only.\n";
	return 0;
    }

    return width();
}

// copy contents of the internal buffer to VAL (buf to val).
//
// In the case of a cardinal type, assume that val points to an array large
// enough to hold N instances of the `C' representation of the element type.
// In the case of a non-cardinal type, assume that val points to an array
// loarge enough to hold N instances of the DODS class used to represent that
// type. 
//
// Returns: The number of bytes used to store the array.
 
unsigned int
Vector::buf2val(void **val)
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
Vector::set_vec(int i, BaseType *val)
{
    assert(i > -1);
    assert(val);

    if (i >= _vec.capacity())
	_vec.resize(i + 10);

    _vec.elem(i) = val;
}
 
BaseType *
Vector::var(const String &name)
{
    return _var;
#ifdef NEVER
    if (_var->name() == name)
	return _var;
    else
	return _var->var(name);
#endif
}

// Add the BaseType pointer to this ctor type instance. Propagate the name of
// the BaseType instance to this instance. This ensures that variables at any
// given level of the DDS table have unique names (i.e., that Arrays do not
// have their default name "").
// NB: Part p defaults to nil for this class

void
Vector::add_var(BaseType *v, Part p)
{
    _var = v;
    set_name(v->name());	// Vector name == Base object's name

    DBG(cerr << "Vector::add_var: Added variable " << v << " (" \
	<< v->name() << " " << v->type_name() << ")" << endl);
}

void
Vector::print_decl(ostream &os, String space, bool print_semi,
		  bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    os << space << type_name();
    var()->print_decl(os, " ", print_semi, constraint_info, constrained);
}

void 
Vector::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";

    unsigned int i;
    unsigned int l = length();

    for (i = 0; i < l-1; ++i) {
	var(i)->print_val(os, "", false);
	os << ", ";
    }

    var(i)->print_val(os, "", false);

    if (print_decl_p)
	os << "};" << endl;
    else
	os << "}";
}

bool
Vector::check_semantics(bool all) 
{
    return BaseType::check_semantics();
}
