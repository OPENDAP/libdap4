// XDRStreamMarshaller.cc

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

#include "config.h"

#include "XDRStreamMarshaller.h"

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

//#define DODS_DEBUG 1

#include "Vector.h"
#include "util.h"
#include "debug.h"

namespace libdap {

char *XDRStreamMarshaller::d_buf = 0;

#define XDR_DAP_BUFF_SIZE 256

/** Build an instance of XDRStreamMarshaller. Bind the C++ stream out to this
 * instance. If the checksum parameter is true, initialize a checksum buffer
 * and enable the use of the reset_checksum() and get_checksum() methods.
 *
 * @param out Write to this stream object.
 * @param checksum If true, compute checksums. False by default
 * @param write_data If true, write data values. True by default
 */
XDRStreamMarshaller::XDRStreamMarshaller(ostream &out) : // , bool checksum, bool write_data) :
    /*&d_sink(0),*/ d_out(out)//, _MD_CTX(0), _write_data(write_data), _checksum_ctx_valid(false)
{
    if (!d_buf)
        d_buf = (char *) malloc(XDR_DAP_BUFF_SIZE);
    if (!d_buf)
        throw Error("Failed to allocate memory for data serialization.");

    //&d_sink = new XDR;
    xdrmem_create( &d_sink, d_buf, XDR_DAP_BUFF_SIZE, XDR_ENCODE);

#if CHECKSUMS
    if (checksum) {
        _MD_CTX = EVP_MD_CTX_create();
    }
#endif
}

XDRStreamMarshaller::XDRStreamMarshaller() :
    Marshaller(), /*&d_sink(0),*/ d_out(cout)
{
    throw InternalErr(__FILE__, __LINE__, "Default constructor not implemented.");
}

XDRStreamMarshaller::XDRStreamMarshaller(const XDRStreamMarshaller &m) :
    Marshaller(m), /*&d_sink(0),*/ d_out(cout)
{
    throw InternalErr(__FILE__, __LINE__, "Copy constructor not implemented.");
}

XDRStreamMarshaller &
XDRStreamMarshaller::operator=(const XDRStreamMarshaller &)
{
    throw InternalErr(__FILE__, __LINE__, "Copy operator not implemented.");

    return *this;
}

XDRStreamMarshaller::~XDRStreamMarshaller()
{
    xdr_destroy(&d_sink); //delete_xdrstdio(&d_sink);
    //delete(&d_sink);
    //&d_sink = 0;

#if CHECKSUMS
    if (_MD_CTX)
        EVP_MD_CTX_destroy(_MD_CTX);
#endif
}

#if 0
/** Initialize the checksum buffer. This resets the checksum calculation.
 * @exception InternalErr if called when the object was created without
 * checksum support.
 */
void XDRStreamMarshaller::reset_checksum()
{
#if CHECKSUMS
    if (_MD_CTX == 0)
        throw InternalErr( __FILE__, __LINE__, "reset_checksum() called by checksum is not enabled.");

    if (EVP_DigestInit_ex(_MD_CTX, EVP_sha1(), 0) == 0)
        throw Error("Failed to initialize checksum object.");

    _checksum_ctx_valid = true;
#endif
}

/** Get the current checksum. It is not possible to continue computing the
 * checksum once this has been called.
 * @exception InternalErr if called when the object was created without
 * checksum support or if called when the checksum has already been returned.
 */
string XDRStreamMarshaller::get_checksum()
{
#if CHECKSUMS
    if (_MD_CTX == 0)
        throw InternalErr(__FILE__, __LINE__, "checksum_init() called by checksum is not enabled.");

    if (_checksum_ctx_valid) {
        // '...Final()' 'erases' the context so the next call without a reset
        // returns a bogus value.
        _checksum_ctx_valid = false;

        vector<unsigned char> md(EVP_MAX_MD_SIZE);
        unsigned int md_len;
        if (EVP_DigestFinal_ex(_MD_CTX, &md[0], &md_len) == 0)
            throw Error("Error computing the checksum (checksum computation).");

        ostringstream oss;
        oss.setf(ios::hex, ios::basefield);
        for (unsigned int i = 0; i < md_len; ++i) {
            oss << setfill('0') << setw(2) << (unsigned int) md[i];
        }

        _checksum = oss.str();
    }

    return _checksum;
#else
    return "";
#endif
}

void XDRStreamMarshaller::checksum_update(const void *data, unsigned long len)
{
#if CHECKSUMS
    if (_MD_CTX == 0)
        throw InternalErr( __FILE__, __LINE__, "checksum_init() called by checksum is not enabled.");

    if (!_checksum_ctx_valid)
        throw InternalErr( __FILE__, __LINE__, "Invalid checksum context (checksum update).");

    if (EVP_DigestUpdate(_MD_CTX, data, len) == 0) {
        _checksum_ctx_valid = false;
        throw Error("Error computing the checksum (checksum update).");
    }
#endif
}
#endif

void XDRStreamMarshaller::put_byte(dods_byte val)
{
#if CHECKSUM
    if (_MD_CTX)
        checksum_update(&val, sizeof(dods_byte));
#endif
//    if (_write_data) {
    DBG( std::cerr << "put_byte: " << val << std::endl );

    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!xdr_char(&d_sink, (char *) &val))
        throw Error("Network I/O Error. Could not send byte data.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_int16(dods_int16 val)
{
#if 0
    if (_MD_CTX)
        checksum_update(&val, sizeof(dods_int16));
#endif
//    if (_write_data) {
    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send int 16 data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!XDR_INT16(&d_sink, &val))
        throw Error("Network I/O Error. Could not send int 16 data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send int 16 data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_int32(dods_int32 val)
{
#if 0
    if (_MD_CTX)
        checksum_update(&val, sizeof(dods_int32));
#endif
//    if (_write_data) {
    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send int 32 data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!XDR_INT32(&d_sink, &val))
        throw Error("Network I/O Error. Culd not read int 32 data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send int 32 data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_float32(dods_float32 val)
{
#if 0
    if (_MD_CTX)
        checksum_update(&val, sizeof(dods_float32));
#endif
//    if (_write_data) {
    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send float 32 data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!xdr_float(&d_sink, &val))
        throw Error("Network I/O Error. Could not send float 32 data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send float 32 data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_float64(dods_float64 val)
{
#if 0
    if (_MD_CTX)
        checksum_update(&val, sizeof(dods_float64));
#endif
//    if (_write_data) {
    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send float 64 data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!xdr_double(&d_sink, &val))
        throw Error("Network I/O Error. Could not send float 64 data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send float 64 data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_uint16(dods_uint16 val)
{
#if 0
    if (_MD_CTX)
        checksum_update(&val, sizeof(dods_uint16));
#endif
//    if (_write_data) {
    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send uint 16 data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!XDR_UINT16(&d_sink, &val))
        throw Error("Network I/O Error. Could not send uint 16 data. This may be due to a\nbug in libdap or a problem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send uint 16 data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_uint32(dods_uint32 val)
{
#if 0
    if (_MD_CTX)
        checksum_update(&val, sizeof(dods_uint32));
#endif
//    if (_write_data) {
    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send uint 32 data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!XDR_UINT32(&d_sink, &val))
        throw Error("Network I/O Error. Could not send uint 32 data. This may be due to a\nbug in libdap or a problem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send uint 32 data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_str(const string &val)
{
#if 0
	if (_MD_CTX)
	checksum_update(val.c_str(), val.length());
#endif
	int size = val.length() + 8;
#if 0
	char *str_buf = (char *) malloc(size);

	if (!str_buf) {
		throw Error("Failed to allocate memory for string data serialization.");
	}
#endif

	XDR str_sink;
	vector<char> str_buf(size);
	//XDR *str_sink = new XDR;
	try {
		xdrmem_create(&str_sink, &str_buf[0], size, XDR_ENCODE);

		if (!xdr_setpos( &str_sink, 0 ))
			throw Error(
					"Network I/O Error. Could not send string data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

		const char *out_tmp = val.c_str();
		if (!xdr_string(&str_sink, (char **) &out_tmp, size))
			throw Error(
					"Network I/O Error. Could not send string data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");

		unsigned int bytes_written = xdr_getpos( &str_sink );
		if (!bytes_written)
			throw Error(
					"Network I/O Error. Could not send string data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

		d_out.write(&str_buf[0], bytes_written);

		xdr_destroy(&str_sink);
	}
	catch (...) {
		xdr_destroy(&str_sink);
		throw;
	}
}

void XDRStreamMarshaller::put_url(const string &val)
{
    put_str(val);
}

void XDRStreamMarshaller::put_opaque(char *val, unsigned int len)
{
#if 0
    if (_MD_CTX)
        checksum_update(&val, len);
#endif
//    if (_write_data) {
    if (len > XDR_DAP_BUFF_SIZE)
        throw Error("Network I/O Error. Could not send opaque data - length of opaque data larger than allowed");

    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send opaque data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!xdr_opaque(&d_sink, val, len))
        throw Error("Network I/O Error. Could not send opaque data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send opaque data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_int( int val )
{
#if 0
    if (_MD_CTX)
        checksum_update(&val, sizeof(int));
#endif
//    if (_write_data) {
    if (!xdr_setpos( &d_sink, 0 ))
        throw Error("Network I/O Error. Could not send int data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if (!xdr_int(&d_sink, &val))
        throw Error("Network I/O Error(1). Could not send int data.\nThis may be due to a bug in libdap or a\nproblem with the network connection.");

    unsigned int bytes_written = xdr_getpos( &d_sink );
    if (!bytes_written)
        throw Error("Network I/O Error. Could not send int data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    d_out.write(d_buf, bytes_written);
//    }
}

void XDRStreamMarshaller::put_vector(char *val, int num, Vector &)
{
	if (!val) throw InternalErr(__FILE__, __LINE__, "Could not send byte vector data. Buffer pointer is not set.");
#if 0
	if (_MD_CTX)
	checksum_update(val, num);
#endif
	put_int(num);

	// this is the word boundary for writing xdr bytes in a vector.
	const unsigned int add_to = 8;
#if 0
	char *byte_buf = (char *) malloc(num + add_to);
	if (!byte_buf) throw Error("Failed to allocate memory for byte vector data serialization.");
#endif
	vector<char> byte_buf(num + add_to);
	XDR byte_sink;
	try {
		xdrmem_create(&byte_sink, &byte_buf[0], num + add_to, XDR_ENCODE);
		if (!xdr_setpos( &byte_sink, 0 ))
			throw Error(
					"Network I/O Error. Could not send byte vector data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

		if (!xdr_bytes(&byte_sink, (char **) &val, (unsigned int *) &num, num + add_to))
			throw Error(
					"Network I/O Error(2). Could not send byte vector data.\nThis may be due to a bug in libdap or a\nproblem with the network connection.");

		unsigned int bytes_written = xdr_getpos( &byte_sink );
		if (!bytes_written)
			throw Error(
					"Network I/O Error. Could not send byte vector data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

		d_out.write(&byte_buf[0], bytes_written);

		xdr_destroy(&byte_sink);
	}
	catch (...) {
		xdr_destroy(&byte_sink);
		throw;
	}
}

void
XDRStreamMarshaller::put_vector( char *val, int num, int width, Vector &vec )
{
    put_vector(val, num, width, vec.var()->type());
}


void XDRStreamMarshaller::put_vector(char *val, unsigned int num, int width, Type type)
{
	if (!val) throw InternalErr(__FILE__, __LINE__, "Buffer pointer is not set.");
#if CHECKSUM
	if (_MD_CTX)
	checksum_update(val, num * width);
#endif
	put_int(num);

	int use_width = width;
	if (use_width < 4) use_width = 4;

	// the size is the number of elements num times the width of each
	// element, then add 4 bytes for the number of elements
	int size = (num * use_width) + 4;

	// allocate enough memory for the elements
#if 0
	char *vec_buf = (char *) malloc(size);
	if (!vec_buf)
	throw Error("Failed to allocate memory for vector data serialization.");
#endif
	vector<char> vec_buf(size);
	XDR vec_sink;
	try {
		xdrmem_create(&vec_sink, &vec_buf[0], size, XDR_ENCODE);

		// set the position of the sink to 0, we're starting at the beginning
		if (!xdr_setpos( &vec_sink, 0 ))
			throw Error(
					"Network I/O Error. Could not send vector data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

		// write the array to the buffer
		if (!xdr_array(&vec_sink, (char **) &val, (unsigned int *) &num, size, width, XDRUtils::xdr_coder(type)))
			throw Error(
					"Network I/O Error(2). Could not send vector data.\nThis may be due to a bug in libdap or a\nproblem with the network connection.");

		// how much was written to the buffer
		unsigned int bytes_written = xdr_getpos( &vec_sink );
		if (!bytes_written)
			throw Error(
					"Network I/O Error. Could not send vector data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

		// write that much out to the output stream
		d_out.write(&vec_buf[0], bytes_written);

		xdr_destroy(&vec_sink);
	}
	catch (...) {
		xdr_destroy(&vec_sink);
		throw;
	}
}

void XDRStreamMarshaller::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "XDRStreamMarshaller::dump - (" << (void *) this << ")" << endl;
}

} // namespace libdap

