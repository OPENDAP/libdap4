
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

#define DODS_DEBUG
#include "DDXParser.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

class DDXParserTest:public TestFixture {
private:
    DDXParser *ddx_parser;
    DDS *dds;

public:
    DDXParserTest() {} 
    ~DDXParserTest() {} 

    void setUp() {
	ddx_parser = new DDXParser;
	dds = new DDS;
    } 

    void tearDown() {
	delete ddx_parser;
	delete dds;
    }

    CPPUNIT_TEST_SUITE( DDXParserTest );

    CPPUNIT_TEST(intern_test);
#if 0
    CPPUNIT_TEST(errant_database_test);
#endif

    CPPUNIT_TEST_SUITE_END();

    void intern_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.01.ddx", dds);
	    CPPUNIT_ASSERT(dds->get_dataset_name() == "SimpleTypes");
	    dds->print_xml(stdout, false, "    ");
	}
	catch (DDXParseFailed &e) {
	    cerr << endl << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Parse failed.");
	}
    }

    void errant_database_test() {
	try {
	    ddx_parser->intern("ddx-testsuite/test.01.error.ddx", dds);
	    CPPUNIT_ASSERT(!"test.01.error.ddx should fail!");
	}
	catch (DDXParseFailed &e) {
	    DBG(cerr << "Error: " << e.get_error_message() << endl);
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DDXParserTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: DDXParserTest.cc,v $
// Revision 1.1  2003/05/29 19:07:15  jimg
// Added.
//
