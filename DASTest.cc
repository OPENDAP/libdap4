
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
 
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

#include "DAS.h"
#include "debug.h"

using namespace CppUnit;

class DASTest: public TestFixture {
private:
    DAS *das, *das2;

public:
    DASTest() {}
    ~DASTest() {}

    void setUp() { 
	das = new DAS();
	das2 = new DAS();
    } 

    void tearDown() { 
	delete das; das = 0;
	delete das2; das2 = 0;
    }

    CPPUNIT_TEST_SUITE( DASTest );

    CPPUNIT_TEST(error_values_test);
    CPPUNIT_TEST(symbol_name_test);

    CPPUNIT_TEST_SUITE_END();

    void error_values_test() {
	try {
	    das->parse("das-testsuite/bad_value_test.1");
	    DBG2(das->print(stderr));
	}
	catch (Error &e) {
	    e.display_message();
	    CPPUNIT_ASSERT(!"Caught an unexpected Error object.");
	}
    }

    void symbol_name_test() {
	das->parse("das-testsuite/test.34");
	CPPUNIT_ASSERT(das->get_table("var1")->get_attr("y#z", 0) == "15");

	string s = das->get_table("var1.component1.inner component")->get_attr("tag");
	string *str = new string("\"xyz123\"");
	CPPUNIT_ASSERT(s == str->c_str());
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DASTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}
