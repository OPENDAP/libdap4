
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
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
 
#ifndef clause_iter_adapter_h
#define clause_iter_adapter_h

#ifndef __POWERPC__
#ifdef __GNUG__
#pragma interface
#endif
#endif

#include <vector>
#include <stdio.h>

#include "IteratorAdapter.h"
#include "Clause.h"

/**
    The ClauseIterAdpater class is a specific IteratorAdapter for the Clause
    class that represents any stl iterator used for a vector of Clauses.
    This class adapts the GNU Pix to stl iterators.

    @brief stl adapter class specific to the Clause class
    @see IteratorAdapter
    @see ArrayIterAdapter
    @see AttrIterAdapter
    @see BTIterAdapter
 */
class ClauseIterAdapter : public IteratorAdapter
{
    public:
	/** @name iterator typedefs*/
	//@{

	typedef std::vector<Clause *>::iterator iter ;
	typedef std::vector<Clause *>::const_iterator citer ;
	//@}

	/** @name Instance management functions */
	//@{

	/** Constructor which takes the vector of Clause objects and
	    maintains an iterator over that vector

	    @param vec vector of Clause objects to iterate over
	*/
	ClauseIterAdapter( std::vector<Clause *> &vec ) ;

	virtual ~ClauseIterAdapter( ) ;
	//@}

	/** @name Iteration methods */
	//@{

	/** Point the iterator to the first element in the list
	*/
	virtual void first( ) ;

	/** Move the iterate to the next element in the list
	*/
	virtual void next( ) ;

	/** Returns true if the iterator is pointing to an element in the list,
	    else it returns false saying that there is nothing in the list or
	    the iterator has reached the end of the list
	*/
	virtual operator bool( ) ;

	/** Returns true if this stl iterator is equal to the stl iterator
	    held by the passed inner iterator, else returns false if they
	    are not equal.

	    @param i compare the passed adapter to "this" adapter
	*/
	virtual bool operator==( const IteratorAdapter &i ) ;
	//@}

	/** @name Internal adapter methods */
	//@{

	/** Return the current element pointed to by the stl iterator
	*/
	Clause *entry( ) ;

	/** Return the stl iterator held by this internal adapter
	*/
	iter &getIterator( ) ;
	//@}

    protected:

    private:
	/* private data members */
	std::vector<Clause *> &_vec ;
	iter _i ;
} ;

// $Log: ClauseIterAdapter.h,v $
// Revision 1.5  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.4.2.1  2003/06/23 11:49:18  rmorris
// The #pragma interface directive to GCC makes the dynamic typing functionality
// go completely haywire under OS X on the PowerPC.  We can't use that directive
// on that platform and it was ifdef'd out for that case.
//
// Revision 1.4  2003/04/02 19:14:19  pwest
// Added doxygen comments to iterator adapter classes
//
//

#endif // clause_iter_adapter_h
