
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
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

#include <unistd.h>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "AISResources.h"
#include "debug.h"

using namespace CppUnit;

class AISResourcesTest:public TestFixture {
private:
    AISResources *ais;
    string fnoc1, fnoc2, fnoc3;
    string fnoc1_ais, fnoc2_ais, fnoc3_ais;

public:
    AISResourcesTest() {} 
    ~AISResourcesTest() {} 

    void setUp() {
	fnoc1 = "http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc";
	fnoc2 = "http://localhost/dods-test/nph-dods/data/nc/fnoc2.nc";
	fnoc3 = "http://localhost/dods-test/nph-dods/data/nc/fnoc3.nc";
	fnoc1_ais = "http://localhost/ais/fnoc1.nc.das";
	fnoc2_ais = "ais_testsuite/fnoc2_replace.das";
	fnoc3_ais = "http://localhost/ais/fnoc3_fallback.das";

	ais = new AISResources;

	Resource r1(fnoc1_ais);
	ais->add_resource(fnoc1, r1);
	Resource r2(fnoc2_ais, fallback);
	ais->add_resource(fnoc2, r2);
	ais->add_resource(fnoc3, r1);
	ais->add_resource(fnoc3, r2);
    } 

    void tearDown() { 
	delete ais;
    }

#if 0
    bool re_match(Regex &r, const char *s) {
	return r.match(s, strlen(s)) == (int)strlen(s);
    }
#endif

    CPPUNIT_TEST_SUITE( AISResourcesTest );

    CPPUNIT_TEST(add_resource_test);
    CPPUNIT_TEST(add_resource_vector_test);
    CPPUNIT_TEST(is_resource_test);
    CPPUNIT_TEST(get_resource_test);
    CPPUNIT_TEST(read_database_test);
    CPPUNIT_TEST(write_database_test);

    CPPUNIT_TEST_SUITE_END();

    void add_resource_test() {
	// setUp() makes the add_resource calls. 02/13/03 jhrg
	CPPUNIT_ASSERT(ais->d_db.find(fnoc1) != ais->d_db.end());
	CPPUNIT_ASSERT(ais->d_db.find(fnoc1)->second.size() == 1);
	CPPUNIT_ASSERT(ais->d_db.find(fnoc1)->second[0].get_url() == fnoc1_ais);
	CPPUNIT_ASSERT(ais->d_db.find(fnoc1)->second[0].get_rule() == overwrite);

	CPPUNIT_ASSERT(ais->d_db.find(fnoc2) != ais->d_db.end());
	CPPUNIT_ASSERT(ais->d_db.find(fnoc2)->second.size() == 1);
	CPPUNIT_ASSERT(ais->d_db.find(fnoc2)->second[0].get_url() == fnoc2_ais);
	CPPUNIT_ASSERT(ais->d_db.find(fnoc2)->second[0].get_rule() == fallback);

	CPPUNIT_ASSERT(ais->d_db.find(fnoc3) != ais->d_db.end());
	CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second.size() == 2);
	CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second[0].get_url() == fnoc1_ais);
	CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second[1].get_url() == fnoc2_ais);
	CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second[0].get_rule() == overwrite);
	CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second[1].get_rule() == fallback);
    }

    void add_resource_vector_test() {
	AISResources *ais2 = new AISResources;

	Resource r1(fnoc1_ais);
	ResourceVector rv1(1, r1);
	ais2->add_resource(fnoc2, rv1);

	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2) != ais2->d_db.end());
	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second.size() == 1);
	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[0].get_url() == fnoc1_ais);
	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[0].get_rule() == overwrite);

	Resource r2(fnoc2_ais, fallback);
	ResourceVector rv2(1, r2);
	ais2->add_resource(fnoc2, rv2);


	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2) != ais2->d_db.end());
	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second.size() == 2);
	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[0].get_url() == fnoc1_ais);
	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[1].get_url() == fnoc2_ais);
	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[0].get_rule() == overwrite);
	CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[1].get_rule() == fallback);
    }

    void is_resource_test() {
	CPPUNIT_ASSERT(ais->is_resource(fnoc1));
	CPPUNIT_ASSERT(ais->is_resource(fnoc2));
	CPPUNIT_ASSERT(ais->is_resource(fnoc3));
    }

    void get_resource_test() {
	ResourceVector trv1 = ais->get_resource(fnoc1);
	CPPUNIT_ASSERT(trv1.size() == 1);
	CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
	CPPUNIT_ASSERT(trv1[0].get_rule() == overwrite);

	ResourceVector trv2 = ais->get_resource(fnoc2);
	CPPUNIT_ASSERT(trv2.size() == 1);
	CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
	CPPUNIT_ASSERT(trv2[0].get_rule() == fallback);

	ResourceVector trv3 = ais->get_resource(fnoc3);
	CPPUNIT_ASSERT(trv3.size() == 2);
	CPPUNIT_ASSERT(trv3[0].get_url() == fnoc1_ais);
	CPPUNIT_ASSERT(trv3[0].get_rule() == overwrite);
	CPPUNIT_ASSERT(trv3[1].get_url() == fnoc2_ais);
	CPPUNIT_ASSERT(trv3[1].get_rule() == fallback);

	try {
	    ResourceVector trv4 = ais->get_resource("http://never");
	    CPPUNIT_ASSERT(!"get_resource() failed to throw NoSuchPrimaryResource");
	}
	catch (NoSuchPrimaryResource &nspr) {
	    CPPUNIT_ASSERT("get_resource() correctly threw NoSuchPrimaryResource");
	}
    }

    void read_database_test() {
	try {
	    AISResources *ais2 = new AISResources;
	    ais2->read_database("ais_testsuite/ais_database.xml");

	    ResourceVector trv1 = ais2->get_resource(fnoc1);
	    CPPUNIT_ASSERT(trv1.size() == 1);
	    CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
	    CPPUNIT_ASSERT(trv1[0].get_rule() == overwrite);

	    ResourceVector trv2 = ais2->get_resource(fnoc2);
	    CPPUNIT_ASSERT(trv2.size() == 1);
	    CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
	    CPPUNIT_ASSERT(trv2[0].get_rule() == replace);

	    ResourceVector trv3 = ais2->get_resource(fnoc3);
	    CPPUNIT_ASSERT(trv3.size() == 1);
	    CPPUNIT_ASSERT(trv3[0].get_url() == fnoc3_ais);
	    CPPUNIT_ASSERT(trv3[0].get_rule() == fallback);
	}
	catch (AISDatabaseReadFailed &adrf) {
	    CPPUNIT_ASSERT(!"Document not well formed and/or valid!");
	}
    }

    void write_database_test() {
	try {
	    ais->write_database("dummy.xml");
	    AISResources *ais2 = new AISResources;

	    ais2->read_database("ais_testsuite/ais_database.xml");

	    ResourceVector trv1 = ais2->get_resource(fnoc1);
	    CPPUNIT_ASSERT(trv1.size() == 1);
	    CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
	    CPPUNIT_ASSERT(trv1[0].get_rule() == overwrite);

	    ResourceVector trv2 = ais2->get_resource(fnoc2);
	    CPPUNIT_ASSERT(trv2.size() == 1);
	    CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
	    CPPUNIT_ASSERT(trv2[0].get_rule() == replace);

	    ResourceVector trv3 = ais2->get_resource(fnoc3);
	    CPPUNIT_ASSERT(trv3.size() == 1);
	    CPPUNIT_ASSERT(trv3[0].get_url() == fnoc3_ais);
	    CPPUNIT_ASSERT(trv3[0].get_rule() == fallback);
	}
	catch (AISDatabaseReadFailed &adrf) {
	    CPPUNIT_ASSERT(!"Document not well formed and/or valid!");
	}
	catch (AISDatabaseWriteFailed &adwf) {
	    CPPUNIT_ASSERT(!"Write failed!");
	}

	try {
	    ais->write_database("/dev/dummy.xml");
	    CPPUNIT_ASSERT(!"Write succeeded, but should not!");
	}
	catch (AISDatabaseWriteFailed &adwf) {
	    CPPUNIT_ASSERT("Write failed as intended");
	}

    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AISResourcesTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    unlink("dummy.xml");

    return 0;
}

// $Log: AISResourcesTest.cc,v $
// Revision 1.3  2003/02/25 23:25:55  jimg
// Fixed for the latest rev of ais_database.xml.
//
// Revision 1.2  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.1  2003/02/20 23:05:57  jimg
// Added.
//
