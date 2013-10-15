// XDRUtils.cc

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

#include "XDRUtils.h"
#include "debug.h"
#include "Str.h"

using namespace libdap ;

// This function is used to allocate memory for, and initialize, a new XDR
// pointer. It sets the stream associated with the (XDR *) to STREAM.
//
// NB: STREAM is not one of the C++/libg++ iostream classes; it is a (FILE
// *).

//  These func's moved to xdrutil_ppc.* under the PPC as explained there
#ifndef __POWERPC__
XDR *
new_xdrstdio(FILE *stream, enum xdr_op xop)
{
    XDR *xdr = new XDR;

    xdrstdio_create(xdr, stream, xop);

    return xdr;
}

XDR *
set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop)
{
    xdrstdio_create(xdr, stream, xop);

    return xdr;
}

// Delete an XDR pointer allocated using the above function. Do not close the
// associated FILE pointer.

void
delete_xdrstdio(XDR *xdr)
{
    xdr_destroy(xdr);

    delete xdr; xdr = 0;
}
#endif

// This function is used to en/decode Str and Url type variables. It is
// defined as extern C since it is passed via function pointers to routines
// in the xdr library where it is executed. This function is defined so
// that Str and Url have an en/decoder which takes exactly two arguments: an
// XDR * and a string reference.
//
// NB: this function is *not* used for arrays (i.e., it is not the function
// referenced by BaseType's _xdr_coder field when the object is a Str or Url.
// Also note that \e max_str_len is an obese number but that really does not
// matter; xdr_string() would never actually allocate that much memory unless
// a string that size was sent from the server.
// Returns: XDR's bool_t; TRUE if no errors are detected, FALSE
// otherwise. The formal parameter BUF is modified as a side effect.

extern "C" bool_t
xdr_str(XDR *xdrs, string &buf)
{
    DBG(cerr << "In xdr_str, xdrs: " << xdrs << endl);

    switch (xdrs->x_op) {
    case XDR_ENCODE: { // BUF is a pointer to a (string *)
            const char *out_tmp = buf.c_str();

            return xdr_string(xdrs, (char **)&out_tmp, max_str_len);
        }

    case XDR_DECODE: {
            char *in_tmp = NULL;

            bool_t stat = xdr_string(xdrs, &in_tmp, max_str_len);
            if (!stat)
                return stat;

            buf = in_tmp;

            free(in_tmp);

            return stat;
        }

    default:
        return 0;
    }
}

namespace libdap {

/** The <tt>xdr_coder</tt> function (also "filter primitive") is used to
    encode and decode each element in a multiple element data
    structure.  These functions are used to convert data to and from
    its local representation to the XDR representation, which is
    used to transmit and receive the data.  See <tt>man xdr</tt> for more
    information about the available XDR filter primitives.

    \note This class data is only used for multiple element data
    types.  The simple data types (Int, Float, and so on), are
    translated directly.

    \note Even though Byte is a cardinal type, xdr_char is <i>not</i>
    used to transport Byte arrays over the network. Instead, Byte is
    a special case handled in Array.

    @brief Returns a function used to encode elements of an array.
    @return A C function used to encode data in the XDR format.
*/
xdrproc_t
XDRUtils::xdr_coder( const Type &t )
{
    switch( t )
    {
	case dods_int16_c:
	    return (xdrproc_t)XDR_INT16 ;
	    break ;
	case dods_uint16_c:
	    return (xdrproc_t)XDR_UINT16 ;
	    break ;
	case dods_int32_c:
	    return (xdrproc_t)XDR_INT32 ;
	    break ;
	case dods_uint32_c:
	    return (xdrproc_t)XDR_UINT32 ;
	    break ;
	case dods_float32_c:
	    return (xdrproc_t)XDR_FLOAT32 ;
	    break ;
	case dods_float64_c:
	    return (xdrproc_t)XDR_FLOAT64 ;
	    break ;
	case dods_byte_c:
	case dods_str_c:
	case dods_url_c:
	case dods_array_c:
	case dods_structure_c:
	case dods_sequence_c:
	case dods_grid_c:
	default:
	    return NULL ;
	    break ;
    }
}

} // namespace libdap

