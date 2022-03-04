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

#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "BaseTypeFactory.h"
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
#include "crc.h"

#include "DDS.h"

#include "GNURegex.h"

#include "util.h"
#include "debug.h"
#include "ce_expr.tab.hh"

#include "testFile.h"
#include "run_tests_cppunit.h"
#include "run_tests_cppunit.h"
#include "test_config.h"


using namespace CppUnit;
using namespace std;

namespace libdap {

class BaseTypeFactoryTest: public TestFixture {
private:
    BaseTypeFactory btf;
    
public:
    BaseTypeFactoryTest()
    {
    }
    ~BaseTypeFactoryTest()
    {
    }

    void setUp()
    {
        btf = BaseTypeFactory();
    }

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE(BaseTypeFactoryTest);

    CPPUNIT_TEST(new_variables_test);
    CPPUNIT_TEST(error_test);

    CPPUNIT_TEST_SUITE_END();

    void ctor_test()
    {
        BaseTypeFactory *btf1 = new BaseTypeFactory();
        delete btf1;
    }

    void new_variables_test()
    {
        BaseType *bt;
        
        bt = btf.NewVariable(dods_byte_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Byte*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_int16_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Int16*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_uint16_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<UInt16*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_int32_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Int32*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_uint32_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<UInt32*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_float32_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Float32*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_float64_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Float64*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_str_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Str*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_url_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Url*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_array_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Array*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_structure_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Structure*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_sequence_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Sequence*>(bt) && !bt->is_dap4());
        delete bt;
        bt = btf.NewVariable(dods_grid_c, "a");
        CPPUNIT_ASSERT(bt->name() == "a" && dynamic_cast<Grid*>(bt) && !bt->is_dap4());
        delete bt;
    }

    void error_test()
    {
        CPPUNIT_ASSERT_THROW(btf.NewVariable(dods_group_c, "a"), InternalErr);
        CPPUNIT_ASSERT_THROW(btf.ptr_duplicate(), InternalErr);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BaseTypeFactoryTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    return run_tests<BaseTypeFactoryTest>(argc, argv) ? 0: 1;
}
