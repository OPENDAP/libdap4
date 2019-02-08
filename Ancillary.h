// Ancillary.h

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//         Patrick West <pwest@opendap.org>
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
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>
//      pwest           Patrick West <pwest@opendap.org>

#ifndef S_Ancillary_h
#define S_Ancillary_h 1

#include <string>

#include "DAS.h"
#include "DDS.h"

namespace libdap
{

class Ancillary
{
public:
    static string	find_ancillary_file( const string &pathname,
					     const string &ext,
					     const string &dir,
					     const string &file ) ;

    static string	find_group_ancillary_file( const string &pathname,
						   const string &ext ) ;

    static void		read_ancillary_das( DAS &das,
					    const string &pathname,
					    const string &dir = "",
					    const string &file = "" ) ;

    static void		read_ancillary_dds( DDS &dds,
					    const string &pathname,
					    const string &dir = "",
					    const string &file = "" ) ;
} ;

} // namespace libdap

#endif // S_Ancillary_h

