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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

// Tests for Byte. Tests features of BaseType, too. 7/19/2001 jhrg

#include <sstream>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>

#include "BaseType.h"
#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "ce_expr.tab.hh"
#include "crc.h"
#include "D4Attributes.h"

#include "run_tests_cppunit.h"
#include "run_tests_cppunit.h"
#include "test_config.h"


#include "testFile.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

class ByteTest: public TestFixture {
private:
    Byte * tb1;
    Byte *tb2;
    Byte *tb3;
    Byte *tb4;
    Byte *tb5;
    char a[1024];    

public:
    ByteTest()
    {
    }
    ~ByteTest()
    {
    }

    void setUp()
    {
        tb1 = new Byte("tb1");
        tb2 = new Byte("tb2 name with spaces");
        tb3 = new Byte("tb3 %");
        tb4 = new Byte("tb4 #");
        tb5 = new Byte("a", "b");
        a[0] = 0;
    }

    void tearDown()
    {
        delete tb1;
        tb1 = 0;
        delete tb2;
        tb2 = 0;
        delete tb3;
        tb3 = 0;
        delete tb4;
        tb4 = 0;
        delete tb5;
        tb5 = 0;
    }

    CPPUNIT_TEST_SUITE(ByteTest);

    CPPUNIT_TEST(cons_test);
    CPPUNIT_TEST(equals_test);
    CPPUNIT_TEST(checksum_test);
    CPPUNIT_TEST(val2buf_test);
    CPPUNIT_TEST(buf2val_test);
    CPPUNIT_TEST(dump_test);
    CPPUNIT_TEST(dump_2_test);
    CPPUNIT_TEST(print_test);
    CPPUNIT_TEST(type_compare_test);
    CPPUNIT_TEST(name_mangling_test);
    CPPUNIT_TEST(decl_mangling_test);
    CPPUNIT_TEST(basetype_test);
    CPPUNIT_TEST(basetype_print_val_test);
    CPPUNIT_TEST(basetype_print_xml_test);
    CPPUNIT_TEST(basetype_print_xml_2_test);
    CPPUNIT_TEST(basetype_FQN_test);
    CPPUNIT_TEST(basetype_print_decl_test);
    CPPUNIT_TEST(set_attributes_test);

    CPPUNIT_TEST_SUITE_END();

    void cons_test()
    {
        CPPUNIT_ASSERT(tb5->value() == 0 && tb5->dataset() == "b" && tb5->name() == "a" &&
                       tb5->type() == dods_byte_c);
        CPPUNIT_ASSERT(tb2->value() == 0);
    }

    void equals_test()
    {
        Byte b3 = Byte("a", "b");
        Byte b4 = Byte("e");
        
        CPPUNIT_ASSERT(b3.set_value(42) && b3.value() == 42);
        b4 = b3;
        CPPUNIT_ASSERT(b3.value() == 42);
        b4 = b4;
        CPPUNIT_ASSERT(b4.value() == 42);
    }

    void checksum_test()
    {
        Crc32 cs;
        tb5->compute_checksum(cs);
    }

    void val2buf_test()
    {
        char i = 3;
        tb3->val2buf(&i, true);
        CPPUNIT_ASSERT(tb3->value() == 3);        
        CPPUNIT_ASSERT_THROW(tb3->val2buf(NULL, true), InternalErr);
    }

    void buf2val_test()
    {
        char i = 42;
        void *v = &i;
        signed char *v2 = NULL;
        CPPUNIT_ASSERT(tb1->set_value(6));
        CPPUNIT_ASSERT(tb1->buf2val(&v) == 1 && i == 6);
        CPPUNIT_ASSERT_THROW(tb1->buf2val(NULL), InternalErr);
        CPPUNIT_ASSERT(tb1->buf2val((void **)&v2) == 1 && *(signed char *)v2 == 6);
        delete v2;
    }

    void dump_test()
    {
        ofstream ofs("ByteTest_dump.output", ios::trunc);
        tb1->set_value(21);
        tb1->dump(ofs);
        ofs.close();
        ifstream ifs("ByteTest_dump.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(a[0] == 21);
    }

    void dump_2_test()
    {
        D4Attribute att;
        D4Attributes attrs;        
        ofstream ofs("ByteTest_dump_2.output", ios::trunc);
        att.set_name("Waldo");
        att.set_type(attr_byte_c);
        att.add_value("1");
        attrs.add_attribute(&att);
        tb1->set_attributes(&attrs);
        tb1->set_value(21);
        tb1->dump(ofs);
        ofs.close();
        ifstream ifs("ByteTest_dump_2.output");
        while(!ifs.eof())
             ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(a[0] == 21);
    }

    void print_test()
    {
        FILE *fp;
        CPPUNIT_ASSERT(fp = fopen("ByteTest.output", "w"));
        tb1->set_value(22);
        tb1->print_val(fp, " ", true);
        fclose(fp);
        ifstream ifs("ByteTest.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, "22;"));
    }

    void type_compare_test()
    {
        Byte b1 = Byte("a");
        Int8 i8 = Int8("a");
        Int16 i16 = Int16("a");
        UInt16 ui16 = UInt16("a");
        Int32 i32 = Int32("a", "b");
        UInt32 ui32 = UInt32("a", "b");
        Float32 f32 = Float32("a");
        Float64 f64 = Float64("a");
        Url url = Url("a");
        Str str = Str("a");
        Array array = Array("a", &i16, true);
        
        b1.set_value(42);
        i8.set_value(42);
        i16.set_value(42);
        ui16.set_value(42);
        i32.set_value(42);
        ui32.set_value(42);
        f32.set_value(42);
        f64.set_value(42);
        CPPUNIT_ASSERT(b1.value() == 42);
        CPPUNIT_ASSERT(b1.ops(&b1, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&b1, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&i8, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&i16, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&ui16, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&i32, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&ui32, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&f32, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&f64, SCAN_EQUAL));

        CPPUNIT_ASSERT_THROW(b1.d4_ops(&url, SCAN_EQUAL), Error);
        CPPUNIT_ASSERT_THROW(b1.d4_ops(&str, SCAN_EQUAL), Error);
        CPPUNIT_ASSERT_THROW(b1.d4_ops(&array, SCAN_EQUAL), Error);
    }    

    void name_mangling_test()
    {
        CPPUNIT_ASSERT(tb1->name() == "tb1");
        CPPUNIT_ASSERT(tb2->name() == "tb2 name with spaces");
        CPPUNIT_ASSERT(tb3->name() == "tb3 %");
        CPPUNIT_ASSERT(tb4->name() == "tb4 #");
    }

    void decl_mangling_test()
    {
        ostringstream sof;
        tb1->print_decl(sof, "", false);
        CPPUNIT_ASSERT(sof.str().find("Byte tb1") != string::npos);

        tb2->print_decl(sof, "", false);
        CPPUNIT_ASSERT(sof.str().find("Byte tb2%20name%20with%20spaces") != string::npos);

        tb3->print_decl(sof, "", false);
        CPPUNIT_ASSERT(sof.str().find("Byte tb3%20%") != string::npos);

        tb4->print_decl(sof, "", false);
        CPPUNIT_ASSERT(sof.str().find("Byte tb4%20%23") != string::npos);
    }

    void basetype_test()
    {
        BaseType *bt = static_cast<BaseType*>(tb1);
        BaseType::btp_stack bs;
        BaseType::btp_stack &bsr = bs;
        CPPUNIT_ASSERT_THROW(bt->BaseType::width(false), InternalErr);
        CPPUNIT_ASSERT_THROW(bt->BaseType::ops(bt, SCAN_EQUAL), InternalErr);
        CPPUNIT_ASSERT(!bt->synthesized_p());
        bt->set_synthesized_p(true);
        CPPUNIT_ASSERT(bt->synthesized_p());
        CPPUNIT_ASSERT_THROW(bt->add_var(bt), InternalErr);
        CPPUNIT_ASSERT_THROW(bt->add_var_nocopy(bt), InternalErr);
        CPPUNIT_ASSERT_THROW(bt->ops(bt, SCAN_EQUAL), InternalErr);
        CPPUNIT_ASSERT(bt->BaseType::var("", bsr) == 0);
        CPPUNIT_ASSERT(bt->BaseType::length() == 1);
        bt->BaseType::set_length(1);
        bt->BaseType::clear_local_data();        
        CPPUNIT_ASSERT(!tb1->read_p());
    }

    void basetype_print_val_test()
    {
        BaseType *bt = static_cast<BaseType*>(tb1);
        FILE *fp;
        CPPUNIT_ASSERT(fp = fopen("ByteTest_BaseType_print.output", "w"));
        bt->BaseType::print_val(fp, "", true);
        fclose(fp);
        ifstream ifs("ByteTest_BaseType_print.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, "0;"));        
    }
    
    void basetype_print_xml_test()
    {
        BaseType *bt = static_cast<BaseType*>(tb1);
        FILE *fp;
        CPPUNIT_ASSERT(fp = fopen("ByteTest_BaseType_print_xml.output", "w"));
        bt->BaseType::print_xml(fp, "", true);
        fclose(fp);
        ifstream ifs("ByteTest_BaseType_print_xml.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, "encoding=\"ISO-8859-1\"?>"));        
    }

    void basetype_print_xml_2_test()
    {
        BaseType *bt = static_cast<BaseType*>(tb1);
        ofstream ofs("ByteTest_xml_2.output", ios::trunc);
        bt->BaseType::print_xml(ofs, "", true);
        ofs.close();
        ifstream ifs("ByteTest_xml_2.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, "encoding=\"ISO-8859-1\"?>"));
    }

    void basetype_FQN_test()
    {
        CPPUNIT_ASSERT(tb1->FQN() == "tb1");
    }

    void basetype_print_decl_test()
    {
        BaseType *bt = static_cast<BaseType*>(tb1);        
        FILE *fp;
        CPPUNIT_ASSERT(fp = fopen("ByteTest_BaseType_print_decl.output", "w"));
        bt->BaseType::print_decl(fp, "", true, true, true);        
        fclose(fp);        
        ifstream ifs("ByteTest_BaseType_print_decl.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, ""));
    }

    void set_attributes_test()
    {
        D4Attributes attrs;
        D4Attribute a;

        a.set_name("first");
        a.set_type(attr_byte_c);
        a.add_value("1");
        a.add_value("2");
        attrs.add_attribute(&a);
        
        BaseType *bt = static_cast<BaseType*>(tb1);        
        bt->set_attributes(&attrs);
        D4Attributes *a2 = bt->attributes();
        CPPUNIT_ASSERT(a2->find("first")->name() == "first");
    }

    
};

CPPUNIT_TEST_SUITE_REGISTRATION (ByteTest);

int main(int argc, char *argv[])
{
    return run_tests<ByteTest>(argc, argv) ? 0: 1;
}
