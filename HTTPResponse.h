
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

#ifndef http_response_h
#define http_response_h

#include <stdio.h>

#ifndef response_h
#include "Response.h"
#endif

#ifndef _debug_h
#include "debug.h"
#endif

/** Encapsulate a response. Instead of directly returning the FILE pointer
    from which a response is read, return an instance of this object. */
class HTTPResponse : public Response {
private:
    string d_file;

protected:
    /** @name Suppressed default methods */
    //@{
    HTTPResponse() {}
    HTTPResponse(const HTTPResponse &rs) {}
    HTTPResponse &operator=(const HTTPResponse &rhs) {
	throw InternalErr(__FILE__, __LINE__, "Unimplemented assignment");
    }
    //@}

public:
    /** Build a Response object and include the name of the temp file
	that holds the HTTP response. When instance is deleted, remove this
	file. */
    HTTPResponse(FILE *s, const string &temp_file) 
	: Response(s), d_file(temp_file) {}

    /** Close the temporary file. When an instance is destroyed, close its
	assciated temporary file. */
    virtual ~HTTPResponse() {
	DBG(cerr << "Freeing HTTPConnect resources (" + d_file + ")... ");
	// Delete temp file
	unlink(d_file.c_str());
	DBGN(cerr << endl);
    }
};

// $Log: HTTPResponse.h,v $
// Revision 1.1  2003/03/04 05:57:40  jimg
// Added.
//

#endif // http_response_h
