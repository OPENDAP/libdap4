// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      pwest       Patrick West (pwest@ucar.edu)

// pwest 12/02/2002

#ifdef __GNUG__
#pragma implementation
#endif

#include "LongIterAdapter.h"

LongIterAdapter::LongIterAdapter( std::vector<long> &vec ) :
    _vec( vec )
{
}

LongIterAdapter::~LongIterAdapter( )
{
}

void
LongIterAdapter::first( )
{
    _i = _vec.begin() ;
}

void
LongIterAdapter::next( )
{
    _i++ ;
}

LongIterAdapter::operator bool( )
{
    return ( _i != _vec.end() ) ;
}

bool
LongIterAdapter::operator==( const IteratorAdapter &i )
{ 
    return _i == ((const LongIterAdapter &)i)._i ; 
}

long
LongIterAdapter::entry( )
{
    return *_i ;
}

LongIterAdapter::iter &
LongIterAdapter::getIterator( )
{
    return _i ;
}

