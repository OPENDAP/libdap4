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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// Tests for the AISResources class.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "GetOpt.h"

#include "DDXParserSAX2.h"
#include "BaseTypeFactory.h"
#include "ObjectType.h"
#include "mime_util.h"
#include "debug.h"

#include <test_config.h>

using namespace CppUnit;
using namespace std;
using namespace libdap;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

namespace libdap {

class DDXParserTest: public TestFixture {
private:
    BaseTypeFactory *factory;
    DDXParser *ddx_parser;
    DDS *dds;

public:
    DDXParserTest()
    {
    }
    ~DDXParserTest()
    {
    }

    void setUp()
    {
        factory = new BaseTypeFactory;
        ddx_parser = new DDXParser(factory);
        dds = new DDS(factory);
    }

    void tearDown()
    {
        delete ddx_parser;
        ddx_parser = 0;
        delete factory;
        factory = 0;
        delete dds;
        dds = 0;
    }

    CPPUNIT_TEST_SUITE (DDXParserTest);

    CPPUNIT_TEST (other_xml_parse_test1);
    CPPUNIT_TEST (other_xml_parse_test2);
    CPPUNIT_TEST (other_xml_parse_test3);
    CPPUNIT_TEST (dap_version_test);
    CPPUNIT_TEST (no_blob_version_32_test);
    CPPUNIT_TEST (blob_in_version_32_test);
    CPPUNIT_TEST (parsing_ddx_from_dataddx_test);
    CPPUNIT_TEST (top_level_attribute_test);
    CPPUNIT_TEST (top_level_attribute_container_test);
    CPPUNIT_TEST (top_level_simple_types_test);
    CPPUNIT_TEST (top_level_simple_types_with_attributes_test);
    CPPUNIT_TEST (simple_arrays_test);
    CPPUNIT_TEST (simple_arrays_multi_dim_test);
    CPPUNIT_TEST (simple_arrays_attributes_test);
    CPPUNIT_TEST (structure_test);
    CPPUNIT_TEST (sequence_test);
    CPPUNIT_TEST (grid_test);
    CPPUNIT_TEST (intern_stream_test);
    CPPUNIT_TEST (intern_ddx_from_dataddx_test);
    // FILE I/O tests
    CPPUNIT_TEST (top_level_simple_types_test_file_stream);
    CPPUNIT_TEST (structure_test_file_ptr);

#if 0
    // All of these fail; maybe because on OSX 10.9 something about
    // libxml2 is odd. Investigate. jhrg 2/4/14
    // FIXME

    // C++ Stream I/O tests
    CPPUNIT_TEST(top_level_simple_types_test_cpp_stream);
    CPPUNIT_TEST(structure_test_cpp_stream);
    CPPUNIT_TEST(sequence_test_cpp_stream);
    CPPUNIT_TEST(grid_test_cpp_stream);
#endif
    // Error tests
    CPPUNIT_TEST (unknown_tag_test);
    CPPUNIT_TEST (bad_nesting_test);
    CPPUNIT_TEST (unknown_end_tag_test);
    CPPUNIT_TEST (variable_in_attribtue_container_test);
    CPPUNIT_TEST (simple_type_missing_attribute_test);
    CPPUNIT_TEST (array_missing_dimension_test);
    CPPUNIT_TEST (array_missing_dimension_stream_read_test);

    CPPUNIT_TEST_SUITE_END();

    void other_xml_parse_test1()
    {
        try {
            string cid;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.1.other_xml.ddx", dds, cid);
            DBG(dds->print_xml(cout, false, "    "));
            CPPUNIT_ASSERT(dds->get_dataset_name() == "200803061600_HFRadar_USEGC_6km_rtv_SIO.nc");
            CPPUNIT_ASSERT(dds->get_dap_major() == 3);
            CPPUNIT_ASSERT(dds->get_dap_minor() == 3);
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.00.ddx failed.");
        }
    }

    void other_xml_parse_test2()
    {
        try {
            string cid;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.2.other_xml.ddx", dds, cid);
            DBG(dds->print_xml(cout, false, "    "));
            CPPUNIT_ASSERT(dds->get_dataset_name() == "200803061600_HFRadar_USEGC_6km_rtv_SIO.nc");
            CPPUNIT_ASSERT(dds->get_dap_major() == 3);
            CPPUNIT_ASSERT(dds->get_dap_minor() == 3);
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.00.ddx failed.");
        }
    }

    void other_xml_parse_test3()
    {
        try {
            string cid;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.3.other_xml.ddx", dds, cid);
            DBG(dds->print_xml(cout, false, "    "));
            CPPUNIT_ASSERT(dds->get_dataset_name() == "200803061600_HFRadar_USEGC_6km_rtv_SIO.nc");
            CPPUNIT_ASSERT(dds->get_dap_major() == 3);
            CPPUNIT_ASSERT(dds->get_dap_minor() == 3);
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.00.ddx failed.");
        }
    }

    void dap_version_test()
    {
        FILE *in;
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.00.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            CPPUNIT_ASSERT(dds->get_dap_major() == 3);
            CPPUNIT_ASSERT(dds->get_dap_minor() == 2);

            DBG(dds->print_xml(cout, false));

            string name = string(TEST_SRC_DIR) + "/ddx-testsuite/test.00.ddx";
            in = fopen(name.c_str(), "r");

            ddx_parser->intern_stream(in, dds, blob);

            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            CPPUNIT_ASSERT(dds->get_dap_major() == 3);
            CPPUNIT_ASSERT(dds->get_dap_minor() == 2);

            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.00.ddx failed.");
        }

        fclose(in);
    }

    void no_blob_version_32_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.0c.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            CPPUNIT_ASSERT(dds->get_dap_major() == 3);
            CPPUNIT_ASSERT(dds->get_dap_minor() == 2);

            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.0c.ddx failed.");
        }
    }

    void blob_in_version_32_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.0d.ddx", dds, blob);
            CPPUNIT_FAIL("Parsing test.0d.ddx should fail since it's a DAP 3.2 doc with a dataBLOB tag.");

            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_ASSERT("test.0d.ddx failed as expected.");
        }
    }

    void parsing_ddx_from_dataddx_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/DDX_from_dataddx.xml", dds, blob);
            CPPUNIT_ASSERT(true);
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }

    }

    void top_level_attribute_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.01.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");

            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.01.ddx failed.");
        }
    }

    void top_level_attribute_container_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.02.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.02.ddx failed.");
        }
    }

    // ALiases are broken *** 05/29/03 jhrg
    void top_level_attribute_alias_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.03.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.03.ddx failed.");
        }
    }

    void top_level_simple_types_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.04.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            DBG(dds->print_xml(cout, false));
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

#if 0 // working
    void top_level_simple_types_with_attributes_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR
                + "/ddx-testsuite/test.05.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            DBG(dds->print_xml(cout, false))
            ;
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.05.ddx failed.");
        }
    }
#else
    void top_level_simple_types_test_file_stream()
    {
        FILE *in;
        try {
            string blob;
            in = fopen(((string) TEST_SRC_DIR + "/ddx-testsuite/test.04.ddx").c_str(), "r");
            ddx_parser->intern_stream(in, dds, blob);
            fclose(in);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            fclose(in);
            DBG(cerr << endl << "DDXParseFailed: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.04.ddx failed.");
        }
        catch (Error &e) {
            fclose(in);
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.04.ddx failed.");
        }
    }

    void top_level_simple_types_test_cpp_stream()
    {
        try {
            string blob;
            ifstream in(((string) TEST_SRC_DIR + "/ddx-testsuite/test.04.ddx").c_str());
            ddx_parser->intern_stream(in, dds, blob);
            DBG(dds->print_xml(cout, false));
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
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

    void top_level_simple_types_with_attributes_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.05.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.05.ddx failed.");
        }
    }
#endif

    void simple_arrays_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.06.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "OneDimensionalSimpleArrays");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.06.ddx failed.");
        }
    }

    void simple_arrays_multi_dim_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.07.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "MultiDimensionalSimpleArrays");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.07.ddx failed.");
        }
    }

    void simple_arrays_attributes_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.08.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.08.ddx failed.");
        }
    }

    void structure_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.09.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.09.ddx failed.");
        }
    }

    void sequence_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.0a.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.0a.ddx failed.");
        }
    }

    void structure_test_file_ptr()
    {
        FILE *in;
        try {
            string blob;
            in = fopen((string(TEST_SRC_DIR) + "/ddx-testsuite/test.09.ddx").c_str(), "r");
            ddx_parser->intern_stream(in, dds, blob);
            fclose(in);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            fclose(in);
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.09.ddx failed.");
        }
    }

    void structure_test_cpp_stream()
    {
        try {
            string blob;
            ifstream input((string(TEST_SRC_DIR) + "/ddx-testsuite/test.09.ddx").c_str());
            ddx_parser->intern_stream(input, dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.09.ddx failed.");
        }
    }

    void sequence_test_cpp_stream()
    {
        try {
            string blob;
            ifstream input((string(TEST_SRC_DIR) + "/ddx-testsuite/test.0a.ddx").c_str());
            ddx_parser->intern_stream(input, dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.0a.ddx failed.");
        }
    }

    void grid_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.0b.ddx", dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.0b.ddx failed.");
        }
    }

    void grid_test_cpp_stream()
    {
        try {
            string blob;
            ifstream input((string(TEST_SRC_DIR) + "/ddx-testsuite/test.0b.ddx").c_str());
            ddx_parser->intern_stream(input, dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.0b.ddx failed.");
        }
    }

    void intern_stream_test()
    {
        try {
            string file_name = (string) TEST_SRC_DIR + "/ddx-testsuite/test.0b.ddx";
            FILE *in = fopen(file_name.c_str(), "r");
            string blob;
            ddx_parser->intern_stream(in, dds, blob);
            CPPUNIT_ASSERT(dds->get_dataset_name() == "testdata");
            DBG(dds->print_xml(cout, false));
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << endl << "Error: " << e.get_error_message() << endl);
            CPPUNIT_FAIL("test.0b.ddx failed.");
        }
    }

    // Error tests start here.

    void unknown_tag_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/error.01.ddx", dds, blob);
            CPPUNIT_FAIL("error.01.ddx should fail!");
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }

    void bad_nesting_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/error.02.ddx", dds, blob);
            CPPUNIT_FAIL("error.02.ddx should fail!");
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }

    void unknown_end_tag_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/error.03.ddx", dds, blob);
            CPPUNIT_FAIL("error.03.ddx should fail!");
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }

    void variable_in_attribtue_container_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/error.04.ddx", dds, blob);
            CPPUNIT_FAIL("error.04.ddx should fail!");
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }

    void array_missing_dimension_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/error.05.ddx", dds, blob);
            CPPUNIT_FAIL("error.05.ddx should fail!");
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }

    void simple_type_missing_attribute_test()
    {
        try {
            string blob;
            ddx_parser->intern((string) TEST_SRC_DIR + "/ddx-testsuite/error.06.ddx", dds, blob);
            CPPUNIT_FAIL("error.06.ddx should fail!");
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }

    void array_missing_dimension_stream_read_test()
    {
        try {
            string file_name = (string) TEST_SRC_DIR + "/ddx-testsuite/error.05.ddx";
            FILE *in = fopen(file_name.c_str(), "r");
            string blob;
            ddx_parser->intern_stream(in, dds, blob);
            CPPUNIT_FAIL("error.05.ddx should fail!");
        }
        catch (DDXParseFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }

    void intern_ddx_from_dataddx_test()
    {
        try {
            string file_name = (string) TEST_SRC_DIR + "/ddx-testsuite/dataddx_without_top_headers.dap";
            FILE *in = fopen(file_name.c_str(), "r");

            // First read the initial set of 'part' headers for the DDX
            read_multipart_headers(in, "text/xml", dods_ddx);

            string blob;
            ddx_parser->intern_stream(in, dds, blob, "--boundary-string-1");
            CPPUNIT_ASSERT(dds->get_dataset_name() == "fnoc1.nc");

            DBG(dds->print_xml(cout, false));
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }
};

}

CPPUNIT_TEST_SUITE_REGISTRATION (DDXParserTest);

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;
    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: DDXParserTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::DDXParserTest::suite()->getTests();
            unsigned int prefix_len = libdap::DDXParserTest::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            break;
        }
        default:
            break;
        }

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        for (; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = libdap::DDXParserTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    xmlMemoryDump();

    return wasSuccessful ? 0 : 1;
}
