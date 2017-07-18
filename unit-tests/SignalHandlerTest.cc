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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#ifndef WIN32
#include <unistd.h>  // for alarm sleep
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "GetOpt.h"
#include "SignalHandler.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

namespace libdap {

/** Test Handler. This is used with the SignalHandlerTest unit tests. */
class TestHandler: public EventHandler {
public:
    int flag;

    TestHandler() :
        flag(0)
    {
    }

    virtual void handle_signal(int signum)
    {
        DBG(cerr << "signal number " << signum << " received" << endl);
        flag = 1;
    }
};

class SignalHandlerTest: public TestFixture {
private:
    SignalHandler *sh;
    TestHandler *th;

public:
    SignalHandlerTest()
    {
    }
    ~SignalHandlerTest()
    {
    }

    void setUp()
    {
        sh = SignalHandler::instance();
        th = new TestHandler;
    }

    void tearDown()
    {
        delete th;
        th = 0;
    }

    // Tests for methods
    void register_handler_test()
    {
        sh->register_handler(SIGALRM, th);
        CPPUNIT_ASSERT(sh->d_signal_handlers[SIGALRM] == th);
    }

    void remove_handler_test()
    {
        sh->register_handler(SIGALRM, th);
        CPPUNIT_ASSERT(sh->remove_handler(SIGALRM) == th);
    }

    void alarm_test()
    {
        sh->register_handler(SIGALRM, th, true);
        CPPUNIT_ASSERT(th->flag == 0);
        alarm(1);

        // sleep(2) also works _except_ when run with valgrind; reason
        // unknown. jhrg 4/26/13
        int start, end;
        start = end = time(0);
        while (end < start + 2)
            end = time(0);

        DBG(cerr << "Event handler 'flag' value: " << th->flag << endl);
        CPPUNIT_ASSERT(th->flag == 1);
    }

    CPPUNIT_TEST_SUITE (SignalHandlerTest);

    CPPUNIT_TEST (register_handler_test);
    CPPUNIT_TEST (remove_handler_test);
    CPPUNIT_TEST (alarm_test);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (SignalHandlerTest);

}

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
            cerr << "Usage: SignalHandlerTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::SignalHandlerTest::suite()->getTests();
            unsigned int prefix_len = libdap::SignalHandlerTest::suite()->getName().append("::").length();
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
        for ( ; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = libdap::SignalHandlerTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

