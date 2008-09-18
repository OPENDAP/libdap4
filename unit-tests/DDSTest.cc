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

//#define DODS_DEBUG

#include "DDS.h"

#include "GNURegex.h"
#include "util.h"
#include "debug.h"
#include <test_config.h>

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

        CPPUNIT_TEST_SUITE( DDSTest )
            ;

            CPPUNIT_TEST(symbol_name_test);
            CPPUNIT_TEST(print_xml_test);
            CPPUNIT_TEST(find_hdf4_dimension_attribute_home_test);
            CPPUNIT_TEST(find_matching_container_test);
            CPPUNIT_TEST(transfer_attributes_test);
            // These test both transfer_attributes() and print_xml()
            CPPUNIT_TEST(print_xml_test2);
            CPPUNIT_TEST(print_xml_test3);
            CPPUNIT_TEST(print_xml_test3_1);
            CPPUNIT_TEST(print_xml_test4);
            CPPUNIT_TEST(print_xml_test5);
            CPPUNIT_TEST(print_xml_test5_1);

            CPPUNIT_TEST(print_xml_test6);
            CPPUNIT_TEST(print_xml_test7);

            //CPPUNIT_TEST(add_global_attribute_test);

CPPUNIT_TEST_SUITE_END        ();

        void find_hdf4_dimension_attribute_home_test() {
            try {
                dds1->parse((string)TEST_SRC_DIR + "/dds-testsuite/3B42.980909.5.HDF.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/3B42.980909.5.hacked.HDF.das");

                AttrTable::Attr_iter eb = das.var_begin();
                AttrTable::Attr_iter ei = eb + 2;
                CPPUNIT_ASSERT((*ei)->name == "percipitate_dim_0");
                BaseType *btp = dds1->find_hdf4_dimension_attribute_home(*ei);
                CPPUNIT_ASSERT(btp->name() == "percipitate" && btp->is_vector_type());
                ei = eb + 4;
                btp = dds1->find_hdf4_dimension_attribute_home(*ei);
                CPPUNIT_ASSERT(btp->name() == "percipitate" && btp->is_vector_type());

                ei = eb + 1;
                btp = dds1->find_hdf4_dimension_attribute_home(*ei);
                CPPUNIT_ASSERT(!btp);

                dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/hdf_dimension_attribute_grid.dds");
                DAS das2;
                das2.parse((string)TEST_SRC_DIR + "/dds-testsuite/hdf_dimension_attribute_grid.das");
                eb = das2.var_begin();
                ei = eb + 1;
                CPPUNIT_ASSERT((*ei)->name == "g_dim_0");
                btp = dds2->find_hdf4_dimension_attribute_home(*ei);
                CPPUNIT_ASSERT(btp->name() == "x" && btp->is_vector_type());
                CPPUNIT_ASSERT(btp->get_parent()->type() == dods_grid_c);

                ei = eb + 2;
                CPPUNIT_ASSERT((*ei)->name == "g_dim_1");
                btp = dds2->find_hdf4_dimension_attribute_home(*ei);
                CPPUNIT_ASSERT(btp->name() == "y" && btp->is_vector_type());
                CPPUNIT_ASSERT(btp->get_parent()->type() == dods_grid_c);
            }

            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error thrown!");
            }
        }

        void find_matching_container_test() {
            try {
                dds1->parse((string)TEST_SRC_DIR + "/dds-testsuite/fnoc1.nc.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/fnoc1.nc.das");

                AttrTable::Attr_iter i = das.var_begin();
                while (i != das.var_end()) {
                    if ((*i)->type != Attr_container)
                    continue;
                    DBG(cerr << "looking for a destination for '" << (*i)->name
                            << "'" << endl);
                    BaseType *dest_var = 0;
                    AttrTable *dest = dds1->find_matching_container((*i), &dest_var);
                    BaseType *variable = dds1->var((*i)->name);
                    AttrTable *matching = variable ? &variable->get_attr_table(): 0;
                    if (variable) {
                        DBG(cerr << "variable: " << variable->name() << endl);
                        if (dest_var)
                        DBG(cerr << "dest_var: " << dest_var->name() << endl);
                        CPPUNIT_ASSERT(dest_var == variable);
                        CPPUNIT_ASSERT(dest == matching);
                    }
                    else {
                        CPPUNIT_ASSERT(dest_var == 0);
                        CPPUNIT_ASSERT(dest != 0);
                    }

                    ++i;
                }

                dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/hdf_dimension_attribute_grid.dds");
                DAS das2;
                das2.parse((string)TEST_SRC_DIR + "/dds-testsuite/hdf_dimension_attribute_grid.das");

                // There are three containers in the DAS and the _dim_0/1
                // containers should be bound to the Grid's map vectors.
                i = das2.var_begin();
                DBG(cerr << "looking for a destination for '" << (*i)->name << "'"
                        << endl);
                BaseType *dest_var = 0;
                AttrTable *dest = dds2->find_matching_container((*i), &dest_var);
                Grid *g = dynamic_cast<Grid*>(dds2->var("g"));
                CPPUNIT_ASSERT(g);
                CPPUNIT_ASSERT(dest == &g->get_attr_table());

                ++i;
                DBG(cerr << "looking for a destination for '" << (*i)->name << "'"
                        << endl);
                dest = dds2->find_matching_container((*i), &dest_var);
                Array *m = dynamic_cast<Array*>(*(g->map_begin()));
                CPPUNIT_ASSERT(m);
                CPPUNIT_ASSERT(dest == &m->get_attr_table());

                ++i;
                DBG(cerr << "looking for a destination for '" << (*i)->name << "'"
                        << endl);
                dest = dds2->find_matching_container((*i), &dest_var);
                m = dynamic_cast<Array*>(*(g->map_begin()+1));
                CPPUNIT_ASSERT(m);
                CPPUNIT_ASSERT(dest == &m->get_attr_table());
            }

            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error thrown!");
            }
        }

        // This is here mostly to test for memory leaks using valgrind. Does
        // DDS::add_global_attribute() leak memory? Apparently not. jhrg 3/18/05
#if 0
        void add_global_attribute_test() {
            DAS das;
            das.parse("./das-testsuite/test.1.das");
            AttrTable::Attr_iter i = das.var_begin();
            CPPUNIT_ASSERT(i != das.attr_end());

            dds1->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.1");
            dds1->add_global_attribute(*i);

            CPPUNIT_ASSERT(dds1->d_attr.get_size() == 1);
        }
#endif

        void transfer_attributes_test() {
            try {
                dds1->parse((string)TEST_SRC_DIR + "/dds-testsuite/fnoc1.nc.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/fnoc1.nc.das");
                dds1->transfer_attributes(&das);

                AttrTable &at = dds1->get_attr_table();
                AttrTable::Attr_iter i = at.attr_begin();
                CPPUNIT_ASSERT(at.get_name(i) == "NC_GLOBAL");
                CPPUNIT_ASSERT(at.get_name(++i) == "DODS_EXTRA");
            }
            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error thrown!");
            }

            try {
                dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/3B42.980909.5.HDF.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/3B42.980909.5.hacked.HDF.das");
                dds2->transfer_attributes(&das);

                AttrTable &at = dds2->get_attr_table();
                AttrTable::Attr_iter i = at.attr_begin();
                CPPUNIT_ASSERT(at.get_name(i) == "HDF_GLOBAL");
                CPPUNIT_ASSERT(at.get_name(++i) == "CoreMetadata");
            }
            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error thrown!");
            }

            try {
                BaseTypeFactory factory;
                DDS dds(&factory);
                dds.parse((string)TEST_SRC_DIR + "/dds-testsuite/S2000415.HDF.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/S2000415.HDF.das");
                dds.transfer_attributes(&das);

                AttrTable &at = dds.get_attr_table();
                AttrTable::Attr_iter i = at.attr_begin();
                CPPUNIT_ASSERT(at.get_name(i) == "HDF_GLOBAL");
                AttrTable &at2 = dds.var("WVC_Lat")->get_attr_table();
                DBG2(at2.print(stderr));
                CPPUNIT_ASSERT(at2.get_name(at2.attr_begin()) == "long_name");
            }
            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_ASSERT(!"Error thrown!");
            }
            try {
                BaseTypeFactory factory;
                DDS dds(&factory);
                dds.parse((string)TEST_SRC_DIR + "/dds-testsuite/S2000415.HDF.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/S2000415.HDF.test1.das");
                dds.transfer_attributes(&das);

                AttrTable &at2 = dds.var("WVC_Lat")->get_attr_table();
                DBG2(at2.print(stderr));
                CPPUNIT_ASSERT(at2.get_name(at2.attr_begin()) == "long_name");
            }
            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error thrown!");
            }
#if 0
            // This part of the test requires files written by Patrick that have
            // not been checked into SVN as of 9/18/08 (it looks like the changes
            // to DDSTest.cc were made on the multifile branch around 2/28/08.
            // jhrg 9/18/08
            try {
                BaseTypeFactory factory;
                DDS dds(&factory);
                dds.parse((string)TEST_SRC_DIR + "/dds-testsuite/S2000415.fnoc1.dds");
                DAS das;
                das.parse((string)TEST_SRC_DIR + "/dds-testsuite/S2000415.fnoc1.das");
                dds.container_name( "fnoc1" );
                das.container_name( "fnoc1" );
                dds.transfer_attributes(&das);
                dds.container_name( "S2000415" );
                das.container_name( "S2000415" );
                dds.transfer_attributes(&das);

                dds.print(cout);
                das.print(cout);
                BaseType *bt = dds.var("WVC_Lat");
                CPPUNIT_ASSERT(bt);
                AttrTable &at2 = bt->get_attr_table();
                at2.print(cout);
                DBG2(at2.print(stderr));
                CPPUNIT_ASSERT(at2.get_name(at2.attr_begin()) == "long_name");
            }
            catch (Error &e) {
                cout << "Error: " << e.get_error_message() << endl;
                CPPUNIT_FAIL("Error thrown!");
            }
#endif
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

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");
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
            CPPUNIT_ASSERT(re_match(r, file_to_string(tmp)));

            fclose(tmp);
            remove(DDSTemp);
        }

        void print_xml_test2() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19c");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19c.das");

            dds2->transfer_attributes(&das);

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

            string output = file_to_string(tmp);
            DBG2(cerr << output << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19c\"\n\
.*\
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
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, output));
            remove(DDSTemp);
        }

        void print_xml_test3() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19d");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19d.das");

            dds2->transfer_attributes(&das);

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

            string output = file_to_string(tmp);
            DBG2(cerr << output << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19d\"\n\
.*\
    <Array name=\"b#c\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>&quot;b pound c&quot;</value>\n\
        </Attribute>\n\
        <Int32/>\n\
        <dimension size=\"10\"/>\n\
    </Array>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, output));
            remove(DDSTemp);
        }

        void print_xml_test3_1() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19d");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19d1.das");

            dds2->transfer_attributes(&das);

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

            string output = file_to_string(tmp);
            DBG2(cerr << output << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19d\"\n\
.*\
    <Array name=\"b#c\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>&quot;b pound c&quot;</value>\n\
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

            CPPUNIT_ASSERT(re_match(r, output));
            remove(DDSTemp);
        }

        void print_xml_test4() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19e");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19e.das");

            dds2->transfer_attributes(&das);

            DBG(AttrTable &at2 = dds2->var("c%20d")->get_attr_table());
            DBG(at2.print(stderr));

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

            string output = file_to_string(tmp);
            DBG(cerr << output << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19e\"\n\
.*\
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
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, output));
            remove(DDSTemp);
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

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

            string output = file_to_string(tmp);
            DBG(cerr << output << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19f\"\n\
.*\
    <Grid name=\"huh\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>&quot;The Grid huh&quot;</value>\n\
        </Attribute>\n\
        <Array name=\"Image#data\">\n\
            <Byte/>\n\
            <dimension size=\"512\"/>\n\
        </Array>\n\
        <Map name=\"colors\">\n\
            <Attribute name=\"long_name2\" type=\"String\">\n\
                <value>&quot;The color map vector&quot;</value>\n\
            </Attribute>\n\
            <String/>\n\
            <dimension size=\"512\"/>\n\
        </Map>\n\
    </Grid>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, output));
            remove(DDSTemp);
        }

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

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

            string output = file_to_string(tmp);
            DBG(cerr << output << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19f\"\n\
.*\
    <Grid name=\"huh\">\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>&quot;The Grid huh&quot;</value>\n\
        </Attribute>\n\
        <Array name=\"Image#data\">\n\
            <Byte/>\n\
            <dimension size=\"512\"/>\n\
        </Array>\n\
        <Map name=\"colors\">\n\
            <Attribute name=\"long_name2\" type=\"String\">\n\
                <value>&quot;The color map vector&quot;</value>\n\
            </Attribute>\n\
            <Attribute name=\"units\" type=\"String\">\n\
                <value>&quot;m/s&quot;</value>\n\
            </Attribute>\n\
            <String/>\n\
            <dimension size=\"512\"/>\n\
        </Map>\n\
    </Grid>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, output));
            remove(DDSTemp);
        }

        void print_xml_test6() {
            dds2->parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19b");
            DAS das;
            das.parse((string)TEST_SRC_DIR + "/dds-testsuite/test.19b.das");

            dds2->transfer_attributes(&das);

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

            string output = file_to_string(tmp);
            DBG(cerr << output << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19\"\n\
.*\
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
    </Grid>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, output));
            remove(DDSTemp);
        }

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

            char DDSTemp[] = {"/var/tmp/DDSTestXXXXXX"};
            FILE *tmp = get_temp_file(DDSTemp);
            dds2->print_xml(tmp, false, "http://localhost/dods/test.xyz");

            string output = file_to_string(tmp);
            DBG(cerr << output << endl);

            Regex r("<.xml version=\"1.0\" encoding=\"UTF-8\".>\n\
<Dataset name=\"test.19\"\n\
.*\
    <Structure name=\"s\">\n\
        <Array name=\"b#c\">\n\
            <Attribute name=\"long_name\" type=\"String\">\n\
                <value>&quot;b pound c&quot;</value>\n\
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
                <value>&quot;The Grid huh&quot;</value>\n\
            </Attribute>\n\
            <Array name=\"Image#data\">\n\
                <Byte/>\n\
                <dimension size=\"512\"/>\n\
            </Array>\n\
            <Map name=\"colors\">\n\
                <Attribute name=\"long_name2\" type=\"String\">\n\
                    <value>&quot;The color map vector&quot;</value>\n\
                </Attribute>\n\
                <Attribute name=\"units\" type=\"String\">\n\
                    <value>&quot;m/s&quot;</value>\n\
                </Attribute>\n\
                <String/>\n\
                <dimension size=\"512\"/>\n\
            </Map>\n\
        </Grid>\n\
    </Structure>\n\
\n\
    <dataBLOB href=\"\"/>\n\
</Dataset>\n");

            CPPUNIT_ASSERT(re_match(r, output));
            remove(DDSTemp);
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

