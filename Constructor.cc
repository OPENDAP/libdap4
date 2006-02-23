
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


#include "config.h"

#include <string>
#include <algorithm>
#include <functional>

#include "Constructor.h"
//#include "BTIterAdapter.h"

#include "debug.h"
#include "escaping.h"
#include "Error.h"
#include "InternalErr.h"


using namespace std;

// Private member functions

void
Constructor::_duplicate(const Constructor &)
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

/** Return the iterator for the \e ith variable.
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
    if (constrained && !send_p())
        return;

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
	
	fprintf(out, "%s</%s>\n", space.c_str(), type_name().c_str());
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
