
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
 
#include "config_dap.h"

#include "AISConnect.h"

/** Build an AISConnect. This calls Connect's constructor with \c name and
    accepts its default values for the other parameters. The AIS database to
    use is read from the configuration file <code>~/.dodsrc</code>.
    
    @param name Create a virtual, AIS-enhanced, connection for this OPeNDAP
    data source.
    @exception AISDatabaseReadFailed Thrown if the AIS database listed in the
    <code>~/.dodsrc</code> file could not be opened. */
AISConnect::AISConnect(const string &name) throw(AISDatabaseReadFailed)
    : Connect(name), d_ais_merge(0)
{
    const string &ais_db = RCReader::instance()->get_ais_database();
    if (ais_db != "")
	d_ais_merge = new AISMerge(ais_db);
}

/** Build an AISConnect. This calls Connect's constructor with \c name and
    accepts its default values for the other parameters. The AIS is
    initialized to use the database named by \c ais.
    
    @param name Create a virtual, AIS-enhanced, connection for this OPeNDAP
    data source.
    @param ais Read the AIS information from this XML file.
    @exception AISDatabaseReadFailed Thrown if the AIS database listed in the
    <code>~/.dodsrc</code> file could not be opened. */
AISConnect::AISConnect(const string &name, const string &ais) 
    throw(AISDatabaseReadFailed)
    : Connect(name), d_ais_merge(0)
{
    d_ais_merge = new AISMerge(ais);
}

/** Destroy an AISConnect. */
AISConnect::~AISConnect()
{
    delete d_ais_merge; d_ais_merge = 0;
}

/** Request an AIS-enhanced DAS object. Read the DAS object for this virtual
    connection's data source. Then check the AIS database and merge in any
    ancillary resources listed for it.

    @param das Deposit information to this object.
    @exception Error Thrown if the DAS request fails due to user error.
    @exception InternalErr Thrown if either the DAS request or the AIS merge
    request fails. */
void
AISConnect::request_das(DAS &das) throw(Error, InternalErr)
{
    Connect::request_das(das);

    if (d_ais_merge)
	d_ais_merge->merge(URL(false), das); // URL(false) --> URL w/o CE
}

// $Log: AISConnect.cc,v $
// Revision 1.5  2004/02/19 19:42:51  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.3.2.2  2004/02/11 22:26:45  jimg
// Changed all calls to delete so that whenever we use 'delete x' or
// 'delete[] x' the code also sets 'x' to null. This ensures that if a
// pointer is deleted more than once (e.g., when an exception is thrown,
// the method that throws may clean up and then the catching method may
// also clean up) the second, ..., call to delete gets a null pointer
// instead of one that points to already deleted memory.
//
// Revision 1.4  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.3.2.1  2003/09/06 22:16:26  jimg
// Updated the documentation.
//
// Revision 1.3  2003/03/12 01:07:34  jimg
// Added regular expressions to the AIS subsystem. In an AIS database (XML)
// it is now possible to list a regular expression in place of an explicit
// URL. The AIS will try to match this Regexp against candidate URLs and
// return the ancillary resources for all those that succeed.
//
// Revision 1.2  2003/03/11 10:16:19  jimg
// if there's no AIS database, the field d_ais_merge is null. Make sure not to
// dereference that pointer in that case!
//
// Revision 1.1  2003/02/27 23:17:00  jimg
// Added.
//
