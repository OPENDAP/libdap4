
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

#include <sstream>

#include "DDS.h"

#include "Regex.h"
#include "util.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

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

    bool re_match(Regex &r, const string &s) {
	int match = r.match(s.c_str(), s.length());
	DBG(cerr << "Match: " << match << " should be: " << s.length() 
	    << endl);
	return match == static_cast<int>(s.length());
    }

    CPPUNIT_TEST_SUITE( DDSTest );

    CPPUNIT_TEST(symbol_name_test);
    CPPUNIT_TEST(print_xml_test);
    CPPUNIT_TEST(print_xml_test2);
    CPPUNIT_TEST(transfer_attributes_test);
    CPPUNIT_TEST(add_global_attribute_test);

    CPPUNIT_TEST_SUITE_END();

    // This is here mostly to test for memory leaks using valgrind. Does 
    // DDS::add_global_attribute() leak memory? Apparently not. jhrg 3/18/05
    void add_global_attribute_test() {
        DAS das;
        das.parse("das-testsuite/test.1.das");
        AttrTable::Attr_iter i = das.attr_begin();
        CPPUNIT_ASSERT(i != das.attr_end());
        
        dds1->parse("dds-testsuite/test.1");
        dds1->add_global_attribute(*i);
        
        CPPUNIT_ASSERT(dds1->d_attr.get_size() == 1);
    }

    void transfer_attributes_test() {
        try {
                dds1->parse("dds-testsuite/fnoc1.nc.dds");
                DAS das;
                das.parse("dds-testsuite/fnoc1.nc.das");
                dds1->transfer_attributes(&das);
                
                AttrTable &at = dds1->get_attr_table();
                AttrTable::Attr_iter i = at.attr_begin();
                CPPUNIT_ASSERT(at.get_name(i) == "NC_GLOBAL");
                CPPUNIT_ASSERT(at.get_name(++i) == "DODS_EXTRA");
        }
        catch (Error &e) {
            cout << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error thrown!");
        }
        
        try {
                dds2->parse("dds-testsuite/3B42.980909.5.HDF.dds");
                DAS das;
                das.parse("dds-testsuite/3B42.980909.5.hacked.HDF.das");
                dds2->transfer_attributes(&das);
                
                AttrTable &at = dds2->get_attr_table();
                AttrTable::Attr_iter i = at.attr_begin();
                CPPUNIT_ASSERT(at.get_name(i) == "HDF_GLOBAL");
                CPPUNIT_ASSERT(at.get_name(++i) == "CoreMetadata");
        }
        catch (Error &e) {
            cout << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error thrown!");
        }

        try {
                DDS dds;
                dds.parse("dds-testsuite/S2000415.HDF.dds");
                DAS das;
                das.parse("dds-testsuite/S2000415.HDF.das");
                dds.transfer_attributes(&das);
                
                AttrTable &at = dds.get_attr_table();
                AttrTable::Attr_iter i = at.attr_begin();
                CPPUNIT_ASSERT(at.get_name(i) == "HDF_GLOBAL");
                AttrTable &at2 = dds.var("WVC_Lat")->get_attr_table();
                DBG(at2.print(cerr));
                CPPUNIT_ASSERT(at2.get_name(at2.attr_begin()) == "long_name");
        }
        catch (Error &e) {
            cout << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error thrown!");
        }
    }
    
    void symbol_name_test() {
	try {
	    // read a DDS.
	    dds1->parse("dds-testsuite/test.18");
	    CPPUNIT_ASSERT(dds1->var("oddTemp"));
	
	    dds2->parse("dds-testsuite/test.19b");
	    CPPUNIT_ASSERT(dds2->var("b#c"));
	    CPPUNIT_ASSERT(dds2->var("b%23c"));
	    CPPUNIT_ASSERT(dds2->var("huh.Image#data"));
	    CPPUNIT_ASSERT(dds2->var("c d"));
	    CPPUNIT_ASSERT(dds2->var("c%20d"));
	}
	catch (Error &e) {
	    cerr << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Caught unexpected Error object");
	}
    }

    void print_xml_test() {
	dds2->parse("dds-testsuite/test.19b");

	char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
	FILE *tmp = get_temp_file(DDSTemp);
	dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

	Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19\"\n\
xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n\
xmlns=\"http://www..*.org/ns/DODS\"\n\
xsi:schemaLocation=\"http://www..*.org/ns/DODS  .*\">\n\
\n\
\n\
    <Int32 name=\"a\"/>\n\
    <Array name=\"b#c\">\n\
        <Int32/>\n\
        <dimension size=\"10\"/>\n\
    </Array>\n\
    <Float64 name=\"c d\"/>\n\
    <Grid name=\"huh\">\n\
        <Array name=\"Image#data\">\n\
            <Byte/>\n\
            <dimension size=\"512\"/>\n\
        </Array>\n\
        <Map name=\"colors\">\n\
            <String/>\n\
            <dimension size=\"512\"/>\n\
        </Map>\n\
    <Grid/>\n\
\n\
    <dodsBLOB URL=\"http://localhost/dods/test.xyz\"/>\n\
</Dataset>\n");
	CPPUNIT_ASSERT(re_match(r, file_to_string(tmp)));
        
        fclose(tmp);
	remove(DDSTemp);
    }

    void print_xml_test2() {
	dds2->parse("dds-testsuite/test.19b");
	DAS das;
	das.parse("dds-testsuite/test.19b.das");

	dds2->transfer_attributes(&das);

	char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
	FILE *tmp = get_temp_file(DDSTemp);
	dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

	string output = file_to_string(tmp);
	DBG(cerr << output << endl);

	Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19\"\n\
xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n\
xmlns=\"http://www.dods.org/ns/DODS\"\n\
xsi:schemaLocation=\"http://www.dods.org/ns/DODS  http://argon.coas.oregonstate.edu/ndp/dods.xsd\">\n\
\n\
    <Attribute name=\"NC_GLOBAL\" type=\"Container\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>&quot;Attribute merge test&quot;</value>\n\
        </Attribute>\n\
        <Attribute name=\"primes\" type=\"Int32\">\n\
            <value>2</value>\n\
            <value>3</value>\n\
            <value>5</value>\n\
            <value>7</value>\n\
            <value>11</value>\n\
        </Attribute>\n\
    </Attribute>\n\
\n\
    <Int32 name=\"a\"/>\n\
    <Array name=\"b#c\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>&quot;b pound c&quot;</value>\n\
        </Attribute>\n\
        <Int32/>\n\
        <dimension size=\"10\"/>\n\
    </Array>\n\
    <Float64 name=\"c d\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>&quot;c d with a WWW escape sequence&quot;</value>\n\
        </Attribute>\n\
        <Attribute name=\"sub\" type=\"Container\">\n\
            <Attribute name=\"about\" type=\"String\">\n\
                <value>&quot;Attributes inside attributes&quot;</value>\n\
            </Attribute>\n\
            <Attribute name=\"pi\" type=\"Float64\">\n\
                <value>3.1415</value>\n\
            </Attribute>\n\
        </Attribute>\n\
    </Float64>\n\
    <Grid name=\"huh\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>&quot;The Grid huh&quot;</value>\n\
        </Attribute>\n\
        <Array name=\"Image#data\">\n\
            <Byte/>\n\
            <dimension size=\"512\"/>\n\
        </Array>\n\
        <Map name=\"colors\">\n\
            <Attribute name=\"long_name\" type=\"String\">\n\
                <value>&quot;The color map vector&quot;</value>\n\
            </Attribute>\n\
            <String/>\n\
            <dimension size=\"512\"/>\n\
        </Map>\n\
    <Grid/>\n\
\n\
    <dodsBLOB URL=\"http://localhost/dods/test.xyz\"/>\n\
</Dataset>\n");

	CPPUNIT_ASSERT(re_match(r, output));
	remove(DDSTemp);
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



