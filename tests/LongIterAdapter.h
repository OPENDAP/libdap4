

// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	Patrick West <pwest@ucar.edu>

#ifndef array_iter_adapter_h
#define array_iter_adapter_h

#ifndef __POWERPC__
#ifdef __GNUG__
#pragma interface
#endif
#endif

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

// $Log: LongIterAdapter.h,v $
// Revision 1.2  2003/12/11 01:08:38  jimg
// More fixes after resolving conflicts. This code still fails some tests.
//
// Revision 1.1.4.1  2003/06/24 11:07:20  rmorris
// #ifdef'd out #pragma interface directive under __POWERPC__.  It causes
// dynamic typing problems.
//
// Revision 1.1  2003/01/15 19:32:11  pwest
// Adding files to tests director. Still need to use cppUnit and autoconf to
// create configuration and Makefile
//

#endif // array_iter_adapter_h
