#ifndef _ITERATORADAPTER_H
#define _ITERATORADAPTER_H

#include <stdio.h>

class IteratorAdapter
{
    public:
	/* constructors */
	IteratorAdapter( ) : _i( 0 ), _ref(0) { }
	IteratorAdapter( IteratorAdapter *iter ) : _i( iter ), _ref(0) { if( _i ) _i->incref() ; }
	IteratorAdapter( const IteratorAdapter &iter ) : _i( iter._i ), _ref(0) { if( _i ) _i->incref() ; }

	/* assignment operator */
	IteratorAdapter &operator=( const IteratorAdapter &iter ) { if( _i ) _i->free() ; _i = iter._i ; _ref = 0 ; if( _i ) _i->incref() ; return *this ; }

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

#endif // _ITERATORADAPTER_H

