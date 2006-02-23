
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
#include <iostream>
#include <vector>
#include <map>

#include "GNURegex.h"

#ifndef resource_h
#include "Resource.h"
#endif

#ifndef ais_exceptions_h
#include "AISExceptions.h"
#endif

using namespace std;

typedef vector<Resource> ResourceVector;
typedef ResourceVector::iterator ResourceVectorIter;
typedef ResourceVector::const_iterator ResourceVectorCIter;

/** Maintain a database of AIS resources. Groups of AIS resources are
    accessed using a primary resource.

    Note that read_database() takes filenames because the underlying XML
    parser library uses filenames. The write_database() method takes a
    filename to be symmetrical. 

    @todo Develop some tighter rules about regular expressions in the AIS
    database. How should they be applied relative to explicit entries?

    @brief Manage AIS resources. */
class AISResources {
private:
    typedef map<string, ResourceVector> ResourceMap;
    typedef ResourceMap::iterator ResourceMapIter;
    typedef ResourceMap::const_iterator ResourceMapCIter;
    
    typedef pair<string, ResourceVector> RVPair;
    typedef vector<RVPair> ResourceRegexps;
    typedef ResourceRegexps::iterator ResourceRegexpsIter;
    typedef ResourceRegexps::const_iterator ResourceRegexpsCIter;

    ResourceMap d_db;		// This holds the URL resources
    ResourceRegexps d_re;	// This holds the regular expression res.

    // Scan RegExps looking for a particular regular expression.
    struct FindRegexp : public binary_function<RVPair, string, bool> {
	string local_re;
	FindRegexp(const string &re) : local_re(re) {}
	bool operator()(const RVPair &p) { return p.first == local_re; }
    };

    // Scan RegExps looking for one that matches a URL.
    // *** Make this more efficient by storing the Regex objects in the
    // vector. 03/11/03 jhrg
    struct MatchRegexp : public binary_function<RVPair, string, bool> {
	string candidate;
	MatchRegexp(const string &url) : candidate(url) {}
	bool operator()(const RVPair &p) { 
	    Regex r(p.first.c_str());
	    return r.match(candidate.c_str(), candidate.length()) != -1;
	}
    };

    friend class AISResourcesTest; // unit tests access to private stuff
    friend ostream &operator<<(ostream &os, const AISResources &ais_res);

public:
    /** Build an empty instance. */
    AISResources() {}
    AISResources(const string &database) throw(AISDatabaseReadFailed);

    virtual ~AISResources() {}

    virtual void add_url_resource(const string &url, 
				  const Resource &ancillary);
    virtual void add_url_resource(const string &url, const ResourceVector &rv);

    virtual void add_regexp_resource(const string &regexp, 
				     const Resource &ancillary);
    virtual void add_regexp_resource(const string &regexp,
				     const ResourceVector &rv);

    virtual bool has_resource(const string &primary) const;

    virtual ResourceVector get_resource(const string &primary)
	throw(NoSuchPrimaryResource);

    virtual void read_database(const string &database) 
	throw(AISDatabaseReadFailed);
    
    virtual void write_database(const string &filename) 
	throw(AISDatabaseWriteFailed);
};

#endif // ais_resources_h
