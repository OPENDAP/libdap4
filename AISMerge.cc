
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//         Dan Holloway <dan@hollywood.gso.uri.edu>
//         Reza Nekovei <reza@intcomm.net>
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

#include <stdio.h>

#include "AISMerge.h"
#include "AISExceptions.h"

/** Access an AIS resource. The resource may be a local file or a URL. Assume
    all resource URIs have no leading spaces.
    @param resource The AIS resource. 
    @return Either an open FILE * which points to the resource or null if the
    resource could not be opened. This method does not throw an exception for
    resources that cannot be opened because that can happen for a number of
    reasons which are hardly 'exceptional.' */
FILE *
AISMerge::get_ais_resource(const string &res) throw(Error, InternalErr)
{
    if (res.find("http:") == 0 || res.find("file:") == 0 
	|| res.find("https:") == 0 ) {
	return d_http.fetch_url(res);
    }
    else {
	return fopen(res.c_str(), "r");
    }
}

/** Access the AIS ancillary resources matched to <code>primary</code> and
    merge those with the DAS object <c>das</c>.
    @param primary The URL of the primary resource.
    @param das The target of the merge operation. This must already contain
    the DAS for <c>primary</c>. */
void
AISMerge::merge(const string &primary, DAS &das) throw(InternalErr)
{
    if (!d_ais_db.is_resource(primary))
	return;

    try {
	ResourceVector rv = d_ais_db.get_resource(primary);
	
	for (ResourceVectorIter i = rv.begin(); i != rv.end(); ++i) {
	    FILE *ais_resource = get_ais_resource(i->get_url());
	    switch (i->get_rule()) {
	      case overwrite:
		das.parse(ais_resource);
		break;
	      case replace:
		das.erase();
		das.parse(ais_resource);
		break;
	      case fallback:
		if (das.get_size() == 0)
		    das.parse(ais_resource);
		break;
	    }
	}
    }
    catch (NoSuchPrimaryResource &e) {
	throw InternalErr(string("I caught a 'NoSuchPrimaryResource' exception, it said:\n") + e.get_error_message() + string("\n"));
    }
}

// $Log: AISMerge.cc,v $
// Revision 1.1  2003/02/25 23:26:32  jimg
// Added.
//
