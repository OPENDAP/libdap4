

// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	Patrick West <pwest@ucar.edu>

#ifndef array_iter_adapter_h
#define array_iter_adapter_h


#include <vector>
#include <stdio.h>

#include "IteratorAdapter.h"
#include "Array.h"

class LongIterAdapter : public IteratorAdapter
{
    public:
	typedef std::vector<long>::iterator iter ;
	typedef std::vector<long>::const_iterator citer ;

	/* constructor */
	LongIterAdapter( std::vector<long> &vec ) ;

	/* destructor */
	virtual ~LongIterAdapter( ) ;

	/* derived public methods */
	virtual void first( ) ;
	virtual void next( ) ;
	virtual operator bool( ) ;
	virtual bool operator==( const IteratorAdapter &i ) ;

	/* new public methods */
	long entry( ) ;
	iter &getIterator( ) ;

    protected:

    private:
	/* private data members */
	std::vector<long> &_vec ;
	iter _i ;
} ;

#endif // array_iter_adapter_h
