
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for BaseType.
//
// jhrg 9/6/94

// $Log: BaseType.cc,v $
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

#include "BaseType.h"
#include "util.h"

// Initial definition of the protected static members _xdrin and
// _xdrout. By default they use the stdin and stdout streams (resp).

XDR * BaseType::_xdrin = new_xdrstdio(stdin, XDR_DECODE);
XDR * BaseType::_xdrout = new_xdrstdio(stdout, XDR_ENCODE);
FILE * BaseType::_out = stdout;
FILE * BaseType::_in = stdin;

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

// friend functions

// Delete the current XDR * assigned to _xdrin, free the storage, create a
// new XDR * and assign it to _xdrin.

void 
set_xdrin(FILE *in)
{
    delete_xdrstdio(BaseType::_xdrin);
    if (BaseType::_in != stdin)
	fclose(BaseType::_in);
    BaseType::_xdrin = new_xdrstdio(in, XDR_DECODE);
    BaseType::_in = in;
}

// Same as above except do it for _xdrout instead of _xdrin and store the
// OUT in private member _out. _out is used by mfunc expunge().

void 
set_xdrout(FILE *out)
{
    delete_xdrstdio(BaseType::_xdrout);
    BaseType::_xdrout = new_xdrstdio(out, XDR_ENCODE);
    BaseType::_out = out;
}

// Public mfuncs

// Note that the ctor (as well as the copy ctor via duplicate)
// open/initialize the (XDRS *)s XDRIN and XDROUT to reference sdtin and
// stdout. This means that writing to std{in,out} must work correctly, and
// probably means that is must be OK to mix calls to cout/cin with calls that
// write to std{out,in} (it is for g++ with libg++ at version 2.6 or
// greater).

BaseType::BaseType(const String &n, const Type &t, xdrproc_t xdr)
    : _name(n), _type(t), _xdr_coder(xdr), _read_p(false), _send_p(false)
{
} 

BaseType::BaseType(const BaseType &copy_from)
{
    _duplicate(copy_from);
}
    
BaseType::~BaseType()
{
}

BaseType &
BaseType::operator=(const BaseType &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

String 
BaseType::name() const
{
    return _name; 
}

void 
BaseType::set_name(const String &n)
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

String
BaseType::type_name() const
{
    switch(_type) {
      case dods_null_c:
	return String("Null");
      case dods_byte_c:
	return String("Byte");
      case dods_int32_c:
	return String("Int32");
      case dods_float64_c:
	return String("Float64");
      case dods_str_c:
	return String("String");
      case dods_url_c:
	return String("Url");
      case dods_array_c:
	return String("Array");
      case dods_list_c:
	return String("List");
      case dods_structure_c:
	return String("Structure");
      case dods_sequence_c:
	return String("Sequence");
      case dods_function_c:
	return String("Function");
      case dods_grid_c:
	return String("Grid");
      default:
	cerr << "BaseType::type_name: Undefined type" << endl;
	return String("");
    }
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
    _read_p = state;
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
BaseType::var(const String &)
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

xdrproc_t
BaseType::xdr_coder()
{
    return _xdr_coder;
}

// Use these mfuncs to access the xdrin/out pointers.

XDR *
BaseType::xdrin() const
{
    return _xdrin;
}

XDR *
BaseType::xdrout() const
{
    return _xdrout;
}

// send a printed representation of the variable's declaration to cout. If
// print_semi is true, append a semicolon and newline.

void 
BaseType::print_decl(ostream &os, String space, bool print_semi, 
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
BaseType::check_semantics(bool)
{
    bool sem = (_type != dods_null_c && (const char *)_name);

    if (!sem) 
	cerr << "Every variable must have both a name and a type" << endl;

    return sem;
}

// Flush the output buffer.
// Returns: false if an error was detected by fflush(), true otherwise.

bool
BaseType::expunge()
{
    return fflush(_out) == 0;
}

// Member functions for the relational operators used in evaluating a
// relational clause in a constraint expression. Each class that wants these
// to do something interesting must supply their own versions. These print en
// error message and return False.

bool 
BaseType::ops(BaseType &, int)
{
    cerr << "Unimplemented operator" << endl;

    return false;
}

