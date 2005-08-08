
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
//      pwest       Patrick West <pwest@ucar.edu>

// Implementation for PassiveStructure.
//
// pwest 11/04/03

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdlib.h>

#include "PassiveStructure.h"
#include "InternalErr.h"
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using std::cerr;
using std::endl;

/** The PassiveStructure constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications. 
      
    @param n A string containing the name of the variable to be
    created. 
*/
PassiveStructure::PassiveStructure(const string &n) : Structure(n)
{
}

PassiveStructure::PassiveStructure(const PassiveStructure &copy_from) : Structure(copy_from)
{
}
    
BaseType *
PassiveStructure::ptr_duplicate()
{
    return new PassiveStructure(*this);
}

PassiveStructure::~PassiveStructure()
{
    DBG(cerr << "~PassiveStructure" << endl);
}

PassiveStructure &
PassiveStructure::operator=(const PassiveStructure &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<BaseType &>(*this) = rhs;

    _duplicate(rhs);

    return *this;
}

bool
PassiveStructure::read( const string &dataset )
{
    for( Vars_iter i = _vars.begin(); i != _vars.end(); i++ )
    {
	(*i)->read( dataset ) ;
    }
    set_read_p( true ) ;

    return true ;
}

// $Log: PassiveStructure.cc,v $
// Revision 1.2  2005/02/16 22:22:36  pwest
// implemented read
//
// Revision 1.1  2004/07/09 16:34:38  pwest
// Adding Passive Data Model objects
//
