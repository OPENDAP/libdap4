
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

// Implementation for the class Structure
//
// jhrg 9/14/94


#include "config.h"

#include <stdlib.h>

#include "Structure.h"
#include "util.h"
#include "debug.h"
#include "InternalErr.h"
#include "escaping.h"
#if 0
#include "BTIterAdapter.h"
#endif


using std::cerr;
using std::endl;

// Jose Garcia 1/26/2000
// Note: all asserts of nature
// for (Pix p = _vars.first(); p; _vars.next(p)) {
// 	assert(_vars(p));
// had been commented out, later when we get sure
// we do not need then we can remove them all.

void
Structure::_duplicate(const Structure &s)
{
    Structure &cs = const_cast<Structure &>(s);

    DBG(cerr << "Copying strucutre: " << name() << endl);

    for (Vars_iter i = cs._vars.begin(); i != cs._vars.end(); i++)
    {
	DBG(cerr << "Copying field: " << cs.name() << endl);
	// Jose Garcia
	// I think this assert here is part of a debugging 
	// process since it is going along with a DBG call
	// I leave it here since it can be remove by defining NDEBUG.
	// assert(*i);
	BaseType *btp = (*i)->ptr_duplicate();
	btp->set_parent(this);
	_vars.push_back(btp);
    }
}

/** The Structure constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications. 
      
    @param n A string containing the name of the variable to be
    created. 
*/
Structure::Structure(const string &n) :Constructor(n, dods_structure_c)
{
}

/** The Structure copy constructor. */
Structure::Structure(const Structure &rhs) :Constructor(rhs)
{
    _duplicate(rhs);
}

Structure::~Structure()
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	BaseType *btp = *i ;
	delete btp ;  btp = 0;
    }
}

BaseType *
Structure::ptr_duplicate()
{
    return new Structure(*this);
}

Structure &
Structure::operator=(const Structure &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Constructor &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

int
Structure::element_count(bool leaves)
{
    if (!leaves)
	return _vars.size();
    else {
	int i = 0;
	for (Vars_iter j = _vars.begin(); j != _vars.end(); j++)
	{
	    j += (*j)->element_count(leaves);
	}
	return i;
    }
}

bool 
Structure::is_linear()
{
    bool linear = true;
    for (Vars_iter i = _vars.begin(); linear && i != _vars.end(); i++)
    {
	if ((*i)->type() == dods_structure_c)
	    linear = linear && dynamic_cast<Structure*>((*i))->is_linear();
	else 
	    linear = linear && (*i)->is_simple_type();
    }

    return linear;
}

void
Structure::set_send_p(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	//assert(*i);
	(*i)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Structure::set_read_p(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	//assert(*i);
	(*i)->set_read_p(state);
    }

    BaseType::set_read_p(state);
}

/** Set the \e in_selection property for this variable and all of its
    children. 
    
    @brief Set the \e in_selection property.
    @param state Set the property value to \e state. */
void
Structure::set_in_selection(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
	(*i)->set_in_selection(state);
    }

    BaseType::set_in_selection(state);
}

/** @brief Traverse Structure, set Sequence leaf nodes. */
void
Structure::set_leaf_sequence(int level)
{
    for (Vars_iter i = var_begin(); i != var_end(); i++) {
        if ((*i)->type() == dods_sequence_c)
            dynamic_cast<Sequence&>(**i).set_leaf_sequence(++level);
        else if ((*i)->type() == dods_structure_c)
            dynamic_cast<Structure&>(**i).set_leaf_sequence(level);
    }
}

// NB: Part defaults to nil for this class

/** Adds an element to a Structure. 

    @param bt A pointer to the DODS type variable to add to this Structure.
    @param part defaults to nil */
void 
Structure::add_var(BaseType *bt, Part)
{
    // Jose Garcia
    // Passing and invalid pointer to an object is a developer's error.
    if (!bt)
	throw InternalErr(__FILE__, __LINE__, 
			  "The BaseType parameter cannot be null.");

    // Jose Garcia
    // Now we add a copy of bt so the external user is able to destroy bt as
    // he/she whishes. The policy is: "If it is allocated outside, it is
    // deallocated outside, if it is allocated inside, it is deallocated
    // inside"
    BaseType *btp = bt->ptr_duplicate();
    btp->set_parent(this);
    _vars.push_back(btp);
}

unsigned int
Structure::width()
{
    unsigned int sz = 0;

    for( Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	sz += (*i)->width();
    }

    return sz;
}

// Returns: false if an error was detected, true otherwise. 
// NB: this means that serialize() returns true when the CE evaluates to
// false. This bug might be fixed using exceptions.

bool
Structure::serialize(const string &dataset, DDS &dds, XDR *sink, 
		     bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
	read(dataset);		// read() throws Error and InternalErr

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    dds.timeout_off();

    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
	if ((*i)->send_p()) {
	    (*i)->serialize(dataset, dds, sink, false);
	}
    }

    return true;
}

bool
Structure::deserialize(XDR *source, DDS *dds, bool reuse)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	(*i)->deserialize(source, dds, reuse);
    }

    return false;
}

/**  This function assumes that val contains values for all the elements of the
     structure in the order those elements are declared.
     @return Returns the size of the structure. */
unsigned int
Structure::val2buf(void *, bool)
{
    return sizeof(Structure);
}

/** Returns the size of the structure. */
unsigned int
Structure::buf2val(void **)
{
    return sizeof(Structure);
}

// If EXACT is true, then use breadth-first search and assume that NAME is
// the path to a variable where a dot (.) separates the ctor variable(s) from
// the variable to be found. If S is not null, push the path to NAME on the
// statck.
BaseType *
Structure::var(const string &n, bool exact, btp_stack *s)
{
    string name = www2id(n);

    if (exact)
	return exact_match(name, s);
    else
	return leaf_match(name, s);
}

// Get rid of this method ASAP.
// A depth-first search for leaf nodes matching NAME.
BaseType *
Structure::var(const string &n, btp_stack &s)
{
    string name = www2id(n);

    BaseType *btp = exact_match(name, &s);
    if (btp)
	return btp;

    return leaf_match(name, &s);
}

// If S is not null, push the path of the depth-first search for a
// leaf-node called NAME onto S.
BaseType *
Structure::leaf_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	if ((*i)->name() == name) {
	    if (s) {
		DBG(cerr << "Pushing " << this->name() << endl);
		s->push(static_cast<BaseType *>(this));
	    }
	    return *i;
	}
        if ((*i)->is_constructor_type()) {
	    BaseType *btp = (*i)->var(name, false, s);
	    if (btp) {
		if (s) {
		    DBG(cerr << "Pushing " << this->name() << endl);
		    s->push(static_cast<BaseType *>(this));
		}
		return btp;
	    }
	}
    }

    return 0;
}

/** Breadth-first search for NAME. If NAME contains one or more dots (.) */
BaseType *
Structure::exact_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	DBG(cerr << "Looking at " << (*i)->name() << " in: " << *i
	    << endl);
	if ((*i)->name() == name) {
	    DBG(cerr << "Found " << (*i)->name() << " in: " 
		<< *i << endl);
	    if (s) {
		DBG(cerr << "Pushing " << this->name() << endl);
		s->push(static_cast<BaseType *>(this));
	    }
	    return *i;
	}
    }

    string::size_type dot_pos = name.find("."); // zero-based index of `.'
    if (dot_pos != string::npos) {
	string aggregate = name.substr(0, dot_pos);
	string field = name.substr(dot_pos + 1);

	BaseType *agg_ptr = var(aggregate);
	if (agg_ptr) {
	    DBG(cerr << "Descending into " << agg_ptr->name() << endl);
	    if (s) {
		DBG(cerr << "Pushing " << this->name() << endl);
		s->push(static_cast<BaseType *>(this));
	    }
	    return agg_ptr->var(field, true, s); // recurse
	}
	else
	    return 0;		// qualified names must be *fully* qualified
    }

    return 0;
}

#if 0
/** Returns the pseudo-index (Pix) of the first structure element. 
    @deprecated All methods which use or return Pix objects are deprecated.
    @see var_begin() */
Pix
Structure::first_var()
{
    if (_vars.empty())
	return 0;

    BTIterAdapter *i = new BTIterAdapter( _vars ) ;
    i->first() ;
    return i ;
}
#endif

#if 0
Structure::Vars_iter
Structure::var_begin()
{
    return _vars.begin() ;
}

Structure::Vars_iter
Structure::var_end()
{
    return _vars.end() ;
}

/** Return the iterator for the \e ith variable.
    @param i the index
    @return The corresponding  Vars_iter */
Structure::Vars_iter
Structure::get_vars_iter(int i)
{
    return _vars.begin() + i;
}
#endif

#if 0
/** Increments the input index to point to the next element in the
    structure.
    @deprecated All methods which use or return Pix objects are deprecated.
    @see var_begin() */
void
Structure::next_var(Pix p)
{
    p.next();
}

/** Returns a pointer to the <i>p</i>th element.
    @deprecated use iterator operator * with the iterator returned from
    var_begin() to reference the element
    @see var_begin() */
BaseType *
Structure::var(Pix p)
{
    BTIterAdapter *i = (BTIterAdapter *)p.getIterator() ;
    if( i ) {
	return i->entry() ;
    }
    return 0 ;
}
#endif

#if 0
void
Structure::print_decl(ostream &os, string space, bool print_semi,
		      bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    os << space << type_name() << " {" << endl;
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	(*i)->print_decl(os, space + "    ", true,
			 constraint_info, constrained);
    }
    os << space << "} " << id2www(name());

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
Structure::print_decl(FILE *out, string space, bool print_semi,
		      bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    fprintf( out, "%s%s {\n", space.c_str(), type_name().c_str() ) ;
    for (Vars_citer i = _vars.begin(); i != _vars.end(); i++)
    {
	(*i)->print_decl(out, space + "    ", true,
			 constraint_info, constrained);
    }
    fprintf( out, "%s} %s", space.c_str(), id2www( name() ).c_str() ) ;

    if (constraint_info) {
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	fprintf( out, ";\n" ) ;
}
#endif

// print the values of the contained variables
#if 0
void 
Structure::print_val(ostream &os, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";
    for (Vars_iter i = _vars.begin(); i != _vars.end();
	    i++, (void)(i != _vars.end() && os << ", "))
    {
	(*i)->print_val(os, "", false);
    }

    os << " }";

    if (print_decl_p)
	os << ";" << endl;
}
#endif
void 
Structure::print_val(FILE *out, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(out, space, false);
	fprintf( out, " = " ) ;
    }

    fprintf( out, "{ " ) ;
    for (Vars_citer i = _vars.begin(); i != _vars.end();
	    i++, (void)(i != _vars.end() && fprintf( out, ", " ) ) )
    {
	(*i)->print_val(out, "", false);
    }

    fprintf( out, " }" ) ;

    if (print_decl_p)
	fprintf( out, ";\n" ) ;
}

#if 0
// Print the values of the contained variables.
//
// Potential bug: This works only for structures that have sequences at their
// top level. Will it work when sequences are more deeply embedded?

/** Prints the Structure and all elements of any Sequences contained
    within. 
    @see Sequence::print_all_vals
*/
void
Structure::print_all_vals(ostream &os, XDR *, DDS *, string space, bool print_decl_p)
{
    print_val(os, space, print_decl_p);

#if 0
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";

    bool sequence_found = false;
    for (Vars_iter i = _vars.begin();
	 i != _vars.end();
	 i++, (void)(i != _vars.end() && os << ", "))
    {
	switch ((*i)->type()) {
	  case dods_sequence_c:
	    (dynamic_cast<Sequence*>((*i)))->print_all_vals(os, src, dds, 
							      "", false);
	    sequence_found = true;
	    break;
	  
	  case dods_structure_c:
	    (dynamic_cast<Structure*>((*i)))->print_all_vals(os, src, dds, 
							       "", false);
	    break;
	  
	  default:
	    // If a sequence was found, we still need to deserialize()
	    // remaining vars.
	    if(sequence_found)
		(*i)->deserialize(src, dds);
	    (*i)->print_val(os, "", false);
	    break;
	}
    }

    os << " }";

    if (print_decl_p)
	os << ";" << endl;
#endif
}
#endif

// Print the values of the contained variables.
//
// Potential bug: This works only for structures that have sequences at their
// top level. Will it work when sequences are more deeply embedded? 
//
// No longer a problem as Sequences are now read in full. 05/16/03 jhrg

void
Structure::print_all_vals(FILE *out, XDR *, DDS *, string space, bool print_decl_p)
{
    print_val(out, space, print_decl_p);

#if 0
    if (print_decl_p) {
	print_decl(out, space, false);
	fprintf( out, " = " ) ;
    }

    fprintf( out, "{ " ) ;

    bool sequence_found = false;
    for (Vars_citer i = _vars.begin();
	 i != _vars.end();
	 i++, (void)(i != _vars.end() && fprintf( out, ", " ) ) )
    {
	switch ((*i)->type()) {
	  case dods_sequence_c:
	    (dynamic_cast<Sequence*>((*i)))->print_all_vals(out, src, dds, 
							      "", false);
	    sequence_found = true;
	    break;
	  
	  case dods_structure_c:
	    (dynamic_cast<Structure*>((*i)))->print_all_vals(out, src, dds, 
							       "", false);
	    break;
	  
	  default:
	    // If a sequence was found, we still need to deserialize()
	    // remaining vars.
	    if(sequence_found)
		(*i)->deserialize(src, dds);
	    (*i)->print_val(out, "", false);
	    break;
	}
    }

    fprintf( out, " }" ) ;

    if (print_decl_p)
	fprintf( out, ";\n" ) ;
#endif
}

bool
Structure::check_semantics(string &msg, bool all)
{
    if (!BaseType::check_semantics(msg))
	return false;
    
    bool status = true;

    if (!unique_names(_vars, name(), type_name(), msg))
	return false;

    if (all) 
    {
	for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
	{
	    //assert(*i);
	    if (!(*i)->check_semantics(msg, true)) {
		status = false;
		goto exit;
	    }
	}
    }

 exit:
    return status;
}

// $Log: Structure.cc,v $
// Revision 1.59  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.54.2.5  2005/01/19 17:16:32  jimg
// Added set_leaf_sequence() to Structure to mirror its use in DDS. This
// code will now correctly mark Sequences that are parts of Structures.
//
// Revision 1.58  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.54.2.4  2004/07/02 20:41:52  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.57  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.54.2.3  2004/02/11 22:26:46  jimg
// Changed all calls to delete so that whenever we use 'delete x' or
// 'delete[] x' the code also sets 'x' to null. This ensures that if a
// pointer is deleted more than once (e.g., when an exception is thrown,
// the method that throws may clean up and then the catching method may
// also clean up) the second, ..., call to delete gets a null pointer
// instead of one that points to already deleted memory.
//
// Revision 1.56  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.54.2.2  2003/09/06 22:56:26  jimg
// Added set_in_selection() method. Updated the documentation.
//
// Revision 1.54.2.1  2003/07/25 06:04:28  jimg
// Refactored the code so that DDS:send() is now incorporated into
// DODSFilter::send_data(). The old DDS::send() is still there but is
// depracated.
// Added 'smart timeouts' to all the variable classes. This means that
// the new server timeouts are active only for the data read and CE
// evaluation. This went inthe BaseType::serialize() methods because it
// needed to time both the read() calls and the dds::eval() calls.
//
// Revision 1.55  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.54  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.52.2.2  2003/04/15 01:17:12  jimg
// Added a method to get the iterator for a variable (or map) given its
// index. To get the iterator for the ith variable/map, call
// get_vars_iter(i).
//
// Revision 1.53  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.52.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.52  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.51  2003/01/15 19:24:39  pwest
// Removing IteratorAdapterT and replacing with non-templated versions.
//
// Revision 1.50  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.43.4.18  2002/12/31 16:43:20  rmorris
// Patches to handle some of the fancier template code under VC++ 6.0.
//
// Revision 1.43.4.17  2002/12/27 19:34:42  jimg
// Modified the var() methods so that www2id() is called before looking
// up identifier names. See bug 563.
//
// Revision 1.43.4.16  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.43.4.15  2002/12/01 14:37:52  rmorris
// Smalling changes for the win32 porting and maintenance work.
//
// Revision 1.43.4.14  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.43.4.13  2002/09/22 14:31:08  rmorris
// VC++ doesn't consider x in 'for(int x,...)' to be only for the block
// associated with the loop.  Multiple of these therefore case a error
// because VC++ sees multiple definitions.  Changed to use different vars names
// in each such block.
//
// Revision 1.43.4.12  2002/09/12 22:49:58  pwest
// Corrected signature changes made with Pix to IteratorAdapter changes. Rather
// than taking a reference to a Pix, taking a Pix value.
//
// Revision 1.43.4.11  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.43.4.10  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.49  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.48  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.43.4.9  2002/05/22 16:57:51  jimg
// I modified the `data type classes' so that they do not need to be
// subclassed for clients. It might be the case that, for a complex client,
// subclassing is still the best way to go, but you're not required to do
// it anymore.
//
// Revision 1.43.4.8  2002/03/01 21:03:08  jimg
// Significant changes to the var(...) methods. These now take a btp_stack
// pointer and are used by DDS::mark(...). The exact_match methods have also
// been updated so that leaf variables which contain dots in their names
// will be found. Note that constructor variables with dots in their names
// will break the lookup routines unless the ctor is the last field in the
// constraint expression. These changes were made to fix bug 330.
//
// Revision 1.47  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.43.4.7  2001/10/02 17:01:52  jimg
// Made the behavior of serialize and deserialize uniform. Both methods now
// use Error exceptions to signal problems with network I/O and InternalErr
// exceptions to signal other problems. The return codes, always true for
// serialize and always false for deserialize, are now meaningless. However,
// by always returning a code that means OK, old code should continue to work.
//
// Revision 1.46  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.43.4.6  2001/09/25 20:28:54  jimg
// Added is_linear().
//
// Revision 1.43.4.5  2001/09/07 00:38:35  jimg
// Sequence::deserialize(...) now reads all the sequence values at once.
// Its call semantics are the same as the other classes' versions. Values
// are stored in the Sequence object using a vector<BaseType *> for each
// row (those are themselves held in a vector). Three new accessor methods
// have been added to Sequence (row_value() and two versions of var_value()).
// BaseType::deserialize(...) now always returns true. This matches with the
// expectations of most client code (the seqeunce version returned false
// when it was done reading, but all the calls for sequences must be changed
// anyway). If an XDR error is found, deserialize throws InternalErr.
//
// Revision 1.45  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.43.4.4  2001/08/18 00:15:41  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.43.4.3  2001/07/28 01:10:42  jimg
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
// Revision 1.44  2001/06/15 23:49:02  jimg
// Merged with release-3-2-4.
//
// Revision 1.43.4.2  2001/06/07 20:42:02  jimg
// Removed an extraneous assert.
//
// Revision 1.43.4.1  2001/06/05 06:49:19  jimg
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
// Revision 1.43  2000/10/06 01:26:05  jimg
// Changed the way serialize() calls read(). The status from read() is
// returned by the Structure and Sequence serialize() methods; ignored by
// all others. Any exceptions thrown by read() are caught and discarded.
// serialize() returns false if read() throws an exception. This should
// be fixed once all the servers build using the new read() definition.
//
// Revision 1.42  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.41  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.40  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.39  2000/06/16 18:15:00  jimg
// Merged with 3.1.7
//
// Revision 1.37.6.2  2000/06/07 23:05:30  jimg
// The first_*() methods return 0 if there are no variables
//
// Revision 1.38  2000/01/27 06:29:58  jimg
// Resolved conflicts from merge with release-3-1-4
//
// Revision 1.37.6.1  2000/01/26 23:57:37  jimg
// Fixed the return type of string::find.
//
// Revision 1.37.14.2  2000/02/17 05:03:14  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.37.14.1  2000/01/28 22:14:06  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.37  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
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
// Revision 1.34.2.1  1999/02/02 21:57:01  jimg
// String to string version
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
// Failed to change `config.h' to `config.h' in these files.
//
// Revision 1.11  1995/05/10  13:45:31  jimg
// Changed the name of the configuration header file from `config.h' to
// `config.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.10  1995/03/16  17:29:12  jimg
// Added include config.h to top of include list.
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

