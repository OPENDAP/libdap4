
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

#ifndef __POWERPC__
#ifdef __GNUG__
// #pragma interface
#endif
#endif

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
	it. This method performs a deep copy.

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

    virtual BaseType *var(const string &name = "", bool exact_match = true,
			  btp_stack *s = 0);
    virtual BaseType *var(const string &name, btp_stack &s);

    virtual void add_var(BaseType *bt, Part part = nil);

    // This used to be virutal, but unlike the other virtual methods, it has
    // no sinsible definition for the classes here *and* some uses of the DAP
    // don't need this. I provide a default implementation that throws
    // InternalErr. 
    virtual bool read(const string &dataset);
    
    virtual bool check_semantics(string &msg, bool all = false);

    virtual bool ops(BaseType *b, int op, const string &dataset);

    virtual void print_decl(FILE *out, string space = "    ", 
			    bool print_semi = true, 
			    bool constraint_info = false, 
			    bool constrained = false);

    virtual void print_decl(ostream &os, string space = "    ", 
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
    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true) = 0; 

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

	@memo Prints the value of the variable.
	@param os The output stream on which to print the value.
	@param space This value is passed to the print_decl()
	function, and controls the leading spaces of the output.
	@param print_decl_p A boolean value controlling whether the
	variable declaration is printed as well as the value.
	@deprecated Using the C++ iostream class is deprecated.
	@see print_val(FILE *out, string space, bool print_decl_p) = 0;
    */
    virtual void print_val(ostream &os, string space = "",
			   bool print_decl_p = true) = 0;

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

	@memo Prints the value of the variable.
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

/* 
 * $Log: BaseType.h,v $
 * Revision 1.75  2004/07/07 21:08:47  jimg
 * Merged with release-3-4-8FCS
 *
 * Revision 1.71.2.8  2004/07/02 20:41:51  jimg
 * Removed (commented) the pragma interface/implementation lines. See
 * the ChangeLog for more details. This fixes a build problem on HP/UX.
 *
 * Revision 1.71.2.7  2004/05/02 00:07:20  rmorris
 * Mod's to move to winsock2 under win32.  Necessary because of newer libcurl.
 *
 * Revision 1.74  2003/12/10 21:11:57  jimg
 * Merge with 3.4. Some of the files contains erros (some tests fail). See
 * the ChangeLog for information about fixes.
 *
 * Revision 1.73  2003/12/08 18:02:29  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.71.2.6  2003/11/25 18:14:22  jimg
 * Improved the doc comments regarding buf2val() for Sequence.
 *
 * Revision 1.71.2.5  2003/11/19 18:53:05  jimg
 * Added a todo comment about new methods which return values directly without
 * using the buf2val() method.
 *
 * Revision 1.71.2.4  2003/09/28 20:57:22  rmorris
 * Discontinued use of XDR_PROC typedef, using xdrproc_t instead - a
 * define from the xdr portion of the rpc library.
 *
 * Revision 1.71.2.3  2003/09/06 22:24:19  jimg
 * Significant changes to the documentation. Added the in_selection property and
 * accessor/mutator methods for it. Added use of the typedef XDR_PROC which
 * allowed me to remove some of the #if WIN32 code (see dods-datatypes.h.in).
 *
 * Revision 1.71.2.2  2003/07/25 06:04:28  jimg
 * Refactored the code so that DDS:send() is now incorporated into
 * DODSFilter::send_data(). The old DDS::send() is still there but is
 * depracated.
 * Added 'smart timeouts' to all the variable classes. This means that
 * the new server timeouts are active only for the data read and CE
 * evaluation. This went inthe BaseType::serialize() methods because it
 * needed to time both the read() calls and the dds::eval() calls.
 *
 * Revision 1.71.2.1  2003/06/23 11:49:18  rmorris
 * The // #pragma interface directive to GCC makes the dynamic typing functionality
 * go completely haywire under OS X on the PowerPC.  We can't use that directive
 * on that platform and it was ifdef'd out for that case.
 *
 * Revision 1.72  2003/05/23 03:24:57  jimg
 * Changes that add support for the DDX response. I've based this on Nathan
 * Potter's work in the Java DAP software. At this point the code can
 * produce a DDX from a DDS and it can merge attributes from a DAS into a
 * DDS to produce a DDX fully loaded with attributes. Attribute aliases
 * are not supported yet. I've also removed all traces of strstream in
 * favor of stringstream. This code should no longer generate warnings
 * about the use of deprecated headers.
 *
 * Revision 1.71  2003/04/22 19:40:27  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.69.2.2  2003/04/18 03:08:49  jimg
 * I made a number of the send and read predicate methods virtual. I was working
 * on a bug in the grid() function, and while this wasn't a fix, it seems like a
 * reasonable thing to do.
 *
 * Revision 1.70  2003/02/21 00:14:24  jimg
 * Repaired copyright.
 *
 * Revision 1.69.2.1  2003/02/21 00:10:06  jimg
 * Repaired copyright.
 *
 * Revision 1.69  2003/01/23 00:22:23  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.68  2003/01/10 19:46:39  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.61.4.12  2002/12/17 22:35:02  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.61.4.11  2002/09/05 22:52:54  pwest
 * Replaced the GNU data structures SLList and DLList with the STL container
 * class vector<>. To maintain use of Pix, changed the Pix.h header file to
 * redefine Pix to be an IteratorAdapter. Usage remains the same and all code
 * outside of the DAP should compile and link with no problems. Added methods
 * to the different classes where Pix is used to include methods to use STL
 * iterators. Replaced the use of Pix within the DAP to use iterators instead.
 * Updated comments for documentation, updated the test suites, and added some
 * unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
 *
 * Revision 1.67  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.66  2002/06/03 22:21:15  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.61.4.10  2002/05/09 16:56:17  jimg
 * The read method is no longer abstract. BaseType implements a version that
 * throws InternalErr when called. See BaseType.cc.
 *
 * Revision 1.61.4.9  2002/03/01 21:03:08  jimg
 * Significant changes to the var(...) methods. These now take a btp_stack
 * pointer and are used by DDS::mark(...). The exact_match methods have also
 * been updated so that leaf variables which contain dots in their names
 * will be found. Note that constructor variables with dots in their names
 * will break the lookup routines unless the ctor is the last field in the
 * constraint expression. These changes were made to fix bug 330.
 *
 * Revision 1.61.4.8  2002/01/30 19:05:21  jimg
 * Fixed some of the doc++ comment about read(). It still needs work.
 *
 * Revision 1.65  2001/10/14 01:28:38  jimg
 * Merged with release-3-2-8.
 *
 * Revision 1.61.4.7  2001/10/02 16:47:16  jimg
 * Changed the documentation comments for serialize and deserialize.
 *
 * Revision 1.64  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 * Revision 1.61.4.6  2001/09/07 00:38:34  jimg
 * Sequence::deserialize(...) now reads all the sequence values at once.
 * Its call semantics are the same as the other classes' versions. Values
 * are stored in the Sequence object using a vector<BaseType *> for each
 * row (those are themselves held in a vector). Three new accessor methods
 * have been added to Sequence (row_value() and two versions of var_value()).
 * BaseType::deserialize(...) now always returns true. This matches with the
 * expectations of most client code (the seqeunce version returned false
 * when it was done reading, but all the calls for sequences must be changed
 * anyway). If an XDR error is found, deserialize throws InternalErr.
 *
 * Revision 1.63  2001/08/27 16:38:34  jimg
 * Merged with release-3-2-6
 *
 * Revision 1.61.4.5  2001/08/21 16:32:18  dan
 * Changed set_name accessor to 'virtual void' so that name fields
 * of subclassed types could be accessed through the BaseType pointer.
 *
 * Revision 1.61.4.4  2001/08/18 01:48:11  jimg
 * Removed WIN32 compile guards from using statements.
 *
 * Revision 1.62  2001/06/15 23:49:01  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.61.4.3  2001/06/07 16:58:50  jimg
 * Comment/doc fixes.
 *
 * Revision 1.61.4.2  2001/06/05 06:49:19  jimg
 * Added the Constructor class which is to Structures, Sequences and Grids
 * what Vector is to Arrays and Lists. This should be used in future
 * refactorings (I thought it was going to be used for the back pointers).
 * Introduced back pointers so children can refer to their parents in
 * hierarchies of variables.
 * Added to Sequence methods to tell if a child sequence is done
 * deserializing its data.
 * Fixed the operator=() and copy ctors; removed redundency from
 * _duplicate().
 * Changed the way serialize and deserialize work for sequences. Now SOI and
 * EOS markers are written for every `level' of a nested Sequence. This
 * should fixed nested Sequences. There is still considerable work to do
 * for these to work in all cases.
 *
 * Revision 1.61.4.1  2001/05/12 00:01:07  jimg
 * Added the method toString(). Used for debugging
 *
 * Revision 1.61  2000/10/06 01:26:04  jimg
 * Changed the way serialize() calls read(). The status from read() is
 * returned by the Structure and Sequence serialize() methods; ignored by
 * all others. Any exceptions thrown by read() are caught and discarded.
 * serialize() returns false if read() throws an exception. This should
 * be fixed once all the servers build using the new read() definition.
 *
 * Revision 1.60  2000/09/22 02:52:58  jimg
 * Fixes to the tests to recognize some of the new error messages. Also,
 * the test drivers were modified to catch the exceptions now thrown by
 * some of the parsers.
 *
 * Revision 1.59  2000/09/22 02:17:18  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.58  2000/09/21 16:22:07  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.57  2000/09/11 16:33:29  jimg
 * Fixed up the comments/docs.
 *
 * Revision 1.56  2000/08/02 22:46:48  jimg
 * Merged 3.1.8
 *
 * Revision 1.53.6.1  2000/08/02 21:10:07  jimg
 * Removed the header config_dap.h. If this file uses the dods typedefs for
 * cardinal datatypes, then it gets those definitions from the header
 * dods-datatypes.h.
 *
 * Revision 1.55  2000/07/09 21:57:09  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.54  2000/06/07 18:06:58  jimg
 * Merged the pc port branch
 *
 * Revision 1.53.20.1  2000/06/02 18:11:19  rmorris
 * Mod's for Port to Win32.
 *
 * Revision 1.53.14.1  2000/01/28 22:14:04  jgarcia
 * Added exception handling and modify add_var to get a copy of the object
 *
 * Revision 1.53  1999/05/04 19:47:20  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.52  1999/04/29 02:29:27  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.51  1999/03/24 23:37:13  jimg
 * Added support for the Int16, UInt16 and Float32 types
 *
 * Revision 1.50  1999/03/19 16:45:00  jimg
 * Added to the documentation for print_val(). Now contains a blurb about
 * setprecision().
 *
 * Revision 1.49  1999/01/21 20:42:00  tom
 * Fixed comment formatting problems for doc++
 *
 * Revision 1.48  1998/11/23 15:05:55  tom
 * late modifications to documentation
 *
 * Revision 1.47  1998/11/10 01:10:25  jimg
 * Changed text of regexp error message.
 *
 * Revision 1.46  1998/10/21 16:19:47  jimg
 * Added the two member functions: synthesized_p() and set_synthesized_p().
 * These are used to test and record (resp) whether a variable has been
 * synthesized by the server or is part of the data set. This feature was
 * added to help support the creation of variables by the new projection
 * functions. Variables that are created by projection function calls are
 * called `synthesized variables'. Some documentation strings were fixed.
 *
 * Revision 1.45  1998/09/17 17:22:47  jimg
 * Fix documentation.
 * Added BaseType * stack definition using the STL vector class.
 *
 * Revision 1.44.2.1  1999/02/02 21:56:55  jimg
 * String to string version
 *
 * Revision 1.44  1998/08/06 16:08:51  jimg
 * Fixed some of the doc comments.
 *
 * Revision 1.43  1998/07/13 20:20:42  jimg
 * Fixes from the final test of the new build process
 *
 * Revision 1.42  1998/03/17 17:18:52  jimg
 * Added mfuncs element_count(), is_simple_type(), is_vector_type() and
 * is_comstructor_type().
 *
 * Revision 1.41  1998/02/05 20:13:50  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.40  1998/02/04 14:55:30  tom
 * Another draft of documentation.
 *
 * Revision 1.39  1998/01/12 14:27:55  tom
 * Second pass at class documentation.
 *
 * Revision 1.38  1997/12/18 15:06:09  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.37  1997/10/09 22:19:11  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.36  1997/08/11 20:18:29  jimg
 * Really fixed the comment leaders this time...
 *
 * Revision 1.35  1997/08/11 18:19:12  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.34  1997/03/08 19:01:57  jimg
 * Changed default param to check_semantics() from  to String()
 * and removed the default from the argument list in the mfunc definition
 *
 * Revision 1.33  1997/02/28 01:27:52  jimg
 * Changed check_semantics() so that it now returns error messages in a String
 * object (passed by reference).
 *
 * Revision 1.32  1996/12/02 23:10:04  jimg
 * Added dataset as a parameter to the ops member function.
 *
 * Revision 1.31  1996/11/20 00:58:05  jimg
 * Ripped out old code.
 *
 * Revision 1.30  1996/09/19 16:14:26  jimg
 * Fixed syntax errors in the enum `Type'.
 *
 * Revision 1.29  1996/08/26 19:36:41  jimg
 * Added type constants for 32 bit unsigned ints, 16 bit signed and unsigned
 * ints and 32 bit floats.
 *
 * Revision 1.28  1996/06/04 21:33:11  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.27  1996/05/31 23:29:25  jimg
 * Updated copyright notice.
 *
 * Revision 1.26  1996/05/16 22:49:56  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.25  1996/05/14 15:38:16  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.24  1996/04/05 00:21:23  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.23  1996/04/04 17:29:42  jimg
 * Merged recent changes from version 1.1.1 (including changes for the Type
 * enum which caused a problem on the SGI).
 *
 * Revision 1.22  1996/03/05 18:44:52  jimg
 * Added ce_eval to serailize member function.
 * Added ops member function.
 *
 * Revision 1.21  1996/02/02 00:31:00  jimg
 * Merge changes for DODS-1.1.0 into DODS-2.x
 *
 * Revision 1.20  1995/12/09  01:06:33  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.19  1995/12/06  21:45:01  jimg
 * Changed read() from three parameters to two.
 * Added constrained flag to print_decl().
 * Removed store_val() and read_val() (use buf2val() and val2buf() instead).
 *
 * Revision 1.18  1995/10/23  23:20:49  jimg
 * Added _send_p and _read_p fields (and their accessors) along with the
 * virtual mfuncs set_send_p() and set_read_p().
 *
 * Revision 1.17  1995/08/26  00:31:25  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.16  1995/08/23  00:04:44  jimg
 * Switched from String representation of data type to Type enum.
 * Added type_name() member function so that it is simple to get the string
 * representation of a variable's type.
 * Changed the name of read_val/store_val to buf2val/val2buf.
 *
 * Revision 1.15.2.5  1996/02/27 23:48:28  jimg
 * Fixed errors introduced in the last checkin.
 *
 * Revision 1.15.2.4  1996/02/23 21:37:23  jimg
 * Updated for new configure.in.
 * Fixed problems on Solaris 2.4.
 *
 * Revision 1.15.2.3  1995/09/29  19:27:59  jimg
 * Fixed problems with xdr.h on an SGI.
 * Fixed conflict of d_int32_t (which was in an enum type defined by
 * BaseType) on the SGI.
 *
 * Revision 1.15.2.2  1995/09/27  19:06:58  jimg
 * Add casts to `cast away' const and unsigned in places where we call various
 * xdr functions (which don't know about, or use, const or unsigned.
 *
 * Revision 1.15.2.1  1995/09/14  16:45:20  jimg
 * Changed _duplicate() member function from private to protected so that the
 * chilren of BaseType can call it in their implementations of _duplicate().
 *
 * Revision 1.15  1995/05/10  13:45:09  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.14  1995/03/04  14:34:56  jimg
 * Major modifications to the transmission and representation of values:
 * Added card() virtual function which is true for classes that
 * contain cardinal types (byte, int float, string).
 * Changed the representation of Str from the C rep to a C++
 * class represenation.
 * Chnaged read_val and store_val so that they take and return
 * types that are stored by the object (e.g., inthe case of Str
 * an URL, read_val returns a C++ String object).
 * Modified Array representations so that arrays of card()
 * objects are just that - no more storing strings, ... as
 * C would store them.
 * Arrays of non cardinal types are arrays of the DODS objects (e.g.,
 * an array of a structure is represented as an array of Structure
 * objects).
 *
 * Revision 1.13  1995/02/16  22:46:02  jimg
 * Added _in private member. It is used to keep a copy of the input FILE *
 * so that when the next chunk of data is read in the previous one can be
 * closed. Since the netio library unlinks the tmp file before returning
 * the FILE *, closing it effectively deletes the tmp file.
 *
 * Revision 1.12  1995/02/10  02:41:58  jimg
 * Added new mfuncs to access _name and _type.
 * Made private and protected filed's names start with `_'.
 * Added store_val() as a abstract virtual mfunc.
 *
 * Revision 1.11  1995/01/19  21:59:10  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.10  1995/01/18  18:35:28  dan
 * Defined abstract virtual function 'readVal' which provides access
 * to the object's buf for retrieving data subsequent to deserializing.
 *
 * Revision 1.9  1995/01/11  16:06:48  jimg
 * Added static XDR pointers to BaseType class and removed the XDR pointers
 * that were class members - now there is only one xdrin and one xdrout
 * for all children of BaseType.
 * Added friend functions to help in setting the FILE * associated with
 * the XDR *s.
 * Removed FILE *in member (but FILE *out was kept as FILE * _out, mfunc
 * expunge()).
 * Changed ctor so that it no longer takes FILE * params.
 *
 * Revision 1.8  1994/12/16  22:04:21  jimg
 * Added the mfuncs var() and add_var(). These are used by ctor types. They
 * need to be defined here so that access to them via BaseType * will work
 * (actually, so the code will compile). These versions just print error
 * messages. See Array.h, ... for examples of the real mfuncs.
 *
 * Revision 1.7  1994/12/12  20:33:03  jimg
 * Added enum Part - used to be part of CtorType.
 *
 * Revision 1.6  1994/11/29  19:14:15  jimg
 * Added mroe support for data transmission; BaseType now contains enough
 * functionality to support transmission of all the simple datatypes.
 * Added in and out FILE *.
 * Added boolean flag in serialize which will cause the output buffer to
 * be flushed when data is serialized.
 * Added xdr_coder for serialization of arrays and lists.
 *
 * Revision 1.5  1994/11/22  14:05:29  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.4  1994/10/17  23:30:47  jimg
 * Added ptr_duplicate virtual mfunc. Child classes can also define this
 * to copy parts that BaseType does not have (and allocate correctly sized
 * pointers.
 * Removed protected mfunc error() -- use errmsg library instead.
 * Added formatted printing of types (works with DDS::print()).
 *
 * Revision 1.3  1994/09/23  14:34:44  jimg
 * Added mfunc check_semantics().
 * Moved definition of dtor to BaseType.cc.
 *
 * Revision 1.2  1994/09/15  21:08:56  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
 * Revision 1.1  1994/09/09  15:28:42  jimg
 * Class for base type variables. Int32, ... inherit from this class.
 */

#endif // _basetype_h
