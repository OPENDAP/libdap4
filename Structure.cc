
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

// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Structure.cc,v $
// Revision 1.21  1996/04/05 00:21:40  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.20  1996/03/05 17:36:12  jimg
// Added ce_eval to serailize member function.
// Added debugging information to _duplicate member function.
//
// Revision 1.19  1996/02/02 00:31:13  jimg
// Merge changes for DODS-1.1.0 into DODS-2.x
//
// Revision 1.18  1995/12/09  01:07:00  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.17  1995/12/06  21:56:32  jimg
// Added `constrained' flag to print_decl.
// Removed third parameter of read.
// Modified print_decl() to print only those parts of a dataset that are
// selected when `constrained' is true.
//
// Revision 1.16  1995/10/23  23:21:04  jimg
// Added _send_p and _read_p fields (and their accessors) along with the
// virtual mfuncs set_send_p() and set_read_p().
//
// Revision 1.15  1995/08/26  00:31:49  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.14  1995/08/23  00:11:08  jimg
// Changed old, deprecated member functions to new ones.
// Switched from String representation of type to enum.
//
// Revision 1.13.2.1  1995/09/14 20:58:13  jimg
// Moved some loop index variables out of the loop statement.
//
// Revision 1.13  1995/07/09  21:29:06  jimg
// Added copyright notice.
//
// Revision 1.12  1995/05/10  15:34:06  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.11  1995/05/10  13:45:31  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.10  1995/03/16  17:29:12  jimg
// Added include config_dap.h to top of include list.
// Added TRACE_NEW switched dbnew includes.
// Fixed bug in read_val() where **val was passed incorrectly to
// subordinate read_val() calls.
//
// Revision 1.9  1995/03/04  14:34:51  jimg
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
// Revision 1.8  1995/02/10  02:22:59  jimg
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
// Revision 1.7  1995/01/19  20:05:24  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.6  1995/01/11  15:54:49  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.5  1994/12/16  15:16:39  dan
// Modified Structure class removing inheritance from class CtorType
// and directly inheriting from class BaseType to alloc calling
// BaseType's constructor directly.
//
// Revision 1.4  1994/11/22  14:06:10  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.3  1994/10/17  23:34:47  jimg
// Added code to print_decl so that variable declarations are pretty
// printed.
// Added private mfunc duplicate().
// Added ptr_duplicate().
// Added Copy ctor, dtor and operator=.
//
// Revision 1.2  1994/09/23  14:45:26  jimg
// Added mfunc check_semantics().
// Added sanity checking on the variable list (is it empty?).
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "Structure.h"
#include "DDS.h"
#include "util.h"
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void
Structure::_duplicate(const Structure &s)
{
    BaseType::_duplicate(s);

    Structure &cs = (Structure &)s; // cast away const

    DBG(cerr << "Copying strucutre: " << name() << endl);

    for (Pix p = cs._vars.first(); p; cs._vars.next(p)) {
	DBG(cerr << "Copying field: " << cs.name() << endl);
	_vars.append(cs._vars(p)->ptr_duplicate());
    }
}

Structure::Structure(const String &n) : BaseType(n, d_structure_t)
{
}

Structure::Structure(const Structure &rhs)
{
    _duplicate(rhs);
}

Structure::~Structure()
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	delete _vars(p);
}

const Structure &
Structure::operator=(const Structure &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

void
Structure::set_send_p(bool state)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->set_send_p(state);

    BaseType::set_send_p(state);
}

void
Structure::set_read_p(bool state)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->set_read_p(state);

    BaseType::set_read_p(state);
}

// NB: Part p defaults to nil for this class

void 
Structure::add_var(BaseType *bt, Part)
{
    _vars.append(bt);
}

unsigned int
Structure::width()
{
    return sizeof(Structure);
}

// Returns: false if an error was detected, true otherwise. 
// NB: this means that serialize() returns true when the CE evaluates to
// false. This bug might be fixed using exceptions.

bool
Structure::serialize(const String &dataset, DDS &dds, bool ce_eval, bool flush)
{
    bool status = true;

    if (!read_p() && !read(dataset))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    for (Pix p = first_var(); p; next_var(p)) 
	if (var(p)->send_p() 
	    && !(status = var(p)->serialize(dataset, dds, false, false))) 
	    break;

    // flush the stream *even* if status is false, but preserve the value of
    // status if it's false.
    if (flush)
	status = status && expunge();

    return status;
}

bool
Structure::deserialize(bool reuse)
{
    bool status = true;

    for (Pix p = first_var(); p; next_var(p)) {
	status = var(p)->deserialize(reuse);
	if (!status) 
	  break;
    }

    return status;
}

// This mfunc assumes that val contains values for all the elements of the
// strucuture in the order those elements are declared.

unsigned int
Structure::val2buf(void *, bool)
{
    return sizeof(Structure);
}

unsigned int
Structure::buf2val(void **)
{
    return sizeof(Structure);
}

BaseType *
Structure::var(const String &name)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	if (_vars(p)->name() == name)
	    return _vars(p);

    return 0;
}

Pix
Structure::first_var()
{
    return _vars.first();
}

void
Structure::next_var(Pix &p)
{
    if (!_vars.empty() && p)
	_vars.next(p);
}

BaseType *
Structure::var(Pix p)
{
    if (!_vars.empty() && p)
	return _vars(p);
    else 
      return NULL;
}

void
Structure::print_decl(ostream &os, String space, bool print_semi,
		      bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    os << space << type_name() << " {" << endl;
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->print_decl(os, space + "    ", true, constraint_info,
			     constrained);
    os << space << "} " << name();

    if (constraint_info) {
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	os << ";" << endl;
}

// print the values of the contained variables

void 
Structure::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";
    for (Pix p = _vars.first(); p; _vars.next(p), p && os << ", ")
	_vars(p)->print_val(os, "", false);

    os << " }";

    if (print_decl_p)
	os << ";" << endl;
}

bool
Structure::check_semantics(bool all)
{
    if (!BaseType::check_semantics())
	return false;

    if (!unique(_vars, (const char *)name(), (const char *)type_name()))
	return false;

    if (all) 
	for (Pix p = _vars.first(); p; _vars.next(p))
	    if (!_vars(p)->check_semantics(true))
		return false;

    return true;
}

