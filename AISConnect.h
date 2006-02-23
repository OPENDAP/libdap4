
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
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

#ifndef ais_connect_h
#define ais_connect_h

#ifndef _connect_h
#include "Connect.h"
#endif

#ifndef ais_merge_h
#include "AISMerge.h"
#endif

#ifndef ais_exceptions_h
#include "AISExceptions.h"
#endif

/** Provide seemless client-side AIS services. */
class AISConnect: public Connect {
private:
    AISMerge *d_ais_merge;

protected:
    /** @name Suppress the C++ defaults for these. */
    //@{
    AISConnect() {}
    AISConnect(const AISConnect &ac) : Connect(ac) {}
    //@}

public:
    AISConnect(const string &name) throw(AISDatabaseReadFailed);
    AISConnect(const string &name, const string &ais_db) 
	throw(AISDatabaseReadFailed);

    virtual ~AISConnect();

    virtual void request_das(DAS &das) throw(Error, InternalErr);
};

#endif // ais_connect_h 
