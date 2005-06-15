
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
    AISConnect(const AISConnect &ac) {}
    //@}

public:
    AISConnect(const string &name) throw(AISDatabaseReadFailed);
    AISConnect(const string &name, const string &ais_db) 
	throw(AISDatabaseReadFailed);

    virtual ~AISConnect();

    virtual void request_das(DAS &das) throw(Error, InternalErr);
};

// $Log: AISConnect.h,v $
// Revision 1.3  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.2  2003/03/12 01:07:34  jimg
// Added regular expressions to the AIS subsystem. In an AIS database (XML)
// it is now possible to list a regular expression in place of an explicit
// URL. The AIS will try to match this Regexp against candidate URLs and
// return the ancillary resources for all those that succeed.
//
// Revision 1.1  2003/02/28 06:54:17  jimg
// Added
//

#endif // ais_connect_h 
