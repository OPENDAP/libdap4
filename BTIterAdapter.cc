// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      pwest       Patrick West (pwest@ucar.edu)

// pwest 12/02/2002

#ifdef __GNUG__
#pragma implementation
#endif

#include "BTIterAdapter.h"

BTIterAdapter::
BTIterAdapter( std::vector<BaseType *> &vec ) :
    _vec( vec )
{
}

BTIterAdapter::
~BTIterAdapter(void)
{
}

void BTIterAdapter::
first(void)
{
    _i = _vec.begin() ;
}

void BTIterAdapter::
next(void)
{
    _i++ ;
}

BTIterAdapter::
operator bool(void)
{
    return ( _i != _vec.end() ) ;
}

bool BTIterAdapter::
operator==( const IteratorAdapter &i )
{ 
    return _i == ((const BTIterAdapter &)i)._i ; 
}

BaseType *BTIterAdapter::
entry(void)
{
    return *_i ;
}

BTIterAdapter::iter &BTIterAdapter::
getIterator(void)
{
    return _i ;
}

