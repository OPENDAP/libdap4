
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for BaseType.
//
// jhrg 9/6/94

// $Log: BaseType.cc,v $
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

#include <stdio.h>		// for stdin and stdout
#include <assert.h>

#include "BaseType.h"
#include "util.h"

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

// Private copy mfunc

void
BaseType::_duplicate(const BaseType &bt)
{
    _name = bt._name;
    _type = bt._type;
    _read_p = bt._read_p;	// added, reza
    _send_p = bt._send_p;	// added, reza
    _xdr_coder = bt._xdr_coder;	// just copy this function pointer
}

// Public mfuncs

// Note that the ctor (as well as the copy ctor via duplicate)
// open/initialize the (XDRS *)s XDRIN and XDROUT to reference sdtin and
// stdout. This means that writing to std{in,out} must work correctly, and
// probably means that is must be OK to mix calls to cout/cin with calls that
// write to std{out,in} (it is for g++ with libg++ at version 2.6 or
// greater).

BaseType::BaseType(const string &n, const Type &t, xdrproc_t xdr)
#ifdef WIN32
    : _name(n), _type(t), _xdr_coder((int *)xdr), _read_p(false), _send_p(false),
#else
    : _name(n), _type(t), _xdr_coder(xdr), _read_p(false), _send_p(false),
#endif
      _synthesized_p(false)
{
} 

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

string 
BaseType::name() const
{
    return _name; 
}

void 
BaseType::set_name(const string &n)
{ 
    _name = n; 
}

Type
BaseType::type() const
{
    return _type;
}

void
BaseType::set_type(const Type &t)
{
    _type = t;
}

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
      case dods_list_c:
	return string("List");
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
      case dods_list_c:
      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	return false;
    }

    return false;
}

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
      case dods_list_c:
	return true;

      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	return false;
    }
    
    return false;
}

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
      case dods_list_c:
	return false;

      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	return true;
    }

    return false;
}

int
BaseType::element_count(bool)
{
    return 1;
}

bool
BaseType::synthesized_p()
{
    return _synthesized_p;
}

void
BaseType::set_synthesized_p(bool state)
{
    _synthesized_p = state;
}

// Return the state of _read_p (true if the value of the variable has been
// read (and is in memory) false otherwise).

bool
BaseType::read_p()
{
    return _read_p;
}

void
BaseType::set_read_p(bool state)
{
    if (! _synthesized_p) {
	DBG(cerr << "Changing read_p state of " << name() << endl);
	_read_p = state;
    }
}


// Return the state of _send_p (true if the variable should be sent, false
// otherwise).

bool
BaseType::send_p()
{
    return _send_p;
}

void 
BaseType::set_send_p(bool state)
{
    _send_p = state;
}

// Defined by constructor types (Array, ...)
//
// Return a pointer to the contained variable in a ctor class.

BaseType *
BaseType::var(const string &, bool)
{
    return (BaseType *)0;
}

BaseType *
BaseType::var(const string &, btp_stack &)
{
    return (BaseType *)0;
}

// Defined by constructor types (Array, ...)

void
BaseType::add_var(BaseType *, Part)
{
}

// Using this mfunc, objects that contain a (BaseType *) can get the xdr
// function used to serialize the object.
#ifdef WIN32
int *
#else
xdrproc_t
#endif
BaseType::xdr_coder()
{
    return _xdr_coder;
}

// send a printed representation of the variable's declaration to cout. If
// print_semi is true, append a semicolon and newline.

void 
BaseType::print_decl(ostream &os, string space, bool print_semi, 
		     bool constraint_info, bool constrained)
{
    // if printing the constrained declaration, exit if this variable was not
    // selected. 
    if (constrained && !send_p())
	return;

    os << space << type_name() << " " << _name;

    if (constraint_info) {
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	os << ";" << endl;
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

bool
BaseType::check_semantics(string &msg, bool)
{
    bool sem = (_type != dods_null_c && _name.length());

    if (!sem) 
	msg = "Every variable must have both a name and a type\n";

    return sem;
}

// Member functions for the relational operators used in evaluating a
// relational clause in a constraint expression. Each class that wants these
// to do something interesting must supply their own versions. These print an
// error message and return False.

bool 
BaseType::ops(BaseType *, int, const string &)
{
    assert("Unimplemented operator" && false);

    cerr << "Unimplemented operator" << endl;

    return false;
}

