
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
// Revision 1.31  1996/05/06 21:15:58  jimg
// Added the member functions dimension_start, _stop and _stride to this class.
// Changed the first argument of add_constraint from Pix &p to Pix p (the member
// function does not change its argument).
//
// Revision 1.30  1996/04/05 00:21:17  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.29  1996/03/05 18:48:24  jimg
// Fixed bugs in the print_array software.
// Fixed the ctor and related CE member functions (so that projections work).
//
// Revision 1.28  1996/02/01 22:22:42  jimg
// Merged changes between DODS-1.1 and DODS 2.x.
//
// Revision 1.27  1995/12/09  01:06:29  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.26  1995/12/06  21:40:58  jimg
// Added reset_constraint(), clear_constraint() and add_constraint() to manage
// the new constraint-related members (in struct dimension).
// Fixed variour member functions to return information about sizes,
// ... correctly both before and after constraints are set.
//
// Revision 1.25  1995/11/22  22:31:02  jimg
// Modified so that the Vector class is now the parent class.
//
// Revision 1.24  1995/10/23  23:20:44  jimg
// Added _send_p and _read_p fields (and their accessors) along with the
// virtual mfuncs set_send_p() and set_read_p().
//
// Revision 1.23  1995/08/26  00:31:21  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.22  1995/08/22  23:45:33  jimg
// Removed DBMALLOC code.
// Added set_vec/vec mfuncs so that non-Array software can access the
// BaseType * vector.
// Changed names read_val and store_val to buf2val and val2buf. The old names
// remain.
// removed the card member function: used the new _type enum with a switch in
// its place.
//
// Revision 1.21.2.4  1995/09/27 21:49:03  jimg
// Fixed casts.
//
// Revision 1.21.2.3  1995/09/27  19:06:56  jimg
// Add casts to `cast away' const and unsigned in places where we call various
// xdr functions (which don't know about, or use, const or unsigned.
//
// Revision 1.21.2.2  1995/09/14  20:59:50  jimg
// Fixed declaration of, and calls to, _duplicate() by changing the formal
// param from a pointer to a reference.
//
// Revision 1.21.2.1  1995/07/11  18:17:09  jimg
// Added cast of xdr_array (used in BaseType's constructor) to xdrproc_t.
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
// Added card() virtual function which is true for classes that
// contain cardinal types (byte, int float, string).
// Changed the representation of Str from the C rep to a C++
// class represenation.
// Chnaged read_val and store_val so that they take and return
// types that are stored by the object (e.g., inthe case of Str
// an URL, read_val returns a C++ String object).
// Modified Array representations so that arrays of card()
// objects are just that - no more storing strings, ... as
// C would store them.
// Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// an array of a structure is represented as an array of Structure
// objects).
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
Array::_duplicate(const Array &a)
{
    Vector::_duplicate(a);

    _shape = a._shape;
}

void
Array::update_length(int size)
{
    if (length() == -1)
	set_length(size);
    else
	set_length(length() * size);
}

// Construct an instance of Array. The (BaseType *) is assumed to be
// allocated using new - The dtor for Vector will delete this object.

Array::Array(const String &n, BaseType *v) : Vector(n, v, dods_array_c)
{
}

Array::Array(const Array &rhs)
{
    _duplicate(rhs);
}

Array::~Array()
{
}

const Array &
Array::operator=(const Array &rhs)
{
    if (this == &rhs)
	return *this;
    
    _duplicate(rhs);

    return *this;
}

// Add the dimension DIM to the list of dimensions for this array. If NAME is
// given, set it to the name of this dimension. NAME defaults to "".
//
// Sets Vector's length member as a side effect.
//
// Returns: void

void 
Array::append_dim(int size, String name)
{ 
    dimension d;

    // This is invariant
    d.size = size;
    d.name = name;

    // this information changes with each constraint expression
    d.start = 0; 
    d.stop = size -1;
    d.stride = 1;
    d.c_size = size;
    d.selected = true;		// assume all dims selected.

    _shape.append(d); 

    update_length(size);
}

// Reset the dimension contraint information so that the entire array is
// `selected'

void
Array::reset_constraint()
{
    set_length(-1);

    for (Pix p = _shape.first(); p; _shape.next(p)) {
	dimension d = _shape(p);
	
	d.start = 0;
	d.stop = d.size;
	d.stride = 1;
	d.c_size = d.size;

	d.selected = true;

	update_length(d.size);
    }
}

// Tell the Array object to clear the constraint information about
// dimensions. Do this *once* before calling add_constraint() for each new 
// constraint expression. Only the dimensions explicitly selected using
// Array::add_constraint(...) will be sent.

void
Array::clear_constraint()
{
    for (Pix p = _shape.first(); p; _shape.next(p)) {
	dimension d = _shape(p);
	
	d.start = 0;
	d.stop = 0;
	d.stride = 0;
	d.c_size = 0;
	d.selected = false;
    }

    set_length(-1);
}

// the start and stop indeces are inclusive.

void 
Array::add_constraint(Pix p, int start, int stride, int stop)
{
    dimension &d = _shape(p);

    d.start = start;
    d.stop = stop;
    d.stride = stride;

    d.c_size = (stop - start + 1) / stride;
    
    DBG(cerr << "add_constraint: c_size = " << d.c_size << endl);

    d.selected = true;

    update_length(d.c_size);
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

// Return the number of dimensions contained in the array. When CONSTRAINED
// is true, return the number of dimensions given the most recently evaluated
// constraint expression. By default, constraint is false.

unsigned int
Array::dimensions(bool constrained)
{
  unsigned int dim = 0;
  for(Pix p = first_dim(); p; next_dim(p)) 
      if (constrained) {
	  if (_shape(p).selected)
	      dim++;
      }
      else
	  dim++;

  return dim;
}

// Return the size of the array dimension referred to by P. If CONSTRAINED is
// true, return the size of this dimension giventhe current
// constraint. CONSTRAINED is false by default.

int 
Array::dimension_size(Pix p, bool constrained) 
{ 
    int size = 0;

    if (!_shape.empty() && p)
	if (constrained) {
	    if (_shape(p).selected)
		size = _shape(p).c_size;
	    else
		size = 0;
	}
	else
	    size = _shape(p).size; 

    return size;
}

int 
Array::dimension_start(Pix p, bool constrained = false) 
{ 
    int start = 0;

    if (!_shape.empty() && p)
	if (constrained) {
	    if (_shape(p).selected)
		start = _shape(p).start;
	    else
		start= 0;
	}
	else
	    start = _shape(p).start; 

    return start;
}

int 
Array::dimension_stop(Pix p, bool constrained = false) 
{ 
    int stop = 0;

    if (!_shape.empty() && p)
	if (constrained) {
	    if (_shape(p).selected)
		stop = _shape(p).stop;
	    else
		stop= 0;
	}
	else
	    stop = _shape(p).stop; 

    return stop;
}

int 
Array::dimension_stride(Pix p, bool constrained = false) 
{ 
    int stride = 0;

    if (!_shape.empty() && p)
	if (constrained) {
	    if (_shape(p).selected)
		stride = _shape(p).stride;
	    else
		stride= 0;
	}
	else
	    stride = _shape(p).stride; 

    return stride;
}

// Return the name of the array dimension referred to by P.

String
Array::dimension_name(Pix p) 
{ 
    if (!_shape.empty() && p)
	return _shape(p).name; 
}

void
Array::print_decl(ostream &os, String space, bool print_semi,
		  bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    // print it, but w/o semicolon
    var()->print_decl(os, space, false, constraint_info, constrained); 

    for (Pix p = _shape.first(); p; _shape.next(p)) {
	if (constrained && !_shape(p).selected)
	    continue;		// only print selected dimensions
	os << "[";
	if (_shape(p).name != "")
	    os << _shape(p).name << " = ";
	if (constrained)
	    os << _shape(p).c_size << "]";
	else
	    os << _shape(p).size << "]";
    }

    if (print_semi)
	os << ";" << endl;
}

// Print an array. This is a private memebr function.
//
// OS is the stream used for writing
// INDEX is the index of VEC to start printing
// DIMS is the number of dimensions in the array
// SHAPE is the sixe of the dimensions of the array.
//
// Returns: the number of elements written.

unsigned int
Array::print_array(ostream &os, unsigned int index, unsigned int dims, 
		   unsigned int shape[])
{
    if (dims == 1) {
	os << "{";
	for (int i = 0; i < shape[0]-1; ++i) {
	    var(index++)->print_val(os, "", false);
	    os << ", ";
	}
	var(index++)->print_val(os, "", false);
	os << "}";

	return index;
    }
    else {
	os << "{";
	for (int i = 0; i < shape[dims-1]-1; ++i) {
	    index = print_array(os, index, dims - 1, shape + 1);
	    os << "},";
	}
	index = print_array(os, index, dims - 1, shape + 1);
	os << "}";

	return index;
    }
}

// print the value given the current constraint.

void 
Array::print_val(ostream &os, String space, bool print_decl_p)
{
    // print the declaration if print decl is true.
    // for each dimension,
    //   for each element, 
    //     print the array given its shape, number of dimensions.
    // Add the `;'
    
    if (print_decl_p) {
	print_decl(os, space, false, false, false);
	os << " = ";
    }

    unsigned int dims = dimensions(true);
    unsigned int shape[dims];
    unsigned int i = 0;
    for (Pix p = first_dim(); p; next_dim(p))
	shape[i++] = dimension_size(p, true);

    print_array(os, 0, dims, shape);

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
