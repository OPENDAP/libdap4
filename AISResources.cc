
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

#include "config_dap.h"

#include <iostream>
#include <fstream>

#include "AISResources.h"
#include "AISDatabaseParser.h"

/** Output the XML fragment for a Resource. This function is a friend of the
    Resource class. 
    @see Resource. */
ostream &
operator<<(ostream &os, const Resource &r)
{
    os << "<ancillary";
    if (r.d_rule != overwrite) {
	os << " rule=\"";
	(r.d_rule == fallback) ? os << "fallback\"": os << "replace\"";
    }
    os << " url=\"" << r.d_url << "\"/>";

    return os;
}

/** Output the XML for a collection of AIS resources. This function is a
    friend of the AISResource class.
    @see AISResources */
ostream &
operator<<(ostream &os, const AISResources &ais_res)
{
    os << "<?xml version=\"1.0\" encoding=\"US-ASCII\" standalone=\"yes\"?>"
       << endl;
    os << "<!DOCTYPE ais SYSTEM \"http://www.opendap.org/ais/ais_database.dtd\">" << endl;
    os << "<ais xmlns=\"http://xml.opendap.org/ais\">" << endl;

    for (AISResources::ResourceMapCIter pos = ais_res.d_db.begin(); 
	 pos != ais_res.d_db.end(); ++pos) {
	os << "<entry>" << endl;
	// write primary
	os << "<primary url=\"" << pos->first << "\"/>" << endl;
	// write the vector of Resource objects
	for (ResourceVectorCIter i = pos->second.begin(); 
	     i != pos->second.end(); ++i) {
	    os << *i << endl;
	}
	os << "</entry>" << endl;
    }

    os << "</ais>" << endl;

    return os;
}

/** Use an existing AIS database to build an instance. 
    @param database Pathname of the database/document. */
AISResources::AISResources(const string &database) throw(AISDatabaseReadFailed)
{
    read_database(database);
}

/** Add the given ancillary resource to the in-memory collection of
    mappings between primary and ancillary data sources. 
    @param primary The target of the new mapping.
    @param Match this ancillary resource to the target (primary). */
void 
AISResources::add_resource(const string &primary, const Resource &ancillary)
{
    ResourceMapIter pos = d_db.find(primary);
    if (pos == d_db.end()) {
	// no entry for primary yet, make a ResourceVector
	ResourceVector rv(1, ancillary);
	d_db.insert(std::make_pair(primary, rv));
    }
    else {
	// There's already a ResourceVector, append to it.
	pos->second.push_back(ancillary);
    }
}

/** Add a vector of AIS resources for the given primary data source URL. If
    there is already an entry for the primary, append the new ancillary
    resources to those.
    @param primary The target of the new mapping.
    @param Ancillary resources matched to this primary resource. */
void 
AISResources::add_resource(const string &primary, const ResourceVector &rv)
{
    ResourceMapIter pos = d_db.find(primary);
    if (pos == d_db.end()) {
	d_db.insert(std::make_pair(primary, rv));
    }
    else {
	// There's already a ResourceVector, append to it.
	for (ResourceVectorCIter i = rv.begin(); i != rv.end(); ++i)
	    pos->second.push_back(*i);
    }
}

/** Return True if the given primary resource is listed in the current
    set of AIS resource mappings. That is, return true if there are some
    AIS resources registered for the given primary resource.
    @param primary The URL of the primary resource. 
    @return True if there are AIS resources for <code>primary</code>. */
bool 
AISResources::has_resource(const string &primary)
{
    return d_db.find(primary) != d_db.end();
}

/** Return a vector of AIS Resource objects which are bound to the given
    primary resource. 
    @param primary The URL of the primary resource
    @return a vector of Resource objects.
    @exception NoSuchPrimaryResource thrown if <code>primary</code> is
    not present in the current mapping. */
ResourceVector
AISResources::get_resource(const string &primary) 
    throw(NoSuchPrimaryResource)
{
    const ResourceMapIter &iter = d_db.find(primary);

    if (iter != d_db.end())
	return iter->second;
    else
	throw NoSuchPrimaryResource();
}

/** Read the AIS database (an XML 'configuration file') and internalize it.
    @param database A file/pathname to the AIS database.
    @exception AISDatabaseReadFailed thrown if the database could not be
    read. */
void 
AISResources::read_database(const string &database) 
    throw(AISDatabaseReadFailed)
{
    AISDatabaseParser parser;

    parser.intern(database, this);
}

/** Write the current in-memory mapping of primary and ancillary
    resources to the named file so that the read_database() method can
    read them and recreate the in-memory mapping.
    @param filename A local file; write the database to this file. Create
    if necessary.
    @exception AISDatabaseWriteFailed thrown if the database could not be
    written. */
void 
AISResources::write_database(const string &filename) 
    throw(AISDatabaseWriteFailed)
{
    ofstream fos;
    fos.open(filename.c_str());

    if (!fos)
	throw AISDatabaseWriteFailed("Could not open file :" + filename);

    fos << *this << endl;
    
    if (!fos)
	throw AISDatabaseWriteFailed();
}

// $Log: AISResources.cc,v $
// Revision 1.4  2003/02/26 06:35:48  jimg
// Added iostream header. Moved the file open out of the constructor; this
// seemed to be throwing an exception when the file did not exist, even though
// that makes no sense...
//
// Revision 1.3  2003/02/26 01:27:04  jimg
// Fixed call to AISDatabaseParser::intern (changed the name from parse).
//
// Revision 1.2  2003/02/26 00:39:39  jimg
// Changed name of is_resource to has_resource.
//
// Revision 1.1  2003/02/20 22:15:01  jimg
// Added.
//
