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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

//#define DODS_DEBUG

#include <GetOpt.h>
#include <GNURegex.h>
#include <util.h>
#include <debug.h>

#include "D4AsyncUtil.h"
#include "XMLWriter.h"
#include "debug.h"

#include "testFile.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

class D4AsyncDocTest: public TestFixture {
private:
    XMLWriter *xml;

public:
    D4AsyncDocTest() :
        xml(0)
    {
    }

    ~D4AsyncDocTest()
    {
    }

    void setUp()
    {
        xml = new XMLWriter;

    }

    void tearDown()
    {
        delete xml;
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_required()
    {
        DBG(cerr << endl << " ---- test_async_required - BEGIN" << endl);

        D4AsyncUtil dau;
        DBG(cerr << endl);

        string *stylesheet_ref = 0;

        dau.writeD4AsyncRequired(*xml, 6001, 600007, stylesheet_ref);

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_required - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncRequired.xml");
        DBG(cerr << "[test_async_required - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);

        DBG(cerr << endl << " ---- test_async_required - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_required_with_stylesheet_ref()
    {
        DBG(cerr << endl << " ---- test_async_required - BEGIN" << endl);

        D4AsyncUtil dau;
        DBG(cerr << endl);

        string stylesheet_ref = "http://someServer:8080/opendap/xsl/asyncResponse.xsl";

        dau.writeD4AsyncRequired(*xml, 6001, 600007, &stylesheet_ref);

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_required - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncRequired_StyleRef.xml");
        DBG(cerr << "[test_async_required - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);

        DBG(cerr << endl << " ---- test_async_required - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_accepted()
    {
        DBG(cerr << endl << " ---- test_async_accepted - BEGIN" << endl);

        D4AsyncUtil dau;
        DBG(cerr << endl);

        string stylesheet_ref = "http://someServer:8080/opendap/xsl/asyncResponse.xsl";

        dau.writeD4AsyncAccepted(*xml, 6003, 600009,
            "http://test.opendap.org:8080/opendap/storedResults/result_87697163.dap");

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_accepted - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncAccepted.xml");
        DBG(cerr << "[test_async_accepted - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
        DBG(cerr << endl << " ---- test_async_accepted - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_accepted_with_stylesheet_ref()
    {
        DBG(cerr << endl << " ---- test_async_accepted - BEGIN" << endl);

        D4AsyncUtil dau;
        DBG(cerr << endl);
        string stylesheet_ref = "http://someServer:8080/opendap/xsl/asyncResponse.xsl";

        dau.writeD4AsyncAccepted(*xml, 6003, 600009,
            "http://test.opendap.org:8080/opendap/storedResults/result_87697163.dap", &stylesheet_ref);

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_accepted - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncAccepted_StyleRef.xml");
        DBG(cerr << "[test_async_accepted - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
        DBG(cerr << endl << " ---- test_async_accepted - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_pending()
    {
        DBG(cerr << endl << " ---- test_async_pending - BEGIN" << endl);

        D4AsyncUtil dau;
        DBG(cerr << endl);

        dau.writeD4AsyncPending(*xml);

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_pending - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncPending.xml");
        DBG(cerr << "[test_async_pending - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
        DBG(cerr << endl << " ---- test_async_pending - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_pending_with_stylesheet_ref()
    {
        DBG(cerr << endl << " ---- test_async_pending - BEGIN" << endl);

        D4AsyncUtil dau;
        DBG(cerr << endl);
        string stylesheet_ref = "http://someServer:8080/opendap/xsl/asyncResponse.xsl";

        dau.writeD4AsyncPending(*xml, &stylesheet_ref);

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_pending - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncPending_StyleRef.xml");
        DBG(cerr << "[test_async_pending - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
        DBG(cerr << endl << " ---- test_async_pending - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_ResponseGone()
    {
        DBG(cerr << endl << " ---- test_async_ResponseGone - BEGIN" << endl);

        D4AsyncUtil dau;
        DBG(cerr << endl);

        dau.writeD4AsyncResponseGone(*xml);

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_ResponseGone - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncGone.xml");
        DBG(cerr << "[test_async_ResponseGone - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
        DBG(cerr << endl << " ---- test_async_ResponseGone - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_ResponseGone_with_stylesheet_ref()
    {
        DBG(cerr << endl << " ---- test_async_ResponseGone - BEGIN" << endl);

        D4AsyncUtil dau;
        DBG(cerr << endl);
        string stylesheet_ref = "http://someServer:8080/opendap/xsl/asyncResponse.xsl";

        dau.writeD4AsyncResponseGone(*xml, &stylesheet_ref);

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_ResponseGone - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncGone_StyleRef.xml");
        DBG(cerr << "[test_async_ResponseGone - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
        DBG(cerr << endl << " ---- test_async_ResponseGone - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_ResponseRejected()
    {

        D4AsyncUtil dau;
        DBG(cerr << endl << " ---- test_async_ResponseRejected - BEGIN" << endl);

        dau.writeD4AsyncResponseRejected(*xml, TIME,
            "The time to process your request is longer than the time you indicated was acceptable.");

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_ResponseRejected - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncRejected.xml");
        DBG(cerr << "[test_async_ResponseRejected - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);

        DBG(cerr << endl << " ---- test_async_ResponseRejected - END" << endl);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_async_ResponseRejected_with_stylesheet_ref()
    {

        D4AsyncUtil dau;
        DBG(cerr << endl << " ---- test_async_ResponseRejected - BEGIN" << endl);
        string stylesheet_ref = "http://someServer:8080/opendap/xsl/asyncResponse.xsl";

        dau.writeD4AsyncResponseRejected(*xml, TIME,
            "The time to process your request is longer than the time you indicated was acceptable.", &stylesheet_ref);

        string doc = xml->get_doc();
        DBG(cerr << "[test_async_ResponseRejected - candidate doc]" << endl << doc << endl);

        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4AsyncRejected_StyleRef.xml");
        DBG(cerr << "[test_async_ResponseRejected - baseline doc]" << endl << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);

        DBG(cerr << endl << " ---- test_async_ResponseRejected - END" << endl);
    }

    CPPUNIT_TEST_SUITE (D4AsyncDocTest);

    CPPUNIT_TEST (test_async_required);
    CPPUNIT_TEST (test_async_required_with_stylesheet_ref);
    CPPUNIT_TEST (test_async_accepted);
    CPPUNIT_TEST (test_async_accepted_with_stylesheet_ref);
    CPPUNIT_TEST (test_async_pending);
    CPPUNIT_TEST (test_async_pending_with_stylesheet_ref);
    CPPUNIT_TEST (test_async_ResponseGone);
    CPPUNIT_TEST (test_async_ResponseGone_with_stylesheet_ref);
    CPPUNIT_TEST (test_async_ResponseRejected);
    CPPUNIT_TEST (test_async_ResponseRejected_with_stylesheet_ref);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4AsyncDocTest);

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
            cerr << "Usage: D4AsyncDocTest has the following tests:" << endl;
            const std::vector<Test*> &tests = D4AsyncDocTest::suite()->getTests();
            unsigned int prefix_len = D4AsyncDocTest::suite()->getName().append("::").length();
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
            test = D4AsyncDocTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

#if 0

int main(int, char**) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = runner.run("", false);

    return wasSuccessful ? 0 : 1;
}
#endif
