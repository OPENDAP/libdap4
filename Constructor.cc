
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

#ifdef _GNUG_
// #pragma implementation
#endif

#include "config_dap.h"

#include <string>
#include <algorithm>
#include <functional>

#include "Constructor.h"
#include "BTIterAdapter.h"

#include "debug.h"
#include "escaping.h"
#include "Error.h"
#include "InternalErr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using namespace std;

// Private member functions

void
Constructor::_duplicate(const Constructor &s)
{
}

// Public member functions

Constructor::Constructor(const string &n, const Type &t) 
    : BaseType(n, t)
{
}

Constructor::Constructor(const Constructor &rhs) : BaseType(rhs)
{
}

Constructor::~Constructor()
{
}

Constructor &
Constructor::operator=(const Constructor &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<BaseType &>(*this) = rhs; // run BaseType=

    _duplicate(rhs);

    return *this;
}

/** @name Pix interface; deprecated */
//@{
/** @brief Returns an index to the first variable in a Constructor instance.

    For a Structure, this returns the first variable, for a Sequence, it is
    the template of the variable in the first column.
*/
Pix
Constructor::first_var()
{
    if (_vars.empty())
	return 0;

    BTIterAdapter *i = new BTIterAdapter( _vars ) ;
    i->first() ;
    return i ;
}

/** @brief Increments the Constructor instance.  
    This returns a pointer to the
    next ``column'' in the Constructor, not the next row. */
void
Constructor::next_var(Pix p)
{
    p.next() ;
}

/** @brief Returns a pointer to a Constructor member.  
    This may be another Constructor. */
BaseType *
Constructor::var(Pix p)
{
    BTIterAdapter *i = static_cast<BTIterAdapter *>(p.getIterator());
    if( i ) {
	return i->entry() ;
    }
    return 0 ;
}
//@}

/** Returns an iterator referencing the first structure element. */
Constructor::Vars_iter
Constructor::var_begin()
{
    return _vars.begin() ;
}

/** Returns an iterator referencing the end of the list of structure
    elements. Does not reference the last structure element. */
Constructor::Vars_iter
Constructor::var_end()
{
    return _vars.end() ;
}

/** Return a reverse iterator that references the last element. */
Constructor::Vars_riter
Constructor::var_rbegin()
{
    return _vars.rbegin();
}

/** Return a reverse iterator that references a point 'before' the first
    element. */
Constructor::Vars_riter
Constructor::var_rend()
{
    return _vars.rend();
}

/** Return the iterator for the \i ith variable.
    @param i the index
    @return The corresponding  Vars_iter */
Constructor::Vars_iter
Constructor::get_vars_iter(int i)
{
    return _vars.begin() + i;
}

/** Return the BaseType pointer for the \e ith variable.
    @param i This index
    @return The corresponding BaseType*. */
BaseType *
Constructor::get_var_index(int i)
{
    return *(_vars.begin() + i);
}

void
Constructor::print_decl(ostream &os, string space, bool print_semi,
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

    if (constraint_info) {	// Used by test drivers only.
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	os << ";" << endl;
}

void
Constructor::print_decl(FILE *out, string space, bool print_semi,
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

    if (constraint_info) {	// Used by test drivers only.
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	fprintf( out, ";\n" ) ;
}

class PrintField : public unary_function<BaseType *, void> {
    FILE *d_out;
    string d_space;
    bool d_constrained;
public:
    PrintField(FILE *o, string s, bool c) 
	: d_out(o), d_space(s), d_constrained(c) {}

    void operator()(BaseType *btp) {
	btp->print_xml(d_out, d_space, d_constrained);
    }
};
	
void
Constructor::print_xml(FILE *out, string space, bool constrained)
{
    bool has_attributes = false; // *** fix me
    bool has_variables = (var_begin() != var_end());

    fprintf(out, "%s<%s", space.c_str(), type_name().c_str());
    if (!name().empty())
	fprintf(out, " name=\"%s\"", id2xml(name()).c_str());
    
    if (has_attributes || has_variables) {
	fprintf(out, ">\n");

	get_attr_table().print_xml(out, space + "    ", constrained);

	for_each(var_begin(), var_end(),
		 PrintField(out, space + "    ", constrained));
	
	fprintf(out, "%s<%s/>\n", space.c_str(), type_name().c_str());
    }
    else {
	fprintf(out, "/>\n");
    }
}

/** True if the instance can be flattened and printed as a single table
    of values. For Arrays and Grids this is always false. For Structures
    and Sequences the conditions are more complex. The implementation
    provided by this class always returns false. Other classes should
    override this implementation.

    @todo Change the name to is_flattenable or something like that. 05/16/03
    jhrg

    @brief Check to see whether this variable can be printed simply.
    @return True if the instance can be printed as a single table of
    values, false otherwise. */
bool
Constructor::is_linear()
{
    return false;
}

// $Log: Constructor.cc,v $
// Revision 1.16  2005/03/23 23:14:41  pwest
// algorithm and functional needed on sun and can be included on Linux, so no need to wrap in ifdef WIN32
//
// Revision 1.15  2004/11/16 22:50:20  jimg
// Fixed tests. Also fixed a bug intorduced in Vector where a template
// with no name caused some software (any code which depends on the
// template having the same name as the array) to fail.
//
// Revision 1.14  2004/11/16 17:56:05  jimg
// Added accessors for the new reverse iterators. Also added a new method
// to access variables using an integer index.
//
// Revision 1.13  2004/07/19 07:25:42  rmorris
// #include <functional> for "unary_function" under win32.
//
// Revision 1.12  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.8.2.3  2004/07/02 20:41:51  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.11  2003/12/10 21:11:57  jimg
// Merge with 3.4. Some of the files contains erros (some tests fail). See
// the ChangeLog for information about fixes.
//
// Revision 1.10  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.8.2.2  2003/09/06 22:37:50  jimg
// Updated the documentation.
//
// Revision 1.8.2.1  2003/06/05 20:15:25  jimg
// Removed many uses of strstream and replaced them with stringstream.
//
// Revision 1.9  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.8  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.7  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.6.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.6  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.5  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.3  2002/08/08 06:54:56  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.4  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.3  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.1.2.2  2001/09/25 20:35:28  jimg
// Added a default definition for is_linear().
//
// Revision 1.2  2001/06/15 23:49:01  jimg
// Merged with release-3-2-4.
//
// Revision 1.1.2.1  2001/06/05 16:04:39  jimg
// Created.
//
