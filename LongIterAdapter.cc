// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      pwest       Patrick West (pwest@ucar.edu)

// pwest 12/02/2002

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>

using std::cerr ;
using std::endl ;

#include "LongIterAdapter.h"

LongIterAdapter::
LongIterAdapter( std::vector<long> &vec ) :
    _vec( vec )
{
}

LongIterAdapter::
~LongIterAdapter(void)
{
}

void LongIterAdapter::
first(void)
{
    _i = _vec.begin() ;
}

void LongIterAdapter::
next(void)
{
    _i++ ;
}

LongIterAdapter::
operator bool(void)
{
    return ( _i != _vec.end() ) ;
}

bool LongIterAdapter::
operator==( const IteratorAdapter &i )
{ 
    return _i == ((const LongIterAdapter &)i)._i ; 
}

long LongIterAdapter::
entry(void)
{
    return *_i ;
}

LongIterAdapter::iter &LongIterAdapter::
getIterator(void)
{
    return _i ;
}

