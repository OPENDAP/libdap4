
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
 
#ifndef attr_iter_adapter_h
#define attr_iter_adapter_h

#ifdef __GNUG__
#pragma interface
#endif

#include <vector>
#include <stdio.h>

#include "IteratorAdapter.h"
#include "AttrTable.h"

/**
    The AttrIterAdpater class is a specific IteratorAdapter for the
    AttrTable class that represents the stl iterator used for the AttrTable
    vector of attributes. This class adapts the GNU Pix to stl iterators.

    @brief stl adapter class specific to the AttrTable class
    @see IteratorAdapter
    @see ArrayIterAdapter
    @see BTIterAdapter
    @see ClauseIterAdapter
 */
class AttrIterAdapter : public IteratorAdapter
{
    public:
	/** @name iterator typedefs*/
	//@{

	typedef std::vector<AttrTable::entry *>::iterator iter ;
	typedef std::vector<AttrTable::entry *>::const_iterator citer ;
	//@}

	/** @name Instance management functions */
	//@{

	/** Constructor which takes the AttrTable vector and maintains an
	    iterator over that vector

	    @param vec AttrTable vector to iterate over
	*/
	AttrIterAdapter( std::vector<AttrTable::entry *> &vec ) ;

	virtual ~AttrIterAdapter( ) ;
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
	AttrTable::entry *entry( ) ;

	/** Return the stl iterator held by this internal adapter
	*/
	iter &getIterator( ) ;
	//@}

    protected:

    private:
	/* private data members */
	std::vector<AttrTable::entry *> &_vec ;
	iter _i ;
} ;

// $Log: AttrIterAdapter.h,v $
// Revision 1.4  2003/04/02 19:14:19  pwest
// Added doxygen comments to iterator adapter classes
//
//

#endif // attr_iter_adapter_h
