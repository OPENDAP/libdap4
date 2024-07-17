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
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#ifndef WIN32

#include <unistd.h> // for alarm sleep

#else
#include <fcntl.h>
#include <io.h>
#include <process.h>
#endif

#include "SignalHandler.h"
#include "debug.h"
#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

namespace libdap {

/** Test Handler. This is used with the SignalHandlerTest unit tests. */
class TestHandler : public EventHandler {
public:
    int flag = 0;

    TestHandler() = default;

    void handle_signal(int signum) override {
        DBG(cerr << "signal number " << signum << " received" << endl);
        flag = 1;
    }
};

class SignalHandlerTest : public TestFixture {
private:
    SignalHandler *sh = nullptr;
    std::unique_ptr<TestHandler> th = make_unique<TestHandler>();

public:
    SignalHandlerTest() = default;

    ~SignalHandlerTest() = default;

    void setUp() override { sh = SignalHandler::instance(); }

    // Tests for methods
    void register_handler_test() {
        SignalHandler::register_handler(SIGALRM, th.get());
        CPPUNIT_ASSERT(sh->d_signal_handlers[SIGALRM] == th.get());
    }

    void remove_handler_test() {
        SignalHandler::register_handler(SIGALRM, th.get());
        CPPUNIT_ASSERT(SignalHandler::remove_handler(SIGALRM) == th.get());
    }

    void alarm_test() {
        SignalHandler::register_handler(SIGALRM, th.get(), true);
        CPPUNIT_ASSERT(th->flag == 0);
        alarm(1);

        // sleep(2) also works _except_ when run with valgrind; reason
        // unknown. jhrg 4/26/13
        time_t start, end;
        start = end = time(nullptr);
        while (end < start + 2)
            end = time(nullptr);

        DBG(cerr << "Event handler 'flag' value: " << th->flag << endl);
        CPPUNIT_ASSERT(th->flag == 1);
    }

    CPPUNIT_TEST_SUITE(SignalHandlerTest);

    CPPUNIT_TEST(register_handler_test);
    CPPUNIT_TEST(remove_handler_test);
    CPPUNIT_TEST(alarm_test);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SignalHandlerTest);

} // namespace libdap

int main(int argc, char *argv[]) { return run_tests<libdap::SignalHandlerTest>(argc, argv) ? 0 : 1; }
