
// -*- mode: c++; c-basic-offset:4 -*-

#ifndef _ITERATORADAPTER_H
#define _ITERATORADAPTER_H

#include <stdio.h>

class IteratorAdapter
{
#if 0
    friend bool operator==(IteratorAdapter op1, IteratorAdapter op2);
#endif

public:
    /* constructors */
    IteratorAdapter( ) : _i( 0 ), _ref(0) { }
    IteratorAdapter( IteratorAdapter *iter ) : _i( iter ), _ref(0) { 
	if( _i ) 
	    _i->incref(); 
    }
    IteratorAdapter( const IteratorAdapter &iter ) : _i( iter._i ), _ref(0) { 
	if( _i ) 
	    _i->incref(); 
    }

    /* assignment operator */
    IteratorAdapter &operator=( const IteratorAdapter &iter ) {
	if (&iter == this)	/* assignment to self 09/12/02 jhrg */
	    return *this;

	if( _i ) 
	    _i->free(); 
	_i = iter._i; 
	_ref = 0; 
	if( _i ) 
	    _i->incref(); 
	return *this; 
    }

    /* destructor */
    virtual ~IteratorAdapter( ) { if( _i ) _i->free() ; }

    /* public methods */
    virtual void first( ) { if( _i ) _i->first() ; }
    virtual void next( ) { if( _i ) _i->next() ; }
    virtual operator bool( ) { if( _i ) return *_i ; return false ; }
    IteratorAdapter *getIterator( ) { return _i ; }

    /* reference counting */
    virtual void incref( ) { _ref++ ; }
    virtual void decref( ) { _ref-- ; }
    virtual void free( ) { if( --_ref == 0 ) delete this ; }

protected:

private:
    /* private data members */
    IteratorAdapter *_i ;
    int _ref ;
} ;

#if 0
bool 
operator==(IteratorAdapter op1, IteratorAdapter op2)
{
    return op1._i == op2._i;
}
#endif

// $Log: IteratorAdapter.h,v $
// Revision 1.2  2002/09/13 16:27:47  jimg
// Added a CVS log.
// Added an emacs mode setting to match the rest of our code (always an ongoing
// process...). Reformatted so that it doesn't wrap in an 80 column editor.
// Added an attempt at operator==() which causes problems down in the idl
// command line client. I've wrapped it in guards; if the code proves useless,
// we should remove it.
//

#endif // _ITERATORADAPTER_H

