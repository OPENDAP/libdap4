
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

#ifndef response_h
#define response_h

#include <stdio.h>

#ifndef _debug_h
#include "debug.h"
#endif

using namespace std;

/** Encapsulate a response. Instead of directly returning the FILE pointer
    from which a response is read, return an instance of this object. For a
    simple system where all that needs to be done to free the stream and its
    associated resources, this is overkill. However, some streams may require
    complex operations to free their resources once the client is done with
    the stream. Those classes should return a subclass of Response
    which has those operations built into the destructor. */
class Response {
private:
    /// The data stream
    FILE *d_stream;

    ObjectType d_type;
    string d_version;

protected:
    /** @name Suppressed default methods */
    //@{
    Response() {}
    Response(const Response &rs) {}
    Response &operator=(const Response &rhs) {
	throw InternalErr(__FILE__, __LINE__, "Unimplemented assignment");
    }
    //@}

public:
    /** Initialize with a stream. Create an instance initialized to a stream.
	by default the get_type() and get_version() return default values of
	unknown_type and "dods/0.0", respectively. Specializations (see
	HTTPResponse and HTTPConnect) may fill these fields in with other
	values. */
    Response(FILE *s) 
	: d_stream(s), d_type(unknown_type), d_version("dods/0.0") { }

    /** Close the stream. */
    virtual ~Response() { 
	DBG(cerr << "Closing stream... ");
	fclose(d_stream); 
	DBGN(cerr << endl);
    }

    /** @name Accessors */
    //@{
    virtual FILE *get_stream() const { return d_stream; }
    virtual ObjectType get_type() const { return d_type; }
    virtual string get_version() const { return d_version; }
    //@}

    /** @name Mutators */
    //@{
    virtual void set_type(ObjectType o) { d_type = o; }
    virtual void set_version(const string &v) { d_version = v; }
    //@}
};

// $Log: Response.h,v $
// Revision 1.2  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.1.2.1  2003/05/06 06:44:15  jimg
// Modified HTTPConnect so that the response headers are no longer a class
// member. This cleans up the class interface and paves the way for using
// the multi interface of libcurl. That'll have to wait for another day...
//
// Revision 1.1  2003/03/04 05:57:40  jimg
// Added.
//

#endif // response_h
