
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for BaseType.
//
// jhrg 9/6/94

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

#include <stdio.h>		// for stdin and stdout

#include <sstream>
#include <string>

// #define DODS_DEBUG

#include "debug.h"
#include "BaseType.h"
#include "util.h"
#include "InternalErr.h"
#include "escaping.h"

using namespace std;

// Protected copy mfunc

/** Perform a deep copy. Copies the values of \e bt into \c *this. Pointers
    are dereferenced and their values are copied into a newly allocated
    instance. 
    
    @brief Perform a deep copy.
    @param bt The source object. */
void
BaseType::_duplicate(const BaseType &bt)
{
    _name = bt._name;
    _type = bt._type;
    _read_p = bt._read_p;	// added, reza
    _send_p = bt._send_p;	// added, reza
    d_in_selection = bt.d_in_selection;
    _synthesized_p = bt._synthesized_p; // 5/11/2001 jhrg
    _xdr_coder = bt._xdr_coder;	// just copy this function pointer

    d_parent = bt.d_parent;	// copy pointers 6/4/2001 jhrg

    d_attr = bt.d_attr;		// Deep copy.
}

// Public mfuncs

/** The BaseType constructor needs a name, a type, and the name of
    an XDR filter.  The BaseType class exists to provide data to
    type classes that inherit from it.  The constructors of those
    classes call the BaseType constructor; it is never called
    directly. 

    Note that the constructor (as well as the copy constructor via
    _duplicate) open/initialize the (XDRS *)s XDRIN and XDROUT to
    reference sdtin and stdout. This means that writing to
    std{in,out} must work correctly, and probably means that is
    must be OK to mix calls to cout/cin with calls that write to
    std{out,in} (it is for g++ with libg++ at version 2.6 or
    greater).

    @brief The BaseType constructor.
    @param n A string containing the name of the new variable.
    @param t The type of the variable.
    @param xdr A pointer to an XDR filter to use to transmit the
    data in this variable to a client DODS process.
    @see Type */
BaseType::BaseType(const string &n, const Type &t, xdrproc_t xdr)
    : _name(n), _type(t), _xdr_coder(xdr), _read_p(false), _send_p(false),
      d_in_selection(false), _synthesized_p(false), d_parent(0)
{
} 

/** @brief The BaseType copy constructor. */
BaseType::BaseType(const BaseType &copy_from)
{
    _duplicate(copy_from);
}
    
BaseType::~BaseType()
{
    DBG(cerr << "Entering ~BaseType (" << this << ")" << endl);
    DBG(cerr << "Exiting ~BaseType" << endl);
}

BaseType &
BaseType::operator=(const BaseType &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

/** Write out the object's internal fields in a string. To be used for
    debugging when regular inspection w/ddd or gdb isn't enough.

    @return A string which shows the object's internal stuff. */
string
BaseType::toString()
{
    ostringstream oss;
    oss << "BaseType (" << this << "):" << endl
	<< "          _name: " << _name << endl
	<< "          _type: " << _type << endl
	<< "          _read_p: " << _read_p << endl
	<< "          _send_p: " << _send_p << endl
	<< "          _synthesized_p: " << _synthesized_p << endl 
	<< "          d_parent: " << d_parent << endl
	<< "          d_attr: " << hex << &d_attr << dec << endl;

    return oss.str();
}

/** @brief Returns the name of the class instance. 
 */
string 
BaseType::name() const
{
    return _name; 
}

/** @brief Sets the name of the class instance. */
void 
BaseType::set_name(const string &n)
{ 
    string name = n;
    _name = www2id(name);	// www2id writes into its param.
}

/** @brief Returns the type of the class instance. */
Type
BaseType::type() const
{
    return _type;
}

/** @brief Sets the type of the class instance. */
void
BaseType::set_type(const Type &t)
{
    _type = t;
}

/** @brief Returns the type of the class instance as a string. */
string
BaseType::type_name() const
{
    switch(_type) {
      case dods_null_c:
	return string("Null");
      case dods_byte_c:
	return string("Byte");
      case dods_int16_c:
	return string("Int16");
      case dods_uint16_c:
	return string("UInt16");
      case dods_int32_c:
	return string("Int32");
      case dods_uint32_c:
	return string("UInt32");
      case dods_float32_c:
	return string("Float32");
      case dods_float64_c:
	return string("Float64");
      case dods_str_c:
	return string("String");
      case dods_url_c:
	return string("Url");
      case dods_array_c:
	return string("Array");
      case dods_structure_c:
	return string("Structure");
      case dods_sequence_c:
	return string("Sequence");
      case dods_grid_c:
	return string("Grid");
      default:
	cerr << "BaseType::type_name: Undefined type" << endl;
	return string("");
    }
}

/** @brief Returns true if the instance is a simple type variable. */
bool
BaseType::is_simple_type()
{
    switch (type()) {
      case dods_null_c:
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
      case dods_float64_c:
      case dods_str_c:
      case dods_url_c:
	return true;

      case dods_array_c:
      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	return false;
    }

    return false;
}

/** @brief Returns true if the instance is a vector type variable. */
bool
BaseType::is_vector_type()
{
    switch (type()) {
      case dods_null_c:
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
      case dods_float64_c:
      case dods_str_c:
      case dods_url_c:
	return false;

      case dods_array_c:
	return true;

      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	return false;
    }
    
    return false;
}

/** @brief Returns true if the instance is a constructor type variable. */
bool
BaseType::is_constructor_type()
{
    switch (type()) {
      case dods_null_c:
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
      case dods_float64_c:
      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
	return false;

      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	return true;
    }

    return false;
}

/** Return a count of the total number of variables in this variable.
    This is used to count the number of variables held by a constructor
    variable - for simple type and vector variables it always
    returns 1.

    For compound data types, there are two ways to count members.
    You can count the members, or you can count the simple members
    and add that to the count of the compound members.  For
    example, if a Structure contains an Int32 and another
    Structure that itself contains two Int32 members, the element
    count of the top-level structure could be two (one Int32 and
    one Structure) or three (one Int32 by itself and two Int32's
    in the subsidiary Structure).  Use the <i>leaves</i> parameter
    to control which kind of counting you desire.

    @brief Count the members of constructor types. 
    @return Returns 1 for simple
    types.  For compound members, the count depends on the
    <i>leaves</i> argument.
    @param leaves This parameter is only relevant if the object
    contains other compound data types.  If FALSE, the function
    counts only the data variables mentioned in the object's
    declaration.  If TRUE, it counts the simple members, and adds
    that to the sum of the counts for the compound members.
    This parameter has no effect for simple type variables. */
int
BaseType::element_count(bool)
{
    return 1;
}

/** Returns true if the variable is a synthesized variable. A synthesized
    variable is one that is added to the dataset by the server (usually
    with a `projection function'. */
bool
BaseType::synthesized_p()
{
    return _synthesized_p;
}

/** Set the synthesized flag. Before setting this flag be sure to set the
    <tt>read_p()</tt> state. Once this flag is set you cannot
    alter the state of the <tt>read_p</tt> flag!
	
    @see synthesized_p() */
void
BaseType::set_synthesized_p(bool state)
{
    _synthesized_p = state;
}

// Return the state of _read_p (true if the value of the variable has been
// read (and is in memory) false otherwise).

/** Returns true if the value(s) for this variable have been read from the
    data source, oterwise returns false. This method is used to determine
    when values need to be read using the read() method. When read_p()
    returns true, this library assumes that buf2val() (and other methods
    such as get_vec()) can be used to access the value(s) of a variable.

    @brief Has this variable been read?
    @return True if the variable's value(s) have been read, false otherwise. */
bool
BaseType::read_p()
{
    return _read_p;
}

/** Sets the value of the <tt>read_p</tt> property. This indicates that the
    value(s) of this variable has/have been read. An implementation of the
    read() method would typically use this to set the \c read_p property to
    true.

    @note For synthesized variables, this method does nothing. Thus, if a
    synthesized variable is added to a Sequence, the Sequence can iteratively
    reset the \e read_p property without affecting the value of that property
    for the synthesized variable. That's important since a synthesized
    variable's value is calculated, not read. 

    @todo Look at making synthesized variables easier to implement and at
    making them more integrated into the overall CE evaluation process.
    Maybe the code that computes the synthesized var's value should be in the
    that variable's read() method? This might provide a way to get rid of the
    awkward 'projection functions' by replacing them with real children of
    BaseType. It would also provide a way to clean up the way the
    \e synthesized_p prop intrudes on the \e read_p prop.

    @brief Sets the value of the \e read_p property.
    @param state Set the \e read_p property to this state. */
void
BaseType::set_read_p(bool state)
{
    if (! _synthesized_p) {
	DBG(cerr << "Changing read_p state of " << name() << endl);
	_read_p = state;
    }
}

/** Returns the state of the \c send_p property. If true, this variable
    should be sent to the client, if false, it should not. If no constraint
    expression (CE) has been evaluated, this property is true for all
    variables in a data source (i.e., for all the variables listed in a DDS).
    If a CE has been evaluated, this property is true only for those
    variables listed in the <em>projection part</em> of the CE.

    @brief Should this variable be sent?
    @return True if the variable should be sent to the client, false
    otherwise. */
bool
BaseType::send_p()
{
    return _send_p;
}

/** Sets the value of the <tt>send_p</tt> flag.  This
    function is meant to be called from <tt>serialize()</tt>.  Data is
    ready to be sent when <i>both</i> the <tt>_send_p</tt> and
    <tt>_read_p</tt> flags are set to TRUE.

    @param state The logical state to set the <tt>send_p</tt> flag.
*/
void 
BaseType::set_send_p(bool state)
{
    DBG(cerr << "Calling BaseType::set_send_p() for: " << this->name() 
	<< endl);
    _send_p = state;
}


/** Get this variable's AttrTable. It's generally a bad idea to return a
    reference to a contained object, but in this case it seems that building
    an interface inside BaseType is overkill. 

    Use the AttrTable methods to manipulate the table. */
AttrTable &
BaseType::get_attr_table()
{
    return d_attr;
}

/** Set this variable's attribute table.
    @param at Source of the attribtues. */
void
BaseType::set_attr_table(const AttrTable &at)
{
    d_attr = at;
}

/** Does this variable appear in either the selection part or as a function
    argument in the current constrain expression. If this property is set
    (true) then implementation of the read() method should read this
    variable.

    @see BaseType::read()
    @brief Is this variable part of the current selection? */
bool 
BaseType::is_in_selection()
{
    return d_in_selection;
}

/** Set the \e in_selection property to \e state. This property indicates
    that the variable is used as a parameter to a constraint expression
    function or that it appears as an argument in a selection sub-expression.
    If set (true), implementations of the BaseType::read() method should read
    this variable.

    @param state Set the \e in_selection property to this state.
    @see BaseType::read() */
void
BaseType::set_in_selection(bool state)
{
    d_in_selection = state;
}

// Protected method.
/** Set the <tt>parent</tt> property for this variable. Only instances of
    Constructor or Vector should call this method.

    @param parent Pointer to the Constructor of Vector parent variable.
    @exception InternalErr thrown if called with anything other than a
    Constructor or Vector. */
void
BaseType::set_parent(BaseType *parent) throw(InternalErr)
{
    if (!dynamic_cast<Constructor *>(parent)
	&& !dynamic_cast<Vector *>(parent))
	throw InternalErr("Call to set_parent with incorrect variable type.");

    d_parent = parent;
}

// Public method.

/** Return a pointer to the Constructor or Vector which holds (contains)
    this variable. If this variable is at the top level, this method
    returns null.

    NB: The modifier for this property is protected. It should only be
    modified by Constructor and Vector.

    @return A BaseType pointer to the variable's parent. */
BaseType *
BaseType::get_parent()
{
    return d_parent;
}

// Defined by constructor types (Array, ...)
//
// Return a pointer to the contained variable in a ctor class.

/** Returns a pointer to the contained variable in a composite class. The
    composite classes are those made up of aggregated simple data types.
    Array, Grid, and Structure are composite types, while Int and Float are
    simple types. This function is only used by composite classes. The
    BaseType implementation always returns null.

    Several of the subclasses overload this function with alternate access
    methods that make sense for that particular data type. For example, the
    Array class defines a <tt>*var(int i)</tt> method that returns the ith
    entry in the Array data, and the Structure provides a <tt>*var(Pix
    p)</tt> function using a pseudo-index to access the different members of
    the structure.

    @brief Returns a pointer to a member of a constructor class.
    @param n The name of the class member.  
    @param exact
    True if only interested in variables whose full names match
    \e n exactly. If false, returns the first variable
    whose name matches \e n. For example, if
    \e n is \c x and \c point.x is a variable, then var("x",
    false) would return a BaseType pointer to \c point.x. If
    \e exact was <tt>true</tt> then \e n
    would need to be \c "point.x" for var to return that
    pointer. This feature simplifies constraint expressions for
    datasets which have complex, nested, constructor variables.
    @param s Record the path to \e n.

    @return A pointer to the member named in the \e n argument. If no name is
    given, the function returns the first (only) variable.  For example, an
    Array has only one variable, while a Structure can have many. */
BaseType *
BaseType::var(const string &, bool, btp_stack*)
{
    return static_cast<BaseType *>(0);
}

/** This version of var(...) searches for <i>name</i> and returns a
    pointer to the BaseType object if found. It uses the same search
    algorithm as above when <i>exact_match</i> is false. In addition to
    returning a pointer to the variable, it pushes onto <i>s</i> a
    BaseType pointer to each constructor type that ultimately contains
    <i>name</i>.

    @note The BaseType implementation always returns null.

    @deprecated This method is deprecated because is tries first to use
    exact_match and, if that fails, then tries leaf_match. It's better to use
    the alternate form of var(...) and specify exactly what you'd like to do.

    @param n Find the variable whose name is <i>name</i>.
    @param s Record the path to <i>name</i>.
    @return A pointer to the named variable. */
BaseType *
BaseType::var(const string &, btp_stack &)
{
    return static_cast<BaseType *>(0);
}

/** Adds a variable to an instance of a constructor class, such as Array,
    Structure <em>et cetera</em>. This function is only used by those
    classes. For constructors with more than one variable, the variables
    appear in the same order in which they were added (i.e., the order in
    which add_var() was called). Since this method is only for use by Vectors
    and Constructors, the BaseType implementation does nothing.

    @note Implementation of this method in Structure, Sequence, et c., first
    copy \e bt and then insert the copy. If \e bt is itself a constructor
    type you must either use the var() method to get a pointer to the actual
    instance added to \c *this or you must first add all of <em>bt</em>'s
    children to it before adding it to \c *this. The implementations use
    _duplicate() to perform a deep copy of \e bt.

    @brief Add a variable.

    @todo The BaseType implementation of this method should throw an
    InternalErr.

    @todo We should get rid of the Part parameter and adopt the convention
    that the first variable is the Array and all subsequent ones are Maps
    (when dealing with a Grid, the only time Part matters). This would enable
    several methods to migrate from Structure, Sequence and Grid to
    Constructor. 

    @param bt The variable to be added to this instance. The caller of this
    method <i>must</i> free memory it allocates for <tt>v</tt>. This method
    will make a deep copy of the object pointed to by <tt>v</tt>.
    @param part The part of the constructor data to be modified. Only
    meaningful for Grid variables.
    
    @see Part */
void
BaseType::add_var(BaseType *, Part)
{
}

/** This method should be implemented for each of the data type classes (Byte,
    ..., Grid) when using the DAP class library to build a server. This
    method is only for DAP servers. The library provides a default
    definition here which throws an InternalErr exception.

    When implementing a new DAP server, the Byte, ..., Grid data type classes
    are usually specialized. In each of those specializations read() should
    be defined to read values from the data source and store them in the
    object's local buffer. The read() method is called by other methods in
    this library. When writing read(), follow these rules:

    <ul>
    <li> read() should throw Error if it encounters an error. The message
	  should be verbose enough to be understood by someone running a
	  client on a different machine.</li>
    <li> The value(s) should be read iff either send_p() of
          is_in_selection() return true. If neither of these return true, the
	  value(s) should not be read. This is important when writing read()
	  for a Constructor type such as Grid where a client may ask for only
	  the map vectors (ans thus reading the much larger Array part is not
	  needed).</li>
    <li>The Array::read() and Grid::read() methods should take into account
	  any restrictions on Array sizes.</li>
    </ul>

    @brief Read data into a local buffer. 

    @return The return value of this method should always be false. This
    method used to use the return value to indicate that Sequence values
    remained and still needed to be read. However, all Sequence values are
    now read in a single call to read(). The return value is retained because
    it's conceivable that a server might want to implement read() and make
    use of the return value. Only the specialization would use the return
    value; it is always ignored by the DAP library.

    @param dataset A string naming the dataset from which the data is to
    be read. The meaning of this string will vary among data APIs.

    @see BaseType */
bool 
BaseType::read(const string &dataset)
{
    throw InternalErr("Unimplemented BaseType::read() method called.");
}

/** The <tt>xdr_coder</tt> function (also "filter primitive") is used to
    encode and decode each element in a multiple element data
    structure.  These functions are used to convert data to and from
    its local representation to the XDR representation, which is
    used to transmit and receive the data.  See <tt>man xdr</tt> for more
    information about the available XDR filter primitives.

    Note that this class data is only used for multiple element data
    types.  The simple data types (Int, Float, and so on), are
    translated directly.

    @brief Returns a function used to encode elements of an array. 
    @return A C function used to encode data in the XDR format.
*/
xdrproc_t
BaseType::xdr_coder()
{
    return _xdr_coder;
}

// send a printed representation of the variable's declaration to cout. If
// print_semi is true, append a semicolon and newline.

/** Write the variable's declaration in a C-style syntax. This
    function is used to create textual representation of the Data
    Descriptor Structure (DDS).  See <i>The DODS User Manual</i> for
    information about this structure.

    A simple array declaration might look like this:
    \verbatim
    Float64 lat[lat = 180];
    \endverbatim
    While a more complex declaration (for a Grid, in this case),
    would look like this:
    \verbatim
    Grid {
    ARRAY:
    Int32 sst[time = 404][lat = 180][lon = 360];
    MAPS:
    Float64 time[time = 404];
    Float64 lat[lat = 180];
    Float64 lon[lon = 360];
    } sst;
    \endverbatim

    @brief Print an ASCII representation of the variable structure.
    @deprecated Use the version of this method which takes a FILE pointer. 

    @param os The output stream on which to print the
    declaration.
    @param space Each line of the declaration will begin with the
    characters in this string.  Usually used for leading spaces.
    @param print_semi A boolean value indicating whether to print a
    semicolon at the end of the declaration.
    @param constraint_info A boolean value indicating whether
    constraint information is to be printed with the declaration.
    If the value of this parameter is TRUE, <tt>print_decl()</tt> prints
    the value of the variable's <tt>send_p()</tt> flag after the
    declaration. 
    @param constrained If this boolean value is TRUE, the variable's
    declaration is only printed if is the <tt>send_p()</tt> flag is TRUE.
    If a constraint expression is in place, and this variable is not
    requested, the <tt>send_p()</tt> flag is FALSE.

    @see DDS
    @see DDS::CE
*/
void 
BaseType::print_decl(ostream &os, string space, bool print_semi, 
		     bool constraint_info, bool constrained)
{
    // if printing the constrained declaration, exit if this variable was not
    // selected. 
    if (constrained && !send_p())
	return;

    os << space << type_name() << " " << id2www(_name);

    if (constraint_info) {
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	os << ";" << endl;
}

/** Write the variable's declaration in a C-style syntax. This
    function is used to create textual representation of the Data
    Descriptor Structure (DDS).  See <i>The DODS User Manual</i> for
    information about this structure.

    A simple array declaration might look like this:
    \verbatim
    Float64 lat[lat = 180];
    \endverbatim
    While a more complex declaration (for a Grid, in this case),
    would look like this:
    \verbatim
    Grid {
    ARRAY:
    Int32 sst[time = 404][lat = 180][lon = 360];
    MAPS:
    Float64 time[time = 404];
    Float64 lat[lat = 180];
    Float64 lon[lon = 360];
    } sst;
    \endverbatim

    @brief Print an ASCII representation of the variable structure.
    @param out The output stream on which to print the
    declaration.
    @param space Each line of the declaration will begin with the
    characters in this string.  Usually used for leading spaces.
    @param print_semi A boolean value indicating whether to print a
    semicolon at the end of the declaration.
    @param constraint_info A boolean value indicating whether
    constraint information is to be printed with the declaration.
    If the value of this parameter is TRUE, <tt>print_decl()</tt> prints
    the value of the variable's <tt>send_p()</tt> flag after the
    declaration. 
    @param constrained If this boolean value is TRUE, the variable's
    declaration is only printed if is the <tt>send_p()</tt> flag is TRUE.
    If a constraint expression is in place, and this variable is not
    requested, the <tt>send_p()</tt> flag is FALSE.

    @see DDS
    @see DDS::CE
*/
void 
BaseType::print_decl(FILE *out, string space, bool print_semi, 
		     bool constraint_info, bool constrained)
{
    // if printing the constrained declaration, exit if this variable was not
    // selected. 
    if (constrained && !send_p())
	return;

    fprintf( out, "%s%s %s", space.c_str(), type_name().c_str(),
	     id2www(_name).c_str() ) ;

    if (constraint_info) {
	if (send_p())
	    fprintf( stdout, ": Send True" ) ;
	else
	    fprintf( stdout, ": Send False" ) ;
    }

    if (print_semi)
	fprintf( out, ";\n" ) ;
}

/** Write the XML representation of this variable. This method is used to
    build the DDX XML response.
    @param out Destination.
    @param space Use this to indent child declarations. Default is "".
    @param constrained If true, only print this if it's part part of the
    current projection. Default is False. */
void
BaseType::print_xml(FILE *out, string space, bool constrained)
{
    if (constrained && !send_p())
	return;

    fprintf(out, "%s<%s", space.c_str(), type_name().c_str());
    if (!_name.empty())
	fprintf(out, " name=\"%s\"", id2xml(_name).c_str());

    if (get_attr_table().get_size() > 0) {
	fprintf(out, ">\n");	// close the varaible's tag
	get_attr_table().print_xml(out, space + "    ", constrained);
	// After attributes, print closing tag
	fprintf(out, "%s</%s>\n", space.c_str(), type_name().c_str());
    }
    else {
	fprintf(out, "/>\n");	// no attributes; just close tag.
    }
}

// Compares the object's current state with the semantics of a particular
// type. This will typically be defined in ctor classes (which have
// complicated semantics). For BaseType, an object is semantically correct if
// it has both a non-null name and type.
//
// NB: This is not the same as an invariant -- during the parse objects exist
// but have no name. Also, the bool ALL defaults to false for BaseType. It is
// used by children of CtorType.
//
// Returns: true if the object is semantically correct, false otherwise.

/** This function checks the class instance for internal
    consistency.  This is important to check for complex constructor
    classes.  For BaseType, an object is semantically correct if it
    has both a non-null name and type.

    For example, an Int32 instance would return FALSE if it had no
    name or no type defined.  A Grid instance might return FALSE for
    more complex reasons, such as having Map arrays of the wrong
    size or shape.

    This function is used by the DDS class, and will rarely, if
    ever, be explicitly called by a DODS application program.  A
    variable must pass this test before it is sent, but there may be
    many other stages in a retrieve operation where it would fail.

    @brief Compare an object's current state with the sematics of its
    type.
    @return Returns FALSE when the current state violates some
    aspect of the type semantics, TRUE otherwise.
  
    @param msg A returned string, containing a message indicating
    the source of any problem.
    @param all For complex constructor types (Grid,
    Sequence, Structure), this flag indicates whether to check the
    sematics of the member variables, too.

    @see DDS::check_semantics 
*/
bool
BaseType::check_semantics(string &msg, bool)
{
    bool sem = (_type != dods_null_c && _name.length());

    if (!sem) 
	msg = "Every variable must have both a name and a type\n";

    return sem;
}

/** This method contains the relational operators used by the constraint
    expression evaluator in the DDS class. Each class that wants to be able
    to evaluate relational expressions must overload this function. The
    implementation in BaseType throws an InternalErr exception. The DAP
    library classes Byte, ..., Url provide specializations of this method. It
    is not meaningful for classes such as Array because relational
    expressions using Array are not supported.

    The <i>op</i> argument refers to a table generated by bison from
    the constraint expression parser.  Use statements like the
    following to correctly interpret its value:

    \verbatim
    switch (op) {
        case EQUAL: return i1 == i2;
        case NOT_EQUAL: return i1 != i2;
        case GREATER: return i1 > i2;
        case GREATER_EQL: return i1 >= i2;
        case LESS: return i1 < i2;
        case LESS_EQL: return i1 <= i2;
        case REGEXP: throw Error("Regular expressions are not supported for integer values");
        default: throw Error("Unknown operator");
    }
    \endverbatim

    This function is used by the constraint expression evaluator.

    @brief Evaluate relational operators.
    @param b Comare the value of this instance with \e b.
    @param op An integer index indicating which relational operator
    is implied. Choose one from the following: <tt>EQUAL</tt>,
    <tt>NOT_EQUAL</tt>, <tt>GREATER</tt>, <tt>GREATER_EQL</tt>,
    <tt>LESS</tt>, <tt>LESS_EQL</tt>, and <tt>REGEXP</tt>. 
    @param dataset The name of the dataset from which the instance's
    data has come (or is to come).
    @return The boolean value of the comparison. */
bool 
BaseType::ops(BaseType *, int, const string &)
{
    // Even though ops is a public method, it can never be called because
    // they will never have a BaseType object since this class is abstract,
    // however any of the child classes could by mistake call BaseType::ops
    // so this is an internal error. Jose Garcia
    throw InternalErr(__FILE__, __LINE__, "Unimplemented operator.");
}

// $Log: BaseType.cc,v $
// Revision 1.56  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.54  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
// Revision 1.51.2.5  2004/01/22 17:09:52  jimg
// Added std namespace declarations since the DBG() macro uses cerr.
//
// Revision 1.55  2003/12/10 21:11:57  jimg
// Merge with 3.4. Some of the files contains erros (some tests fail). See
// the ChangeLog for information about fixes.
//
//
// Revision 1.51.2.3  2003/09/28 20:57:22  rmorris
// Discontinued use of XDR_PROC typedef, using xdrproc_t instead - a
// define from the xdr portion of the rpc library.
//
// Revision 1.51.2.2  2003/09/06 22:23:24  jimg
// Significant changes to the documentation. Added the in_selection property and
// accessor/mutator methods for it. Added use of the typedef XDR_PROC which
// allowed me to remove some of the #if WIN32 code (see dods-datatypes.h.in).
//
// Revision 1.51.2.1  2003/06/05 20:15:25  jimg
// Removed many uses of strstream and replaced them with stringstream.
//
// Revision 1.53  2003/05/30 16:52:45  jimg
// Minor fixes to the comments.
//
// Revision 1.52  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.51  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.50  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.49.2.1  2003/02/21 00:10:06  jimg
// Repaired copyright.
//
// Revision 1.49  2003/01/23 00:22:23  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.48  2003/01/10 19:46:39  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.42.4.10  2002/12/17 22:35:02  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.47  2002/06/27 16:26:48  tom
// typo
//
// Revision 1.46  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.45  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.42.4.9  2002/05/09 16:55:08  jimg
// Added a definition for read(). This method was abstract; now it has an
// implementation, albeit one that just throws an InternalErr exception. This
// means that clients do not have to subclass the datatypes anymore! Simple
// clients such as geturl should work just fine linking to the library without a
// dummy set of specializations for the datatypes.
//
// Revision 1.42.4.8  2002/04/03 13:34:29  jimg
// Added using std::endl and std::ends.
//
// Revision 1.42.4.7  2002/03/01 21:03:08  jimg
// Significant changes to the var(...) methods. These now take a btp_stack
// pointer and are used by DDS::mark(...). The exact_match methods have also
// been updated so that leaf variables which contain dots in their names
// will be found. Note that constructor variables with dots in their names
// will break the lookup routines unless the ctor is the last field in the
// constraint expression. These changes were made to fix bug 330.
//
// Revision 1.42.4.6  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.44  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.42.4.5  2001/08/18 01:48:30  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.42.4.4  2001/07/28 01:10:41  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.43  2001/06/15 23:49:01  jimg
// Merged with release-3-2-4.
//
// Revision 1.42.4.3  2001/06/07 16:58:06  jimg
// Added explicit include of debug.h.
//
// Revision 1.42.4.2  2001/06/05 06:49:19  jimg
// Added the Constructor class which is to Structures, Sequences and Grids
// what Vector is to Arrays and Lists. This should be used in future
// refactorings (I thought it was going to be used for the back pointers).
// Introduced back pointers so children can refer to their parents in
// hierarchies of variables.
// Added to Sequence methods to tell if a child sequence is done
// deserializing its data.
// Fixed the operator=() and copy ctors; removed redundency from
// _duplicate().
// Changed the way serialize and deserialize work for sequences. Now SOI and
// EOS markers are written for every `level' of a nested Sequence. This
// should fixed nested Sequences. There is still considerable work to do
// for these to work in all cases.
//
// Revision 1.42.4.1  2001/05/12 00:00:26  jimg
// Fixed a bug where the field _synthesized_p was not copied in _duplicate.
// Added an implementation of toString(). Used for debugging.
//
// Revision 1.42  2000/09/22 02:17:18  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.41  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.40  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.39  2000/06/16 18:14:59  jimg
// Merged with 3.1.7
//
// Revision 1.37.6.1  2000/06/14 16:59:01  jimg
// Added instrumentation for the dtor.
//
// Revision 1.38  2000/06/07 18:06:57  jimg
// Merged the pc port branch
//
// Revision 1.37.20.1  2000/06/02 18:11:19  rmorris
// Mod's for Port to Win32.
//
// Revision 1.37.14.2  2000/02/17 05:03:12  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.37.14.1  2000/01/28 22:14:04  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.37  1999/05/04 19:47:20  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.36  1999/04/29 02:29:27  jimg
// Merge of no-gnu branch
//
// Revision 1.35  1999/03/24 23:37:13  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.34 1998/10/21 16:18:19 jimg Added the two member functions:
// synthesized_p() and set_synthesized_p(). These are used to test and record
// (resp) whether a variable has been synthesized by the server or is part of
// the data set. This feature was added to help support the creation of
// variables by the new projection functions. Variables that are created by
// projection function calls are called `synthesized variables'.
//
// Revision 1.33  1998/09/17 17:23:20  jimg
// Changes for the new variable lookup scheme. Fields of ctor types no longer
// need to be fully qualified. my.thing.f1 can now be named `f1' in a CE. Note
// that if there are two `f1's in a dataset, the first will be silently used;
// There's no warning about the situation. The new code in the var member
// function passes a stack of BaseType pointers so that the projection
// information (send_p field) can be set properly.
//
// Revision 1.32.6.1  1999/02/02 21:56:55  jimg
// String to string version
//
// Revision 1.32  1998/03/19 23:20:05  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.31  1998/03/17 17:18:32  jimg
// Added mfuncs element_count(), is_simple_type(), is_vector_type() and
// is_comstructor_type().
//
// Revision 1.30  1997/03/08 19:01:55  jimg
// Changed default param to check_semantics() from  to String()
// and removed the default from the argument list in the mfunc definition
//
// Revision 1.29  1997/02/28 01:27:50  jimg
// Changed check_semantics() so that it now returns error messages in a String
// object (passed by reference).
//
// Revision 1.28  1996/12/02 23:10:02  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.27  1996/10/28 23:43:55  jimg
// Added UInt32 to type names returned by type_name() member function.
//
// Revision 1.26  1996/06/04 21:33:09  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.25  1996/05/31 23:29:24  jimg
// Updated copyright notice.
//
// Revision 1.24  1996/05/30 17:17:14  jimg
// Added read_p and send_p to the set of members copied by _duplicate(). From
// Reza.
//
// Revision 1.23  1996/05/14 15:38:14  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.22  1996/04/05 00:21:21  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.21  1996/04/04 19:18:32  jimg
// Merged changes from version 1.1.1.
//
// Revision 1.20  1996/03/05 18:45:29  jimg
// Added ops member function.
//
// Revision 1.19  1995/12/06  21:49:53  jimg
// var(): now returns null for anything that does not define its own version.
// print_decl(): uses `constrained' flag.
//
// Revision 1.18  1995/10/23  23:20:47  jimg
// Added _send_p and _read_p fields (and their accessors) along with the
// virtual mfuncs set_send_p() and set_read_p().
//
// Revision 1.17  1995/08/26  00:31:24  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.16  1995/08/23  00:04:45  jimg
// Switched from String representation of data type to Type enum.
// Added type_name() member function so that it is simple to get the string
// representation of a variable's type.
// Changed the name of read_val/store_val to buf2val/val2buf.
//
// Revision 1.15.2.2  1996/03/01 00:06:07  jimg
// Removed bad attempt at multiple connect implementation.
//
// Revision 1.15.2.1  1996/02/23 21:37:21  jimg
// Updated for new configure.in.
// Fixed problems on Solaris 2.4.
//
// Revision 1.15  1995/07/09  21:28:52  jimg
// Added copyright notice.
//
// Revision 1.14  1995/05/10  15:33:54  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.13  1995/05/10  13:45:06  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.12  1995/03/16  17:26:36  jimg
// Moved include of config_dap.h to top of includes.
// Added TRACE_NEW switched dbnew debugging includes.
//
// Revision 1.11  1995/02/16  22:46:00  jimg
// Added _in private member. It is used to keep a copy of the input FILE *
// so that when the next chunk of data is read in the previous one can be
// closed. Since the netio library unlinks the tmp file before returning
// the FILE *, closing it effectively deletes the tmp file.
//
// Revision 1.10  1995/02/10  02:41:56  jimg
// Added new mfuncs to access _name and _type.
// Made private and protected filed's names start with `_'.
// Added store_val() as a abstract virtual mfunc.
//
// Revision 1.9  1995/01/18  18:33:25  dan
// Added external declarations for utility functions, new_xdrstdio and
// delete_xdrstdio.
//
// Revision 1.8  1995/01/11  16:06:47  jimg
// Added static XDR pointers to BaseType class and removed the XDR pointers
// that were class members - now there is only one xdrin and one xdrout
// for all children of BaseType.
// Added friend functions to help in setting the FILE * associated with
// the XDR *s.
// Removed FILE *in member (but FILE *out was kept as FILE * _out, mfunc
// expunge()).
// Changed ctor so that it no longer takes FILE * params.
//
// Revision 1.7  1994/12/16  22:01:42  jimg
// Added mfuncs var() and add_var() to BaseType. These print an error
// message when called with a simple BaseType (Int32, ...). Classes like
// Array use them and provide their own definitions.
//
// Revision 1.6  1994/11/29  19:59:01  jimg
// Added FILE * input and output buffers. All data set and all data received
// passes through these buffers. This simplifies testing and makes using
// the toolkit with files a little easier.
// Added xdrin and xdrout members (both are XDR *). These are the source and
// sink for xdr data.
// Modified ctor and duplicate() to correctly handle xdrin/out.
// Added expunge() which flushes the output buffer.
//
// Revision 1.5  1994/11/22  14:05:26  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.4  1994/10/17  23:30:46  jimg
// Added ptr_duplicate virtual mfunc. Child classes can also define this
// to copy parts that BaseType does not have (and allocate correctly sized
// pointers.
// Removed protected mfunc error() -- use errmsg library instead.
// Added formatted printing of types (works with DDS::print()).
//
// Revision 1.3  1994/09/23  14:34:42  jimg
// Added mfunc check_semantics().
// Moved definition of dtor to BaseType.cc.
//
// Revision 1.2  1994/09/15  21:08:36  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/09  15:28:41  jimg
// Class for base type variables. Int32, ... inherit from this class.

