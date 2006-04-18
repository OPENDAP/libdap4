
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//         Dan Holloway <dan@hollywood.gso.uri.edu>
//         Reza Nekovei <reza@intcomm.net>
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>
//      dan             Dan Holloway <dan@hollywood.gso.uri.edu>
//      reza            Reza Nekovei <reza@intcomm.net>

// Abstract base class for the variables in a dataset. This is used to store
// the type-invariant information that describes a variable as given in the
// DODS API.
//
// jhrg 9/6/94

#ifndef _basetype_h
#define _basetype_h 1


#ifdef WIN32
#include <rpc.h>
#include <winsock2.h>
#include <xdr.h>
#define xdr_proc_t int *
#else
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#include <vector>
#include <stack>
#include <iostream>
#include <string>

#ifndef _attrtable_h
#include "AttrTable.h"
#endif

#ifndef _internalerr_h
#include "InternalErr.h"
#endif

#ifndef __DODS_DATATYPES_
#include "dods-datatypes.h"
#endif

using namespace std;

class BaseType;			// Forward declarations
class DDS;
class ConstraintEvaluator;

typedef stack<BaseType *> btp_stack;

/** <b>Part</b> names the parts of multi-section constructor types.
    For example, the <b>Grid</b> class has an <i>array</i> and
    the array <i>maps</i>. Use the <tt>nil</tt> value for data types that
    don't have separate parts.

    \code
    enum Part {
    nil,
    array,
    maps
    };
    \endcode

    @brief Names the parts of multi-section constructor data types.  
    @see Grid
    @see BaseType
*/

enum Part {
    nil,			// nil is for types that don't have parts...
    array,
    maps
};

/** <b>Type</b> identifies the data type stored in a particular type
    class.  All the DODS Data Access Protocol (DAP) types inherit from
    the BaseType class.

    \code
    enum Type {
    dods_null_c,
    dods_byte_c,
    dods_int16_c,
    dods_uint16_c,
    dods_int32_c,
    dods_uint32_c,
    dods_float32_c,
    dods_float64_c,
    dods_str_c,
    dods_url_c,
    dods_array_c,
    dods_structure_c,
    dods_sequence_c,
    dods_grid_c
    };
    \endcode

    @brief Identifies the data type.
    @see BaseType
*/

enum Type {
    dods_null_c,
    dods_byte_c,
    dods_int16_c,
    dods_uint16_c,
    dods_int32_c,		// Added `dods_' to fix clash with IRIX 5.3.
    dods_uint32_c,
    dods_float32_c,
    dods_float64_c,
    dods_str_c,
    dods_url_c,
    dods_array_c,
    dods_structure_c,
    dods_sequence_c,
    dods_grid_c
};

/** This defines the basic data type features for the DODS data access
    protocol (DAP) data types. All the DAP type classes (Float64, Array,
    etc.) subclass it. This class is an abstract one; no variables will ever
    be stored as BaseType instances, only as instances of its child classes.

    These classes and their methods give a user the capacity to set up
    sophisticated data types. They do <i>not</i> provide sophisticated ways to
    access and use this data. On the server side, in many cases, the class
    instances will have no data in them at all until the
    <tt>serialize</tt> function 
    is called to send data to the client. On the client side, most DODS
    application programs will unpack the data promptly into whatever local
    data structure the programmer deems the most useful.

    In order to use these classes on the server side of a DODS
    client/server connection, you must write a <tt>read</tt> method
    for each of the data types you expect to encounter in the
    application. This function, whose purpose is to read data from a
    local source into the class instance data buffer, is called in
    <tt>serialize</tt>, when the data is about to be sent to the
    client.  The <tt>read</tt> function may be called earlier, in the
    case of data subset requests (constraint expressions) whose
    evaluation requires it. (For example, the constraint expression
    ``<tt>a,b&b>c</tt>'' requires that <tt>c</tt> be read even though
    it will not be sent.)

    For some data types, the <tt>read</tt> function must be aware of
    the constraints 
    to be returned. These cautions are outlined where they occur.

    @todo We really need a better way to get values out of these types, esp
    the Float32, Int16, ..., types. In \e most cases we know the type, so a
    type specific method (one that requires a downcast to use) is OK. For
    example, Byte might have a method <tt>dods_byte Byte::value()</tt>. Sure
    you have to downcast from BaseType to Byte in order to use it, but you
    have to figure out you have a Byte to use Byte::buf2val() anyway, so
    what's the big deal? Having a method that returns the \e value would
    simplify code that reads from data sets to extract meta data (like
    lat/lon corner points, et c.).

    @brief The basic data type for the DODS DAP types.  */

class BaseType {
private:
    string _name;		// name of the instance
    Type _type;			// instance's type

    // xdr_coder is used as an argument to xdr procedures that encode groups
    // of things (e.g., xdr_array()). Each leaf class's constructor must set
    // this.
    xdrproc_t _xdr_coder;

    bool _read_p;		// true if the value has been read
    bool _send_p;		// Is the variable in the projection?
    bool d_in_selection;	// Is the variable in the selection?
    bool _synthesized_p;	// true if the variable is synthesized
    
    // d_parent points to the Constructor or Vector which holds a particular
    // variable. It is null for simple variables. The Vector and Constructor
    // classes must maintain this variable.
    BaseType *d_parent;

    // Attributes for this variable. Added 05/20/03 jhrg
    AttrTable d_attr;

protected:
    void _duplicate(const BaseType &bt);

public:
    BaseType(const string &n = "", const Type &t = dods_null_c,
	     xdrproc_t xdr = NULL);

    BaseType(const BaseType &copy_from);
    virtual ~BaseType();

    virtual string toString();

    BaseType &operator=(const BaseType &rhs);

    /** Clone this instance. Allocate a new instance and copy \c *this into
	it. This method must perform a deep copy.

	@return A newly allocated copy of \c this. */
    virtual BaseType *ptr_duplicate() = 0; // alloc new instance and dup THIS.

    string name() const;
    virtual void set_name(const string &n);

    Type type() const;
    void set_type(const Type &t);
    string type_name() const;	

    virtual bool is_simple_type();
    virtual bool is_vector_type();
    virtual bool is_constructor_type();

    virtual bool synthesized_p();
    virtual void set_synthesized_p(bool state);

    virtual int element_count(bool leaves = false);

    virtual bool read_p();
    virtual void set_read_p(bool state);

    virtual bool send_p();
    virtual void set_send_p(bool state);

    virtual AttrTable &get_attr_table();
    virtual void set_attr_table(const AttrTable &at);

    virtual bool is_in_selection();
    virtual void set_in_selection(bool state);

    xdrproc_t xdr_coder();

    virtual void set_parent(BaseType *parent) throw(InternalErr);
    virtual BaseType *get_parent();

        /** Returns a pointer to the contained variable in a composite class. The
            composite classes are those made up of aggregated simple data types.
            Array, Grid, and Structure are composite types, while Int and Float are
            simple types. This function is only used by composite classes. The
            BaseType implementation always returns null.
        
            Several of the subclasses provide alternate access
            methods that make sense for that particular data type. For example, the
            Array class defines a <tt>*var(int i)</tt> method that returns the ith
            entry in the Array data, and the Structure provides a 
            <tt>*var(Vars_iter)</tt> function using a pseudo-index to access the 
            different members of the structure.
        
            @brief Returns a pointer to a member of a constructor class.
            @param name The name of the class member.  Defaults to ""
            @param exact_match
            True if only interested in variables whose full names match
            \e n exactly. If false, returns the first variable
            whose name matches \e name. For example, if
            \e name is \c x and \c point.x is a variable, then var("x",
            false) would return a BaseType pointer to \c point.x. If
            \e exact_match was <tt>true</tt> then \e name
            would need to be \c "point.x" for var to return that
            pointer. This feature simplifies constraint expressions for
            datasets which have complex, nested, constructor variables.
            Defaults to true.
            @param s Record the path to \e name. Defaults to null, in which case it is
            not used.
        
            @return A pointer to the member named in the \e n argument. If no name is
            given, the function returns the first (only) variable.  For example, an
            Array has only one variable, while a Structure can have many. */
    virtual BaseType *var(const string &name = "", bool exact_match = true,
			  btp_stack *s = 0);
    virtual BaseType *var(const string &name, btp_stack &s);

    virtual void add_var(BaseType *bt, Part part = nil);

    // This used to be virutal, but unlike the other virtual methods, it has
    // no sensible definition for the classes here *and* some uses of the DAP
    // don't need this. I provide a default implementation that throws
    // InternalErr. 
    virtual bool read(const string &dataset);
    
    virtual bool check_semantics(string &msg, bool all = false);

    virtual bool ops(BaseType *b, int op, const string &dataset);

    virtual void print_decl(FILE *out, string space = "    ", 
			    bool print_semi = true, 
			    bool constraint_info = false, 
			    bool constrained = false);

    virtual void print_xml(FILE *out, string space = "    ", 
			   bool constrained =false);

    /** @name Abstract Methods */
    //@{

    /** Return the number of bytes that are required to hold the instance's
	value. In the case of simple types such as Int32, this is the size of
	one Int32 (four bytes). For a String or Url type,
	<tt>width()</tt> returns 
	the number of bytes needed for a <tt>String *</tt> variable,
	not the bytes 
	needed for all the characters, since that value cannot be determined
	from type information alone. For Structure, and other constructor
	types size() returns the number of bytes needed to store pointers to
	the C++ objects.

	@brief Returns the size of the class instance data. */
    virtual unsigned int width() = 0;

    /** Reads the class data into the memory referenced by <i>val</i>.
	The caller must allocate enough storage to <i>val</i> to hold the
	class data.  If <i>val</i> is NULL, however, memory will be
	allocated by this function with <tt>new()</tt>.  Even if the memory is
	allocated this way, the caller is responsible for deallocating
	that memory.  Array and List values for simple types are
	stored as C would store an array.

	@brief Reads the class data.  

	@param val A pointer to a pointer to the memory into which the
	class data will be copied.  If the value pointed to is NULL,
	memory will be allocated to hold the data, and the pointer value
	modified accordingly.  The calling program is responsible for
	deallocating the memory indicated by this pointer.

	@return The size (in bytes) of the information copied to <i>val</i>.  
    */
    virtual unsigned int buf2val(void **val) = 0;

    /** Store the value pointed to by <i>val</i> in the object's internal
	buffer. This function does not perform any checks, so users must
	be sure that the thing pointed to can actually be stored in the
	object's buffer.  For example, an array cannot easily be fit
	into the data buffer for an Int32 object.  

	Only simple objects (Int, Float, Byte, and so on) and arrays and
	lists of these simple objects may be stored using this function.
	To put data into more complex constructor functions, use the
	functions provided by that class.  For example, use the <tt>var()</tt>
	and <tt>add_var()</tt> members of the Grid class to manipulate data in
	that class.

	@brief Loads class data.

	@param val A pointer to the data to be inserted into the class
	data buffer.

	@param reuse A boolean value, indicating whether the class
	internal data storage can be reused or not.  If this argument is
	TRUE, the class buffer is assumed to be large enough to hold the
	incoming data, and it is <i>not</i> reallocated.  If FALSE, new
	storage is allocated.  If the internal buffer has not been
	allocated at all, this argument has no effect.
	This is currently used only in the Vector class.

	@return The size (in bytes) of the information copied from <i>val</i>.
	@see Grid
	@see Vector::val2buf */
    virtual unsigned int val2buf(void *val, bool reuse = false) = 0;

    /** Sends the data from the indicated (local) dataset through the
	connection identified by the <i>sink</i> parameter.  If the data
	is not already incorporated into the DDS object, read the data
	from the dataset.  

	This function is only used on the server side of the
	client/server connection, and is generally only called from the
	DDS::send() function.  It has no BaseType implementation; each
	datatype child class supplies its own implementation.

	@brief Move data to the net.
	@param dataset The (local) name of dataset to be read.
	@param dds The Data Descriptor Structure object corresponding to
	this dataset.  See <i>The DODS User Manual</i> for information
	about this structure.
	@param sink A valid XDR pointer generally created with a call to
	<tt>new_xdrstdio()</tt>. This typically routes data to a TCP/IP socket.
	@param ce_eval A boolean value indicating whether to evaluate
	the DODS constraint expression that may accompany this dataset.
	The constraint expression is stored in <i>dds</i>.
	@return This method always returns true. Older versions used the
	return value to signal success or failure. 
	@exception InternalErr.
	@exception Error.
	@see DDS */
    virtual bool serialize(const string &dataset, ConstraintEvaluator &eval,
                           DDS &dds, XDR *sink, bool ce_eval = true) = 0; 

    /** Receives data from the network connection identified by the 
	<tt>source</tt> parameter.  The data is put into the class data buffer
	according to the input <tt>dds</tt>.  

	This function is only used on the client side of the
	DODS client/server connection.

	@brief Receive data from the net.
	@param source A valid XDR pointer to the process connection to
	the net.  This is generally created with a call to
	<tt>new_xdrstdio()</tt>. 
	@param dds The Data Descriptor Structure object corresponding to
	this dataset.  See <i>The DODS User Manual</i> for information
	about this structure.  This would have been received from the
	server in an earlier transmission.
	@param reuse A boolean value, indicating whether the class
	internal data storage can be reused or not.  If this argument is
	TRUE, the class buffer is assumed to be large enough to hold the
	incoming data, and it is <i>not</i> reallocated.  If FALSE, new
	storage is allocated.  If the internal buffer has not been
	allocated at all, this argument has no effect.
	@return Always returns TRUE.
	@exception Error when a problem reading from the XDR stream is
	found.
	@see DDS 
    */
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false) = 0;

    /** Prints the value of the variable, with its declaration.  This
	function is primarily intended for debugging DODS applications.
	However, it can be overloaded and used to do some useful things. Take a
	look at the asciival and writeval clients, both of which overload this
	to output the values of variables in different ways.

	NB: This function uses C++'s iostream to handle creating the print
	representations of simple type variables. For floating point numbers
	this is set to six digits of precision by default. If you want more
	precision (IEEE 64-bit floats have 15 digits of precision, 32-bit
	floats have 8), use the setprecision() I/O manipulator. 

	@brief Prints the value of the variable.
	@param out The output FILE on which to print the value.
	@param space This value is passed to the print_decl()
	function, and controls the leading spaces of the output.
	@param print_decl_p A boolean value controlling whether the
	variable declaration is printed as well as the value.
    */
    virtual void print_val(FILE *out, string space = "",
			   bool print_decl_p = true) = 0;
    //@}
};

#endif // _basetype_h
