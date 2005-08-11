
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
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for PassiveByte type. 
//
// jhrg 9/7/94

#ifndef _passivebyte_h
#define _passivebyte_h 1


#include "Byte.h"
#include "dods-datatypes.h"

/** This class is used to hold eight bits of information.  No sign
    information is implied in its value.

    @brief Holds a single byte.
    @see BaseType
    */
class PassiveByte: public Byte {
public:
    PassiveByte(const string &n = "");

    virtual ~PassiveByte();

    PassiveByte(const PassiveByte &copy_from);

    PassiveByte &operator=(const PassiveByte &rhs);

    virtual BaseType *ptr_duplicate();

    virtual bool read( const string &dataset ) ;

    virtual bool set_value( dods_byte val ) ;
};

/* 
 * $Log: PassiveByte.h,v $
 * Revision 1.1  2004/07/09 16:34:38  pwest
 * Adding Passive Data Model objects
 *
 */

#endif // _passivebyte_h

