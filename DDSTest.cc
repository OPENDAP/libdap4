
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

#include "DDS.h"

using namespace CppUnit;

class DDSTest : public TestFixture {
private:
    DDS *dds1, *dds2;

public: 
    DDSTest() {}
    ~DDSTest() {}

    void setUp() {
	dds1 = new DDS("test1");
	dds2 = new DDS("test2");
    }

    void tearDown() {
	delete dds1; dds1 = 0;
	delete dds2; dds2 = 0;
    }

    CPPUNIT_TEST_SUITE( DDSTest );

    CPPUNIT_TEST(symbol_name_test);

    CPPUNIT_TEST_SUITE_END();

    void symbol_name_test() {
	// read a DDS.
	dds1->parse("dds-testsuite/test.18");
	assert(dds1->var("oddTemp"));
	
	dds2->parse("dds-testsuite/test.19b");
	assert(dds2->var("b#c"));
	assert(!dds2->var("b%23c"));
	assert(dds2->var("huh.Image#data"));
	assert(dds2->var("c d"));
	assert(!dds2->var("c%20d"));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DDSTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}



