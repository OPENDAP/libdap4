

// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	Patrick West <pwest@ucar.edu>

#ifndef array_iter_adapter_h
#define array_iter_adapter_h

#ifdef __GNUG__
#pragma interface
#endif

#include <vector>
#include <stdio.h>

#include "IteratorAdapter.h"
#include "Array.h"

class ArrayIterAdapter : public IteratorAdapter
{
    public:
	typedef std::vector<Array::dimension>::iterator iter ;
	typedef std::vector<Array::dimension>::const_iterator citer ;

	/* constructor */
	ArrayIterAdapter( std::vector<Array::dimension> &vec ) ;

	/* destructor */
	virtual ~ArrayIterAdapter( ) ;

	/* derived public methods */
	virtual void first( ) ;
	virtual void next( ) ;
	virtual operator bool( ) ;
	virtual bool operator==( const IteratorAdapter &i ) ;

	/* new public methods */
	Array::dimension entry( ) ;
	iter &getIterator( ) ;

    protected:

    private:
	/* private data members */
	std::vector<Array::dimension> &_vec ;
	iter _i ;
} ;

#endif // array_iter_adapter_h
