

// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	Patrick West <pwest@ucar.edu>

#ifndef clause_iter_adapter_h
#define clause_iter_adapter_h

#ifdef __GNUG__
#pragma interface
#endif

#include <vector>
#include <stdio.h>

#include "IteratorAdapter.h"
#include "Clause.h"

class ClauseIterAdapter : public IteratorAdapter
{
    public:
	typedef std::vector<Clause *>::iterator iter ;
	typedef std::vector<Clause *>::const_iterator citer ;

	/* constructor */
	ClauseIterAdapter( std::vector<Clause *> &vec ) ;

	/* destructor */
	virtual ~ClauseIterAdapter( ) ;

	/* derived public methods */
	virtual void first( ) ;
	virtual void next( ) ;
	virtual operator bool( ) ;
	virtual bool operator==( const IteratorAdapter &i ) ;

	/* new public methods */
	Clause *entry( ) ;
	iter &getIterator( ) ;

    protected:

    private:
	/* private data members */
	std::vector<Clause *> &_vec ;
	iter _i ;
} ;

#endif // clause_iter_adapter_h
