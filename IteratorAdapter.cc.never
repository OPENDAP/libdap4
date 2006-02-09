
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// pwest 12/02/2002


#include "IteratorAdapter.h"

IteratorAdapter::IteratorAdapter( ) :
    _i( 0 ),
    _ref( 0 )
{
}

IteratorAdapter::IteratorAdapter( IteratorAdapter *iter ) :
    _i( iter ),
    _ref( 0 )
{ 
    if( _i ) {
	_i->incref(); 
    }
}

IteratorAdapter::IteratorAdapter( const IteratorAdapter &iter ) :
    _i( 0 ),
    _ref( 0 )
{ 
    if( _i ) {
	_i->decref() ;
	_i = 0 ;
    }

    _i = iter._i ;

    if( _i ) {
	_i->incref(); 
    }
}

IteratorAdapter::~IteratorAdapter( )
{
    if( _i ) {
	_i->free() ;
    }
}

IteratorAdapter &
IteratorAdapter::operator=( const IteratorAdapter &iter )
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

void
IteratorAdapter::first( )
{
    if( _i ) {
	_i->first() ;
    }
}

void
IteratorAdapter::next( )
{
    if( _i ) {
	_i->next() ;
    }
}

IteratorAdapter::operator bool( )
{
    if( _i ) {
	return *_i ;
    }
    
    return false ;
}

bool
IteratorAdapter::operator==( const IteratorAdapter &i )
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

bool
IteratorAdapter::operator!=( const IteratorAdapter &i )
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

IteratorAdapter *
IteratorAdapter::getIterator( )
{
    return _i ;
}

void
IteratorAdapter::incref( )
{
    _ref++ ;
}

void
IteratorAdapter::decref( )
{
    _ref-- ;
}

void
IteratorAdapter::free( )
{
    if( --_ref == 0 ) {
	delete this ;
    }
}

// $Log: IteratorAdapter.cc,v $
// Revision 1.8  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.6.2.1  2004/07/02 20:41:52  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.7  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.6  2003/04/02 19:14:19  pwest
// Added doxygen comments to iterator adapter classes
//
//
