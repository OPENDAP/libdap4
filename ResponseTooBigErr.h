
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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
 
#ifndef _response_too_big_err_h
#define _response_too_big_err_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <string>

#ifndef _error_h
#include "Error.h"
#endif


/** ResponseTooBigErr is thrown by HTTPCache::parse_header and write_body if
    the response that's being cached is too big. The HTTPCache methods
    determine just what 'too big' means.

    @author jhrg */

class ResponseTooBigErr: public Error {

public:
    ResponseTooBigErr(const string &msg);
    ResponseTooBigErr();
};

// $Log: ResponseTooBigErr.h,v $
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.1  2002/10/08 05:30:28  jimg
// Added.
//

#endif // _response_too_big_err_h
