
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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

// Tests for Byte. Tests features of BaseType, too. 7/19/2001 jhrg

#include <strstream.h>

#include "Byte.h"

using namespace CppUnit;

class ByteTest : public TestFixture {
private:
    Byte *tb1;
    Byte *tb2;
    Byte *tb3;
    Byte *tb4;

public: 
    ByteTest() {}
    ~ByteTest() {}

    void setUp() {
	tb1 = new Byte("tb1");
	tb2 = new Byte("tb2 name with spaces");
	tb3 = new Byte("tb3 %");
	tb4 = new Byte("tb4 #");
    }

    void tearDown() {
	delete tb1; tb1 = 0;
	delete tb2; tb2 = 0;
	delete tb3; tb3 = 0;
	delete tb4; tb4 = 0;
    }

    CPPUNIT_TEST_SUITE( ByteTest );

    CPPUNIT_TEST(name_mangling_test);
    CPPUNIT_TEST(decl_mangling_test);

    CPPUNIT_TEST_SUITE_END();

    void name_mangling_test() {
	CPPUNIT_ASSERT(tb1->name() == "tb1");
	CPPUNIT_ASSERT(tb2->name() == "tb2 name with spaces");
	CPPUNIT_ASSERT(tb3->name() == "tb3 %");
	CPPUNIT_ASSERT(tb4->name() == "tb4 #");
    }

    void decl_mangling_test() {
	ostrstream oss;
	tb1->print_decl(oss, "", false); oss << ends;
	CPPUNIT_ASSERT(string(oss.str()) == "Byte tb1");

	ostrstream oss2;
	tb2->print_decl(oss2, "", false); oss2 << ends;
	CPPUNIT_ASSERT(string(oss2.str()) == "Byte tb2%20name%20with%20spaces");

	ostrstream oss3;
	tb3->print_decl(oss3, "", false); oss3 << ends;
	CPPUNIT_ASSERT(string(oss3.str()) == "Byte tb3%20%25");

	ostrstream oss4;
	tb4->print_decl(oss4, "", false); oss4 << ends;
	CPPUNIT_ASSERT(string(oss4.str()) == "Byte tb4%20%23");
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(ByteTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}



