// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      pwest       Patrick West (pwest@ucar.edu)

// pwest 12/02/2002

#ifdef __GNUG__
#pragma implementation
#endif

#include "ArrayIterAdapter.h"

ArrayIterAdapter::
ArrayIterAdapter( std::vector<Array::dimension> &vec ) :
    _vec( vec )
{
}

ArrayIterAdapter::
~ArrayIterAdapter(void)
{
}

void ArrayIterAdapter::
first(void)
{
    _i = _vec.begin() ;
}

void ArrayIterAdapter::
next(void)
{
    _i++ ;
}

ArrayIterAdapter::
operator bool(void)
{
    return ( _i != _vec.end() ) ;
}

bool ArrayIterAdapter::
operator==( const IteratorAdapter &i )
{ 
    return _i == ((const ArrayIterAdapter &)i)._i ; 
}

Array::dimension ArrayIterAdapter::
entry(void)
{
    return *_i ;
}

ArrayIterAdapter::iter &ArrayIterAdapter::
getIterator(void)
{
    return _i ;
}

