
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
 
// Tests for the AISMerge class.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "Connect.h"
#include "AISMerge.h"
#include "debug.h"

using namespace CppUnit;

class AISMergeTest:public TestFixture {
private:
    AISMerge *ais_merge;

    static string fnoc1, fnoc2, fnoc3, bears, coads, three_fnoc;
    static string fnoc1_ais, fnoc2_ais, digit_ais, fnoc3_das;

    static string fnoc1_ais_string, bears_1_ais_string, coads_ais_string;
    static string fnoc1_merge_ais, fnoc2_merge_ais, fnoc3_merge_ais;
    static string three_fnoc_merge_ais, starts_with_number_ais_string;

    string dump2string(FILE *res) {
	string stuff = "";
	char line[256];
	while(!feof(res) && !ferror(res) && fgets(&line[0], 256, res) != 0)
	    stuff += line;

	return stuff;
    }

public:
    AISMergeTest() {} 
    ~AISMergeTest() {} 

    void setUp() {
	ais_merge = new AISMerge("ais_testsuite/ais_database.xml");
    } 

    void tearDown() { 
	delete ais_merge;
    }

    CPPUNIT_TEST_SUITE( AISMergeTest );

    CPPUNIT_TEST(get_ais_resource_test);
    CPPUNIT_TEST(merge_test);

    CPPUNIT_TEST_SUITE_END();

    void get_ais_resource_test() {
	try {
	    ResourceVector rv = ais_merge->d_ais_db.get_resource(fnoc1);
	    Response *res = ais_merge->get_ais_resource(rv[0].get_url());
	    CPPUNIT_ASSERT(dump2string(res->get_stream()).find(fnoc1_ais_string) 
			   != string::npos);

	    rv = ais_merge->d_ais_db.get_resource(bears);
	    res = ais_merge->get_ais_resource(rv[0].get_url());
	    CPPUNIT_ASSERT(dump2string(res->get_stream()).find(bears_1_ais_string) 
			   != string::npos);

	    rv = ais_merge->d_ais_db.get_resource(coads);
	    res = ais_merge->get_ais_resource(rv[0].get_url());
	    CPPUNIT_ASSERT(dump2string(res->get_stream()).find(coads_ais_string) 
			   != string::npos);

	    rv = ais_merge->d_ais_db.get_resource(three_fnoc);
	    res = ais_merge->get_ais_resource(rv[0].get_url());
	    CPPUNIT_ASSERT(dump2string(res->get_stream()).find(starts_with_number_ais_string) 
			   != string::npos);
	}
	catch (Error &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Error");
	}
    }

    void merge_test() {
	try {
	    Connect *conn;
	    DAS das;
	    ostringstream oss;

	    conn = new Connect(fnoc1); // test overwrite (default)
	    conn->request_das(das);
	    ais_merge->merge(fnoc1, das);
	    das.print(oss);
	    CPPUNIT_ASSERT(oss.str().find(fnoc1_merge_ais) != string::npos);

	    delete conn; conn = 0;
	    das.erase();
	    oss.str("");

	    conn = new Connect(fnoc2); // test replace
	    conn->request_das(das);
	    ais_merge->merge(fnoc2, das);
	    das.print(oss);
	    CPPUNIT_ASSERT(oss.str().find(fnoc2_merge_ais) != string::npos);

	    delete conn; conn = 0;
	    das.erase();
	    oss.str("");
	    
	    conn = new Connect(fnoc3); // test fallback
	    conn->request_das(das); // with a non-empty das, nothing happens
	    ais_merge->merge(fnoc3, das);
	    das.print(oss);
	    CPPUNIT_ASSERT(oss.str().find(fnoc3_das) != string::npos);

	    oss.str(""); das.erase(); // empty das, should add attributes
	    ais_merge->merge(fnoc3, das);
	    das.print(oss);
	    CPPUNIT_ASSERT(oss.str().find(fnoc3_merge_ais) != string::npos);

	    conn = new Connect(three_fnoc); // test regexp
	    conn->request_das(das); // with a non-empty das, nothing happens
	    ais_merge->merge(three_fnoc, das);
	    das.print(oss);
	    CPPUNIT_ASSERT(oss.str().find(three_fnoc_merge_ais) 
			   != string::npos);
	}
	catch (Error &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Error");
	}
    }
};

string AISMergeTest::fnoc1 = "http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc";
string AISMergeTest::fnoc2 = "http://localhost/dods-test/nph-dods/data/nc/fnoc2.nc";
string AISMergeTest::fnoc3 = "http://localhost/dods-test/nph-dods/data/nc/fnoc3.nc";
string AISMergeTest::bears = "http://localhost/dods-test/nph-dods/data/nc/bears.nc";
string AISMergeTest::coads = "http://localhost/dods-test/nph-dods/data/nc/coads_climatology.nc";
string AISMergeTest::three_fnoc = "http://localhost/dods-test/nph-dods/data/nc/3fnoc.nc";

string AISMergeTest::fnoc1_ais = "http://localhost/ais/fnoc1.nc.das";
string AISMergeTest::fnoc2_ais = "http://localhost/ais/fnoc2.nc.das";
string AISMergeTest::digit_ais = "ais_testsuite/starts_with_number.das";

string AISMergeTest::fnoc1_ais_string = 
"Attributes {\n\
    u {\n\
	String DODS_Name \"UWind\";\n\
    }\n\
    v {\n\
	String DODS_Name \"VWind\";\n\
    }\n\
}";

string AISMergeTest::bears_1_ais_string =
"Attributes {\n\
    bears {\n\
	String longname \"Test data\";\n\
    }\n\
}";

string AISMergeTest::coads_ais_string =
"Attributes {\n\
    COADSX {\n\
        String long_name \"Longitude\";\n\
    }\n\
}";

string AISMergeTest::starts_with_number_ais_string =
"Attributes {\n\
    NC_GLOBAL {\n\
        String AIS_Test_info \"This dataset's name starts with a digit.\";\n\
    }\n\
}";

string AISMergeTest::fnoc3_das =
"Attributes {\n\
    u {\n\
        String units \"meter per second\";\n\
        String long_name \"Vector wind eastward component\";\n\
        String missing_value \"-32767\";\n\
        String scale_factor \"0.005\";\n\
    }\n\
    v {\n\
        String units \"meter per second\";\n\
        String long_name \"Vector wind northward component\";\n\
        String missing_value \"-32767\";\n\
        String scale_factor \"0.005\";\n\
    }\n\
    lat {\n\
        String units \"degree North\";\n\
    }\n\
    lon {\n\
        String units \"degree East\";\n\
    }\n\
    time {\n\
        String units \"hours from base_time\";\n\
    }\n\
    NC_GLOBAL {\n\
        String base_time \"88-245-00:00:00\";\n\
        String title \" FNOC UV wind components from 1988-245 to 1988-247.\";\n\
    }\n\
    DODS_EXTRA {\n\
        String Unlimited_Dimension \"time_a\";\n\
    }\n\
}";

string AISMergeTest::fnoc1_merge_ais = 
"Attributes {\n\
    u {\n\
        String units \"meter per second\";\n\
        String long_name \"Vector wind eastward component\";\n\
        String missing_value \"-32767\";\n\
        String scale_factor \"0.005\";\n\
        String DODS_Name \"UWind\";\n\
    }\n\
    v {\n\
        String units \"meter per second\";\n\
        String long_name \"Vector wind northward component\";\n\
        String missing_value \"-32767\";\n\
        String scale_factor \"0.005\";\n\
        String DODS_Name \"VWind\";\n\
    }\n\
    lat {\n\
        String units \"degree North\";\n\
    }\n\
    lon {\n\
        String units \"degree East\";\n\
    }\n\
    time {\n\
        String units \"hours from base_time\";\n\
    }\n\
    NC_GLOBAL {\n\
        String base_time \"88- 10-00:00:00\";\n\
        String title \" FNOC UV wind components from 1988- 10 to 1988- 13.\";\n\
    }\n\
    DODS_EXTRA {\n\
        String Unlimited_Dimension \"time_a\";\n\
    }\n\
}";

string AISMergeTest::fnoc2_merge_ais =
"Attributes {\n\
    u {\n\
        String units \"meter per second\";\n\
        String long_name \"UWind\";\n\
    }\n\
    v {\n\
        String units \"meter per second\";\n\
        String long_name \"VWind\";\n\
    }\n\
    lat {\n\
        String units \"degree North\";\n\
        String long_name \"Latitude\";\n\
    }\n\
    lon {\n\
        String units \"degree East\";\n\
        String long_name \"Longitude\";\n\
    }\n\
    time {\n\
    }\n\
    NC_GLOBAL {\n\
    }\n\
    DODS_EXTRA {\n\
    }\n\
}";

string AISMergeTest::fnoc3_merge_ais =
"Attributes {\n\
    u {\n\
        String long_name \"UWind\";\n\
    }\n\
    v {\n\
        String long_name \"VWind\";\n\
    }\n\
    lat {\n\
        String long_name \"Latitude\";\n\
    }\n\
    lon {\n\
        String long_name \"Longitude\";\n\
    }\n\
}";

string AISMergeTest::three_fnoc_merge_ais =
"Attributes {\n\
    u {\n\
        String long_name \"UWind\", \"Vector wind eastward component\";\n\
        String units \"meter per second\";\n\
        String missing_value \"-32767\";\n\
        String scale_factor \"0.005\";\n\
    }\n\
    v {\n\
        String long_name \"VWind\", \"Vector wind northward component\";\n\
        String units \"meter per second\";\n\
        String missing_value \"-32767\";\n\
        String scale_factor \"0.005\";\n\
    }\n\
    lat {\n\
        String long_name \"Latitude\";\n\
        String units \"degree North\";\n\
    }\n\
    lon {\n\
        String long_name \"Longitude\";\n\
        String units \"degree East\";\n\
    }\n\
    time {\n\
        String units \"hours from base_time\";\n\
    }\n\
    NC_GLOBAL {\n\
        String base_time \"88-245-00:00:00\";\n\
        String title \" FNOC UV wind components from 1988-245 to 1988-247.\";\n\
        String AIS_Test_info \"This dataset's name starts with a digit.\";\n\
    }\n\
    DODS_EXTRA {\n\
        String Unlimited_Dimension \"time_a\";\n\
    }\n\
}";

CPPUNIT_TEST_SUITE_REGISTRATION(AISMergeTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: AISMergeTest.cc,v $
// Revision 1.4  2003/04/23 21:33:53  jimg
// Changes for the unit tests. This involved merging Rob's VC++ changes
// and fixing a bug in escaping.cc (a call to string::insert invalidated
// an iterator in a loop).
//
// Revision 1.3  2003/03/12 01:07:34  jimg
// Added regular expressions to the AIS subsystem. In an AIS database (XML)
// it is now possible to list a regular expression in place of an explicit
// URL. The AIS will try to match this Regexp against candidate URLs and
// return the ancillary resources for all those that succeed.
//
// Revision 1.2  2003/03/04 17:56:43  jimg
// Now uses Response objects.
//
// Revision 1.1  2003/02/25 23:29:52  jimg
// Added.
//
