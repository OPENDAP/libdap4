
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Structure.cc,v $
// Revision 1.36  1998/11/10 00:58:49  jimg
// Fixed up memory leaks in the calls to unique_names().
//
// Revision 1.35  1998/09/17 17:08:52  jimg
// Changes for the new variable lookup scheme. Fields of ctor types no longer
// need to be fully qualified. my.thing.f1 can now be named `f1' in a CE. Note
// that if there are two `f1's in a dataset, the first will be silently used;
// There's no warning about the situation. The new code in the var member
// function passes a stack of BaseType pointers so that the projection
// information (send_p field) can be set properly.
// Changed the implementation of print_all_vals to use type() instead of
// type_name().
// Added leaf_match and exact_match.
//
// Revision 1.34  1998/08/06 16:21:25  jimg
// Fixed the misuse of the read(...) member function. See Grid.c (from jeh).
//
// Revision 1.33  1998/04/03 17:43:32  jimg
// Patch from Jake Hamby. Added print_all_vals member function. Fixed print_val
// so that structures with sequences work properly.
//
// Revision 1.32  1998/03/17 17:50:37  jimg
// Added an implementation of element_count().
//
// Revision 1.31  1997/09/22 22:45:14  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.30  1997/03/08 19:02:08  jimg
// Changed default param to check_semantics() from  to String()
// and removed the default from the argument list in the mfunc definition
//
// Revision 1.29  1997/02/28 01:27:59  jimg
// Changed check_semantics() so that it now returns error messages in a String
// object (passed by reference).
//
// Revision 1.28  1997/02/10 02:32:43  jimg
// Added assert statements for pointers
//
// Revision 1.27  1996/09/16 18:09:49  jimg
// Fixed var(const String name) so that it would correctly descend names of the
// form <base>.<name> where <name> may itself contain `dots'.
//
// Revision 1.26  1996/08/13 18:37:49  jimg
// Added void casts to values computed in print_val() for loops.
//
// Revision 1.25  1996/06/04 21:33:45  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.24  1996/05/31 23:30:05  jimg
// Updated copyright notice.
//
// Revision 1.23  1996/05/16 22:49:52  jimg
// Dan's changes for version 2.0. Added a parameter to read that returns
// an error code so that EOF can be distinguished from an actual error when
// reading sequences. This *may* be replaced by an error member function
// in the future.
//
// Revision 1.22  1996/05/14 15:38:38  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
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
	assert(cs._vars(p));
	_vars.append(cs._vars(p)->ptr_duplicate());
    }
}

Structure::Structure(const String &n) : BaseType(n, dods_structure_c)
{
}

Structure::Structure(const Structure &rhs)
{
    _duplicate(rhs);
}

Structure::~Structure()
{
    for (Pix p = _vars.first(); p; _vars.next(p)) {
	assert(_vars(p));
	delete _vars(p);
    }
}

const Structure &
Structure::operator=(const Structure &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

int
Structure::element_count(bool leaves)
{
    if (!leaves)
	return _vars.length();
    else {
	int i = 0;
	for (Pix p = first_var(); p; next_var(p))
	    i += var(p)->element_count(leaves);
	return i;
    }
}

void
Structure::set_send_p(bool state)
{
    for (Pix p = _vars.first(); p; _vars.next(p)) {
	assert(_vars(p));
	_vars(p)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Structure::set_read_p(bool state)
{
    for (Pix p = _vars.first(); p; _vars.next(p)) {
	assert(_vars(p));
	_vars(p)->set_read_p(state);
    }

    BaseType::set_read_p(state);
}

// NB: Part p defaults to nil for this class

void 
Structure::add_var(BaseType *bt, Part)
{
    assert(bt);

    _vars.append(bt);
}

unsigned int
Structure::width()
{
    unsigned int sz = 0;

    for( Pix p = first_var(); p; next_var(p))
	sz += var(p)->width();

    return sz;
}

// Returns: false if an error was detected, true otherwise. 
// NB: this means that serialize() returns true when the CE evaluates to
// false. This bug might be fixed using exceptions.

bool
Structure::serialize(const String &dataset, DDS &dds, XDR *sink, 
		     bool ce_eval = true)
{
    bool status = true;
    int error = 0;

    if (!read_p()) {
	read(dataset, error);
	if (error)
	    return false;
    }

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    for (Pix p = first_var(); p; next_var(p)) 
	if (var(p)->send_p() 
	    && !(status = var(p)->serialize(dataset, dds, sink, false))) 
	    break;

    return status;
}

bool
Structure::deserialize(XDR *source, DDS *dds, bool reuse = false)
{
    bool status = true;

    for (Pix p = first_var(); p; next_var(p)) {
        BaseType *v = var(p);
	// Because sequences have multiple rows, bail out and let the caller
	// deserialize as they read the data.
	if (v->type() == dods_sequence_c)
	  break;
	status = v->deserialize(source, dds, reuse);
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
Structure::var(const String &name, btp_stack &s)
{
    for (Pix p = _vars.first(); p; _vars.next(p)) {
	assert(_vars(p));
	
	if (_vars(p)->name() == name) {
	    s.push((BaseType *)this);
	    return _vars(p);
	}

        if (_vars(p)->is_constructor_type()) {
	    BaseType *btp = _vars(p)->var(name, s);
	    if (btp) {
		s.push((BaseType *)this);
		return btp;
	    }
	}
    }

    return 0;
}

BaseType *
Structure::var(const String &name, bool exact)
{
    if (exact)
	return exact_match(name);
    else
	return leaf_match(name);
}

BaseType *
Structure::leaf_match(const String &name)
{
    for (Pix p = _vars.first(); p; _vars.next(p)) {
	assert(_vars(p));
	
	if (_vars(p)->name() == name)
	    return _vars(p);
        if (_vars(p)->is_constructor_type()) {
	    BaseType *btp = _vars(p)->var(name, false);
	    if (btp)
		return btp;
	}
    }

    return 0;
}

BaseType *
Structure::exact_match(const String &name)
{
    if (name.contains(".")) {
	String n = (String)name; // cast away const
	String aggregate = n.before(".");
	String field = n.from(".");
	field = field.after(".");

	BaseType *agg_ptr = var(aggregate);
	if (agg_ptr)
	    return agg_ptr->var(field);	// recurse
	else
	    return 0;		// qualified names must be *fully* qualified
    }
    else {
	for (Pix p = _vars.first(); p; _vars.next(p)) {
	    assert(_vars(p));
	    if (_vars(p)->name() == name)
		return _vars(p);
	}
    }

    return 0;
}

Pix
Structure::first_var()
{
    assert(_vars.first());
    return _vars.first();
}

void
Structure::next_var(Pix &p)
{
    if (!_vars.empty() && p) {
	_vars.next(p);
    }
}

BaseType *
Structure::var(Pix p)
{
    if (!_vars.empty() && p) {
	assert(_vars(p));
	return _vars(p);
    }
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
    for (Pix p = _vars.first(); p; _vars.next(p)) {
	assert(_vars(p));
	_vars(p)->print_decl(os, space + "    ", true, constraint_info,
			     constrained);
    }
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
    for (Pix p = _vars.first(); p; _vars.next(p), (void)(p && os << ", ")) {
	assert(_vars(p));
	_vars(p)->print_val(os, "", false);
    }

    os << " }";

    if (print_decl_p)
	os << ";" << endl;
}

// Print the values of the contained variables.
//
// Potential bug: This works only for structures that have sequences at their
// top level. Will it work when sequences are more deeply embedded?

void
Structure::print_all_vals(ostream &os, XDR *src, DDS *dds, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";
    bool sequence_found = false;
    for (Pix p = first_var(); p; next_var(p), (void)(p && os << ", ")) {
	assert(var(p));
	switch (var(p)->type()) {
	  case dods_sequence_c:
	    (dynamic_cast<Sequence*>(var(p)))->print_all_vals(os, src, dds, 
							      "", false);
	    sequence_found = true;
	    break;
	  
	  case dods_structure_c:
	    (dynamic_cast<Structure*>(var(p)))->print_all_vals(os, src, dds, 
							       "", false);
	    break;
	  
	  default:
	    // If a sequence was found, we still need to deserialize()
	    // remaining vars.
	    if(sequence_found)
		var(p)->deserialize(src, dds);
	    var(p)->print_val(os, "", false);
	    break;
	}
    }

    os << " }";

    if (print_decl_p)
	os << ";" << endl;
}

bool
Structure::check_semantics(String &msg, bool all = false)
{
    if (!BaseType::check_semantics(msg))
	return false;
    
    bool status = true;

    char *n = new char[name().length()+1];
    strcpy(n , (const char *)name());
    char *tn = new char[type_name().length()+1];
    strcpy(tn, (const char *)type_name());

    if (!unique_names(_vars, n, tn, msg)) {
	status = false;
	goto exit;
    }

    if (all) 
	for (Pix p = _vars.first(); p; _vars.next(p)) {
	    assert(_vars(p));
	    if (!_vars(p)->check_semantics(msg, true)) {
		status = false;
		goto exit;
	    }
	}

 exit:
    delete[] n;
    delete[] tn;
    return status;
}

