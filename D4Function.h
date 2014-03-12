// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2014 OPeNDAP, Inc.
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.


#ifndef D_FUNCTION_H_
#define D_FUNCTION_H_

namespace libdap {

class BaseType;
class D4RValueList;
class DMR;

// D4Function is a pointer to a function that takes a pointer to an RValueList and
// a reference to a DMR and returns a pointer to a BaseTYpe.
//
// I think this would be better as a 'function that takes...' instead of a 'pointer
// to a function that takes...' but I used this to make the code fit more closely to
// the pattern established by the DAP2 CE functions. jhrg 3/10/14

typedef BaseType* (*D4Function)(D4RValueList *, DMR &);

}// namespace libdap

#endif /* D_FUNCTION_H_ */
