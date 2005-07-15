
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
using namespace std;

class AISDatabaseParserTest:public TestFixture {
private:
    string fnoc1, fnoc2, fnoc3, number, bears, three_fnoc;
    string fnoc1_ais, fnoc2_ais, fnoc3_ais, number_ais;
    AISDatabaseParser *ais_parser;
    AISResources *ais;

public:
    AISDatabaseParserTest() {} 
    ~AISDatabaseParserTest() {} 

    void setUp() {
	fnoc1 = "http://test.opendap.org/opendap/nph-dods/data/nc/fnoc1.nc";
	fnoc2 = "http://test.opendap.org/opendap/nph-dods/data/nc/fnoc2.nc";
	fnoc3 = "http://test.opendap.org/opendap/nph-dods/data/nc/fnoc3.nc";
	// number is the regular expression that will be in the
	// ais_database.xml file.
	number = "http://test.opendap.org/opendap/nph-dods/data/nc/[0-9]+.*\\.nc";
	bears = "http://test.opendap.org/opendap/nph-dods/data/nc/123bears.nc";
	three_fnoc = "http://test.opendap.org/opendap/nph-dods/data/nc/3fnoc.nc";

	fnoc1_ais = "http://test.opendap.org/ais/fnoc1.nc.das";
	fnoc2_ais = "ais_testsuite/fnoc2_replace.das";
	fnoc3_ais = "http://test.opendap.org/ais/fnoc3_fallback.das";
	number_ais = "ais_testsuite/starts_with_number.das";

	ais = new AISResources;
	ais_parser = new AISDatabaseParser();
    } 

    void tearDown() { 
	delete ais_parser; ais_parser = 0;
	delete ais; ais = 0;
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
	    CPPUNIT_ASSERT(trv1[0].get_rule() == Resource::overwrite);

	    ResourceVector trv2 = ais->get_resource(fnoc2);
	    CPPUNIT_ASSERT(trv2.size() == 1);
	    CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
	    CPPUNIT_ASSERT(trv2[0].get_rule() == Resource::replace);

	    ResourceVector trv3 = ais->get_resource(fnoc3);
	    CPPUNIT_ASSERT(trv3.size() == 1);
	    CPPUNIT_ASSERT(trv3[0].get_url() == fnoc3_ais);
	    CPPUNIT_ASSERT(trv3[0].get_rule() == Resource::fallback);

	    ResourceVector trv4 = ais->get_resource(bears);
	    CPPUNIT_ASSERT(trv4.size() == 1);
	    CPPUNIT_ASSERT(trv4[0].get_url() == number_ais);
	    CPPUNIT_ASSERT(trv4[0].get_rule() == Resource::overwrite);
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
	    CPPUNIT_ASSERT(!"ais_error_4.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}

	try {
	    ais_parser->intern("ais_testsuite/ais_error_5.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_5.xml should fail!");
	}
	catch (AISDatabaseReadFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}

	try {
	    ais_parser->intern("ais_testsuite/ais_error_6.xml", ais);
	    CPPUNIT_ASSERT(!"ais_error_6.xml should fail!");
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
// Revision 1.9  2004/02/19 19:42:51  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.7.2.1  2004/02/11 22:26:45  jimg
// Changed all calls to delete so that whenever we use 'delete x' or
// 'delete[] x' the code also sets 'x' to null. This ensures that if a
// pointer is deleted more than once (e.g., when an exception is thrown,
// the method that throws may clean up and then the catching method may
// also clean up) the second, ..., call to delete gets a null pointer
// instead of one that points to already deleted memory.
//
// Revision 1.8  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.7  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.6  2003/03/12 01:07:34  jimg
// Added regular expressions to the AIS subsystem. In an AIS database (XML)
// it is now possible to list a regular expression in place of an explicit
// URL. The AIS will try to match this Regexp against candidate URLs and
// return the ancillary resources for all those that succeed.
//
// Revision 1.5  2003/03/04 17:58:32  jimg
// Fixed Resource::ResourceRule enum usage (fallback, overwrite, replace).
//
// Revision 1.4  2003/02/26 01:27:49  jimg
// Changed the name of the parse() method to intern().
//
// Revision 1.3  2003/02/25 23:25:30  jimg
// Fixed for latest rev of the ais_database.xml.
//
// Revision 1.2  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
