

// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	Patrick West <pwest@ucar.edu>

#ifndef bt_iter_adapter_h
#define bt_iter_adapter_h

#ifdef __GNUG__
#pragma interface
#endif

#include <vector>
#include <stdio.h>

#include "IteratorAdapter.h"
#include "BaseType.h"

class BTIterAdapter : public IteratorAdapter
{
    public:
	typedef std::vector<BaseType *>::iterator iter ;
	typedef std::vector<BaseType *>::const_iterator citer ;

	/* constructor */
	BTIterAdapter( std::vector<BaseType *> &vec ) ;

	/* destructor */
	virtual ~BTIterAdapter( ) ;

	/* derived public methods */
	virtual void first( ) ;
	virtual void next( ) ;
	virtual operator bool( ) ;
	virtual bool operator==( const IteratorAdapter &i ) ;

	/* new public methods */
	BaseType *entry( ) ;
	iter &getIterator( ) ;

    protected:

    private:
	/* private data members */
	std::vector<BaseType *> &_vec ;
	iter _i ;
} ;

#endif // bt_iter_adapter_h
