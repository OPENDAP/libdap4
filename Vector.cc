
// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for class Vector. This class is the basis for all the
// vector-type classes in DODS (Array, List). 
//
// 11/21/95 jhrg

// $Log: Vector.cc,v $
// Revision 1.17  1997/03/08 19:02:11  jimg
// Changed default param to check_semantics() from  to String()
// and removed the default from the argument list in the mfunc definition
//
// Revision 1.16  1997/02/28 01:28:03  jimg
// Changed check_semantics() so that it now returns error messages in a String
// object (passed by reference).
//
// Revision 1.15  1996/12/02 18:22:08  jimg
// Added cases for uint32 to various parts of Vector.
//
// Revision 1.14  1996/11/13 19:23:10  jimg
// Fixed debugging.
//
// Revision 1.13  1996/08/13 18:39:25  jimg
// Added __unused__ to definition of char rcsid[].
// Fixed int -vs- unsigned int discrepancies.
//
// Revision 1.12  1996/06/04 21:33:50  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.11  1996/05/31 23:30:42  jimg
// Updated copyright notice.
//
// Revision 1.10  1996/05/30 17:14:56  jimg
// Fixed the allocation of vectors of DODS variable objects; use the
// ptr_duplicate member function instead of the copy ctor in val2buf().
// Fixed allocation of vector of String in buf2val(). From Reza.
//
// Revision 1.9  1996/05/29 22:08:53  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.8  1996/05/16 22:49:53  jimg
// Dan's changes for version 2.0. Added a parameter to read that returns
// an error code so that EOF can be distinguished from an actual error when
// reading sequences. This *may* be replaced by an error member function
// in the future.
//
// Revision 1.7  1996/05/14 15:38:46  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.6  1996/04/05 00:22:09  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.5  1996/03/05 01:09:09  jimg
// Added to the Vector dtor (now the BaseType * vector is properly deleted.
// Created the vec_resize() member function.
// Modified serialize() member function so that the ce_eval flag is used.
//
// Revision 1.4  1996/02/01 17:43:14  jimg
// Added support for lists as operands in constraint expressions.
//
// Revision 1.3  1995/12/09  01:07:33  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.2  1995/12/06  19:52:26  jimg
// Modified print_decl() so that the declaration is printed only if the
// variable is selected.
//
// Revision 1.1  1995/11/22  22:30:18  jimg
// Created.
//

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: Vector.cc,v 1.17 1997/03/08 19:02:11 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "Vector.h"
#include "util.h"
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
    :BaseType(n, t), _length(-1), _var(v), _buf(0), _vec(0)
{
    DBG(cerr << "Entering Vector ctor for object: " << this << endl);
}

Vector::Vector(const Vector &rhs)
{
    DBG(cerr << "Entering Vector const ctor for object: " << this << endl);
    DBG(cerr << "RHS: " << &rhs << endl);

    _duplicate(rhs);
}

static void
delete_base_type(BaseType *bt)
{
    if (bt)
	delete bt;
}

Vector::~Vector()
{
    DBG(cerr << "Entering Vector dtor for object: " << this << endl);

    delete _var;

    if (_buf)
	delete[] _buf;
    else {
	_vec.apply(delete_base_type);
    }
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
      case dods_byte_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float64_c: {
	  // Transfer the ith value to the BaseType *_var; There are more
	  // efficient ways to get a whole array using buf2val() but this is
	  // an OK way to get a single value or several non-contiguous values.
	unsigned int sz = _var->width();
	_var->val2buf(_buf + (i * sz));
	return _var;
	break;
      }

      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
      case dods_list_c:
      case dods_structure_c:
      case dods_sequence_c:
      case dods_function_c:
      case dods_grid_c:
	return _vec[i];
	break;

      default:
	cerr << "Vector::var: Unrecognized type" << endl;
    }

    return 0;
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

int
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

void
Vector::vec_resize(int l)
{
    int s = _vec.capacity();
    _vec.resize((l > 0) ? l : 0);

    BaseType **cur = &_vec.elem(s);
    BaseType **fin = &_vec.elem(l-1);
    while (cur <= fin)
	*cur++ = 0;
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
Vector::serialize(const String &dataset, DDS &dds, XDR *sink,
		  bool ce_eval = true)
{
    bool status = true;
    int error = 0;

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    unsigned int num = length();

    switch (_var->type()) {
      case dods_byte_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float64_c:
	assert(_buf);

	if (!(bool)xdr_int(sink, &num)) // send vector length
	    return false;

	if (_var->type() == dods_byte_c)
	    status = (bool)xdr_bytes(sink, (char **)&_buf, &num,
				     DODS_MAX_ARRAY); 
	else
	    status = (bool)xdr_array(sink, (char **)&_buf, &num,
				     DODS_MAX_ARRAY, _var->width(),
				     _var->xdr_coder()); 
	break;

      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
      case dods_list_c:
      case dods_structure_c:
      case dods_sequence_c:
      case dods_function_c:
      case dods_grid_c:
	assert(_vec.capacity());

	status = (bool)xdr_int(sink, &num); // send length
	if (!status)
	    return status;

	for (unsigned i = 0; status && i < num; ++i)	// test status in loop
	    status = _vec[i]->serialize(dataset, dds, sink, false);
	
	break;

      default:
	cerr << "Vector::serialize: Unknow type\n";
	status = false;
	break;
    }

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
Vector::deserialize(XDR *source, bool reuse = false)
{
    bool status;
    unsigned int num;

    switch (_var->type()) {
      case dods_byte_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float64_c:
	if (_buf && !reuse) {
	    delete[] _buf;
	    _buf = 0;
	}

	status = (bool)xdr_int(source, &num);
	if (!status)
	    return status;
	    
	set_length(num);	// set the length for this instance of the list

	if (!_buf) {
	    _buf = new char[width()]; // we always do the allocation!
	    DBG(cerr << "List::deserialize: allocating " \
		<< width() << " bytes for an array of " \
		<< length() << " " << _var->type_name() << endl);
	}

	if (_var->type() == dods_byte_c)
	    status = (bool)xdr_bytes(source, (char **)&_buf, &num,
				     DODS_MAX_ARRAY); 
	else
	    status = (bool)xdr_array(source, (char **)&_buf, &num,
				     DODS_MAX_ARRAY, _var->width(), 
				     _var->xdr_coder());
	DBG(cerr << "List::deserialize: read " << num <<  " elements\n");

	break;

      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
      case dods_list_c:
      case dods_structure_c:
      case dods_sequence_c:
      case dods_function_c:
      case dods_grid_c:
	status = (bool)xdr_int(source, &num);
	if (!status)
	    return status;

	vec_resize(num);
	set_length(num);

	for (unsigned i = 0; status && i < num; ++i) {
	    _vec[i] = _var->ptr_duplicate();
	    _vec[i]->deserialize(source);
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
      case dods_byte_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float64_c: {
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

      case dods_str_c:
      case dods_url_c: {
	unsigned elem_wid = _var->width();
	unsigned len = length();

	vec_resize(len);

 	for (unsigned i = 0; i < len; ++i) {
	    _vec[i] = _var->ptr_duplicate(); //changed, reza
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
      case dods_byte_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float64_c:
	if (!*val)
	    *val = new char[wid];

	(void)memcpy(*val, _buf, wid);

	break;

      case dods_str_c:
      case dods_url_c: {
	unsigned int elem_wid = _var->width();
	unsigned int len = length();

 	if (!*val)
	    *val = new String [len]; 

	for (unsigned i = 0; i < len; ++i) {
	    void *val_elem = *val + i * elem_wid;
	    _vec[i]->buf2val(&val_elem);
	}

	break;
      }

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
	vec_resize(i + 10);

    _vec.elem(i) = val;
}
 
BaseType *
Vector::var(const String &)
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
Vector::add_var(BaseType *v, Part)
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
Vector::check_semantics(String &msg, bool) 
{
    return BaseType::check_semantics(msg);
}
