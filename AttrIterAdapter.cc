// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      pwest       Patrick West (pwest@ucar.edu)

// pwest 12/02/2002

#ifdef __GNUG__
#pragma implementation
#endif

#include "AttrIterAdapter.h"

AttrIterAdapter::
AttrIterAdapter( std::vector<AttrTable::entry *> &vec ) :
    _vec( vec )
{
}

AttrIterAdapter::
~AttrIterAdapter(void)
{
}

void AttrIterAdapter::
first(void)
{
    _i = _vec.begin() ;
}

void AttrIterAdapter::
next(void)
{
    _i++ ;
}

AttrIterAdapter::
operator bool(void)
{
    return ( _i != _vec.end() ) ;
}

bool AttrIterAdapter::
operator==( const IteratorAdapter &i )
{ 
    return _i == ((const AttrIterAdapter &)i)._i ; 
}

AttrTable::entry *AttrIterAdapter::
entry(void)
{
    return *_i ;
}

AttrIterAdapter::iter &AttrIterAdapter::
getIterator(void)
{
    return _i ;
}

