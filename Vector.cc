
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for class Vector. This class is the basis for all the
// vector-type classes in DODS (Array, List). 
//
// 11/21/95 jhrg

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: Vector.cc,v 1.35 2000/10/06 01:26:05 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <algorithm>

#include "Vector.h"
#include "util.h"
#include "debug.h"
#include "InternalErr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

void
Vector::_duplicate(const Vector &v)
{
    BaseType::_duplicate(v);

    _length = v._length;
    _var = v._var->ptr_duplicate(); // use ptr_duplicate() 

    if (v._vec.size() == 0)
	_vec = v._vec;
    else {
	for (unsigned int i = 0; i < _vec.size(); ++i)
	    _vec[i] = v._vec[i];
    }

    _buf = 0;			// init to null
    if (v._buf)			// only copy if data present
	val2buf(v._buf);	// store this's value in v's _BUF.
}

Vector::Vector(const string &n, BaseType *v, const Type &t) 
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

Vector::~Vector()
{
    DBG(cerr << "Entering ~Vector (" << this << ")" << endl);

    delete _var;

    if (_buf)
	delete[] _buf;
    else	
	for (unsigned int i = 0; i < _vec.size(); ++i)
	    delete _vec[i];

    DBG(cerr << "Exiting ~Vector" << endl);
}

const Vector &
Vector::operator=(const Vector &rhs)
{
    if (this == &rhs)
	return *this;
    
    _duplicate(rhs);

    return *this;
}

int
Vector::element_count(bool leaves)
{
    if (!leaves)
	return 1;
    else
	// var() only works for simple types! 
	return var(0)->element_count(leaves);
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

BaseType *
Vector::var(const string &name, bool exact_match)
{
    // Make sure to check for the case where name is the default (the empty
    // string). 9/1/98 jhrg
    if (_var->is_constructor_type()) {
	if (name == "" || _var->name() == name)
	    return _var;
	else
	    return _var->var(name, exact_match);
    }
    else
	return _var;
}

BaseType *
Vector::var(const string &name, btp_stack &s)
{
    if (_var->is_constructor_type())
	return _var->var(name, s);
    else {
	s.push((BaseType *)this);
	return _var;
    }
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
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
      case dods_float64_c: {
	  // Transfer the ith value to the BaseType *_var; There are more
	  // efficient ways to get a whole array using buf2val() but this is
	  // an OK way to get a single value or several non-contiguous values.
	unsigned int sz = _var->width();
	_var->val2buf((char *)_buf + (i * sz));
	return _var;
	break;
      }

      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
      case dods_list_c:
      case dods_structure_c:
      case dods_sequence_c:
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
    // Jose Garcia
    if (!_var)
      throw InternalErr(__FILE__, __LINE__, 
	      "Cannot get width since *this* object is not holding data.");

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

// #l# is the number of elements the vector can hold (e.g., if l == 20, then
// the vector can hold elements 0, .., 19).

void
Vector::vec_resize(int l)
{
    _vec.resize((l > 0) ? l : 0, 0); // Fill with NULLs
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
Vector::serialize(const string &dataset, DDS &dds, XDR *sink, 
		  bool ce_eval)
{
    bool status = true;

    try {
      if (!read_p())
	read(dataset);
    }
    catch (Error &e) {
      return false;
    }

#if 0
    if (!read_p()) 
	read(dataset);
#endif

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    // length() is not capacity; it must be set explicitly in read().
    int num = length();

    switch (_var->type()) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
      case dods_float64_c:
	// Jose Garcia
	// If we are trying to serialize the data is because the user "hit"
	// DDS::send however the internal buffer is unset so that is because
	// the read method failed to do its job. This is outside the libdap++
	// and is rather a problem of how read is implemented in the
	// surrogate library.
	if(!_buf)
	    throw InternalErr(__FILE__, __LINE__, 
			      "Buffer pointer is not set.");

	if (!(bool)xdr_int(sink, (int *)&num)) // send vector length
	    return false;

	// Note that xdr_bytes and xdr_array both send the length themselves,
	// so the length is actually sent twice. 12/31/99 jhrg
	if (_var->type() == dods_byte_c)
	    status = (bool)xdr_bytes(sink, (char **)&_buf, 
				     (unsigned int *)&num,
				     DODS_MAX_ARRAY); 
	else
	    status = (bool)xdr_array(sink, (char **)&_buf,
				     (unsigned int *)&num,
				     DODS_MAX_ARRAY, _var->width(),
#ifdef WIN32
				     (xdrproc_t)(_var->xdr_coder())
#else
				     _var->xdr_coder()
#endif 
				     );
	break;

      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
      case dods_list_c:
      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	//Jose Garcia
	// I think not setting the capacity of _vec
	// must be an internal error, however I am not sure...
	if(_vec.capacity() == 0)
	    throw InternalErr(__FILE__, __LINE__, 
			      "The capacity of *this* vector is 0.");

	status = (bool)xdr_int(sink, (int *)&num); // send length
	if (!status)
	    return status;

	for (int i = 0; status && i < num; ++i)	// test status in loop
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
// stored using the `C' representations because these objects often are used
// to build huge arrays (e.g., an array of 1024 by 1024 bytes). However,
// arrays of non-cardinal types are stored as Vectors of the C++ objects or
// DODS objects (Str and Url are vectors of the string class, Structure, ...,
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
Vector::deserialize(XDR *source, DDS *dds, bool reuse)
{
    bool status;
    unsigned int num;
unsigned int i = 0;

    switch (_var->type()) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
      case dods_float64_c:
	if (_buf && !reuse) {
	    delete[] _buf;
	    _buf = 0;
	}

	status = (bool)xdr_int(source, (int *)&num);
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
#ifdef WIN32
					(xdrproc_t)(_var->xdr_coder()));
#else 
				     _var->xdr_coder());
#endif

	DBG(cerr << "Vector::deserialize: read " << num <<  " elements\n");

	break;

      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
      case dods_list_c:
      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	status = (bool)xdr_int(source, (int *)&num);
	if (!status)
	    return status;

	vec_resize(num);
	set_length(num);

	for (i = 0; status && i < num; ++i)
	    {
		_vec[i] = _var->ptr_duplicate();
		_vec[i]->deserialize(source, dds);
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
    // Jose Garcia

    // I *think* this method has been mainly designed to be use by read which
    // is implemented in the surrogate library. Passing NULL as a pointer to
    // this method will be an error of the creator of the surrogate library.
    // Even though I recognize the fact that some methods inside libdap++ can
    // call val2buf, I think by now no coding bugs such as missusing val2buf
    // will be in libdap++, so it will be an internal error from the
    // surrogate library.
    if(!val)
	throw InternalErr(__FILE__, __LINE__, 
			  "The incoming pointer does not contain any data.");

    switch (_var->type()) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
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
	    _vec[i]->val2buf((char *)val + i * elem_wid, reuse);
 	}

	break;
      }

      default:
		cerr <<
			"Array::val2buf: Can be called for arrays of Byte, Int16, Uint16, Int32," << 
			"Uint32, Float32, Float64, String and Url only.\n";
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
    // Jose Garcia
    // The same comment in Vector::val2buf applies here!
    if (!val)
	throw InternalErr(__FILE__, __LINE__, "NULL pointer.");
    
    int wid = width();

    switch (_var->type()) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
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
	    *val = new string [len]; 

	for (unsigned i = 0; i < len; ++i) {
	    void *val_elem = (char *)*val + i * elem_wid;
	    _vec[i]->buf2val(&val_elem);
	}

	break;
      }

      default:
		cerr <<
			"Array::buf2val: Can be called for arrays of Byte, Int16, Uint16, Int32," << 
			"Uint32, Float32, Float64, String and Url only.\n";
	return 0;
    }

    return wid;
}

// Given an index I into the _vec BaseType * vector, set the Ith element to
// VAL. 
//
// Returns: False if a type mis-match is detected, True otherwise.

void
Vector::set_vec(unsigned int i, BaseType *val)
{
    // Jose Garcia
    // This is a public method which allows users to set the elements
    // of *this* vector. Passing an invalid index, a NULL pointer or 
    // missmatching the vector type are internal errors.
    if(i<0)
	throw InternalErr(__FILE__, __LINE__, "Invalid data: wrong index.");
    if(!val)
	throw InternalErr(__FILE__, __LINE__, 
			  "Invalid data: null pointer to BaseType object.");

    if (val->type() != _var->type())
	throw InternalErr(__FILE__, __LINE__, 
  "invalid data: type of incoming object does not match *this* vector type.");

    if (i >= _vec.capacity())
	vec_resize(i + 10);
    _vec[i] = val;

}
 
// Add the BaseType pointer to this ctor type instance. Propagate the name of
// the BaseType instance to this instance. This ensures that variables at any
// given level of the DDS table have unique names (i.e., that Arrays do not
// have their default name "").
// NB: Part p defaults to nil for this class

void
Vector::add_var(BaseType *v, Part)
{
  // Jose Garcia
  // By getting a copy of this object to be assigned to _var
  // we let the owner of 'v' to deallocate it as necessary.
  _var = v->ptr_duplicate();
  set_name(v->name());	// Vector name == Base object's name
  
  DBG(cerr << "Vector::add_var: Added variable " << v << " (" \
      << v->name() << " " << v->type_name() << ")" << endl);
}

void
Vector::print_decl(ostream &os, string space, bool print_semi,
		  bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    os << space << type_name();
    var()->print_decl(os, " ", print_semi, constraint_info, constrained);
}

void 
Vector::print_val(ostream &os, string space, bool print_decl_p)
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
Vector::check_semantics(string &msg, bool) 
{
    return BaseType::check_semantics(msg);
}

// $Log: Vector.cc,v $
// Revision 1.35  2000/10/06 01:26:05  jimg
// Changed the way serialize() calls read(). The status from read() is
// returned by the Structure and Sequence serialize() methods; ignored by
// all others. Any exceptions thrown by read() are caught and discarded.
// serialize() returns false if read() throws an exception. This should
// be fixed once all the servers build using the new read() definition.
//
// Revision 1.34  2000/10/02 18:48:59  jimg
// Changed type of num in the serialize method to int
//
// Revision 1.33  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.32  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.31  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.30  2000/06/16 18:15:00  jimg
// Merged with 3.1.7
//
// Revision 1.27.6.2  2000/06/14 16:59:01  jimg
// Added instrumentation for the dtor.
//
// Revision 1.27.6.1  2000/06/07 23:08:31  jimg
// Added code to explicitly delete BaseType *s in _vec.
// Also tried recoding using DLList, but that didn't fix the problem I was
// after---fixed in the client code but decided to leave this is with #if 0
// just in case.
//
// Revision 1.29  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.28.8.1  2000/06/02 18:29:32  rmorris
// Mod's for port to Win32.
//
// Revision 1.28.2.2  2000/02/17 05:03:16  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.28.2.1  2000/01/28 22:14:07  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.28  2000/01/05 22:37:18  jimg
// Added a comment about the odd `protocol' for sending array/list lengths twice
// for arrays/lists of simple types.
// Removed some cruft.
//
// Revision 1.27  1999/05/04 19:47:23  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.26  1999/04/29 02:29:34  jimg
// Merge of no-gnu branch
//
// Revision 1.25  1999/03/24 23:34:49  jimg
// Added support for the new Int16, UInt16 and Float32 types.
//
// Revision 1.24  1998/09/17 17:05:46  jimg
// Changes for the new variable lookup scheme. Fields of ctor types no longer
// need to be fully qualified. my.thing.f1 can now be named `f1' in a CE. Note
// that if there are two `f1's in a dataset, the first will be silently used;
// There's no warning about the situation. The new code in the var member
// function passes a stack of BaseType pointers so that the projection
// information (send_p field) can be set properly.
//
// Revision 1.23.2.2  1999/02/05 09:32:35  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.23.2.1  1999/02/02 21:57:04  jimg
// String to string version
//
// Revision 1.23  1998/08/06 16:22:38  jimg
// Fixed the misuse of the read(...) member function. See Grid.c (from jeh).
//
// Revision 1.22  1998/03/17 17:51:06  jimg
// Added an implementation of element_count().
//
// Revision 1.21  1998/02/05 20:13:58  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.20  1997/12/31 21:48:12  jimg
// Enclosed print_basetype_pointer() function in #if DODS_DEBUG == 1 #endif
// to stop compiler warnings about it being unused.
//
// Revision 1.19  1997/12/15 22:33:00  jimg
// Added type checking set_vec. If the type of the element to include in the
// vector does not match the vector's type, set_vec() returns false.
//
// Revision 1.18  1997/09/22 22:37:53  jimg
// Fixed a bug in vec_resize.
//
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
// Added not_used to definition of char rcsid[].
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

