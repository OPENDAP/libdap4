// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      pwest       Patrick West (pwest@ucar.edu)

// pwest 12/02/2002

#ifdef __GNUG__
#pragma implementation
#endif

#include "ClauseIterAdapter.h"

ClauseIterAdapter::
ClauseIterAdapter( std::vector<Clause *> &vec ) :
    _vec( vec )
{
}

ClauseIterAdapter::
~ClauseIterAdapter(void)
{
}

void ClauseIterAdapter::
first(void)
{
    _i = _vec.begin() ;
}

void ClauseIterAdapter::
next(void)
{
    _i++ ;
}

ClauseIterAdapter::
operator bool(void)
{
    return ( _i != _vec.end() ) ;
}

bool ClauseIterAdapter::
operator==( const IteratorAdapter &i )
{ 
    return _i == ((const ClauseIterAdapter &)i)._i ; 
}

Clause *ClauseIterAdapter::
entry(void)
{
    return *_i ;
}

ClauseIterAdapter::iter &ClauseIterAdapter::
getIterator(void)
{
    return _i ;
}

