// XDRStreamUnMarshaller.cc

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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
#include "config.h"
#include "XDRStreamUnMarshaller.h"

#include <cstring> // for memcpy
#include <string>
#include <sstream>

//#define DODS_DEBUG2 1
//#define DODS_DEBUG 1

#include "Str.h"
#include "Vector.h"
#include "Array.h"
#include "util.h"
#include "InternalErr.h"
#include "debug.h"

namespace libdap {

char *XDRStreamUnMarshaller::_buf = 0 ;

XDRStreamUnMarshaller::XDRStreamUnMarshaller( istream &in )
    : _source( 0 ), _in( in )
{
    if (!_buf)
	_buf = (char *) malloc(XDR_DAP_BUFF_SIZE);
    if (!_buf)
	throw Error("Failed to allocate memory for data serialization.");

    _source = new XDR;
    xdrmem_create(_source, _buf, XDR_DAP_BUFF_SIZE, XDR_DECODE);
}

XDRStreamUnMarshaller::XDRStreamUnMarshaller()
    : UnMarshaller(), _source( 0 ), _in( cin )
{
    throw InternalErr( __FILE__, __LINE__, "Default constructor not implemented." ) ;
}

XDRStreamUnMarshaller::XDRStreamUnMarshaller( const XDRStreamUnMarshaller &um )
    : UnMarshaller( um ), _source( 0 ), _in( cin )
{
    throw InternalErr( __FILE__, __LINE__, "Copy constructor not implemented." ) ;
}

XDRStreamUnMarshaller &
XDRStreamUnMarshaller::operator=( const XDRStreamUnMarshaller & )
{
    throw InternalErr( __FILE__, __LINE__, "Copy operator not implemented." ) ;

    return *this ;
}

XDRStreamUnMarshaller::~XDRStreamUnMarshaller( )
{
    if ( _source )
	delete_xdrstdio( _source ) ;
    _source = 0;
}

void
XDRStreamUnMarshaller::get_byte( dods_byte &val )
{
    if (xdr_setpos( _source, 0 ) < 0)
	throw Error("Failed to reposition input stream");
    if (!(_in.read( _buf, 4 ))) {
	if (_in.eof())
	    throw Error("Premature EOF in input stream");
	else {
	    ostringstream ss("Error reading from input stream: ");
	    ss << _in.rdstate();
	    throw Error(ss.str());
	}
    }

    DBG2( std::cerr << "_in.gcount(): " << _in.gcount() << std::endl );
    DBG2( std::cerr << "_in.tellg(): " << _in.tellg() << std::endl );
    DBG2( std::cerr << "_buf[0]: " << hex << _buf[0] << "; _buf[1]: " << _buf[1]
             << "; _buf[2]: " << _buf[2] << "; _buf[3]: " << _buf[3]
             << dec << std::endl );

    if( !xdr_char( _source, (char *)&val ) )
        throw Error("Network I/O Error. Could not read byte data.");

    DBG2(std::cerr << "get_byte: " << val << std::endl );
}

void
XDRStreamUnMarshaller::get_int16( dods_int16 &val )
{
    xdr_setpos( _source, 0 );
    _in.read( _buf, 4 );

    if( !XDR_INT16( _source, &val ) )
        throw Error("Network I/O Error. Could not read int 16 data.");
}

void
XDRStreamUnMarshaller::get_int32( dods_int32 &val )
{
    xdr_setpos( _source, 0 );
    _in.read( _buf, 4 );

    if( !XDR_INT32( _source, &val ) )
        throw Error("Network I/O Error. Could not read int 32 data.");
}

void
XDRStreamUnMarshaller::get_float32( dods_float32 &val )
{
    xdr_setpos( _source, 0 );
    _in.read( _buf, 4 );

    if( !xdr_float( _source, &val ) )
        throw Error("Network I/O Error. Could not read float 32 data.");
}

void
XDRStreamUnMarshaller::get_float64( dods_float64 &val )
{
    xdr_setpos( _source, 0 );
    _in.read( _buf, 8 );

    if( !xdr_double( _source, &val ) )
        throw Error("Network I/O Error. Could not read float 64 data.");
}

void
XDRStreamUnMarshaller::get_uint16( dods_uint16 &val )
{
    xdr_setpos( _source, 0 );
    _in.read( _buf, 4 );

    if( !XDR_UINT16( _source, &val ) )
        throw Error("Network I/O Error. Could not read uint 16 data.");
}

void
XDRStreamUnMarshaller::get_uint32( dods_uint32 &val )
{
    xdr_setpos( _source, 0 );
    _in.read( _buf, 4 );

    if( !XDR_UINT32( _source, &val ) )
        throw Error("Network I/O Error. Could not read uint 32 data.");
}

void
XDRStreamUnMarshaller::get_str( string &val )
{
    int i;
    get_int( i ) ;
    DBG(std::cerr << "i: " << i << std::endl);

    // Must round up string size to next 4
    i = ( ( i + 3 ) / 4 ) * 4;
    DBG(std::cerr << "i: " << i << std::endl);

    char *in_tmp = 0;
    //char *buf = 0;
    //XDR *source = 0;
    // Must address the case where the string is larger than the buffer
    if ( i + 4 > XDR_DAP_BUFF_SIZE ) {
	char *buf = (char *) malloc( i + 4 );
	if (!buf)
		throw InternalErr(__FILE__, __LINE__, "Error allocating memory");
	XDR *source = new XDR;	
	xdrmem_create(source, buf, i + 4, XDR_DECODE);
	memcpy( buf, _buf, 4 );

	_in.read( buf + 4, i );

	xdr_setpos( source, 0 );
	if( !xdr_string( source, &in_tmp, max_str_len ) ) {
	    delete_xdrstdio( source );
	    throw Error("Network I/O Error. Could not read string data.");
	}

	delete_xdrstdio( source );
    }
    else {
	_in.read( _buf + 4, i );

	xdr_setpos( _source, 0 );
	if( !xdr_string( _source, &in_tmp, max_str_len ) )
	    throw Error("Network I/O Error. Could not read string data.");
    }

    val = in_tmp ;

    free( in_tmp ) ;
}

void
XDRStreamUnMarshaller::get_url( string &val )
{
    get_str( val ) ;
}

void
XDRStreamUnMarshaller::get_opaque( char *val, unsigned int len )
{
    xdr_setpos( _source, 0 );

    // Round len up to the next multiple of 4. There is also the RNDUP()
    // macro in xdr.h, at least on OS/X.
    len += len&3;
    if ( static_cast<int>(len) > XDR_DAP_BUFF_SIZE )
	throw Error("Network I/O Error. Length of opaque data larger than allowed");

    _in.read( _buf, len );

     xdr_opaque( _source, val, len ) ;
}

void
XDRStreamUnMarshaller::get_int( int &val )
{
    xdr_setpos( _source, 0 );
    _in.read( _buf, 4 );

    if( !xdr_int( _source, &val ) )
	throw Error("Network I/O Error(1).");

    DBG(std::cerr << "get_int: " << val << std::endl);
}

void
XDRStreamUnMarshaller::get_vector( char **val, unsigned int &num, Vector & )
{
    int i;
    get_int( i ) ;  // This leaves the XDR encoded value in _buf; used later
    DBG(std::cerr << "i: " << i << std::endl);

    // Must round up string size to next 4
    i += i&3;
    DBG(std::cerr << "i: " << i << std::endl);

    //char *buf = 0;
    //XDR *source = 0;
    // Must address the case where the string is larger than the buffer
    if ( i + 4 > XDR_DAP_BUFF_SIZE ) {
	char *buf = (char *) malloc( i + 4 );
	if (!buf)
		throw InternalErr(__FILE__, __LINE__, "Error allocating memory");
	XDR *source = new XDR;	
	xdrmem_create(source, buf, i + 4, XDR_DECODE);
	memcpy( buf, _buf, 4 );

	_in.read( buf + 4, i );
	DBG2(cerr << "bytes read: " << _in.gcount() << endl);

	xdr_setpos( source, 0 );
	if( !xdr_bytes( _source, val, &num, DODS_MAX_ARRAY) ) {
	    delete_xdrstdio( source );
	    throw Error("Network I/O Error. Could not read byte array data.");
	}

	delete_xdrstdio( source );
    }
    else {
	_in.read( _buf + 4, i );
	DBG2(cerr << "bytes read: " << _in.gcount() << endl);

	xdr_setpos( _source, 0 );
	if( !xdr_bytes( _source, val, &num, DODS_MAX_ARRAY) )
	    throw Error("Network I/O Error. Could not read byte array data.");
    }
}

void
XDRStreamUnMarshaller::get_vector( char **val, unsigned int &num, int width, Vector &vec )
{
    int i;
    get_int( i ) ; // This leaves the XDR encoded value in _buf; used later
    DBG(std::cerr << "i: " << i << std::endl);

    width += width&3;
    DBG(std::cerr << "width: " << width << std::endl);

    //char *buf = 0;
    //XDR *source = 0;
    int size = i * width; // + 4; // '+ 4' to hold the int already read
    BaseType *var = vec.var();

    // Must address the case where the string is larger than the buffer
    if (size > XDR_DAP_BUFF_SIZE) {
	char *buf = (char *) malloc( size + 4 );
	if (!buf)
		throw InternalErr(__FILE__, __LINE__, "Error allocating memory");
	XDR *source = new XDR;	
	xdrmem_create(source, buf, size + 4, XDR_DECODE);
	DBG2(cerr << "size: " << size << endl);
	memcpy(buf, _buf, 4);

	_in.read(buf + 4, size); // +4 for the int already read
	DBG2(cerr << "bytes read: " << _in.gcount() << endl);

	xdr_setpos( source, 0 );
	if (!xdr_array( source, val, &num, DODS_MAX_ARRAY, width,
		        XDRUtils::xdr_coder( var->type() ) ) ) {
	    delete_xdrstdio( source );
	    throw Error("Network I/O Error. Could not read array data.");
	}

	delete_xdrstdio(source);
    }
    else {
	_in.read(_buf + 4, size);
	DBG2(cerr << "bytes read: " << _in.gcount() << endl);

	xdr_setpos( _source, 0 );
	if (!xdr_array( _source, val, &num, DODS_MAX_ARRAY, width,
		        XDRUtils::xdr_coder( var->type() ) ) )
	    throw Error("Network I/O Error. Could not read array data.");
    }
}

void
XDRStreamUnMarshaller::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "XDRStreamUnMarshaller::dump - ("
         << (void *)this << ")" << endl ;
}

} // namespace libdap

