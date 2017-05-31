// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2014 OPeNDAP, Inc.
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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sstream>

#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "DDS.h"
#include "DMR.h"
#include "XMLWriter.h"
#include "D4BaseTypeFactory.h"
#include "D4ParserSax2.h"

#include "GNURegex.h"
#include "GetOpt.h"
#include "util.h"
#include "debug.h"

#include "testFile.h"
#include "test_config.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) {x;} } while(false)

using namespace CppUnit;
using namespace std;
using namespace libdap;

class DMRTest: public TestFixture {
private:

public:
    DMRTest()
    {
    }
    ~DMRTest()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    bool re_match(Regex &r, const string &s)
    {
        int match = r.match(s.c_str(), s.length());
        DBG(cerr << "Match: " << match << " should be: " << s.length() << endl);
        return match == static_cast<int>(s.length());
    }

    /**
     * Given the name of a DDS and optional DAS file, build a DMR using the
     * hackery known as transform_to_dap4 and the new DMR ctor.
     *
     * @param dds_file
     * @param attr
     * @return A pointer to the new DMR; caller must delete
     */
    DMR *build_dmr(const string &dds_file, const string &das_file = "")
    {
        DBG(cerr << __func__ << "() - BEGIN" << endl);
        DBG(cerr << __func__ << "() - dds_file: " << dds_file << endl);
        DBG(cerr << __func__ << "() - das_file: " << das_file << endl);

        try {
            string prefix = string(TEST_SRC_DIR) + "/dmr-testsuite/";

            BaseTypeFactory factory;
            DDS dds(&factory, dds_file);
            dds.parse(prefix + dds_file);
            DBG(cerr << "SOURCE DDS: " << endl; dds.print(cerr));

            if (!das_file.empty()) {
                DAS das;
                das.parse(prefix + das_file);
                dds.transfer_attributes(&das);
                DBG(cerr << "SOURCE DAS: " << endl; das.print(cerr));

                DBG(cerr << "dds.print_das(): " << endl; dds.print_das(cerr));
                // DBG(cerr << "dds.print_xml(): " << endl; dds.print_xml(cerr,false,"blob_foo"));
            }

            D4BaseTypeFactory d4_factory;
            DBG(cerr << __func__ << "() - END" << endl);
            return new DMR(&d4_factory, dds);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(string("Caught Error: ") + e.get_error_message());
        }

        return 0;
    }

    void test_template(const string &dds_file, const string &dmr_baseline, const string &attr = "")
    {
        DBG(cerr << __func__ << "() - BEGIN" << endl);
        DMR *dmr = 0;
        try {
            dmr = build_dmr(dds_file, attr);
            XMLWriter xml;
            dmr->print_dap4(xml);
            DBG(cerr << "DMR: " << endl << xml.get_doc() << endl);

            string prefix = string(TEST_SRC_DIR) + "/dmr-testsuite/";
            CPPUNIT_ASSERT(string(xml.get_doc()) == readTestBaseline(prefix + dmr_baseline));
            delete dmr;
        }
        catch (Error &e) {
            delete dmr;
            CPPUNIT_FAIL(string("Caught Error: ") + e.get_error_message());
        }
        DBG(cerr << __func__ << "() - END" << endl);
    }

CPPUNIT_TEST_SUITE (DMRTest);

    CPPUNIT_TEST(test_dmr_from_dds_1);
    CPPUNIT_TEST(test_dmr_from_dds_2);
    CPPUNIT_TEST(test_dmr_from_dds_3);
    CPPUNIT_TEST(test_dmr_from_dds_4);
    CPPUNIT_TEST(test_dmr_from_dds_5);
    CPPUNIT_TEST(test_dmr_from_dds_6);

    CPPUNIT_TEST(test_dmr_from_dds_with_attr_1);
    CPPUNIT_TEST(test_dmr_from_dds_with_attr_2);

    CPPUNIT_TEST(test_copy_ctor);
    CPPUNIT_TEST(test_copy_ctor_2);
    CPPUNIT_TEST(test_copy_ctor_3);
    CPPUNIT_TEST(test_copy_ctor_4);

    CPPUNIT_TEST_SUITE_END()
    ;

    // Test a DDS with simple scalar types and no attributes
    void test_dmr_from_dds_1()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_template("test.1", "test.1.dmr");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    // What about arrays? This should build shared dimensions
    void test_dmr_from_dds_2()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_template("fnoc1.nc.dds", "fnoc1.nc.dmr");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dmr_from_dds_3()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_template("3B42.980909.5.HDF.dds", "3B42.980909.5.HDF.dmr");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dmr_from_dds_4()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_template("S2000415.HDF.dds", "S2000415.HDF.dmr");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dmr_from_dds_5()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_template("coads_climatology.nc.dds", "coads_climatology.nc.dmr");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dmr_from_dds_6()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_template("structure_1.dds", "structure_1.dds.dmr");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dmr_from_dds_with_attr_1()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_template("test.1", "test.1.attr.dmr", "test.1.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dmr_from_dds_with_attr_2()
    {
        // The 'hacked' file has global attributes
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_template("3B42.980909.5.HDF.dds", "3B42.980909.5.hacked.2.HDF.attr.dmr", "3B42.980909.5.hacked.2.HDF.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_copy_ctor()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        DMR *dmr = build_dmr("test.1", "test.1.das");
        DMR *dmr_2 = new DMR(*dmr);

        XMLWriter xml;
        dmr->print_dap4(xml);
        string dmr_src = string(xml.get_doc());
        DBG(cerr << "DMR SRC: " << endl << dmr_src << endl);

        XMLWriter xml2;
        dmr_2->print_dap4(xml2);
        string dmr_dest = string(xml2.get_doc());
        DBG(cerr << "DMR DEST: " << endl << dmr_dest << endl);

        delete dmr;
        delete dmr_2;
        CPPUNIT_ASSERT(dmr_src == dmr_dest);

        DBG(cerr << __func__ << "() - END" << endl);
    }

    // This tests if using the copy still works after the original is deleted
    void test_copy_ctor_2()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        DMR *dmr = build_dmr("test.1", "test.1.das");
        DMR *dmr_2 = new DMR(*dmr);

        XMLWriter xml;
        dmr->print_dap4(xml);
        string dmr_src = string(xml.get_doc());
        DBG(cerr << "DMR SRC: " << endl << dmr_src << endl);

        delete dmr;

        XMLWriter xml2;
        dmr_2->print_dap4(xml2);
        string dmr_dest = string(xml2.get_doc());
        DBG(cerr << "DMR DEST: " << endl << dmr_dest << endl);

        delete dmr_2;
        CPPUNIT_ASSERT(dmr_src == dmr_dest);

        DBG(cerr << __func__ << "() - END" << endl);
    }

    // Test the grid/coverage and copy ctor code
    void test_copy_ctor_3()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        DMR *dmr = build_dmr("coads_climatology.nc.dds", "coads_climatology.nc.das");
        DMR *dmr_2 = new DMR(*dmr);

        XMLWriter xml;
        dmr->print_dap4(xml);
        string dmr_src = string(xml.get_doc());
        DBG(cerr << "DMR SRC: " << endl << dmr_src << endl);

        XMLWriter xml2;
        dmr_2->print_dap4(xml2);
        string dmr_dest = string(xml2.get_doc());
        DBG(cerr << "DMR DEST: " << endl << dmr_dest << endl);

        delete dmr;
        delete dmr_2;
        CPPUNIT_ASSERT(dmr_src == dmr_dest);

        DBG(cerr << __func__ << "() - END" << endl);
    }

    // Make the same test as above, but bypass the DMR ctor that uses a DDS object.
    void test_copy_ctor_4()
    {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        D4BaseTypeFactory factory;
        DMR *dmr = new DMR(&factory, "coads");

        string prefix = string(TEST_SRC_DIR) + "/D4-xml/coads_climatology.nc.xml";
        ifstream ifs(prefix.c_str());
        D4ParserSax2 parser;
        parser.intern(ifs, dmr);

        DMR *dmr_2 = new DMR(*dmr);

        XMLWriter xml;
        dmr->print_dap4(xml);
        string dmr_src = string(xml.get_doc());
        DBG(cerr << "DMR SRC: " << endl << dmr_src << endl);

        XMLWriter xml2;
        dmr_2->print_dap4(xml2);
        string dmr_dest = string(xml2.get_doc());
        DBG(cerr << "DMR DEST: " << endl << dmr_dest << endl);

        delete dmr;
        delete dmr_2;
        CPPUNIT_ASSERT(dmr_src == dmr_dest);

        DBG(cerr << __func__ << "() - END" << endl);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DMRTest);

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;

        case 'h': {     // help - show test names
            cerr << "Usage: DMRTest has the following tests:" << endl;
            const std::vector<Test*> &tests = DMRTest::suite()->getTests();
            unsigned int prefix_len = DMRTest::suite()->getName().append("::").length();
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
            test = DMRTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

//##################################################################################################
#if 0 // old way
int main(int argc, char*argv[]) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    GetOpt getopt(argc, argv, "d");
    int option_char;

    while ((option_char = getopt()) != -1)
    switch (option_char) {
        case 'd':
        debug = 1;  // debug is a static global
        break;
        default:
        break;
    }

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        while (i < argc) {
            test = string("DMRTest::") + argv[i++];
            DBG(cerr << "test: " << test << endl);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    xmlMemoryDump();

    return wasSuccessful ? 0 : 1;
}
#endif

