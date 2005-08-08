
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

// Implementation for PassiveArray.
//
// pwest 11/04/03

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdlib.h>

#include "PassiveArray.h"
#include "InternalErr.h"
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using std::cerr;
using std::endl;

/** The PassiveArray constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications. 
      
    @param n A string containing the name of the variable to be created.
    @param v The template variable instance. This is used to determine the
    type of the array. For example, for an Array of Float64, pass a pointer
    to a Float64. */
PassiveArray::PassiveArray(const string &n, BaseType *v)
    : Array(n, v),
      _byte_val( 0 ),
      _int16_val( 0 ),
      _int32_val( 0 ),
      _uint16_val( 0 ),
      _uint32_val( 0 ),
      _float32_val( 0 ),
      _float64_val( 0 ),
      _str_val( 0 )
{
}

PassiveArray::PassiveArray(const PassiveArray &copy_from)
    : Array(copy_from),
      _byte_val( 0 ),
      _int16_val( 0 ),
      _int32_val( 0 ),
      _uint16_val( 0 ),
      _uint32_val( 0 ),
      _float32_val( 0 ),
      _float64_val( 0 ),
      _str_val( 0 )
{
}
    
BaseType *
PassiveArray::ptr_duplicate()
{
    return new PassiveArray(*this);
}

PassiveArray::~PassiveArray()
{
    DBG(cerr << "~PassiveArray" << endl);
}

PassiveArray &
PassiveArray::operator=(const PassiveArray &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate( rhs ) ;

    return *this;
}

bool
PassiveArray::read( const string &dataset )
{
    if( !read_p() )
    {
	switch( var()->type() )
	{
	    case dods_byte_c:
	    {
		val2buf( (void *) _byte_val );
		delete [] _byte_val;
		_byte_val = 0 ;
	    }
	    break;
	    case dods_int16_c:
	    {
		val2buf( (void *) _int16_val );
		delete [] _int16_val;
		_int16_val = 0 ;
	    }
	    break;
	    case dods_uint16_c:
	    {
		val2buf( (void *) _uint16_val );
		delete [] _uint16_val;
		_uint16_val = 0 ;
	    }
	    break;
	    case dods_int32_c:
	    {
		val2buf( (void *) _int32_val );
		delete [] _int32_val;
		_int32_val = 0 ;
	    }
	    break;
	    case dods_uint32_c:
	    {
		val2buf( (void *) _uint32_val );
		delete [] _uint32_val;
		_uint32_val = 0 ;
	    }
	    break;
	    case dods_float32_c:
	    {
		val2buf( (void *) _float32_val );
		delete [] _float32_val;
		_float32_val = 0 ;
	    }
	    break;
	    case dods_float64_c:
	    {
		val2buf( (void *) _float64_val );
		delete [] _float64_val;
		_float64_val = 0 ;
	    }
	    break;
	    case dods_str_c:
	    {
		val2buf( (void*) _str_val );
		delete [] _str_val;
		_str_val = 0 ;
	    }
	    break;
	    default:
	    {
		cerr << __FILE__ << ":"
		     << __LINE__ << "Unsupported DODS/DAP data type"
		     << endl;
		throw InternalErr( "Unsupported DODS/DAP data type" );
	    }
	    break;
	}
	set_read_p( true );
    }

    return true ;
}

bool
PassiveArray::set_value( dods_byte *val, int sz )
{
    if( var()->type() == dods_byte_c && val )
    {
	_byte_val = new dods_byte[sz] ;
	for( register int t = 0; t < sz; t++ )
	{
	    _byte_val[t] = val[t] ;
	}
	set_length( sz ) ;
	return read( "" ) ;
    }
    else
    {
	return false;
    }
}

bool
PassiveArray::set_value( dods_int16 *val, int sz )
{
    if( var()->type() == dods_int16_c && val )
    {
	_int16_val = new dods_int16[sz] ;
	for( register int t = 0; t < sz; t++ )
	{
	    _int16_val[t] = val[t] ;
	}
	set_length( sz ) ;
	return read( "" ) ;
    }
    else
    {
	return false;
    }
}

bool
PassiveArray::set_value( dods_int32 *val, int sz )
{
    if( var()->type() == dods_int32_c && val )
    {
	_int32_val = new dods_int32[sz] ;
	for( register int t = 0; t < sz; t++ )
	{
	    _int32_val[t] = val[t] ;
	}
	set_length( sz ) ;
	return read( "" ) ;
    }
    else
    {
	return false;
    }
}

bool
PassiveArray::set_value( dods_uint16 *val, int sz )
{
    if( var()->type() == dods_uint16_c && val )
    {
	_uint16_val = new dods_uint16[sz] ;
	for( register int t = 0; t < sz; t++ )
	{
	    _uint16_val[t] = val[t] ;
	}
	set_length( sz ) ;
	return read( "" ) ;
    }
    else
    {
	return false;
    }
}

bool
PassiveArray::set_value( dods_uint32 *val, int sz )
{
    if( var()->type() == dods_uint32_c && val )
    {
	_uint32_val = new dods_uint32[sz] ;
	for( register int t = 0; t < sz; t++ )
	{
	    _uint32_val[t] = val[t] ;
	}
	set_length( sz ) ;
	return read( "" ) ;
    }
    else
    {
	return false;
    }
}

bool
PassiveArray::set_value( dods_float32 *val, int sz )
{
    if( var()->type() == dods_float32_c && val )
    {
	_float32_val = new dods_float32[sz] ;
	for( register int t = 0; t < sz; t++ )
	{
	    _float32_val[t] = val[t] ;
	}
	set_length( sz ) ;
	return read( "" ) ;
    }
    else
    {
	return false;
    }
}

bool
PassiveArray::set_value( dods_float64 *val, int sz )
{
    if( var()->type() == dods_float64_c && val )
    {
	_float64_val = new dods_float64[sz] ;
	for( register int t = 0; t < sz; t++ )
	{
	    _float64_val[t] = val[t] ;
	}
	set_length( sz ) ;
	return read( "" ) ;
    }
    else
    {
	return false;
    }
}

bool
PassiveArray::set_value( string *val, int sz )
{
    if( var()->type() == dods_str_c && val )
    {
	_str_val = new string[sz] ;
	for( register int t = 0; t < sz; t++ )
	{
	    _str_val[t] = val[t] ;
	}
	set_length( sz ) ;
	return read( "" ) ;
    }
    else
    {
	return false;
    }
}

// $Log: PassiveArray.cc,v $
// Revision 1.2  2005/04/07 22:32:47  jimg
// Updated doxygen comments: fixed errors; updated comments about set_read_p.
// Removed the VirtualCtor classes. Added a README about the factory
// classes.
//
// Revision 1.1  2004/07/09 16:34:38  pwest
// Adding Passive Data Model objects
//
