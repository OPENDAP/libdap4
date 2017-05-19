// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
// Author: Nathan David Potter <ndp@opendap.org>
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

//#include <cstdio>

#include <pthread.h>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "debug.h"
#include "util.h"

#include "GetOpt.h"
#include "BaseType.h"
#include "Str.h"
#include "DDS.h"
#include "ServerFunction.h"
#include "ServerFunctionsList.h"

using namespace CppUnit;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

void sflut(int, libdap::BaseType *[], libdap::DDS &, libdap::BaseType **btpp)
{
    string info = string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") + "<function name=\"ugr4\" version=\"0.1\">\n"
        + "ServeFunctionsList Unit Test.\n" + "usage: sflut()" + "\n" + "</function>";

    libdap::Str *response = new libdap::Str("info");
    response->set_value(info);
    *btpp = response;
    return;

}

class SFLUT: public libdap::ServerFunction {
public:
    SFLUT()
    {
        setName("sflut");
        setDescriptionString("This is a unit test to test the ServerFunctionList class.");
        setUsageString("sflut()");
        setRole("http://services.opendap.org/dap4/unit-tests/ServerFunctionList");
        setDocUrl("http://docs.opendap.org/index.php/unit-tests");
        setFunction(sflut);
        setVersion("1.0");

    }
};

namespace libdap {
class ServerFunctionsListUnitTest: public CppUnit::TestFixture {

public:

    // Called once before everything gets tested
    ServerFunctionsListUnitTest()
    {
        //    DBG(cerr << " BindTest - Constructor" << endl);

    }

    // Called at the end of the test
    ~ServerFunctionsListUnitTest()
    {
        //    DBG(cerr << " BindTest - Destructor" << endl);
    }

    // Called before each test
    void setup()
    {
        //    DBG(cerr << " BindTest - setup()" << endl);
    }

    // Called after each test
    void tearDown()
    {
        //    DBG(cerr << " tearDown()" << endl);
    }

    CPPUNIT_TEST_SUITE (libdap::ServerFunctionsListUnitTest);

    CPPUNIT_TEST (sflut_test);
    //CPPUNIT_TEST(always_pass);

    CPPUNIT_TEST_SUITE_END();

    void printFunctionNames()
    {
        vector<string> *names = new vector<string>();
        printFunctionNames(names);
        delete names;
    }

    void printFunctionNames(vector<string> *names)
    {
        DBG(
            cerr << "Server_Function_List_Unit_Test::printFunctionNames() - ServerFunctionList::getFunctionNames(): "
                << endl);
        if (names->empty()) {
            DBG(cerr << "     Function list is empty." << endl);
            return;
        }

        for (size_t i = 0; i < names->size(); i++) {
            DBG(cerr << "   name[" << i << "]: " << (*names)[i] << endl);
        }
    }
    void always_pass()
    {
        CPPUNIT_ASSERT(true);
    }

    void sflut_test()
    {
        DBG(cerr << endl);

        SFLUT *ssf = new SFLUT();
        ssf->setName("Server_Function_List_Unit_Test");

        vector<string> names;
        libdap::ServerFunctionsList::TheList()->getFunctionNames(&names);
        printFunctionNames(&names);

        CPPUNIT_ASSERT(names.size() == 0);

        DBG(cerr << "ServerFunctionsListUnitTest::sflut_test() - Adding function(): " << ssf->getName() << endl);
        libdap::ServerFunctionsList::TheList()->add_function(ssf);

        names.clear();
        libdap::ServerFunctionsList::TheList()->getFunctionNames(&names);
        printFunctionNames(&names);
        CPPUNIT_ASSERT(names.size() == 1);

#if 0
        DBG(cerr << "ServerFunctionsListUnitTest::sflut_test() - Deleting the List." << endl);
        libdap::ServerFunctionsList::delete_instance();

        // This is needed because we used pthread_once to ensure that
        // initialize_instance() is called at most once. We manually call
        // the delete method, so the object must be remade. This would never
        // be done by non-test code. jhrg 5/2/13
        libdap::ServerFunctionsList::initialize_instance();

        names.clear();
        libdap::ServerFunctionsList::TheList()->getFunctionNames(&names);
        printFunctionNames(&names);
        CPPUNIT_ASSERT(names.size()==0);
#endif

    }

};
} // libdap namespace

// BindTest

CPPUNIT_TEST_SUITE_REGISTRATION (libdap::ServerFunctionsListUnitTest);

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
            cerr << "Usage: ServerFunctionsListUnitTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::ServerFunctionsListUnitTest::suite()->getTests();
            unsigned int prefix_len = libdap::ServerFunctionsListUnitTest::suite()->getName().append("::").length();
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
            test = libdap::ServerFunctionsListUnitTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

