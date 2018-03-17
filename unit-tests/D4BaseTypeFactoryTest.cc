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

#include <sstream>
#include <string.h>

#include "crc.h"
#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "D4BaseTypeFactory.h"
#include "UInt32.h"
#include "Int64.h"
#include "UInt64.h"
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
#include "GetOpt.h"
#include "util.h"
#include "debug.h"
#include "ce_expr.tab.hh"

#include "testFile.h"
#include "test_config.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) {x;} } while(false)

using namespace CppUnit;
using namespace std;

namespace libdap {

class D4BaseTypeFactoryTest: public TestFixture {
private:
    D4BaseTypeFactory d4btf;
    
public:
    D4BaseTypeFactoryTest()
    {
    }
    ~D4BaseTypeFactoryTest()
    {
    }

    void setUp()
    {
        d4btf = D4BaseTypeFactory();
    }

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE(D4BaseTypeFactoryTest);

    CPPUNIT_TEST(new_variables_test);
    CPPUNIT_TEST(new_url_test);
    CPPUNIT_TEST(error_test);

    CPPUNIT_TEST_SUITE_END();

    void new_variables_test()
    {
        BaseType *bt;
        
        bt = d4btf.NewVariable(dods_byte_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Byte*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_char_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Byte*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_uint8_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Byte*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_int8_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Int8*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_int16_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Int16*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_uint16_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<UInt16*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_int32_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Int32*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_uint32_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<UInt32*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_int64_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Int64*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_uint64_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<UInt64*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_enum_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && bt->is_dap4());
        // CPPUNIT_ASSERT(dynamic_cast<D4Enum*>(bt));
        delete bt;
        bt = d4btf.NewVariable(dods_str_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Str*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_url_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Url*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_opaque_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && bt->is_dap4());
        // CPPUNIT_ASSERT(dynamic_cast<D4Opaque*>(bt));
        delete bt;
        bt = d4btf.NewVariable(dods_structure_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Structure*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_sequence_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && bt->is_dap4());
        // CPPUNIT_ASSERT(dynamic_cast<D4Sequence*>(bt));
        delete bt;
        bt = d4btf.NewVariable(dods_array_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Array*>(bt) && bt->is_dap4());
        delete bt;
        bt = d4btf.NewVariable(dods_group_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && bt->is_dap4());
        // CPPUNIT_ASSERT(dynamic_cast<D4Group*>(bt));
        delete bt;
    }

    void new_url_test()
    {
        Url *up = d4btf.NewUrl("a");
        CPPUNIT_ASSERT(up->name() == "a" && dynamic_cast<Url*>(up) && up->is_dap4());
        delete up;
    }
    
    void error_test()
    {
        CPPUNIT_ASSERT_THROW(d4btf.NewVariable(dods_grid_c, "a"), InternalErr);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(D4BaseTypeFactoryTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;

        case 'h': {     // help - show test names
            cerr << "Usage: D4BaseTypeFactoryTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::D4BaseTypeFactoryTest::suite()->getTests();
            unsigned int prefix_len = libdap::D4BaseTypeFactoryTest::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            return 1;
            break;
        }

        default:
            break;
        }

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        for (; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = libdap::D4BaseTypeFactoryTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

