
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

// Implementation for BaseType.
//
// jhrg 9/6/94

// $Log: BaseType.cc,v $
// Revision 1.15  1995/07/09 21:28:52  jimg
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
    _xdr_coder = bt._xdr_coder;	// just copy this function pointer
}

// friend functions

// Delete the current XDR * assigned to _xdrin, free the strage, create a
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

BaseType::BaseType(const String &n, const String &t, xdrproc_t xdr)
    : _name(n), _type(t), _xdr_coder(xdr)
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

// deprecated

String 
BaseType::get_var_name() const
{ 
    return _name; 
}

void 
BaseType::set_var_name(const String &n)
{ 
    _name = n; 
}

String
BaseType::get_var_type() const
{
    return _type;
}

void
BaseType::set_var_type(const String &t)
{
    _type = t;
}

// new names (changed to fit with the new naming scheme)

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

String
BaseType::type() const
{
    return _type;
}

void
BaseType::set_type(const String &t)
{
    _type = t;
}

// Return a pointer to the contained variable in a ctor class. For BaseType
// this always prints an error message. It is defined here so that the ctor
// descendents of BaseType can access it when they are stored in a BaseType
// pointer.

BaseType *
BaseType::var(const String &name)
{
    cerr << "var() should only be called for contructor types" << endl;
}

// See comment for var().

void
BaseType::add_var(BaseType *v, Part p)
{
    cerr << "add_var() should only be called for constructor types" << endl;
}

// Using this mfunc, objects that contain a (BaseType *) can get the xdr
// function used to serialize the object.

xdrproc_t
BaseType::xdr_coder()
{
    return _xdr_coder;
}

// send a printed representation of the variable's declaration to cout. If
// print_semi is true, append a semicolon and newline.

void 
BaseType::print_decl(ostream &os, String space, bool print_semi)
{
    os << space << _type << " " << _name;
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
BaseType::check_semantics(bool all)
{
    bool sem = ((const char *)_type && (const char *)_name);

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
