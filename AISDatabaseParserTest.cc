
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
 
// Tests for the AISResources class.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "AISDatabaseParser.h"

using namespace CppUnit;

class AISDatabaseParserTest:public TestFixture {
private:
    string fnoc1, fnoc2, fnoc3;
    string fnoc1_ais, fnoc2_ais;
    AISDatabaseParser *ais_parser;
    AISResources *ais;

public:
    AISDatabaseParserTest() {} 
    ~AISDatabaseParserTest() {} 

    void setUp() {
	fnoc1 = "http://dodsdev.gso.uri.edu/dods-3.2/nph-dods/data/nc/fnoc1.nc";
	fnoc2 = "http://dodsdev.gso.uri.edu/dods-3.2/nph-dods/data/nc/fnoc2.nc";
	fnoc3 = "http://dodsdev.gso.uri.edu/dods-3.2/nph-dods/data/nc/fnoc3.nc";
	fnoc1_ais = "http://dodsdev.gso.uri.edu/ais/fnoc1.nc.das";
	fnoc2_ais = "http://dodsdev.gso.uri.edu/ais/fnoc2.nc.das";

	ais = new AISResources;
	ais_parser = new AISDatabaseParser();
    } 

    void tearDown() { 
	delete ais_parser;
	delete ais;
    }

#if 0
    bool re_match(Regex &r, const char *s) {
	return r.match(s, strlen(s)) == (int)strlen(s);
    }
#endif

    CPPUNIT_TEST_SUITE( AISDatabaseParserTest );

    CPPUNIT_TEST(parse_test);
    CPPUNIT_TEST(errant_database_test);

    CPPUNIT_TEST_SUITE_END();

    void parse_test() {
	try {
	    ais_parser->parse("ais_database.xml", ais);

	    ResourceVector trv1 = ais->get_resource(fnoc1);
	    CPPUNIT_ASSERT(trv1.size() == 1);
	    CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
	    CPPUNIT_ASSERT(trv1[0].get_rule() == overwrite);

	    ResourceVector trv2 = ais->get_resource(fnoc2);
	    CPPUNIT_ASSERT(trv2.size() == 1);
	    cerr << "fnoc2 url[0]: " << trv2[0].get_url() << endl;
	    cerr << "fnoc2 rule[0]: " << trv2[0].get_rule() << endl;
	    CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
	    CPPUNIT_ASSERT(trv2[0].get_rule() == fallback);

	    ResourceVector trv3 = ais->get_resource(fnoc3);
	    CPPUNIT_ASSERT(trv3.size() == 2);
	    CPPUNIT_ASSERT(trv3[0].get_url() == fnoc1_ais);
	    CPPUNIT_ASSERT(trv3[0].get_rule() == overwrite);
	    CPPUNIT_ASSERT(trv3[1].get_url() == fnoc2_ais);
	    CPPUNIT_ASSERT(trv3[1].get_rule() == fallback);
	}
	catch (AISDatabaseReadFailed &e) {
	    CPPUNIT_ASSERT(!"Parse failed.");
	}
    }

    void errant_database_test() {
	try {
	    ais_parser->parse("ais_error_1.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_1.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	}

	try {
	    ais_parser->parse("ais_error_2.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_2.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	}

	try {
	    ais_parser->parse("ais_error_3.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_3.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	}

	try {
	    ais_parser->parse("ais_error_4.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_3.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	}

	try {
	    ais_parser->parse("ais_error_5.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_3.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AISDatabaseParserTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}
