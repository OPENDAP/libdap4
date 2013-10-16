// XDRUtils.h

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

#ifndef S_XDRUtils_h
#define S_XDRUtils_h 1

#include <cstdio>
#include "xdr-datatypes.h"
#include "BaseType.h"

#define xdr_proc_t int *

//  These func's moved to xdrutil_ppc.* under the PPC as explained there
#ifdef __POWERPC__
extern "C" XDR *new_xdrstdio(FILE *stream, enum xdr_op xop);
extern "C" XDR *set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop);
extern "C" void delete_xdrstdio(XDR *xdr);
#else
XDR *new_xdrstdio(FILE *stream, enum xdr_op xop);
XDR *set_xdrstdio(XDR *xdr, FILE *stream, enum xdr_op xop);
void delete_xdrstdio(XDR *xdr);
#endif

extern "C" bool_t xdr_str(XDR *xdrs, string &buf);

namespace libdap
{

class XDRUtils
{
private:			XDRUtils() {}
public:
    // xdr_coder is used as an argument to xdr procedures that encode groups
    // of things (e.g., xdr_array()). Each leaf class's constructor must set
    // this.
    static xdrproc_t		xdr_coder( const Type &t ) ;
} ;

} // namespace libdap

#endif // S_XDRUtils_h

