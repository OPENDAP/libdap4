
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
 
#ifndef _iterator_adapter_h
#define _iterator_adapter_h

#ifndef __POWERPC__
#ifdef __GNUG__
#pragma interface
#endif
#endif

#include <stdio.h>

/**
    The IteratorAdapter class allows users to continue the use of the GNU
    Pix type, which is now really a standard template library iterator
    class. The use of the IteratorAdapter classes should be transparent to
    the user, who continues to use the Pix type.

    Pix is now typedef'd to be an IteratorAdapter. It used to be typedef'd
    to be a void *. The user should not have to make any modifications
    to their code to account for this change.

    An IteratorAdapter has-a instance of one of the four derived
    IteratorAdapter classes. These four derived classes hold on to the
    stl vector being iterated over and the stl iterator pointing to the
    current element in the vector. This was done so that the outer
    IteratorAdapter could handle the memory management of the inner
    adapter class.

    \verbatim
    ----------------------------------
    | IteratorAdapter                |
    |                                |
    | ----------------------------   |
    | | IteratorAdapter          |   |
    | |                          |   |
    | |   vector<type>           |   |
    | |   vector<type>::iterator |   |
    | |                          |   |
    | ----------------------------   |
    |                                |
    ----------------------------------
    \endverbatim

    Example of using a Pix:

    \verbatim
    AttrTable at ;
    Pix q ;
    for( q = at.first_attr(); q; at.next_attr( q ) )
    {
	if( at.attr( q )->name == "blee" )
	{
	    ... do something here ...
	}
    }
    \endverbatim

    New users should be using the new methods on classes that return and
    take stl iterators rather than using the methods that return and take
    Pix parameters.

    Example of using the new stl iterator methods:

    \verbatim
    AttrTable at ;
    AttrTable::Attr_iter i ;
    for( i = at.attr_begin(); i != at.attr_end(); i++)
    {
	if( (*i)->name == "blee" )
	{
	    ... do something here ...
	}
    }
    \endverbatim

    @brief Class to allow continued use of Pix, adapting to stl iterators
    @see ArrayIterAdapter
    @see AttrIterAdapter
    @see BTIterAdapter
    @see ClauseIterAdapter
 */
class IteratorAdapter
{

public:
    /** @name Instance management functions */
    //@{

    /** This constructor is called when a Pix is created without assigning
	it to something. For example:

	Pix q ;

	It holds on to no inner adapter, no vector and no iterator
    */
    IteratorAdapter( ) ;

    /** This constructor is called when a new Pix is being created. For
	example, in the AttrTable code for first_attr():

	\verbatim
	Pix 
	AttrTable::first_attr()
	{
	    AttrIterAdapter *i = new AttrIterAdapter( attr_map ) ;
	    i->first() ;
	    return i ;
	}
	\endverbatim

	In this example, an AttrIterAdapter is created using new and
	returned. The return value is Pix, which is an IteratorAdapter. An
	IteratorAdapter is created passing it the AttrIterAdapter. The outer
	IteratorAdapter now manages the AttrIterAdapter and redirects calls
	to the AttrIterAdapter

	@param iter Specific adapter class to manage
    */
    IteratorAdapter( IteratorAdapter *iter ) ;

    /** This is the copy constructor. It assigns to itself the inner adapter
	that the passed IteratorAdapter is holding and bumps up the reference
	counter to let the inner adapter know that someone else is holding on to
	it.

	@param iter This is an outer adapter, not a specific inner adapter
    */
    IteratorAdapter( const IteratorAdapter &iter ) ;

    /** This is the assignment operator and does pretty much the same as
	what the copy constructor does. It assigns to itself the inner adapter
	that the passed IteratorAdapter is holding on to and bumps up the
	reference counter to let the inner adapter know that someone else is
	holding on to it.

	@param iter This is an outer adapter, not a specific inner adapter
    */
    IteratorAdapter &operator=( const IteratorAdapter &iter ) ;

    /** The destructor simply decrements the reference count on the inner
	adapter to let it know that this IteratorAdapter is no longer needing
	it. If the
    */
    virtual ~IteratorAdapter( ) ;
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

    /** Returns true if this inner iterator is equal to the inner
	iterator of the passed adapter, else returns false if they are not
	equal.

	@param i compare the passed adapter to "this" adapter
    */
    virtual bool operator==( const IteratorAdapter &i ) ;

    /** Returns true if this inner iterator is NOT equal to the inner
	iterator of the passed adapter

	@param i compare the passed adapter to "this" adapter
    */
    virtual bool operator!=( const IteratorAdapter &i ) ;
    //@}

    /** @name Internal adapter methods */
    //@{

    /** Return the inner adapter class, one of ArrayIterAdapter,
	AttrIterAdapter, BTIterAdapter or ClauseIterAdapter, or NULL if no
	inner adapter is present. This method is used to access the stl
	iterator, access the element that the stl iterator is pointing to,
	perform reference counting if needed. The following example is taken
	from internal DAP code. The access provided is hidden from the
	external user of the AttrTable class.

	\verbatim
	AttrTable::entry *
	AttrTable::attr(Pix p)
	{
	    AttrIterAdapter *i = (AttrIterAdapter *)p.getIterator() ;

	    if( i ) {
		return i->entry() ;
	    }
	    return 0 ;
	}

	\endverbatim
    */
    IteratorAdapter *getIterator( ) ;
    //@}

    /** @name Internal iterator methods */
    //@{

    /** Increment the reference count on this adapter. This is called on the
	inner adapter, not the outer IteratorAdapter, and is done
	automatically when the outer adapter is done referencing the
	inner adapter. Remember that there is an outer adapter and an
	inner adapter that is specific to the class holding the list to
	iterate over, e.g. ArrayIterAdapter for Array holding the vector
	of dimensions.
    */
    virtual void incref( ) ;

    /** Decrement the reference count on this adapter. This is called on the
	inner adapter, not the outer IteratorAdapter, and is done
	automatically when the outer adapter is done referencing the
	inner adapter. Remember that there is an outer adapter and an
	inner adapter that is specific to the class holding the list to
	iterate over, e.g. ArrayIterAdapter for Array holding the vector
	of dimensions.
    */
    virtual void decref( ) ;

    /** Called when a user of the adapter is done with it. This is done
	automatically by the outer adapter when it is done referencing the
	inner adapter.
    */
    virtual void free( ) ;

    //@}

protected:

private:
    /** @name Private Data Members */
    //@{

    /** pointer to inner adapter, one of ArrayITerAdapter,
	AttrIterAdapter, BTIterAdapter, ClauseIterAdapter
    */
    IteratorAdapter *_i ;

    /** how many objects are currently referencing this object
    */
    int _ref ;

    //@}
} ;

// $Log: IteratorAdapter.h,v $
// Revision 1.8  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.7.2.1  2003/06/23 11:49:18  rmorris
// The #pragma interface directive to GCC makes the dynamic typing functionality
// go completely haywire under OS X on the PowerPC.  We can't use that directive
// on that platform and it was ifdef'd out for that case.
//
// Revision 1.7  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.6  2003/04/02 19:14:19  pwest
// Added doxygen comments to iterator adapter classes
//
// Revision 1.5  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.4.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.4  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.3  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.2.2.5  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.2.2.4  2002/12/05 20:36:19  pwest
// Corrected problems with IteratorAdapter code, making methods non-inline,
// creating source files and template instantiation file. Cleaned up file
// descriptors and memory management problems. Corrected problem in Connect
// where the xdr source was not being cleaned up or a new one created when a
// new file was opened for reading.
//
// Revision 1.2.2.3  2002/11/05 01:11:49  jimg
// Added some boilerplate and fiddled with formatting.
//
// Revision 1.2.2.2  2002/10/29 22:21:00  pwest
// added operator== and operator!= operators to IteratorAdapter and
// IteratorAdapterT classes to handle Pix == Pix use.
//
// Revision 1.2.2.1  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.2  2002/09/13 16:27:47  jimg
// Added a CVS log.
// Added an emacs mode setting to match the rest of our code (always an ongoing
// process...). Reformatted so that it doesn't wrap in an 80 column editor.
// Added an attempt at operator==() which causes problems down in the idl
// command line client. I've wrapped it in guards; if the code proves useless,
// we should remove it.
//

#endif // _ITERATORADAPTER_H

