// XDRStreamMarshaller.h

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

#ifndef I_XDRStreamMarshaller_h
#define I_XDRStreamMarshaller_h 1

#include <iostream>

#include <openssl/md5.h>

using std::ostream ;
using std::cout ;

#include "Marshaller.h"
#include "XDRUtils.h"

#if 0
#include <openssl/md5.h>

unsigned char *MD5(const unsigned char *d, unsigned long n,
                 unsigned char *md);

int MD5_Init(MD5_CTX *c);
int MD5_Update(MD5_CTX *c, const void *data,
                 unsigned long len);
int MD5_Final(unsigned char *md, MD5_CTX *c);
#endif

namespace libdap
{

/** @brief marshaller that knows how to marshal/serialize dap data objects
 * to a C++ iostream using XDR
 *
 * @note This class can now compute checksums for data that will be marshalled.
 * jhrg 1/18/2012
 */
class XDRStreamMarshaller : public Marshaller
{
private:
    static char *	_buf ;
    XDR *			_sink ;
    ostream &		_out ;
    MD5_CTX *       _md5 ;  // jhrg 1/18/12
    bool            _write_data ; // jhrg 1/27/12
    bool            _checksum_ctx_valid ;

    				XDRStreamMarshaller() ;
    				XDRStreamMarshaller( const XDRStreamMarshaller &m ) ;
    XDRStreamMarshaller &operator=( const XDRStreamMarshaller & ) ;

public:
    				/* XDRStreamMarshaller( ostream &out ) ; */
    				XDRStreamMarshaller( ostream &out, bool checksum = false, bool write_data = true) ;
    virtual			~XDRStreamMarshaller() ;

    virtual void    reset_checksum() ;
    virtual string  get_checksum() ;
    virtual void    checksum_update(const void *data, unsigned long len) ;

    virtual void	put_byte( dods_byte val ) ;

    virtual void	put_int16( dods_int16 val ) ;
    virtual void	put_int32( dods_int32 val ) ;

    virtual void	put_float32( dods_float32 val ) ;
    virtual void	put_float64( dods_float64 val ) ;

    virtual void	put_uint16( dods_uint16 val ) ;
    virtual void	put_uint32( dods_uint32 val ) ;

    virtual void	put_str( const string &val ) ;
    virtual void	put_url( const string &val ) ;

    virtual void	put_opaque( char *val, unsigned int len ) ;
    virtual void	put_int( int val ) ;

    virtual void	put_vector( char *val, int num, Vector &vec ) ;
    virtual void	put_vector( char *val, int num, int width, Vector &vec ) ;

    virtual void	dump(ostream &strm) const ;
} ;

} // namespace libdap

#endif // I_XDRStreamMarshaller_h

