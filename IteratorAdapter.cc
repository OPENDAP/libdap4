// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      pwest       Patrick West (pwest@ucar.edu)

// pwest 12/02/2002

#ifdef __GNUG__
#pragma implementation
#endif

#include "IteratorAdapter.h"

IteratorAdapter::
IteratorAdapter( ) :
    _i( 0 ),
    _ref(0)
{
}

IteratorAdapter::
IteratorAdapter( IteratorAdapter *iter ) :
    _i( iter ),
    _ref(0)
{ 
    if( _i ) {
	_i->incref(); 
    }
}

IteratorAdapter::
IteratorAdapter( const IteratorAdapter &iter ) :
    _i( iter._i ),
    _ref(0)
{ 
    if( _i ) {
	_i->incref(); 
    }
}

IteratorAdapter::
~IteratorAdapter(void)
{
    if( _i ) {
	_i->free() ;
    }
}

IteratorAdapter &IteratorAdapter::
operator=( const IteratorAdapter &iter )
{
    if (&iter == this) {	/* assignment to self 09/12/02 jhrg */
	return *this;
    }

    if( _i ) {
	_i->free(); 
    }

    _i = iter._i; 
    _ref = 0; 
    if( _i ) {
	_i->incref(); 
    }

    return *this; 
}

void IteratorAdapter::
first(void)
{
    if( _i ) {
	_i->first() ;
    }
}

void IteratorAdapter::
next(void)
{
    if( _i ) {
	_i->next() ;
    }
}

IteratorAdapter::
operator bool(void)
{
    if( _i ) {
	return *_i ;
    }
    
    return false ;
}

/*
bool IteratorAdapter::
operator==( void *op2 )
{ 
    if( op2 == NULL ) {
	return !(*this) ; 
    }
    
    return false ;
}

bool IteratorAdapter::
operator!=( void *op2 )
{ 
    if( op2 == NULL ) {
	return *this ;
    }
    
    return false ;
}
*/

bool IteratorAdapter::
operator==( const IteratorAdapter &i )
{ 
    if( _i && i._i ) {
	return *_i == *(i._i) ; 
    } else if( _i && !(i._i ) ) {
	return !(*_i) ;
    } else if( !_i && i._i ) {
	return !(*(i._i)) ;
    }
    
    return false ;
}

bool IteratorAdapter::
operator!=( const IteratorAdapter &i )
{ 
    if( _i && i._i ) {
	return !(*_i == *(i._i)) ; 
    } else if( _i && !(i._i ) ) {
	return *_i ;
    } else if( !_i && i._i ) {
	return *(i._i) ;
    }
    
    return true ;
}

IteratorAdapter *IteratorAdapter::
getIterator(void)
{
    return _i ;
}

void IteratorAdapter::
incref(void)
{
    _ref++ ;
}

void IteratorAdapter::
decref(void)
{
    _ref-- ;
}

void IteratorAdapter::
free(void)
{
    if( --_ref == 0 ) {
	delete this ;
    }
}

