
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Sequence.cc,v $
// Revision 1.31  1996/09/16 18:09:16  jimg
// Fixed var(const String name) so that it would correctly descend names of the
// form <base>.<name> where <name> may itself contain `dots'.
//
// Revision 1.30  1996/08/26 21:12:59  jimg
// Changes for version 2.07
//
// Revision 1.29  1996/08/13 18:36:12  jimg
// Added void casts to values computed in print_val() for loops.
//
// Revision 1.28  1996/06/04 21:33:37  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.27  1996/05/31 23:29:58  jimg
// Updated copyright notice.
//
// Revision 1.26  1996/05/29 22:08:46  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.25  1996/05/22 18:05:15  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.24  1996/05/16 22:44:52  jimg
// Dan's changes for 2.0.
//
// Revision 1.23  1996/05/14 15:38:35  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.22  1996/04/05 00:21:38  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.21  1996/04/04 18:10:46  jimg
// Merged changes from version 1.1.1.
// Fixed a bug in serialize() which caused the sequence serialization to end
// when the current CE first evaluated to false
//
// Revision 1.20  1996/03/05 17:44:21  jimg
// Added ce_eval to serailize member function.
//
// Revision 1.19  1996/02/02 00:31:12  jimg
// Merge changes for DODS-1.1.0 into DODS-2.x
//
// Revision 1.18  1995/12/09  01:06:54  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.17  1995/12/06  21:56:29  jimg
// Added `constrained' flag to print_decl.
// Removed third parameter of read.
// Modified print_decl() to print only those parts of a dataset that are
// selected when `constrained' is true.
//
// Revision 1.16  1995/10/23  23:21:01  jimg
// Added _send_p and _read_p fields (and their accessors) along with the
// virtual mfuncs set_send_p() and set_read_p().
//
// Revision 1.15  1995/08/26  00:31:43  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.14  1995/08/23  00:11:06  jimg
// Changed old, deprecated member functions to new ones.
// Switched from String representation of type to enum.
//
// Revision 1.13.2.1  1995/09/14 20:58:15  jimg
// Moved some loop index variables out of the loop statement.
//
// Revision 1.13  1995/07/09  21:29:03  jimg
// Added copyright notice.
//
// Revision 1.12  1995/05/10  15:34:03  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.11  1995/05/10  13:45:27  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.10  1995/03/16  17:29:11  jimg
// Added include config_dap.h to top of include list.
// Added TRACE_NEW switched dbnew includes.
// Fixed bug in read_val() where **val was passed incorrectly to
// subordinate read_val() calls.
//
// Revision 1.9  1995/03/04  14:34:49  jimg
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
// Revision 1.8  1995/02/10  02:23:02  jimg
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
// Revision 1.7  1995/01/19  20:05:26  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.6  1995/01/11  15:54:53  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.5  1994/12/15  21:21:13  dan
// Modified Sequence class to directly inherit from class BaseType
// Modified constructors to reflect new inheritance.
//
// Revision 1.4  1994/12/08  15:55:58  dan
// Added data transmission member functions serialize() and deserialize()
// Modified size() member function to return cumulative size of all members.
// Fixed the duplicate() member function to correctly access the data members
// of the sequence being duplicated.
//
// Revision 1.3  1994/10/17  23:34:49  jimg
// Added code to print_decl so that variable declarations are pretty
// printed.
// Added private mfunc duplicate().
// Added ptr_duplicate().
// Added Copy ctor, dtor and operator=.
//
// Revision 1.2  1994/09/23  14:48:31  jimg
// Fixed some errors in comments.

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"

#include <assert.h>

#include "debug.h"
#include "Sequence.h"
#include "DDS.h"
#include "util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void
Sequence::_duplicate(const Sequence &s)
{
    BaseType::_duplicate(s);

    Sequence &cs = (Sequence &)s; // cast away const
    
    for (Pix p = cs.first_var(); p; cs.next_var(p))
	add_var(cs.var(p)->ptr_duplicate());
}

Sequence::Sequence(const String &n) : BaseType(n, dods_sequence_c) 
{
}

Sequence::Sequence(const Sequence &rhs)
{
    _duplicate(rhs);
}

Sequence::~Sequence()
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	delete _vars(p);
}

const Sequence &
Sequence::operator=(const Sequence &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

void
Sequence::set_send_p(bool state)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->set_send_p(state);

    BaseType::set_send_p(state);
}

void
Sequence::set_read_p(bool state)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->set_read_p(state);

    BaseType::set_read_p(state);
}

// NB: Part p defaults to nil for this class

void 
Sequence::add_var(BaseType *bt, Part)
{
    _vars.append(bt);
}

BaseType *
Sequence::var(const String &name)
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
	for (Pix p = _vars.first(); p; _vars.next(p))
	    if (_vars(p)->name() == name)
		return _vars(p);
    }

    return 0;
}

Pix
Sequence::first_var()
{
    return _vars.first();
}

void
Sequence::next_var(Pix &p)
{
    if (!_vars.empty() && p)
	_vars.next(p);
}

BaseType *
Sequence::var(Pix p)
{
    if (!_vars.empty() && p)
	return _vars(p);
    else 
	return NULL;
}

unsigned int
Sequence::width()
{
    unsigned int sz = 0;

    for( Pix p = first_var(); p; next_var(p))
	sz += var(p)->width();

    return sz;
}

// This version returns zero. Each API-specific subclass should define a more
// reasonable version. jhrg 5/24/96

unsigned int
Sequence::length()
{
    return 0;
}

void
Sequence::set_level(int lvl)
{
    _level = lvl;
}

unsigned int
Sequence::read_level()
{
    return _level;
}

// Note that in Sequence's serialize() mfunc I assume that it is best to read
// the entire instance of the sequence in at once. However, each
// specialization of Sequence::read() will determine exactly what that means.
// 
// jhrg 4/12/96

bool
Sequence::serialize(const String &dataset, DDS &dds, XDR *sink,
		    bool ce_eval = true)
{
    bool status = true;
    int error = 1;

    while (status) {

	// Check to see if the variable needs to be read. Only read when at
	// the `top level' of a Sequence (i.e. only issue a read at the
	// outermost Sequence when dealing with nested Sequences).
	if (!read_p()) {
	    if (read_level() == 0) {
		if (!read(dataset,error)) {
		    if (error != -1) return false;
		    else return true; // EOF condition
		}
	    }
	    else return true;	
	}

	// if we are supposed to eval the selection, then do so. If it's
	// false, then goto the next record in the sequence (don't return as
	// with the other serialize mfuncs).
	if (ce_eval && !dds.eval_selection(dataset)) {
	    set_read_p(false);	// so that the next instance will be read
	    continue;
	}

	for (Pix p = first_var(); p; next_var(p))
	    if (var(p)->send_p() 
		&& !(status = var(p)->serialize(dataset, dds, sink, false))) 
		break;
	
	set_read_p(false);
    }

    return status;
}

bool
Sequence::deserialize(XDR *source, bool reuse = false)
{
    bool stat;

    for (Pix p = first_var(); p; next_var(p)) {
	stat = var(p)->deserialize(source, reuse);
	if (!stat) 
	    return false;
    }

    return stat;
}

unsigned int
Sequence::val2buf(void *, bool)
{
    return sizeof(Sequence);
}

unsigned int
Sequence::buf2val(void **)
{
    return sizeof(Sequence);
}

void
Sequence::print_decl(ostream &os, String space, bool print_semi,
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

void 
Sequence::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";
    for (Pix p = _vars.first(); p; _vars.next(p), (void)(p && os << ", "))
	_vars(p)->print_val(os, "", false);

    os << " }";

    if (print_decl_p)
	os << ";";
}

// print_all_vals is from Todd Karakasian. 
// We need to integrate this into print_val somehow, maybe by adding an XDR *
// to Sequence? This can wait since print_val is mostly used for debugging...

void
Sequence::print_all_vals(ostream& os, XDR *src, String space = "",
			 bool print_decl_p = true)
{
    if (print_decl_p) {
	print_decl(os);
	os << " = ";
    }
    os << "{ ";
    print_val(os, space, false);
    while (deserialize(src)) {
	os << ", ";
	print_val(os, space, false);
    }
    if (print_decl_p)
        os << "};";
}

bool
Sequence::check_semantics(bool all)
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
