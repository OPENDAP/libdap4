

// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	Patrick West <pwest@ucar.edu>

#ifndef attr_iter_adapter_h
#define attr_iter_adapter_h

#ifdef __GNUG__
#pragma interface
#endif

#include <vector>
#include <stdio.h>

#include "IteratorAdapter.h"
#include "AttrTable.h"

class AttrIterAdapter : public IteratorAdapter
{
    public:
	typedef std::vector<AttrTable::entry *>::iterator iter ;
	typedef std::vector<AttrTable::entry *>::const_iterator citer ;

	/* constructor */
	AttrIterAdapter( std::vector<AttrTable::entry *> &vec ) ;

	/* destructor */
	virtual ~AttrIterAdapter( ) ;

	/* derived public methods */
	virtual void first( ) ;
	virtual void next( ) ;
	virtual operator bool( ) ;
	virtual bool operator==( const IteratorAdapter &i ) ;

	/* new public methods */
	AttrTable::entry *entry( ) ;
	iter &getIterator( ) ;

    protected:

    private:
	/* private data members */
	std::vector<AttrTable::entry *> &_vec ;
	iter _i ;
} ;

#endif // attr_iter_adapter_h
