
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
 
// This is a dummy server which currently assumes a data request (as opposed
// to a das or dds request). An executable is built using the Test* classes
// so this will return data values. All output is dumped to stdout except for
// instrumentation, which is sent to stderr. This is new in 3.4. When we move
// on to 3.5, this should be edited so that it is a 'real' handler, using the
// response information in DODSFilter. 07/21/03 jhrg

#include "config_dap.h"

static char not_used rcsid[]={"$Id$"};

#include <iostream>
#include <string>

#include "DODSFilter.h"
#include "DDS.h"
#include "DAS.h"
#if 0
#include "DataDDS.h"
#endif

#include "ObjectType.h"
#include "debug.h"
#include "cgi_util.h"

using namespace std;

int test_variable_sleep_interval = 0;
const string cgi_version = "DODS/3.4, server_handler"; //DODS_SERVER_VERSION;

static void
read_table(DDS &table, const string &name) throw(Error)
{
    table.parse(name);
    
    if (!table.check_semantics(true))
	throw Error("DDS Failed semantic check.");
}

int 
main(int argc, char *argv[])
{
    try { 
	DODSFilter df(argc, argv);
	if (df.get_cgi_version() == "")
	    df.set_cgi_version(cgi_version);

	DDS dds;
	DAS das;

	dds.filename(df.get_dataset_name());
	read_table(dds, df.get_dataset_name());
	
	// Don't name test dataset DDS file XXX.dds...
	df.read_ancillary_dds(dds);
	df.read_ancillary_das(das);

	DBG(dds.print(stderr));
	// DBG(das.print(stderr));

	// Extract params. This provides a way to configure 'datasets' for
	// special tests. It's pretty crude. Maybe it would be better to use
	// command line options?...
	AttrTable *sh_attr = das.get_table("server_handler");
	if (sh_attr) {
	    int timeout = atoi(sh_attr->get_attr("timeout").c_str());
	    int sleep_interval = atoi(sh_attr->get_attr("sleep").c_str());

	    df.set_timeout(timeout);
	    // This global causes the read() methods of the Test* classes to
	    // sleep for sleep_interval seconds, simulating a long, complex
	    // variable access operation.
	    test_variable_sleep_interval = sleep_interval;
	}

	df.send_data(dds, stdout);
    }
    catch (Error &e) {
	set_mime_text(cout, dods_error, cgi_version);
	e.print(cout);
	return 1;
    }

    return 0;
}

// $Log: server_handler.cc,v $
// Revision 1.2  2003/12/08 18:02:31  edavis
// Merge release-3-4 into trunk
//
// Revision 1.1.2.2  2003/07/25 06:04:28  jimg
// Refactored the code so that DDS:send() is now incorporated into
// DODSFilter::send_data(). The old DDS::send() is still there but is
// depracated.
// Added 'smart timeouts' to all the variable classes. This means that
// the new server timeouts are active only for the data read and CE
// evaluation. This went inthe BaseType::serialize() methods because it
// needed to time both the read() calls and the dds::eval() calls.
//
// Revision 1.1.2.1  2003/07/24 00:42:34  jimg
// Added
//
// Revision 1.1  2003/05/13 22:03:34  jimg
// Created. This works with newly modified DODS_Dispatch.pm script and
// DODSFilter class.
//
