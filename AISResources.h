
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

#ifndef ais_resources_h
#define ais_resources_h

#include <string>
#include <vector>
#include <map>

#ifndef resource_rule_h
#include "ResourceRule.h"
#endif 

#ifndef resource_h
#include "Resource.h"
#endif

#ifndef ais_exceptions_h
#include "AISExceptions.h"
#endif

using std::string;
using std::vector;
using std::map;

typedef vector<Resource> ResourceVector;
typedef ResourceVector::iterator ResourceVectorIter;
typedef ResourceVector::const_iterator ResourceVectorCIter;

/** Maintain a database of AIS resources. Groups of AIS resources are
    accessed using a primary resource.

    Note that read_database() takes filenames because the underlying XML
    parser library uses filenames. The write_database() method takes a
    filename to be symmetrical. 

    @brief Manage AIS resources. */
class AISResources {
private:
    friend class AISResourcesTest; // unit tests access to private stuff
    friend ostream &operator<<(ostream &os, const AISResources &ais_res);

    typedef map<string, ResourceVector> ResourceMap;
    typedef ResourceMap::iterator ResourceMapIter;
    typedef ResourceMap::const_iterator ResourceMapCIter;
    
    ResourceMap d_db;

public:
    /** Build an empty instance. */
    AISResources() {}
    AISResources(const string &database) throw(AISDatabaseReadFailed);

    virtual ~AISResources() {}

    virtual void add_resource(const string &primary, 
			      const Resource &ancillary);
    virtual void add_resource(const string &primary, const ResourceVector &rv);

    virtual bool has_resource(const string &primary);

    virtual ResourceVector get_resource(const string &primary) 
	throw(NoSuchPrimaryResource);

    virtual void read_database(const string &database) 
	throw(AISDatabaseReadFailed);
    
    virtual void write_database(const string &filename) 
	throw(AISDatabaseWriteFailed);
};

// $Log: AISResources.h,v $
// Revision 1.3  2003/02/26 00:40:38  jimg
// Changed name of is_resource to has_resource.
//
// Revision 1.2  2003/02/20 22:19:02  jimg
// Added throw() specification to second constructor.
//
// Revision 1.1  2003/02/20 22:10:21  jimg
// Added.
//

#endif // ais_resources_h
