
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

//#define DODS_DEBUG 1

#include "DDXParser.h"
#include "BaseTypeFactory.h"
#include "debug.h"
#include <test_config.h>

using namespace CppUnit;
using namespace std;
using namespace libdap;

class DDXParserTest:public TestFixture {
private:
    BaseTypeFactory *factory;
    DDXParser *ddx_parser;
    DDS *dds;
#if 0
    string blob;
#endif
public:
    DDXParserTest() {}
    ~DDXParserTest() {}

    void setUp() {
	factory = new BaseTypeFactory;
	ddx_parser = new DDXParser(factory);
	dds = new DDS(factory);
    }

    void tearDown() {
	delete ddx_parser; ddx_parser = 0;
	delete factory; factory = 0;
	delete dds; dds = 0;
    }

    CPPUNIT_TEST_SUITE( DDXParserTest );
#if 1
    CPPUNIT_TEST(dap_version_test);
    CPPUNIT_TEST(no_blob_version_32_test);
    CPPUNIT_TEST(blob_in_version_32_test);
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
    CPPUNIT_TEST(intern_stream_test);

    // Error tests

    CPPUNIT_TEST(unknown_tag_test);
    CPPUNIT_TEST(bad_nesting_test);
    CPPUNIT_TEST(unknown_end_tag_test);
    CPPUNIT_TEST(variable_in_attribtue_container_test);
#endif
    CPPUNIT_TEST(simple_type_missing_attribute_test);
#if 1
    CPPUNIT_TEST(array_missing_dimension_test);
    CPPUNIT_TEST(array_missing_dimension_stream_read_test);
#endif
    CPPUNIT_TEST_SUITE_END();

    void dap_version_test() {
    try {
        ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.00.ddx", dds);
        CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
        CPPUNIT_ASSERT(dds->get_dap_major() == 3);
        CPPUNIT_ASSERT(dds->get_dap_minor() == 2);

        DBG(dds->print_xml(cout, false, "    "));
    }
    catch (DDXParseFailed &e) {
        DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
        CPPUNIT_FAIL("test.00.ddx failed.");
    }
    }

    void no_blob_version_32_test() {
    try {
        ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.0c.ddx", dds);
        CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
        CPPUNIT_ASSERT(dds->get_dap_major() == 3);
        CPPUNIT_ASSERT(dds->get_dap_minor() == 2);

        DBG(dds->print_xml(cout, false, "    "));
    }
    catch (DDXParseFailed &e) {
        DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
        CPPUNIT_FAIL("test.0c.ddx failed.");
    }
    }

    void blob_in_version_32_test() {
    try {
        ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.0d.ddx", dds);
        CPPUNIT_FAIL("Parsing test.0d.ddx should fail since it's a DAP 3.2 doc with a dataBLOB tag.");

        DBG(dds->print_xml(cout, false, "    "));
    }
    catch (DDXParseFailed &e) {
        DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
        CPPUNIT_ASSERT("test.0d.ddx failed as expected.");
    }
    }

    void top_level_attribute_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.01.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");

	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.01.ddx failed.");
	}
    }

    void top_level_attribute_container_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.02.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.02.ddx failed.");
	}
    }

    // ALiases are broken *** 05/29/03 jhrg
    void top_level_attribute_alias_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.03.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.03.ddx failed.");
	}
    }

    void top_level_simple_types_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.04.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "DDXParseFailed: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.04.ddx failed.");
	}
        catch (Error &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.04.ddx failed.");
        }
    }

    void top_level_simple_types_with_attributes_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.05.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.05.ddx failed.");
	}
    }

    void simple_arrays_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.06.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "OneDimensionalSimpleArrays");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.06.ddx failed.");
	}
    }

    void simple_arrays_multi_dim_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.07.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "MultiDimensionalSimpleArrays");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.07.ddx failed.");
	}
    }

    void simple_arrays_attributes_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.08.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.08.ddx failed.");
	}
    }

    void structure_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.09.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.09.ddx failed.");
	}
    }

    void sequence_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.0a.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.0a.ddx failed.");
	}
    }

    void grid_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/test.0b.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
	    DBG(dds->print_xml(cout, false, "    "));
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
	    CPPUNIT_FAIL("test.0b.ddx failed.");
	}
    }

    void intern_stream_test() {
        try {
	    string file_name = (string)TEST_SRC_DIR + "/ddx-testsuite/test.0b.ddx";
            FILE *in = fopen(file_name.c_str(), "r");
            ddx_parser->intern_stream(in, dds);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false, "    "));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.0b.ddx failed.");
        }
    }

    // Error tests start here.

    void unknown_tag_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/error.01.ddx", dds);
	    CPPUNIT_FAIL("error.01.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void bad_nesting_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/error.02.ddx", dds);
	    CPPUNIT_FAIL("error.02.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void unknown_end_tag_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/error.03.ddx", dds);
	    CPPUNIT_FAIL("error.03.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void variable_in_attribtue_container_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/error.04.ddx", dds);
	    CPPUNIT_FAIL("error.04.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void array_missing_dimension_test() {
	try {
	    ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/error.05.ddx", dds);
	    CPPUNIT_FAIL("error.05.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }

    void simple_type_missing_attribute_test() {
    try {
        ddx_parser->intern((string)TEST_SRC_DIR + "/ddx-testsuite/error.06.ddx", dds);
        CPPUNIT_FAIL("error.06.ddx should fail!");
    }
    catch (DDXParseFailed &e) {
        DBG(cerr << "Error: " << e.get_error_message() << endl);
    }
    }

    void array_missing_dimension_stream_read_test() {
        try {
	    string file_name = (string)TEST_SRC_DIR + "/ddx-testsuite/error.05.ddx";
            FILE *in = fopen(file_name.c_str(), "r");
            ddx_parser->intern_stream(in, dds);
            CPPUNIT_FAIL("error.05.ddx should fail!");
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DDXParserTest);

int
main( int, char** )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    return (wasSuccessful) ? 0 : 1;
}
