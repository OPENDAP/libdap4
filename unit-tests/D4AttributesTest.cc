// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2013 OPeNDAP, Inc.
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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

//#define DODS_DEBUG

#include "D4Attributes.h"
#include "XMLWriter.h"
#include "debug.h"

#include "testFile.h"
#include "test_config.h"
#include "GetOpt.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

class D4AttributesTest: public TestFixture {
private:
    XMLWriter *xml;
    D4Attributes *attrs;

    D4Attribute a, a2, a3, a4, a5, c, c2;

public:
    D4AttributesTest()
    {
    }

    ~D4AttributesTest()
    {
    }

    void setUp()
    {
        attrs = new D4Attributes;
        xml = new XMLWriter;

        a.set_name("first");
        a.set_type(attr_byte_c);
        a.add_value("1");
        a.add_value("2");

        a2.set_name("second");
        a2.set_type(attr_int32_c);
        a2.add_value("10000");

        c.set_name("container_1");
        c.set_type(attr_container_c);

        a3.set_name("color");
        a3.set_type(attr_str_c);
        vector<string> colors;
        colors.push_back("red");
        colors.push_back("blue");
        colors.push_back("green");
        a3.add_value_vector(colors);

        a4 = a2;
        a4.set_name("control");

        c.attributes()->add_attribute(&a3);

        c2.set_name("container_2");
        c2.set_type(attr_container_c);

        c2.attributes()->add_attribute(&a4);
        c2.attributes()->add_attribute(&c);
    }

    void tearDown()
    {
        delete xml;
        delete attrs;
    }

    void test_type_to_string()
    {
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_null_c) == "null");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_byte_c) == "Byte");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_int16_c) == "Int16");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_uint16_c) == "UInt16");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_int32_c) == "Int32");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_uint32_c) == "UInt32");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_float32_c) == "Float32");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_float64_c) == "Float64");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_str_c) == "String");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_url_c) == "Url");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_int8_c) == "Int8");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_uint8_c) == "UInt8");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_int64_c) == "Int64");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_uint64_c) == "UInt64");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_enum_c) == "Enum");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_opaque_c) == "Opaque");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_container_c) == "Container");
        CPPUNIT_ASSERT(D4AttributeTypeToString(attr_otherxml_c) == "OtherXML");
    }

    void test_string_to_type()
    {
        CPPUNIT_ASSERT(StringToD4AttributeType("null") == attr_null_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Byte") == attr_byte_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Int16") == attr_int16_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("UInt16") == attr_uint16_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Int32") == attr_int32_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("UInt32") == attr_uint32_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Float32") == attr_float32_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Float64") == attr_float64_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("String") == attr_str_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Url") == attr_url_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Int8") == attr_int8_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("UInt8") == attr_uint8_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Int64") == attr_int64_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("UInt64") == attr_uint64_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Enum") == attr_enum_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Opaque") == attr_opaque_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Container") == attr_container_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("OtherXML") == attr_otherxml_c);
        CPPUNIT_ASSERT(StringToD4AttributeType("Punk") == attr_null_c);
    }

    void test_attr_type()
    {
        //attrs->get_dap2_AttrType(attr_null_c);
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_print_empty()
    {
        attrs->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Attributes_empty.xml");
        DBG(cerr << "D4Attributes: " << doc << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_single_attribute()
    {
        a.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Attributes_values_1.xml");
        DBG(cerr << "D4Attributes: " << doc << endl);
        CPPUNIT_ASSERT(doc == baseline);

    }

    void test_print_1()
    {
        attrs->add_attribute(&a);

        attrs->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Attributes_1.xml");
        DBG(cerr << "D4Attributes: " << doc << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_2()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);

        attrs->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Attributes_2.xml");
        DBG(cerr << "D4Attributes: " << doc << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_3()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        attrs->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Attributes_3.xml");
        DBG(cerr << "D4Attributes: " << doc << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_dump()
    {
        attrs->add_attribute(&c2);
        ostringstream sof;        
        attrs->dump(sof);
        CPPUNIT_ASSERT(sof.str().find("<Attribute name=\"container_2\" type=\"Container\">") != string::npos);
    }

    void test_2_dump()
    {
        ostringstream sof;        
        a.dump(sof);
        CPPUNIT_ASSERT(sof.str().find("<Attribute name=\"first\" type=\"Byte\">") != string::npos);
    }

    void test_print_assignment()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);

        D4Attributes lhs = *attrs;

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Attributes_assignment.xml");
        DBG(cerr << "D4Attributes: " << doc << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_assignment_2()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        D4Attributes lhs = *attrs;

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Attributes_assignment_2.xml");
        DBG(cerr << "D4Attributes: " << doc << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_copy_ctor()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);

        D4Attributes lhs(*attrs);

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Attributes_copy_ctor.xml");
        DBG(cerr << "D4Attributes: " << doc << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_find()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        D4Attribute *find_result = attrs->find("color");

        // 'color' is in the container 'container' which is the third
        // attribute
        D4Attribute *baseline = *(attrs->attribute_begin() + 2);
        CPPUNIT_ASSERT(baseline);
        CPPUNIT_ASSERT(baseline->type() == attr_container_c);
        D4Attributes *local_attrs = baseline->attributes();

        // it is the first attribute in that container
        baseline = *(local_attrs->attribute_begin());
        CPPUNIT_ASSERT(baseline);
        CPPUNIT_ASSERT(baseline->type() == attr_str_c);
        CPPUNIT_ASSERT(baseline->name() == "color");

        // We get a pointer to actual container, not a copy
        CPPUNIT_ASSERT(baseline == find_result);
    }

    void test_get()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        D4Attribute *get_result = attrs->get("container_1.color");

        // 'color' is in the container 'container_1' which is the third
        // attribute
        D4Attribute *baseline = *(attrs->attribute_begin() + 2);
        CPPUNIT_ASSERT(baseline);
        CPPUNIT_ASSERT(baseline->type() == attr_container_c);
        D4Attributes *local_attrs = baseline->attributes();

        // it is the first attribute in that container
        baseline = *(local_attrs->attribute_begin());
        CPPUNIT_ASSERT(baseline);
        CPPUNIT_ASSERT(baseline->type() == attr_str_c);
        CPPUNIT_ASSERT(baseline->name() == "color");

        // We get a pointer to actual container, not a copy
        CPPUNIT_ASSERT(baseline == get_result);
    }

    void test_get2()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        D4Attribute *get_result = attrs->get("container_2.container_1.color");

        // 'color' is (also) in the container 'container_1' which is in
        // the container 'container_2' which is the fourth attribute
        D4Attribute *baseline = *(attrs->attribute_begin() + 3);
        CPPUNIT_ASSERT(baseline);
        CPPUNIT_ASSERT(baseline->type() == attr_container_c);
        CPPUNIT_ASSERT(baseline->name() == "container_2");
        D4Attributes *local_attrs = baseline->attributes();

        // 'container_1 is the second attribute in 'container_2'
        baseline = *(local_attrs->attribute_begin() + 1);
        CPPUNIT_ASSERT(baseline);
        CPPUNIT_ASSERT(baseline->type() == attr_container_c);
        CPPUNIT_ASSERT(baseline->name() == "container_1");
        local_attrs = baseline->attributes();

        // finally, 'color' is the first attribute in 'container_1'
        baseline = *(local_attrs->attribute_begin());
        CPPUNIT_ASSERT(baseline->type() == attr_str_c);
        CPPUNIT_ASSERT(baseline->name() == "color");

        // We get a pointer to actual container, not a copy
        CPPUNIT_ASSERT(baseline == get_result);
    }

    // test erasing an attribute
    void test_erase_1()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        D4Attribute *first = attrs->get("first");
        CPPUNIT_ASSERT_MESSAGE("The attribute should be present before calling erase()", first != nullptr);
        // sanity check
        CPPUNIT_ASSERT(first->type() == attr_byte_c);
        CPPUNIT_ASSERT(first->name() == "first");

        attrs->erase("first");

        first = attrs->get("first");
        CPPUNIT_ASSERT_MESSAGE("The attribute should not be present after calling erase()", first == nullptr);
    }

    // test erasing an attribute in a container
    void test_erase_2()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        D4Attribute *color = attrs->get("container_1.color");
        CPPUNIT_ASSERT_MESSAGE("The attribute should be present before calling erase()", color != nullptr);
        // sanity check
        CPPUNIT_ASSERT(color->type() == attr_str_c);
        CPPUNIT_ASSERT(color->name() == "color");

        attrs->erase("container_1.color");

        color = attrs->get("container_1.color");
        // attrs->dump(cerr), attrs <-- Trick to get a return value for the ternary op. jhrg 2/16/22
        DBG(cerr << "after erase: attrs: " << (attrs != nullptr ? attrs->dump(cerr), attrs: 0) << endl);
        CPPUNIT_ASSERT_MESSAGE("The attribute should not be present after calling erase()", color == nullptr);
    }

    // test erasing a container
    void test_erase_3()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        D4Attribute *color = attrs->get("container_1");
        CPPUNIT_ASSERT_MESSAGE("The attribute should be present before calling erase()", color != nullptr);
        // sanity check
        CPPUNIT_ASSERT(color->type() == attr_container_c);
        CPPUNIT_ASSERT(color->name() == "container_1");

        attrs->erase("container_1");

        color = attrs->get("container_1");
        // attrs->dump(cerr), attrs <-- Trick to get a return value for the ternary op. jhrg 2/16/22
        DBG(cerr << "after erase: attrs: " << (attrs != nullptr ? attrs->dump(cerr), attrs: 0) << endl);
        CPPUNIT_ASSERT_MESSAGE("The attribute should not be present after calling erase()", color == nullptr);
    }

    // test erasing a container, using erase_named_attribute() and instead of erase()
    void test_erase_4()
    {
        attrs->add_attribute(&a);
        attrs->add_attribute(&a2);
        attrs->add_attribute(&c);
        attrs->add_attribute(&c2);

        D4Attribute *color = attrs->get("container_1");
        CPPUNIT_ASSERT_MESSAGE("The attribute should be present before calling erase()", color != nullptr);
        // sanity check
        CPPUNIT_ASSERT(color->type() == attr_container_c);
        CPPUNIT_ASSERT(color->name() == "container_1");

        attrs->erase_named_attribute("container_1");

        color = attrs->get("container_1");
        // attrs->dump(cerr), attrs <-- Trick to get a return value for the ternary op. jhrg 2/16/22
        DBG(cerr << "after erase: attrs: " << (attrs != nullptr ? attrs->dump(cerr), attrs: 0) << endl);
        CPPUNIT_ASSERT_MESSAGE("The attribute should not be present after calling erase()", color == nullptr);
    }

    CPPUNIT_TEST_SUITE (D4AttributesTest);

    CPPUNIT_TEST (test_type_to_string);
    
    CPPUNIT_TEST (test_print_empty);
    CPPUNIT_TEST (test_print_single_attribute);
    CPPUNIT_TEST (test_print_1);
    CPPUNIT_TEST (test_print_2);
    CPPUNIT_TEST (test_print_3);
    CPPUNIT_TEST (test_dump);
    CPPUNIT_TEST (test_2_dump);

    CPPUNIT_TEST (test_print_assignment);
    CPPUNIT_TEST (test_print_assignment_2);

    CPPUNIT_TEST (test_print_copy_ctor);

    CPPUNIT_TEST (test_find);
    CPPUNIT_TEST (test_get);
    CPPUNIT_TEST (test_get2);

    CPPUNIT_TEST (test_erase_1);
    CPPUNIT_TEST (test_erase_2);
    CPPUNIT_TEST (test_erase_3);
    CPPUNIT_TEST (test_erase_4);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4AttributesTest);

int main(int argc, char*argv[])
{
    bool sleep_on_exit = false;
    int option_char;
    while ((option_char = getopt(argc, argv, "dhs")) != EOF) {
        switch (option_char) {
            case 'd':
                debug = 1;  // debug is a static global
                break;
            case 'h': {     // help - show test names
                cerr << "Usage: D4AttributesTest has the following tests:" << endl;
                const std::vector<Test *> &tests = D4AttributesTest::suite()->getTests();
                unsigned int prefix_len = D4AttributesTest::suite()->getName().append("::").length();
                for (std::vector<Test *>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                    cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
                }
                break;
            }
            case 's':
                sleep_on_exit = true;
                break;
            default:
                break;
        }
    }
    argc -= optind;
    argv += optind;

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    if (0 == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        for (int i = 0; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = D4AttributesTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    if (sleep_on_exit) sleep(10);
    return wasSuccessful ? 0 : 1;
}

