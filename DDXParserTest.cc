
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

#if 1
#define DODS_DEBUG
#endif
#include "DDXParser.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

class DDXParserTest:public TestFixture {
private:
    DDXParser *ddx_parser;
    DDS *dds;
    string blob;

public:
    DDXParserTest() {} 
    ~DDXParserTest() {} 

    void setUp() {
	ddx_parser = new DDXParser;
	dds = new DDS;
    } 

    void tearDown() {
	delete ddx_parser;
	delete dds;
    }

    CPPUNIT_TEST_SUITE( DDXParserTest );

    CPPUNIT_TEST(top_level_attribute_test);
    CPPUNIT_TEST(top_level_attribute_container_test);
#if 0
    CPPUNIT_TEST(top_level_attribute_alias_test);
#endif
    CPPUNIT_TEST(top_level_simple_types_test);
    CPPUNIT_TEST(top_level_simple_types_with_attributes_test);
    CPPUNIT_TEST(simple_arrays_test);
    CPPUNIT_TEST(simple_arrays_multi_dim_test);
    CPPUNIT_TEST(simple_arrays_attributes_test);
    CPPUNIT_TEST(structure_test);
    CPPUNIT_TEST(sequence_test);
    CPPUNIT_TEST(grid_test);

    // Error tests

    CPPUNIT_TEST(unknown_tag_test);
    CPPUNIT_TEST(bad_nesting_test);
    CPPUNIT_TEST(unknown_end_tag_test);
    CPPUNIT_TEST(variable_in_attribtue_container_test);
    CPPUNIT_TEST(array_missing_dimension_test);

    CPPUNIT_TEST_SUITE_END();

    void top_level_attribute_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.01.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");

	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.01.ddx failed.");
	}
    }

    void top_level_attribute_container_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.02.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.02.ddx failed.");
	}
    }

    // ALiases are broken *** 05/29/03 jhrg
    void top_level_attribute_alias_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.03.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.03.ddx failed.");
	}
    }

    void top_level_simple_types_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.04.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.04.ddx failed.");
	}
    }

    void top_level_simple_types_with_attributes_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.05.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.05.ddx failed.");
	}
    }

    void simple_arrays_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.06.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "OneDimensionalSimpleArrays");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.06.ddx failed.");
	}
    }

    void simple_arrays_multi_dim_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.07.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "MultiDimensionalSimpleArrays");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.07.ddx failed.");
	}
    }

    void simple_arrays_attributes_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.08.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.08.ddx failed.");
	}
    }

    void structure_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.09.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.09.ddx failed.");
	}
    }

    void sequence_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.0a.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.0a.ddx failed.");
	}
    }

    void grid_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.0b.ddx", dds, &blob);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
	    DBG(dds->print_xml(stdout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(!"test.0b.ddx failed.");
	}
    }

    // Error tests start here. 

    void unknown_tag_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/error.01.ddx", dds, &blob);
	    CPPUNIT_ASSERT(!"error.01.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void bad_nesting_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/error.02.ddx", dds, &blob);
	    CPPUNIT_ASSERT(!"error.02.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void unknown_end_tag_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/error.03.ddx", dds, &blob);
	    CPPUNIT_ASSERT(!"error.03.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void variable_in_attribtue_container_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/error.04.ddx", dds, &blob);
	    CPPUNIT_ASSERT(!"error.04.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void array_missing_dimension_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/error.05.ddx", dds, &blob);
	    CPPUNIT_ASSERT(!"error.05.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DDXParserTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: DDXParserTest.cc,v $
// Revision 1.3  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.2  2003/06/03 01:43:01  jimg
// Added support for retrieval of the dodsBLOB url. The intern() method
// takes a point to a string; on return from the call the referenced string
// holds the blob url.
//
// Revision 1.1  2003/05/29 19:07:15  jimg
// Added.
//
