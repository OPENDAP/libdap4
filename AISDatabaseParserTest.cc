
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
#include "debug.h"

using namespace CppUnit;

class AISDatabaseParserTest:public TestFixture {
private:
    string fnoc1, fnoc2, fnoc3;
    string fnoc1_ais, fnoc2_ais, fnoc3_ais;
    AISDatabaseParser *ais_parser;
    AISResources *ais;

public:
    AISDatabaseParserTest() {} 
    ~AISDatabaseParserTest() {} 

    void setUp() {
	fnoc1 = "http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc";
	fnoc2 = "http://localhost/dods-test/nph-dods/data/nc/fnoc2.nc";
	fnoc3 = "http://localhost/dods-test/nph-dods/data/nc/fnoc3.nc";
	fnoc1_ais = "http://localhost/ais/fnoc1.nc.das";
	fnoc2_ais = "ais_testsuite/fnoc2_replace.das";
	fnoc3_ais = "http://localhost/ais/fnoc3_fallback.das";

	ais = new AISResources;
	ais_parser = new AISDatabaseParser();
    } 

    void tearDown() { 
	delete ais_parser;
	delete ais;
    }

    CPPUNIT_TEST_SUITE( AISDatabaseParserTest );

    CPPUNIT_TEST(intern_test);
    CPPUNIT_TEST(errant_database_test);

    CPPUNIT_TEST_SUITE_END();

    void intern_test() {
	try {
	    ais_parser->intern("ais_testsuite/ais_database.xml", ais);

	    ResourceVector trv1 = ais->get_resource(fnoc1);
	    CPPUNIT_ASSERT(trv1.size() == 1);
	    CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
	    CPPUNIT_ASSERT(trv1[0].get_rule() == overwrite);

	    ResourceVector trv2 = ais->get_resource(fnoc2);
	    CPPUNIT_ASSERT(trv2.size() == 1);
	    CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
	    CPPUNIT_ASSERT(trv2[0].get_rule() == replace);

	    ResourceVector trv3 = ais->get_resource(fnoc3);
	    CPPUNIT_ASSERT(trv3.size() == 1);
	    CPPUNIT_ASSERT(trv3[0].get_url() == fnoc3_ais);
	    CPPUNIT_ASSERT(trv3[0].get_rule() == fallback);
	}
	catch (AISDatabaseReadFailed &e) {
	    cerr << endl << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Parse failed.");
	}
    }

    void errant_database_test() {
	try {
	    ais_parser->intern("ais_testsuite/ais_error_1.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_1.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}

	try {
	    ais_parser->intern("ais_testsuite/ais_error_2.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_2.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}

	try {
	    ais_parser->intern("ais_testsuite/ais_error_3.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_3.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}

	try {
	    ais_parser->intern("ais_testsuite/ais_error_4.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_3.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}

	try {
	    ais_parser->intern("ais_testsuite/ais_error_5.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_3.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
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

// $Log: AISDatabaseParserTest.cc,v $
// Revision 1.4  2003/02/26 01:27:49  jimg
// Changed the name of the parse() method to intern().
//
// Revision 1.3  2003/02/25 23:25:30  jimg
// Fixed for latest rev of the ais_database.xml.
//
// Revision 1.2  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
