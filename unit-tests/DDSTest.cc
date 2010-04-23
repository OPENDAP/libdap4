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

#include "config.h"

//#define DODS_DEBUG2
// #define DODS_DEBUG

#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "DDS.h"

#include "GNURegex.h"
#include "util.h"
#include "debug.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

namespace libdap {

class DDSTest: public TestFixture {
    private:
        DDS *dds1, *dds2;
        BaseTypeFactory factory;

    public:
        DDSTest()
        {
        }
        ~DDSTest()
        {
        }

        void setUp()
        {
            //factory = new BaseTypeFactory;
            dds1 = new DDS(&factory, "test1");
            dds2 = new DDS(&factory, "test2");
        }

        void tearDown()
        {
            delete dds1;
            dds1 = 0;
            delete dds2;
            dds2 = 0;
            //delete factory; factory = 0;
        }

        bool re_match(Regex &r, const string &s)
        {
            int match = r.match(s.c_str(), s.length());
            DBG(cerr << "Match: " << match << " should be: " << s.length()
                    << endl);
            return match == static_cast<int> (s.length());
        }

        // The tests commented exercise features no longer supported
        // by libdap. In particular, a DAS must now be properly structured
        // to work with transfer_attributes() - if a handler builds a malformed
        // DAS, it will need to specialize the BaseType::transfer_attributes()
        // method.
        CPPUNIT_TEST_SUITE( DDSTest );
#if 1
        CPPUNIT_TEST(transfer_attributes_test_1);
        CPPUNIT_TEST(transfer_attributes_test_2);

        CPPUNIT_TEST(symbol_name_test);
        // These test both transfer_attributes() and print_xml()
        CPPUNIT_TEST(print_xml_test);
        CPPUNIT_TEST(print_xml_test2);
        CPPUNIT_TEST(print_xml_test3);
#endif
        // CPPUNIT_TEST(print_xml_test3_1);
#if 1
        CPPUNIT_TEST(print_xml_test4);

        CPPUNIT_TEST(print_xml_test5);
#endif
        // CPPUNIT_TEST(print_xml_test5_1);
#if 1
        CPPUNIT_TEST(print_xml_test6);
#endif
        // CPPUNIT_TEST(print_xml_test7);

        CPPUNIT_TEST_SUITE_END();

        void transfer_attributes_test_1() {
            try {
                dds1->parse((string)TEST_SRC_DIR + "/dds-testsuite/fnoc1.nc.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/fnoc1.nc.das");
                dds1->transfer_attributes(&das);

                DBG2(dds1->print_xml(cerr, false, ""));

                AttrTable &at = dds1->get_attr_table();
                AttrTable::Attr_iter i = at.attr_begin();
                CPPUNIT_ASSERT(i != at.attr_end() && at.get_name(i) == "NC_GLOBAL");
                CPPUNIT_ASSERT(i != at.attr_end() && at.get_name(++i) == "DODS_EXTRA");
            }
            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error thrown!");
            }
        }

        void transfer_attributes_test_2() {
            try {
                dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/3B42.980909.5.HDF.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/3B42.980909.5.hacked.HDF.das");
                dds2->transfer_attributes(&das);

                DBG2(dds2->print_xml(cerr, false, ""));

                AttrTable &at = dds2->get_attr_table();
                AttrTable::Attr_iter i = at.attr_begin();
                CPPUNIT_ASSERT(i != at.attr_end() && at.get_name(i) == "HDF_GLOBAL");
                CPPUNIT_ASSERT(i != at.attr_end() && at.get_name(++i) == "CoreMetadata");
            }
            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error thrown!");
            }
        }

        void symbol_name_test() {
            try {
                // read a DDS.
                dds1->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.18");
                CPPUNIT_ASSERT(dds1->var("oddTemp"));

                dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19b");
                CPPUNIT_ASSERT(dds2->var("b#c"));
                CPPUNIT_ASSERT(dds2->var("b%23c"));
                CPPUNIT_ASSERT(dds2->var("huh.Image#data"));
                CPPUNIT_ASSERT(dds2->var("c d"));
                CPPUNIT_ASSERT(dds2->var("c%20d"));
            }
            catch (Error &e) {
                cerr << e.get_error_message() << endl;
                CPPUNIT_FAIL("Caught unexpected Error object");
            }
        }

        void print_xml_test() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19b");
            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");
            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19\"\n\
.*\
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
    </Grid>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");
            CPPUNIT_ASSERT(re_match(r, oss.str()));
            }

        void print_xml_test2() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19c");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19c.das");

            dds2->transfer_attributes(&das);

            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");

            DBG2(cerr << oss.str() << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19c\"\n\
.*\
\n\
    <Attribute name=\"NC_GLOBAL\" type=\"Container\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>Attribute merge test</value>\n\
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
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, oss.str()));

        }

        void print_xml_test3() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19d");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19d.das");

            dds2->transfer_attributes(&das);

            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");

            DBG2(cerr << oss.str() << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19d\"\n\
.*\
    <Array name=\"b#c\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>b pound c</value>\n\
        </Attribute>\n\
        <Int32/>\n\
        <dimension size=\"10\"/>\n\
    </Array>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, oss.str()));
        }

        // This tests the HDF4 <var>_dim_n attribute. support for that was
        // moved to the handler itself.
        void print_xml_test3_1() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19d");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19d1.das");

            dds2->transfer_attributes(&das);

            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");

            DBG2(cerr << oss.str() << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19d\"\n\
.*\
    <Array name=\"b#c\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>b pound c</value>\n\
        </Attribute>\n\
        <Attribute name=\"dim_0\" type=\"Container\">\n\
            <Attribute name=\"add_offset\" type=\"Float64\">\n\
                <value>0.125</value>\n\
            </Attribute>\n\
        </Attribute>\n\
        <Int32/>\n\
        <dimension size=\"10\"/>\n\
    </Array>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, oss.str()));
        }

        void print_xml_test4() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19e");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19e.das");

            dds2->transfer_attributes(&das);

            DBG(AttrTable &at2 = dds2->var("c%20d")->get_attr_table());
            DBG(at2.print(stderr));

            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");

            DBG(cerr << oss.str() << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19e\"\n\
.*\
    <Float64 name=\"c d\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>c d with a WWW escape sequence</value>\n\
        </Attribute>\n\
        <Attribute name=\"sub\" type=\"Container\">\n\
            <Attribute name=\"about\" type=\"String\">\n\
                <value>Attributes inside attributes</value>\n\
            </Attribute>\n\
            <Attribute name=\"pi\" type=\"Float64\">\n\
                <value>3.1415</value>\n\
            </Attribute>\n\
        </Attribute>\n\
    </Float64>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, oss.str()));
        }

        void print_xml_test5() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19f");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19f.das");

            try {
                dds2->transfer_attributes(&das);
            }
            catch (Error &e) {
                cerr << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error exception");
            }

            DBG(AttrTable &at2 = dds2->var("huh")->get_attr_table());
            DBG(at2.print(stderr));

            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");

            DBG(cerr << oss.str() << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19f\"\n\
.*\
    <Grid name=\"huh\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>The Grid huh</value>\n\
        </Attribute>\n\
        <Array name=\"Image#data\">\n\
            <Byte/>\n\
            <dimension size=\"512\"/>\n\
        </Array>\n\
        <Map name=\"colors\">\n\
            <Attribute name=\"long_name2\" type=\"String\">\n\
                <value>The color map vector</value>\n\
            </Attribute>\n\
            <String/>\n\
            <dimension size=\"512\"/>\n\
        </Map>\n\
    </Grid>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, oss.str()));
        }

        // Tests flat DAS into a DDS; no longer supported by libdap; specialize
        // handlers if they make these malformed DAS objects
        void print_xml_test5_1() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19f");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19f1.das");

            try {
                dds2->transfer_attributes(&das);
            }
            catch (Error &e) {
                cerr << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error exception");
            }

            DBG(AttrTable &at2 = dds2->var("huh")->get_attr_table());
            DBG(at2.print(stderr));

            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");

            DBG(cerr << oss.str() << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19f\"\n\
.*\
    <Grid name=\"huh\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>The Grid huh</value>\n\
        </Attribute>\n\
        <Array name=\"Image#data\">\n\
            <Byte/>\n\
            <dimension size=\"512\"/>\n\
        </Array>\n\
        <Map name=\"colors\">\n\
            <Attribute name=\"long_name2\" type=\"String\">\n\
                <value>The color map vector</value>\n\
            </Attribute>\n\
            <Attribute name=\"units\" type=\"String\">\n\
                <value>m/s</value>\n\
            </Attribute>\n\
            <String/>\n\
            <dimension size=\"512\"/>\n\
        </Map>\n\
    </Grid>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, oss.str()));
        }

        void print_xml_test6() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19b");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19b.das");

            dds2->transfer_attributes(&das);

            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");

            DBG(cerr << oss.str() << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19\"\n\
.*\
    <Attribute name=\"NC_GLOBAL\" type=\"Container\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>Attribute merge test</value>\n\
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
            <value>b pound c</value>\n\
        </Attribute>\n\
        <Int32/>\n\
        <dimension size=\"10\"/>\n\
    </Array>\n\
    <Float64 name=\"c d\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>c d with a WWW escape sequence</value>\n\
        </Attribute>\n\
        <Attribute name=\"sub\" type=\"Container\">\n\
            <Attribute name=\"about\" type=\"String\">\n\
                <value>Attributes inside attributes</value>\n\
            </Attribute>\n\
            <Attribute name=\"pi\" type=\"Float64\">\n\
                <value>3.1415</value>\n\
            </Attribute>\n\
        </Attribute>\n\
    </Float64>\n\
    <Grid name=\"huh\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>The Grid huh</value>\n\
        </Attribute>\n\
        <Array name=\"Image#data\">\n\
            <Byte/>\n\
            <dimension size=\"512\"/>\n\
        </Array>\n\
        <Map name=\"colors\">\n\
            <Attribute name=\"long_name\" type=\"String\">\n\
                <value>The color map vector</value>\n\
            </Attribute>\n\
            <String/>\n\
            <dimension size=\"512\"/>\n\
        </Map>\n\
    </Grid>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, oss.str()));
        }

        // Tests flat DAS into a DDS; no longer supported by libdap; specialize
        // handlers if they make these malformed DAS objects
        void print_xml_test7() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19g");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19g.das");

            try {
                dds2->transfer_attributes(&das);
            }
            catch (Error &e) {
                cerr << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error exception");
            }

            DBG(AttrTable &at2 = dds2->var("huh")->get_attr_table());
            DBG(at2.print(stderr));

            ostringstream oss;
            dds2->print_xml(oss, false, "http://localhost/dods/test.xyz");

            DBG(cerr << oss.str() << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19\"\n\
.*\
    <Structure name=\"s\">\n\
        <Array name=\"b#c\">\n\
            <Attribute name=\"long_name\" type=\"String\">\n\
                <value>b pound c</value>\n\
            </Attribute>\n\
            <Attribute name=\"dim_0\" type=\"Container\">\n\
                <Attribute name=\"add_offset\" type=\"Float64\">\n\
                    <value>0.125</value>\n\
                </Attribute>\n\
            </Attribute>\n\
            <Int32/>\n\
            <dimension size=\"10\"/>\n\
        </Array>\n\
        <Grid name=\"huh\">\n\
            <Attribute name=\"long_name\" type=\"String\">\n\
                <value>The Grid huh</value>\n\
            </Attribute>\n\
            <Array name=\"Image#data\">\n\
                <Byte/>\n\
                <dimension size=\"512\"/>\n\
            </Array>\n\
            <Map name=\"colors\">\n\
                <Attribute name=\"long_name2\" type=\"String\">\n\
                    <value>The color map vector</value>\n\
                </Attribute>\n\
                <Attribute name=\"units\" type=\"String\">\n\
                    <value>m/s</value>\n\
                </Attribute>\n\
                <String/>\n\
                <dimension size=\"512\"/>\n\
            </Map>\n\
        </Grid>\n\
    </Structure>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, oss.str()));
        }

    };

    CPPUNIT_TEST_SUITE_REGISTRATION(DDSTest);

}

int main(int, char**)
{
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = runner.run("", false);

    return wasSuccessful ? 0 : 1;
}

