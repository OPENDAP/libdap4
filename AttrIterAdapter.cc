
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

#ifdef __GNUG__
// #pragma implementation
#endif

#include "AttrIterAdapter.h"

AttrIterAdapter::AttrIterAdapter( std::vector<AttrTable::entry *> &vec ) :
    _vec( vec )
{
}

AttrIterAdapter::~AttrIterAdapter( )
{
}

void
AttrIterAdapter::first( )
{
    _i = _vec.begin() ;
}

void
AttrIterAdapter::next( )
{
    _i++ ;
}

AttrIterAdapter::operator bool( )
{
    return ( _i != _vec.end() ) ;
}

bool
AttrIterAdapter::operator==( const IteratorAdapter &i )
{ 
    return _i == ((const AttrIterAdapter &)i)._i ; 
}

AttrTable::entry *
AttrIterAdapter::entry( )
{
    return *_i ;
}

AttrIterAdapter::iter &
AttrIterAdapter::getIterator( )
{
    return _i ;
}

// $Log: AttrIterAdapter.cc,v $
// Revision 1.7  2004/07/07 21:08:46  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.5.2.1  2004/07/02 20:41:51  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.6  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.5  2003/04/02 19:14:19  pwest
// Added doxygen comments to iterator adapter classes
//
//

