// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for class Vector. This class is the basis for all the
// vector-type classes in libdap's <Array, List>.
//
// 11/21/95 jhrg

#include "config.h"

#include <cstring>

//#define DODS_DEBUG

#include <sstream>
#include <vector>
#include <algorithm>

#include "Vector.h"
#include "Marshaller.h"
#include "UnMarshaller.h"

#include "dods-datatypes.h"
#include "escaping.h"
#include "util.h"
#include "debug.h"
#include "InternalErr.h"

using std::cerr;
using std::endl;

namespace libdap {

void Vector::_duplicate(const Vector & v)
{
    d_length = v.d_length;

    // _var holds the type of the elements. That is, it holds a BaseType
    // which acts as a template for the type of each element.
    if (v._var) {
        _var = v._var->ptr_duplicate(); // use ptr_duplicate()
        _var->set_parent(this); // ptr_duplicate does not set d_parent.
    }
    else {
        _var = 0;
    }

    // _vec and _buf (further down) hold the values of the Vector. The field
    // _vec is used when the Vector holds non-numeric data (including strings
    // although it used to be that was not the case jhrg 2/10/05) while _buf
    // holds numeric values.
    if (v._vec.empty()) {
        _vec = v._vec;
    }
    else {
        // Failure to set the size will make the [] operator barf on the LHS
        // of the assignment inside the loop.
        _vec.resize(d_length);
        for (int i = 0; i < d_length; ++i) {
            // There's no need to call set_parent() for each element; we
            // maintain the back pointer using the _var member. These
            // instances are used to hold _values_ only while the _var
            // field holds the type of the elements.
            _vec[i] = v._vec[i]->ptr_duplicate();
        }
    }

    // copy the strings. This copies the values.
    d_str = v.d_str;

    // copy numeric values if there are any.
    _buf = 0; // init to null
    if (v._buf) // only copy if data present
        val2buf(v._buf); // store v's value in this's _BUF.

    _capacity = v._capacity;
}

/**
 * @return whether the type of this Vector is a cardinal type
 * (ie stored in _buf)
 */
bool Vector::m_is_cardinal_type() const
{
    // Not cardinal if no _var at all!
    if (!_var) {
        return false;
    }

    switch (_var->type()) {
        case dods_byte_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_float32_c:
        case dods_float64_c: {
            return true;
            break;
        }

            // These must be handled differently.
        case dods_str_c:
        case dods_url_c:
        case dods_array_c:
        case dods_structure_c:
        case dods_sequence_c:
        case dods_grid_c:
            return false;
            break;

        default:
            cerr << "Vector::var: Unrecognized type" << endl;
            return false;
    } // switch
}

/**
 * Create _buf so that it can store numElts of the
 * (assumed) cardinal type.  This create storage for
 * width() * numElts bytes.
 * If _buf already exists, this DELETES IT and creates a new one.
 * So don't use this if you want to keep the original _buf data around.
 * This also sets the valueCapacity().
 * @param numEltsOfType the number of elements of the cardinal type in var()
 that we want storage for.
 * @return the size of the buffer created.
 * @exception if the Vector's type is not cardinal type.
 */
unsigned int Vector::m_create_cardinal_data_buffer_for_type(unsigned int numEltsOfType)
{
    // Make sure we HAVE a _var, or we cannot continue.
    if (!_var) {
        throw InternalErr(__FILE__, __LINE__, "create_cardinal_data_buffer_for_type: Logic error: _var is null!");
    }

    // Make sure we only do this for the correct data types.
    if (!m_is_cardinal_type()) {
        throw InternalErr(__FILE__, __LINE__, "create_cardinal_data_buffer_for_type: incorrectly used on Vector whose type was not a cardinal (simple data types).");
    }

    m_delete_cardinal_data_buffer();

    // Actually new up the array with enough bytes to hold numEltsOfType of the actual type.
    unsigned int bytesPerElt = _var->width();
    unsigned int bytesNeeded = bytesPerElt * numEltsOfType;
    _buf = new char[bytesNeeded];
    if (!_buf) {
        ostringstream oss;
        oss << "create_cardinal_data_buffer_for_type: new char[] failed to allocate " << bytesNeeded << " bytes!  Out of memory or too large a buffer required!";
        throw InternalErr(__FILE__, __LINE__, oss.str());
    }
    _capacity = numEltsOfType;
    return bytesNeeded;
}

/** Delete _buf and zero it and _capacity out */
void Vector::m_delete_cardinal_data_buffer()
{
    if (_buf) {
        delete[] _buf;
        _buf = 0;
        _capacity = 0;
    }
}

/** Helper to reduce cut and paste in the virtual's.
 *
 */
template<class CardType>
void Vector::set_cardinal_values_internal(const CardType* fromArray, int numElts)
{
    if (numElts < 0) {
        throw InternalErr(__FILE__, __LINE__, "Logic error: Vector::set_cardinal_values_internal() called with negative numElts!");
    }
    if (!fromArray) {
        throw InternalErr(__FILE__, __LINE__, "Logic error: Vector::set_cardinal_values_internal() called with null fromArray!");
    }
    set_length(numElts);
    m_create_cardinal_data_buffer_for_type(numElts);
    memcpy(_buf, fromArray, numElts * sizeof(CardType));
    set_read_p(true);
}

/** The Vector constructor requires the name of the variable to be
 created, and a pointer to an object of the type the Vector is to
 hold.  The name may be omitted, which will create a nameless
 variable.  The template object may not be omitted.

 @param n A string containing the name of the variable to be
 created.
 @param v A pointer to a variable of the type to be included
 in the Vector.
 @param t The type of the resulting Vector object, from the Type
 enum list.  There is no DAP2 Vector object, so all uses of this
 method will be from the List or Array classes.  This defaults to
 <tt>dods_null_c</tt>.

 @see Type
 @brief The Vector constructor.  */
Vector::Vector(const string & n, BaseType * v, const Type & t) :
    BaseType(n, t), d_length(-1), _var(0), _buf(0), _vec(0), _capacity(0)
{
    if (v)
        add_var(v);

    DBG2(cerr << "Entering Vector ctor for object: " << this << endl);
    if (_var)
        _var->set_parent(this);
}

/** The Vector server-side constructor requires the name of the variable
 to be created, the dataset name from which this Vector is created, and
 a pointer to an object of the type the Vector is to hold.  The
 name may be omitted, which will create a nameless variable.
 The template object may not be omitted.

 @param n A string containing the name of the variable to be
 created.
 @param d A string containing the dataset name from which the variable is
 being created.
 @param v A pointer to a variable of the type to be included
 in the Vector.
 @param t The type of the resulting Vector object, from the Type
 enum list.  There is no DAP2 Vector object, so all uses of this
 method will be from the List or Array classes.  This defaults to
 <tt>dods_null_c</tt>.

 @see Type
 @brief The Vector constructor.  */
Vector::Vector(const string & n, const string &d, BaseType * v, const Type & t) :
    BaseType(n, d, t), d_length(-1), _var(0), _buf(0), _vec(0), _capacity(0)
{
    if (v)
        add_var(v);

    DBG2(cerr << "Entering Vector ctor for object: " << this << endl);
    if (_var)
        _var->set_parent(this);
}

/** The Vector copy constructor. */
Vector::Vector(const Vector & rhs) :
    BaseType(rhs)
{
    DBG2(cerr << "Entering Vector const ctor for object: " << this <<
            endl); DBG2(cerr << "RHS: " << &rhs << endl);

    _duplicate(rhs);
}

Vector::~Vector()
{
    DBG2(cerr << "Entering ~Vector (" << this << ")" << endl);

    delete _var;
    _var = 0;

    // Clears all buffers
    clear_local_data();

    DBG2(cerr << "Exiting ~Vector" << endl);
}

Vector & Vector::operator=(const Vector & rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<BaseType &> (*this) = rhs;

    _duplicate(rhs);

    return *this;
}

/**
 * The Vector (and Array) classes are specific to DAP2. They do not support
 * the semantics of DAP4 which allows varying dimensions.
 */
bool Vector::is_dap2_only_type()
{
    return true;
}

void Vector::set_name(const std::string& name)
{
    BaseType::set_name(name);
    // We need to set the template variable name as well since
    // this is what gets output in the dds!  Otherwise, there's a mismatch.
    if (_var) {
        _var->set_name(name);
    }
}

int Vector::element_count(bool leaves)
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

/** This function sets the <tt>send_p</tt> flag for both the Vector itself
 and its element template.  This does not matter much when the
 Vector contains simple data types, but does become significant
 when the Vector contains compound types.

 @brief Indicates that the data is ready to send. */
void Vector::set_send_p(bool state)
{
    _var->set_send_p(state);
    BaseType::set_send_p(state);
}

/** This function sets the <tt>read_p</tt> flag for both the Vector itself
 and its element template.  This does not matter much when the
 Vector contains simple data types, but does become significant
 when the Vector contains compound types.

 @brief Indicates that the data is ready to send.  */
void Vector::set_read_p(bool state)
{
    if (_var) {
        _var->set_read_p(state);
    }
    BaseType::set_read_p(state);
}

/** Returns a copy of the template array element. If the Vector contains
 simple data types, the template will contain the value of the last
 vector element accessed with the <code>Vector::var(int i)</code> function,
 if any. If no such access has been made, or if the Vector contains
 compound data types, the value held by the template instance is
 undefined.

 Note that the parameter <i>exact_match</i> is not used by this mfunc.

 @param n The name of the variable to find.
 @param exact Unused.
 @param s Pointer to a BaseType Pointer Stack. Use this stack to record
 the path to the variable. By default this pointer is null, in which case
 it is not used.

 @return A pointer to the BaseType if found, otherwise null.
 @see Vector::var */
BaseType *Vector::var(const string &n, bool exact, btp_stack *s)
{
    string name = www2id(n);
    DBG(cerr << "Vector::var: Looking for " << n << endl);

    // If this is a Vector of constructor types, look for 'name' recursively.
    // Make sure to check for the case where name is the default (the empty
    // string). 9/1/98 jhrg
    if (_var->is_constructor_type()) {
        if (name == "" || _var->name() == name) {
            if (s)
                s->push(this);
            return _var;
        }
        else {
            BaseType * result = _var->var(name, exact, s);
            if (result && s)
                s->push(this);
            return result;
        }
    }
    else {
        return _var;
    }
}

/** This version of var(...) searches for <i>name</i> and returns a
 pointer to the BaseType object if found. It uses the same search
 algorithm as above when <i>exact_match</i> is false. In addition to
 returning a pointer to the variable, it pushes onto <i>s</i> a
 BaseType pointer to each constructor type that ultimately contains
 <i>name</i>.

 @param n Find the variable whose name is <i>name</i>.
 @param s Record the path to <i>name</i>.
 @return A pointer to the named variable. */
BaseType *Vector::var(const string & n, btp_stack & s)
{
    string name = www2id(n);

    if (_var->is_constructor_type())
        return _var->var(name, s);
    else {
        s.push((BaseType *) this);
        return _var;
    }
}

// Return a pointer the the BaseType object for element I. If the Vector is
// of a cardinal type, store the ith element's value in the BaseType
// object. If it is a Vector of a non-cardinal type, then this mfunc returns
// _vec[i].
//
// NB: I defaults to zero.
//
// Returns: A BaseType pointer to the ith element of the Vector.

/** Returns a pointer to the specified Vector element.  The return
 pointer will reference the element itself, so multiple calls to this
 method should save each value before making the next call.

 @param i The index of the desired Vector element.  Zero
 indicates the first element of the Vector.
 @return A pointer to a BaseType class instance containing
 the value of the indicated element. The BaseType pointer is locally
 maintained and should not be deleted or referenced. Extract the value
 right after the method returns.
 @see BaseType::var */
BaseType *Vector::var(unsigned int i)
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
            _var->val2buf((char *) _buf + (i * sz));
            return _var;
            break;
        }

        case dods_str_c:
        case dods_url_c:
            _var->val2buf(&d_str[i]);
            return _var;
            break;

        case dods_array_c:
        case dods_structure_c:
        case dods_sequence_c:
        case dods_grid_c:
            return _vec[i];
            break;

        default:
            throw Error ("Vector::var: Unrecognized type");
            //cerr << "Vector::var: Unrecognized type" << endl;
            break;
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

/** Returns the number of bytes needed to hold the <i>entire</i>
 array.  This is equal to <tt>length()</tt> times the width of each
 element.

 @brief Returns the width of the data, in bytes. */
unsigned int Vector::width(bool constrained)
{
    // Jose Garcia
    if (!_var) {
        throw InternalErr(__FILE__, __LINE__, "Cannot get width since *this* object is not holding data.");
    }

    return length() * _var->width(constrained);
}

// Returns: the number of elements in the vector.

/** Returns the number of elements in the vector. Note that some
 child classes of Vector use the length of -1 as a flag value.

 @see Vector::append_dim */
int Vector::length() const
{
    return d_length;
}

// set the number of elements in the vector.
//
// Returns: void

/** Sets the length of the vector.  This function does not allocate
 any new space. */
void Vector::set_length(int l)
{
    d_length = l;
}

// \e l is the number of elements the vector can hold (e.g., if l == 20, then
// the vector can hold elements 0, .., 19).

/** Resizes a Vector.  If the input length is greater than the
 current length of the Vector, new memory is allocated (the
 Vector moved if necessary), and the new entries are appended to
 the end of the array and padded with Null values.  If the input
 length is shorter, the tail values are discarded. */
void Vector::vec_resize(int l)
{
    _vec.resize((l > 0) ? l : 0, 0); // Fill with NULLs
    _capacity = l; // capacity in terms of number of elements.
}

/** @brief read data into a variable for later use

 Most uses of a variable are to either serialize its data to a stream of
 some sort or to read values from some stream and intern those in the
 variable for later use. These operations are perform by serialize()
 and deserialize() which follow. This function performs essentially both
 of these operations without actually using a stream device. The data are
 read using the read() method(s) and loaded into the variables directly.

 This method is intended to be used by objects which transform DAP objects
 like the DataDDS into an ASCII CSV representation.

 the data source.
 @param eval A reference to a constraint evaluator
 @param dds The complete DDS to which this variable belongs */
void Vector::intern_data(ConstraintEvaluator &eval, DDS &dds)
{
    DBG(cerr << "Vector::intern_data: " << name() << endl);
    if (!read_p())
        read(); // read() throws Error and InternalErr

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
            // For these cases, read() puts the data into _buf, which is what we
            // need to do 'stuff' with the data.
            break;

        case dods_str_c:
        case dods_url_c:
            // For these cases, read() will put the data into d_str[], which is
            // what the transformation classes need.
            break;

        case dods_array_c:
            // I think this is an error since there can never be an Array of
            // Array.
            throw InternalErr(__FILE__, __LINE__, "Array of Array not supported.");
            break;

        case dods_structure_c:
        case dods_sequence_c:
        case dods_grid_c:
            DBG(cerr << "Vector::intern_data: found ctor" << endl);
            // For these cases, we need to call read() for each of the 'num'
            // elements in the '_vec[]' array of BaseType object pointers.
            if (_vec.capacity() == 0)
                throw InternalErr(__FILE__, __LINE__, "The capacity of *this* vector is 0.");

            for (int i = 0; i < num; ++i)
                _vec[i]->intern_data(eval, dds);

            break;

        default:
            throw InternalErr(__FILE__, __LINE__, "Unknown datatype.");
            break;
    }
}

/** @brief Serialize a Vector.

 This uses the Marshaler class to encode each element of a cardinal
 array. For Arrays of Str and Url types, send the element count over
 as a prefix to the data so that deserialize will know how many elements
 to read.

 NB: Arrays of cardinal types must already be in BUF (in the local machine's
 representation) <i>before</i> this call is made.
 */

bool Vector::serialize(ConstraintEvaluator & eval, DDS & dds, Marshaller &m, bool ce_eval)
{
    int i = 0;// TODO move closer to use

    dds.timeout_on();

    if (!read_p())
        read(); // read() throws Error and InternalErr

#if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#endif

    dds.timeout_off();

    // length() is not capacity; it must be set explicitly in read().
    int num = length();

    switch (_var->type()) {
        case dods_byte_c:
            m.put_vector(_buf, num, *this);
            break;
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_float32_c:
        case dods_float64_c:
            m.put_vector(_buf, num, _var->width(), *this);
            break;

        case dods_str_c:
        case dods_url_c:
            if (d_str.capacity() == 0)
                throw InternalErr(__FILE__, __LINE__, "The capacity of the string vector is 0");

            m.put_int(num);

            for (i = 0; i < num; ++i)
                m.put_str(d_str[i]);

            break;

        case dods_array_c:
        case dods_structure_c:
        case dods_sequence_c:
        case dods_grid_c:
            //Jose Garcia
            // Not setting the capacity of _vec is an internal error.
            if (_vec.capacity() == 0)
                throw InternalErr(__FILE__, __LINE__, "The capacity of *this* vector is 0.");

            m.put_int(num);

            for (i = 0; i < num; ++i)
                _vec[i]->serialize(eval, dds, m, false);

            break;

        default:
            throw InternalErr(__FILE__, __LINE__, "Unknown datatype.");
            break;
    }

    return true;
}

// Read an object from the network and internalize it. For a Vector this is
// handled differently for a `cardinal' type. Vectors of Cardinals are
// stored using the `C' representations because these objects often are used
// to build huge arrays (e.g., an array of 1024 by 1024 bytes). However,
// arrays of non-cardinal types are stored as Vectors of the C++ objects or
// DAP2 objects (Str and Url are vectors of the string class, Structure, ...,
// Grid are vectors of the libdap Structure, ... classes).
//
// The boolean parameter REUSE determines whether internal storage is reused
// or not. If true, the _buf member is assumed to be large enough to hold the
// incoming cardinal data and is *not* reallocated. If false, new storage is
// allocated. If the internal buffer has not yet been allocated, then this
// parameter has no effect (i.e., storage is allocated). This parameter
// effects storage for cardinal data only.
//
// Returns: True is successful, false otherwise.

bool Vector::deserialize(UnMarshaller &um, DDS * dds, bool reuse)
{
    unsigned int num;
    unsigned i = 0;

    switch (_var->type()) {
        case dods_byte_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_float32_c:
        case dods_float64_c:
            if (_buf && !reuse) {
                m_delete_cardinal_data_buffer();
            }

            um.get_int((int &) num);

            DBG(cerr << "Vector::deserialize: num = " << num << endl);
            DBG(cerr << "Vector::deserialize: length = " << length() << endl);

            if (length() == -1)
                set_length(num);

            if (num != (unsigned int) length())
                throw InternalErr(__FILE__, __LINE__, "The server sent declarations and data with mismatched sizes.");

            if (!_buf) {
                // Make _buf be large enough for length() elements of _var->type()
                m_create_cardinal_data_buffer_for_type(length());
                DBG(cerr << "Vector::deserialize: allocating "
                        << width() << " bytes for an array of "
                        << length() << " " << _var->type_name() << endl);
            }

            if (_var->type() == dods_byte_c)
                um.get_vector((char **) &_buf, num, *this);
            else
                um.get_vector((char **) &_buf, num, _var->width(), *this);

            DBG(cerr << "Vector::deserialize: read " << num << " elements\n");

            break;

        case dods_str_c:
        case dods_url_c:
            um.get_int((int &) num);

            if (length() == -1)
                set_length(num);

            if (num != (unsigned int) length())
                throw InternalErr(__FILE__, __LINE__, "The client sent declarations and data with mismatched sizes.");

            d_str.resize((num > 0) ? num : 0); // Fill with NULLs
            _capacity = num; // capacity is number of strings we can fit.

            for (i = 0; i < num; ++i) {
                string str;
                um.get_str(str);
                d_str[i] = str;

            }

            break;

        case dods_array_c:
        case dods_structure_c:
        case dods_sequence_c:
        case dods_grid_c:
            um.get_int((int &) num);

            if (length() == -1)
                set_length(num);

            if (num != (unsigned int) length())
                throw InternalErr(__FILE__, __LINE__, "The client sent declarations and data with mismatched sizes.");

            vec_resize(num);

            for (i = 0; i < num; ++i) {
                _vec[i] = _var->ptr_duplicate();
                _vec[i]->deserialize(um, dds);
            }

            break;

        default:
            throw InternalErr(__FILE__, __LINE__, "Unknown type!");
            break;
    }

    return false;
}

/** Copies data into the class instance buffer.  This function
 assumes that the input \e val points to memory which
 contains, in row major order, enough elements of the correct
 type to fill the array. For an array of a cardinal type the
 memory is simply copied in whole into the Vector buffer.

 If the variable has already been constrained, this method will load only
 number of values/bytes specified by that constraint and will load them
 into the 'front' of the object's internal buffer. This is where serialize()
 expects to find the data.

 For a Vector of Str (OPeNDAP Strings), this assumes \e val points to an
 array of C++ strings.

 This method should not be used for Structure, Sequence or Grid.

 @brief Reads data into the Vector buffer.
 @exception InternalErr Thrown if called for Structure, Sequence or
 Grid.
 @return The number of bytes used by the array.
 @param val A pointer to the input data.
 @param reuse A boolean value, indicating whether the class
 internal data storage can be reused or not.  If this argument is
 TRUE, the class buffer is assumed to be large enough to hold the
 incoming data, and it is <i>not</i> reallocated.  If FALSE, new
 storage is allocated.  If the internal buffer has not been
 allocated at all, this argument has no effect. */
unsigned int Vector::val2buf(void *val, bool reuse)
{
    // Jose Garcia

    // I *think* this method has been mainly designed to be use by read which
    // is implemented in the surrogate library. Passing NULL as a pointer to
    // this method will be an error of the creator of the surrogate library.
    // Even though I recognize the fact that some methods inside libdap++ can
    // call val2buf, I think by now no coding bugs such as misusing val2buf
    // will be in libdap++, so it will be an internal error from the
    // surrogate library.
    if (!val)
        throw InternalErr(__FILE__, __LINE__, "The incoming pointer does not contain any data.");

    switch (_var->type()) {
        case dods_byte_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_float32_c:
        case dods_float64_c: {
            // width(true) returns the size given the constraint
            unsigned int array_wid = width(true);
            if (_buf && !reuse) {
                m_delete_cardinal_data_buffer();
            }

            if (!_buf) { // First time or no reuse (free'd above)
                m_create_cardinal_data_buffer_for_type(length());
            }

            memcpy(_buf, val, array_wid);
            break;
        }

        case dods_str_c:
        case dods_url_c: {
            // Assume val points to an array of C++ string objects. Copy
            // them into the vector<string> field of this object.
            d_str.resize(d_length);
            _capacity = d_length;
            for (int i = 0; i < d_length; ++i)
                d_str[i] = *(static_cast<string *> (val) + i);

            break;
        }

        default:
            throw InternalErr(__FILE__, __LINE__, "Vector::val2buf: bad type");

    }

    return width(true);
}

/** Copies data from the Vector buffer.  This function assumes that
 <i>val</i> points to an array large enough to hold N instances of
 the `C' representation of the \e numeric element type or C++ string
 objects. Never call this method for constructor types Structure,
 Sequence or Grid.

 When reading data out of a variable that has been constrained, this method
 assumes the N values/bytes of constrained data start at the beginning
 of the object's internal buffer. For example, do not load an entire
 Vector's data using val2buf(), constrain and then use this method to
 get the data. Unless your constraint starts with the [0]th element, the
 result will not be the correct values.

 In the case of a Vector of Str objects, this method will return an array
 of C++ std::string objects.

 @note It's best to define the pointer to reference the data as
 'char *data' and then call this method using '..->buf2val((void**)&data)'.
 Then free the storage once you're done using 'delete[] data'. It's not
 correct C++ to use 'delete[]' on a void pointer and the allocated memory
 \e is an array of char, so 'delete[]' is needed.

 @return The number of bytes used to store the array.
 @param val A pointer to a pointer to the memory into which the
 class data will be copied.  If the value pointed to is NULL,
 memory will be allocated to hold the data, and the pointer value
 modified accordingly.  The calling program is responsible for
 deallocating the memory indicated by this pointer.
 @exception InternalErr Thrown if \e val is null.
 @see Vector::set_vec */
unsigned int Vector::buf2val(void **val)
{
    // Jose Garcia
    // The same comment in Vector::val2buf applies here!
    if (!val)
        throw InternalErr(__FILE__, __LINE__, "NULL pointer.");

    unsigned int wid = static_cast<unsigned int> (width(true /* constrained */));
    // This is the width computed using length(). The
    // length() property is changed when a projection
    // constraint is applied. Thus this is the number of
    // bytes in the buffer given the current constraint.

    switch (_var->type()) {
        case dods_byte_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_float32_c:
        case dods_float64_c:
            if (!*val) {
                *val = new char[wid];
            }
            // avoid bus error if _buf is null and this is called improperly.
            if (!_buf) {
                throw InternalErr(__FILE__, __LINE__, "Vector::buf2val: Logic error: called when _buf was null!");
            }

            (void) memcpy(*val, _buf, wid);

            break;

        case dods_str_c:
        case dods_url_c: {
            if (!*val)
                *val = new string[d_length];

            for (int i = 0; i < d_length; ++i)
                *(static_cast<string *> (*val) + i) = d_str[i];

            break;
        }

        default:
            throw InternalErr(__FILE__, __LINE__, "Vector::buf2val: bad type");
            return 0;
    }

    return wid;
}

/** Sets an element of the vector to a given value.  If the type of
 the input and the type of the Vector do not match, an error
 condition is returned.

 Use this function only with Vectors containing compound DAP2
 types.  See <tt>buf2val()</tt> to access members of Vectors containing
 simple types.

 @note This method copies \e val; the caller is responsible for deleting
 instance passed as the actual parameter.

 @brief Sets element <i>i</i> to value <i>val</i>.
 @return void
 @exception InternalErr Thrown if \e i is out of range, \e val is null or
 there was a type mismatch between the BaseType referenced by \e val and
 the \e ith element of this Vector.
 @param i The index of the element to be changed.
 @param val A pointer to the value to be inserted into the
 array.
 @see Vector::buf2val */
void Vector::set_vec(unsigned int i, BaseType * val)
{
    // Jose Garcia
    // This is a public method which allows users to set the elements
    // of *this* vector. Passing an invalid index, a NULL pointer or
    // mismatching the vector type are internal errors.
    if (i >= static_cast<unsigned int> (d_length))
        throw InternalErr(__FILE__, __LINE__, "Invalid data: index too large.");
    if (!val)
        throw InternalErr(__FILE__, __LINE__, "Invalid data: null pointer to BaseType object.");
    if (val->type() != _var->type())
        throw InternalErr(__FILE__, __LINE__, "invalid data: type of incoming object does not match *this* vector type.");

    if (i >= _vec.capacity())
        vec_resize(i + 10);

    _vec[i] = val->ptr_duplicate();
}

/**
 * Remove any read or set data in the private data of this Vector,
 * setting read_p() to false.
 * Essentially clears the _buf, d_str, and _vec of any data.
 * Useful for tightening up memory when the data is no longer needed,
 * but the object cannot yet be destroyed.
 * NOTE: this is not virtual, and only affects the data in Vector itself!
 * On exit: get_value_capacity() == 0 && !read_p()
 */
void Vector::clear_local_data()
{
    if (_buf) {
        delete[] _buf;
        _buf = 0;
    }

    for (unsigned int i = 0; i < _vec.size(); ++i) {
        delete _vec[i];
        _vec[i] = 0;
    }

    // Force memory to be reclaimed.
    _vec.resize(0);
    d_str.resize(0);

    _capacity = 0;
    set_read_p(false);
}

/**
 * Return the capacity of the Vector in terms of number of
 * elements of its data type that it CAN currently hold (i.e. not bytes).
 * For example, this could be
 * the size of the _buf array in bytes / sizeof(T) for the cardinal
 * types T, or the capacity of the d_str vector if T is string or url type.
 */
unsigned int Vector::get_value_capacity() const
{
    return _capacity;
}

/**
 * Allocate enough memory for the Vector to contain
 * numElements data elements of the Vector's type.
 * Must be used before set_value_slice_from_row_major_vector
 * to ensure memory exists.
 * @param numElements  the number of elements of the Vector's type
 *                     to preallocate storage for.
 * @exception if the memory cannot be allocated
 */
void Vector::reserve_value_capacity(unsigned int numElements)
{
    if (!_var) {
        throw InternalErr(__FILE__, __LINE__, "reserve_value_capacity: Logic error: _var is null!");
    }
    switch (_var->type()) {
        case dods_byte_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_float32_c:
        case dods_float64_c: {
            // Make _buf be the right size and set _capacity
            m_create_cardinal_data_buffer_for_type(numElements);
        }
            break;

        case dods_str_c:
        case dods_url_c: {
            // Make sure the d_str has enough room for all the strings.
            // Technically not needed, but it will speed things up for large arrays.
            d_str.reserve(numElements);
            _capacity = numElements;
        }
            break;

        case dods_array_c:
        case dods_structure_c:
        case dods_sequence_c:
        case dods_grid_c: {
            // not clear anyone will go this path, but best to be complete.
            _vec.reserve(numElements);
            _capacity = numElements;
        }
            break;

        default: {
            throw InternalErr(__FILE__, __LINE__, "reserve_value_capacity: Unknown type!");
        }
            break;

    } // switch

}

/**
 * Make sure there's storage allocated for the current length()
 * of the Vector.
 * Same as reserveValueCapacity(length())
 */
void Vector::reserve_value_capacity()
{
    // Use the current length of the vector as the reserve amount.
    reserve_value_capacity(length());
}

/**
 * Copy rowMajorData.length() elements currently in a rowMajorData buffer
 * into this value buffer starting at element index startElement and
 * continuing up to startElement+rowMajorData.length()-1
 *
 * This is used for aggregating together smaller rowMajor vectors
 * into a larger one.
 *
 * Note: unlike the other set_value calls, this does NOT set read_p()
 *       since it is assumed to be used as a partial read and the caller
 *       is expected to set_read_p() when the data is complete.
 *
 * ASSUMES: rowMajorData.read_p() so that the data is valid!
 * ASSUMES: this Vector has enough value_capacity() to contain
 *          all the elements such that:
 *          startElement + rowMajorData.length()
 *          <= this->value_capacity().
 * ASSUMES: the data type of this->var() and rowMajorData.var()
 *          MUST be non-NULL and be the same!
 *
 * @param rowMajorDataC the vector from which to copy data,
 *                     assumed already read in or set.
 * @param startElement the element index
 *                     (NOT byte, but rather data type element)
 *                     to place the first data value.
 * @return the number of elements added, such that:
 *         startElement + the return value is the next "free" element.
 */
unsigned int Vector::set_value_slice_from_row_major_vector(const Vector& rowMajorDataC, unsigned int startElement)
{
    static const string funcName = "set_value_slice_from_row_major_vector:";

    // semantically const from the caller's viewpoint, but some calls are not syntactic const.
    Vector& rowMajorData = const_cast<Vector&> (rowMajorDataC);

    bool typesMatch = rowMajorData.var() && _var && (rowMajorData.var()->type() == _var->type());
    if (!typesMatch) {
        throw InternalErr(__FILE__, __LINE__, funcName + "Logic error: types do not match so cannot be copied!");
    }

    // Make sure the data exists
    if (!rowMajorData.read_p()) {
        throw InternalErr(__FILE__, __LINE__, funcName + "Logic error: the Vector to copy data from has !read_p() and should have been read in!");
    }

    // Check this otherwise the static_cast<unsigned int> below will do the wrong thing.
    if (rowMajorData.length() < 0) {
        throw InternalErr(__FILE__, __LINE__, funcName + "Logic error: the Vector to copy data from has length() < 0 and was probably not initialized!");
    }

    // The read-in capacity had better be at least the length (the amountt we will copy) or we'll memcpy into bad memory
    // I imagine we could copy just the capacity rather than throw, but I really think this implies a problem to be addressed.
    if (rowMajorData.get_value_capacity() < static_cast<unsigned int> (rowMajorData.length())) {
        throw InternalErr(__FILE__, __LINE__, funcName + "Logic error: the Vector to copy from has a data capacity less than its length, can't copy!");
    }

    // Make sure there's enough room in this Vector to store all the elements requested.  Again,
    // better to throw than just copy what we can since it implies a logic error that needs to be solved.
    if (_capacity < (startElement + rowMajorData.length())) {
        throw InternalErr(__FILE__, __LINE__, funcName + "Logic error: the capacity of this Vector cannot hold all the data in the from Vector!");
    }

    // OK, at this point we're pretty sure we can copy the data, but we have to do it differently depending on type.
    switch (_var->type()) {
        case dods_byte_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_float32_c:
        case dods_float64_c: {
            if (!_buf) {
                throw InternalErr(__FILE__, __LINE__, funcName + "Logic error: this->_buf was unexpectedly null!");
            }
            if (!rowMajorData._buf) {
                throw InternalErr(__FILE__, __LINE__, funcName + "Logic error: rowMajorData._buf was unexpectedly null!");
            }
            // memcpy the data into this, taking care to do ptr arithmetic on bytes and not sizeof(element)
            int varWidth = _var->width();
            char* pFromBuf = rowMajorData._buf;
            int numBytesToCopy = rowMajorData.width(true);
            char* pIntoBuf = _buf + (startElement * varWidth);
            memcpy(pIntoBuf, pFromBuf, numBytesToCopy);
        }
            break;

        case dods_str_c:
        case dods_url_c: {
            // Strings need to be copied directly
            for (unsigned int i = 0; i < static_cast<unsigned int> (rowMajorData.length()); ++i) {
                d_str[startElement + i] = rowMajorData.d_str[i];
            }
        }
            break;

        case dods_array_c:
        case dods_structure_c:
        case dods_sequence_c:
        case dods_grid_c: {
            // Not sure that this function will be used for these type of nested objects, so I will throw here.
            // TODO impl and test this path if it's ever needed.
            throw InternalErr(__FILE__, __LINE__, funcName + "Unimplemented method for Vectors of type: dods_array_c, dods_structure_c, dods_sequence_c and dods_grid_c.");
        }
            break;

        default: {
            throw InternalErr(__FILE__, __LINE__, funcName + ": Unknown type!");
        }
            break;

    } // switch (_var->type())

    // This is how many elements we copied.
    return (unsigned int) rowMajorData.length();
}

//@{
/** @brief set the value of a byte array */
bool Vector::set_value(dods_byte *val, int sz)
{
    if (var()->type() == dods_byte_c && val) {
        set_cardinal_values_internal<dods_byte> (val, sz);
        return true;
    }
    else {
        return false;
    }
}

/** @brief set the value of a byte array */
bool Vector::set_value(vector<dods_byte> &val, int sz)
{
    return set_value(&val[0], sz);
}

/** @brief set the value of a int16 array */
bool Vector::set_value(dods_int16 *val, int sz)
{
    if (var()->type() == dods_int16_c && val) {
        set_cardinal_values_internal<dods_int16> (val, sz);
        return true;
    }
    else {
        return false;
    }
}

/** @brief set the value of a int16 array */
bool Vector::set_value(vector<dods_int16> &val, int sz)
{
    return set_value(&val[0], sz);
}

/** @brief set the value of a int32 array */
bool Vector::set_value(dods_int32 *val, int sz)
{
    if (var()->type() == dods_int32_c && val) {
        set_cardinal_values_internal<dods_int32> (val, sz);
        return true;
    }
    else {
        return false;
    }
}

/** @brief set the value of a int32 array */
bool Vector::set_value(vector<dods_int32> &val, int sz)
{
    return set_value(&val[0], sz);
}

/** @brief set the value of a uint16 array */
bool Vector::set_value(dods_uint16 *val, int sz)
{
    if (var()->type() == dods_uint16_c && val) {
        set_cardinal_values_internal<dods_uint16> (val, sz);
        return true;
    }
    else {
        return false;
    }
}

/** @brief set the value of a uint16 array */
bool Vector::set_value(vector<dods_uint16> &val, int sz)
{
    return set_value(&val[0], sz);
}

/** @brief set the value of a uint32 array */
bool Vector::set_value(dods_uint32 *val, int sz)
{
    if (var()->type() == dods_uint32_c && val) {
        set_cardinal_values_internal<dods_uint32> (val, sz);
        return true;
    }
    else {
        return false;
    }
}

/** @brief set the value of a uint32 array */
bool Vector::set_value(vector<dods_uint32> &val, int sz)
{
    return set_value(&val[0], sz);
}

/** @brief set the value of a float32 array */
bool Vector::set_value(dods_float32 *val, int sz)
{
    if (var()->type() == dods_float32_c && val) {
        set_cardinal_values_internal<dods_float32> (val, sz);
        return true;
    }
    else {
        return false;
    }
}

/** @brief set the value of a float32 array */
bool Vector::set_value(vector<dods_float32> &val, int sz)
{
    return set_value(&val[0], sz);
}

/** @brief set the value of a float64 array */
bool Vector::set_value(dods_float64 *val, int sz)
{
    if (var()->type() == dods_float64_c && val) {
        set_cardinal_values_internal<dods_float64> (val, sz);
        return true;
    }
    else {
        return false;
    }
}

/** @brief set the value of a float64 array */
bool Vector::set_value(vector<dods_float64> &val, int sz)
{
    return set_value(&val[0], sz);
}

/** @brief set the value of a string or url array */
bool Vector::set_value(string *val, int sz)
{
    if ((var()->type() == dods_str_c || var()->type() == dods_url_c) && val) {
        d_str.resize(sz);
        _capacity = sz;
        for (register int t = 0; t < sz; t++) {
            d_str[t] = val[t];
        }
        set_length(sz);
        set_read_p(true);
        return true;
    }
    else {
        return false;
    }
}

/** @brief set the value of a string or url array */
bool Vector::set_value(vector<string> &val, int sz)
{
    if (var()->type() == dods_str_c || var()->type() == dods_url_c) {
        d_str.resize(sz);
        _capacity = sz;
        for (register int t = 0; t < sz; t++) {
            d_str[t] = val[t];
        }
        set_length(sz);
        set_read_p(true);
        return true;
    }
    else {
        return false;
    }
}
//@}

//@{

/** @brief Get a copy of the data held by this variable using the passed subsetIndex vector to identify which values to return.
 Read data from this variable's internal storage using the passed std::vector as an sub-setting index to the values to be returned. The
 memory referenced by \c b must point to enough memory
 to hold index.size() bytes.

 @param index A std::vector<long> where each value in the vector is the location in the Vector's internal storage from which to read the returned value
 @param b A pointer to the memory to hold the data; must be at least
 length() * sizeof(dods_byte) in size.*/
void Vector::value(vector<unsigned int> *subsetIndex, dods_byte *b) const
{
    unsigned long currentIndex;

    for(unsigned long i=0; i<subsetIndex->size() ;++i){
        currentIndex = (*subsetIndex)[i] ;
        if(currentIndex> (unsigned int)length()){
            stringstream s;
            s << "Vector::value() - Subset index[" << i <<  "] = " << currentIndex << " references a value that is " <<
                    "outside the bounds of the internal storage [ length()= " << length() << " ] name: '" << name() << "'. ";
            throw Error(s.str());
        }
        b[i] = reinterpret_cast<dods_byte*>(_buf )[currentIndex]; // I like this version - and it works!
    }
}


/** @brief Get a copy of the data held by this variable using the passed subsetIndex vector to identify which values to return. **/
void Vector::value(vector<unsigned int> *subsetIndex, dods_uint16 *b) const
{
    unsigned long currentIndex;

    for(unsigned long i=0; i<subsetIndex->size() ;++i){
        currentIndex = (*subsetIndex)[i] ;
        if(currentIndex> (unsigned int)length()){
            stringstream s;
            s << "Vector::value() - Subset index[" << i <<  "] = " << currentIndex << " references a value that is " <<
                    "outside the bounds of the internal storage [ length()= " << length() << " ] name: '" << name() << "'. ";
            throw Error(s.str());
        }
        b[i] = reinterpret_cast<dods_uint16*>(_buf )[currentIndex]; // I like this version - and it works!
    }
}


/** @brief Get a copy of the data held by this variable using the passed subsetIndex vector to identify which values to return. **/
void Vector::value(vector<unsigned int> *subsetIndex, dods_int16 *b) const
{
    unsigned long currentIndex;

    for(unsigned long i=0; i<subsetIndex->size() ;++i){
        currentIndex = (*subsetIndex)[i] ;
        if(currentIndex> (unsigned int)length()){
            stringstream s;
            s << "Vector::value() - Subset index[" << i <<  "] = " << currentIndex << " references a value that is " <<
                    "outside the bounds of the internal storage [ length()= " << length() << " ] name: '" << name() << "'. ";
            throw Error(s.str());
        }
        b[i] = reinterpret_cast<dods_int16*>(_buf )[currentIndex]; // I like this version - and it works!
    }
}

/** @brief Get a copy of the data held by this variable using the passed subsetIndex vector to identify which values to return. **/
void Vector::value(vector<unsigned int> *subsetIndex, dods_uint32 *b) const
{
    unsigned long currentIndex;

    for(unsigned long i=0; i<subsetIndex->size() ;++i){
        currentIndex = (*subsetIndex)[i] ;
        if(currentIndex> (unsigned int)length()){
            stringstream s;
            s << "Vector::value() - Subset index[" << i <<  "] = " << currentIndex << " references a value that is " <<
                    "outside the bounds of the internal storage [ length()= " << length() << " ] name: '" << name() << "'. ";
            throw Error(s.str());
        }
        b[i] = reinterpret_cast<dods_uint32*>(_buf )[currentIndex]; // I like this version - and it works!
    }
}

/** @brief Get a copy of the data held by this variable using the passed subsetIndex vector to identify which values to return. **/
void Vector::value(vector<unsigned int> *subsetIndex, dods_int32 *b) const
{
    unsigned long currentIndex;

    for(unsigned long i=0; i<subsetIndex->size() ;++i){
        currentIndex = (*subsetIndex)[i] ;
        if(currentIndex> (unsigned int)length()){
            stringstream s;
            s << "Vector::value() - Subset index[" << i <<  "] = " << currentIndex << " references a value that is " <<
                    "outside the bounds of the internal storage [ length()= " << length() << " ] name: '" << name() << "'. ";
            throw Error(s.str());
        }
        b[i] = reinterpret_cast<dods_int32*>(_buf )[currentIndex]; // I like this version - and it works!
    }
}

/** @brief Get a copy of the data held by this variable using the passed subsetIndex vector to identify which values to return. **/
void Vector::value(vector<unsigned int> *subsetIndex, dods_float32 *b) const
{
    unsigned long currentIndex;

    for(unsigned long i=0; i<subsetIndex->size() ;++i){
        currentIndex = (*subsetIndex)[i] ;
        //cerr << "currentIndex: " << currentIndex << endl;
        if(currentIndex> (unsigned int)length()){
            stringstream s;
            s << "Vector::value() - Subset index[" << i <<  "] = " << currentIndex << " references a value that is " <<
                    "outside the bounds of the internal storage [ length()= " << length() << " ] name: '" << name() << "'. ";
            throw Error(s.str());
        }
        // b[i] = *reinterpret_cast<dods_float32*>(_buf ) + currentIndex; // BROKEN
        // b[i] = *(reinterpret_cast<dods_float32*>(_buf ) + currentIndex); // Works but I like other forms
        // b[i] = ((dods_float32*)_buf )[currentIndex]; // Works but isn't as 'safe'

        b[i] = reinterpret_cast<dods_float32*>(_buf )[currentIndex]; // I like this version - and it works!

        //cerr << "b[" << i << "]=" <<  b[i] << endl;
    }
}

/** @brief Get a copy of the data held by this variable using the passed subsetIndex vector to identify which values to return. **/
void Vector::value(vector<unsigned int> *subsetIndex, dods_float64 *b) const
{
    unsigned long currentIndex;

    for(unsigned long i=0; i<subsetIndex->size() ;++i){
        currentIndex = (*subsetIndex)[i] ;
        if(currentIndex> (unsigned int)length()){
            stringstream s;
            s << "Vector::value() - Subset index[" << i <<  "] = " << currentIndex << " references a value that is " <<
                    "outside the bounds of the internal storage [ length()= " << length() << " ] name: '" << name() << "'. ";
            throw Error(s.str());
        }
        b[i] = reinterpret_cast<dods_float64*>(_buf )[currentIndex]; // I like this version - and it works!
    }
}


/** @brief Get a copy of the data held by this variable using the passed subsetIndex vector to identify which values to return. **/
void Vector::value(vector<unsigned int> *subsetIndex, vector<string> &b) const
{
    unsigned long currentIndex;

    if (_var->type() == dods_str_c || _var->type() == dods_url_c){
        for(unsigned long i=0; i<subsetIndex->size() ;++i){
            currentIndex = (*subsetIndex)[i] ;
            if(currentIndex > (unsigned int)length()){
                stringstream s;
                s << "Vector::value() - Subset index[" << i <<  "] = " << currentIndex << " references a value that is " <<
                        "outside the bounds of the internal storage [ length()= " << length() << " ] name: '" << name() << "'. ";
                throw Error(s.str());
            }
            b[i] = d_str[currentIndex];
        }
    }
}







/** @brief Get a copy of the data held by this variable.
 Read data from this variable's internal storage and load it into the
 memory referenced by \c b. The argument \c b must point to enough memory
 to hold length() Bytes.

 @param b A pointer to the memory to hold the data; must be at least
 length() * sizeof(dods_byte) in size.*/
void Vector::value(dods_byte *b) const
{
    if (b && _var->type() == dods_byte_c) {
        memcpy(b, _buf, length() * sizeof(dods_byte));
    }
}

/** @brief Get a copy of the data held by this variable. */
void Vector::value(dods_uint16 *b) const
{
    if (b && _var->type() == dods_uint16_c) {
        memcpy(b, _buf, length() * sizeof(dods_uint16));
    }
}

/** @brief Get a copy of the data held by this variable. */
void Vector::value(dods_int16 *b) const
{
    if (b && _var->type() == dods_int16_c) {
        memcpy(b, _buf, length() * sizeof(dods_int16));
    }
}

/** @brief Get a copy of the data held by this variable. */
void Vector::value(dods_uint32 *b) const
{
    if (b && _var->type() == dods_uint32_c) {
        memcpy(b, _buf, length() * sizeof(dods_uint32));
    }
}

/** @brief Get a copy of the data held by this variable. */
void Vector::value(dods_int32 *b) const
{
    if (b && _var->type() == dods_int32_c) {
        memcpy(b, _buf, length() * sizeof(dods_int32));
    }
}

/** @brief Get a copy of the data held by this variable. */
void Vector::value(dods_float32 *b) const
{
    if (b && _var->type() == dods_float32_c) {
        memcpy(b, _buf, length() * sizeof(dods_float32));
    }
}

/** @brief Get a copy of the data held by this variable. */
void Vector::value(dods_float64 *b) const
{
    if (b && _var->type() == dods_float64_c) {
        memcpy(b, _buf, length() * sizeof(dods_float64));
    }
}

/** @brief Get a copy of the data held by this variable. */
void Vector::value(vector<string> &b) const
{
    if (_var->type() == dods_str_c || _var->type() == dods_url_c)
        b = d_str;
}

/** Allocated memory and copy data into the new buffer. Return the new
 buffer's pointer. The caller must delete the storage. */
void *Vector::value()
{
    void *buffer = new char[width(true)];

    memcpy(buffer, _buf, width(true));

    return buffer;
}
//@}

/** @brief Add the BaseType pointer to this constructor type
 instance.

 Propagate the name of the BaseType instance to this instance. This
 ensures that variables at any given level of the DDS table have
 unique names (i.e., that Arrays do not have their default name ""). If
 <tt>v</tt>'s name is null, then assume that the array \e is named and
 don't overwrite it with <tt>v</tt>'s null name.

 @note As is the case with Array, this method can be called with a null
 BaseType pointer.

 @param v The template variable for the array
 @param p The Part parameter defaults to nil and is ignored by this method.
 */
void Vector::add_var(BaseType * v, Part)
{
#if 0
    if (v && v->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 Vector.");
#endif
    // Delete the current template variable
    if (_var) {
        delete _var;
        _var = 0;
    }

    // if 'v' is null, just set _var to null and exit.
    if (!v) {
        _var = 0;
    }
    else {
        // Jose Garcia
        // By getting a copy of this object to be assigned to _var
        // we let the owner of 'v' to deallocate it as necessary.
        _var = v->ptr_duplicate();

        // If 'v' has a name, use it as the name of the array. If it *is*
        // empty, then make sure to copy the array's name to the template
        // so that software which uses the template's name will still work.
        if (!v->name().empty())
            set_name(v->name());
        else
            _var->set_name(name());

        _var->set_parent(this); // Vector --> child

        DBG(cerr << "Vector::add_var: Added variable " << v << " ("
                << v->name() << " " << v->type_name() << ")" << endl);
    }
}

void Vector::add_var_nocopy(BaseType * v, Part)
{
#if 0
    if (v && v->is_dap4_only_type())
        throw InternalErr(__FILE__, __LINE__, "Attempt to add a DAP4 type to a DAP2 Vector.");
#endif
    // Delete the current template variable
    if (_var) {
        delete _var;
        _var = 0;
    }

    // if 'v' is null, just set _var to null and exit.
    if (!v) {
        _var = 0;
    }
    else {
        _var = v;

        // If 'v' has a name, use it as the name of the array. If it *is*
        // empty, then make sure to copy the array's name to the template
        // so that software which uses the template's name will still work.
        if (!v->name().empty())
            set_name(v->name());
        else
            _var->set_name(name());

        _var->set_parent(this); // Vector --> child

        DBG(cerr << "Vector::add_var: Added variable " << v << " ("
                << v->name() << " " << v->type_name() << ")" << endl);
    }
}

#if 0
// branch version
void Vector::add_var_nocopy(BaseType * v, Part)
{
    // Delete the current template variable
    if (_var) {
        delete _var;
        _var = 0;
    }

    // if 'v' is null, just set _var to null and exit.
    if (!v) {
        _var = 0;
    }
    else {
        _var = v;

        // If 'v' has a name, use it as the name of the array. If it *is*
        // empty, then make sure to copy the array's name to the template
        // so that software which uses the template's name will still work.
        if (!v->name().empty())
            set_name(v->name());
        else
            _var->set_name(name());

        _var->set_parent(this); // Vector --> child

        DBG(cerr << "Vector::add_var: Added variable " << v << " ("
                << v->name() << " " << v->type_name() << ")" << endl);
    }
}
#endif

bool Vector::check_semantics(string & msg, bool)
{
    return BaseType::check_semantics(msg);
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and information about this
 * instance.
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void Vector::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Vector::dump - (" << (void *) this << ")" << endl;
    DapIndent::Indent();
    BaseType::dump(strm);
    strm << DapIndent::LMarg << "# elements in vector: " << d_length << endl;
    if (_var) {
        strm << DapIndent::LMarg << "base type:" << endl;
        DapIndent::Indent();
        _var->dump(strm);
        DapIndent::UnIndent();
    }
    else {
        strm << DapIndent::LMarg << "base type: not set" << endl;
    }
    strm << DapIndent::LMarg << "vector contents:" << endl;
    DapIndent::Indent();
    for (unsigned i = 0; i < _vec.size(); ++i) {
        if (_vec[i])
            _vec[i]->dump(strm);
        else
            strm << DapIndent::LMarg << "vec[" << i << "] is null" << endl;
    }
    DapIndent::UnIndent();
    strm << DapIndent::LMarg << "strings:" << endl;
    DapIndent::Indent();
    for (unsigned i = 0; i < d_str.size(); i++) {
        strm << DapIndent::LMarg << d_str[i] << endl;
    }
    DapIndent::UnIndent();
    if (_buf) {
        switch (_var->type()) {
            case dods_byte_c: {
                strm << DapIndent::LMarg << "_buf: ";
                strm.write(_buf, d_length);
                strm << endl;
            }
                break;
            default: {
                strm << DapIndent::LMarg << "_buf: " << (void *) _buf << endl;
            }
                break;
        }
    }
    else {
        strm << DapIndent::LMarg << "_buf: EMPTY" << endl;
    }
    DapIndent::UnIndent();
}

} // namespace libdap

