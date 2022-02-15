// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2005,2018 OPeNDAP, Inc.
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstring>
#include <string>
#include <memory>

#include <unistd.h>

#include "GNURegex.h"

#include "BaseType.h"
#include "Grid.h"
#include "Array.h"
#include "Float32.h"
#include "D4Dimensions.h"

#include "debug.h"

using namespace CppUnit;
using namespace std;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

namespace libdap {

class GridTest: public TestFixture {
private:
    Grid *d_grid = nullptr;
    Array *d_array = nullptr;
    // Float32 *d_array_proto;
    Array *d_map = nullptr;
    // Float32 *d_map_proto;

public:
    GridTest() = default;

    ~GridTest() = default;

    void setUp()
    {
        unique_ptr<Float32> d_array_proto(new Float32("array_proto"));
        // The name() for the array will be "array_proto"
        d_array = new Array("grid_array", d_array_proto.get());

        d_array->append_dim(4, "dimension");
        dods_float32 buffer_array[4] = { 10.1, 11.2, 12.3, 13.4 };
        d_array->val2buf(buffer_array);

        unique_ptr<Float32> d_map_proto(new Float32("map_proto"));
        d_map = new Array("grid_map", d_map_proto.get());

        d_map->append_dim(4, "dimension");
        dods_float32 buffer_map[4] = { 0.1, 1.2, 2.3, 3.4 };
        d_map->val2buf(buffer_map);

        d_grid = new Grid("grid");

        // These do not copy the variables but the dtor in the parent class does delete the objects
        d_grid->set_array(d_array);
        d_grid->add_map(d_map, false);
    }

    void tearDown() {
        delete d_grid;      // frees the array and map(s)
    }

    bool re_match(Regex &r, const char *s)
    {
        int match_position = r.match(s, strlen(s));
        DBG(cerr << "match position: " << match_position
            << " string length: " << (int)strlen(s) << endl);
        return match_position == (int) strlen(s);
    }

    CPPUNIT_TEST_SUITE (GridTest);

    CPPUNIT_TEST(test_get_array);
    CPPUNIT_TEST(test_using_var_begin);
    CPPUNIT_TEST(test_get_vars_iter_for_array);
    CPPUNIT_TEST(test_get_var_index_for_array);
    CPPUNIT_TEST(test_get_vars_iter_for_map);
    CPPUNIT_TEST(test_get_var_index_for_map);
    CPPUNIT_TEST(test_get_map_iter);
    CPPUNIT_TEST(test_map_begin);
    CPPUNIT_TEST(test_map_rbegin);

    CPPUNIT_TEST_SUITE_END();

    void test_get_array()
    {
        DBG(cerr << "d_grid->get_array()->name(): " << d_grid->get_array()->name() << endl);
        CPPUNIT_ASSERT(d_grid->get_array()->name() == "array_proto");
        DBG(cerr << "d_grid->get_array(): " << hex << d_grid->get_array() << endl);
        DBG(cerr << "d_array: " << hex << d_array << endl);
        CPPUNIT_ASSERT(d_grid->get_array() == d_array);
    }

    // Constructor::vars_begin(). The first variable in the 'vars' vector is the array
    void test_using_var_begin()
    {
        DBG(cerr << "(*d_grid->var_begin())->name(): " << (*d_grid->var_begin())->name() << endl);
        CPPUNIT_ASSERT((*d_grid->var_begin())->name() == "array_proto");
        DBG(cerr << "*d_grid->var_begin(): " << hex << *d_grid->var_begin() << endl);
        DBG(cerr << "d_array: " << hex << d_array << endl);
        CPPUNIT_ASSERT(*d_grid->var_begin() == d_array);
    }

    void test_get_vars_iter_for_array()
    {
        DBG(cerr << "(*d_grid->get_vars_iter(0))->name(): " << (*d_grid->get_vars_iter(0))->name() << endl);
        CPPUNIT_ASSERT((*d_grid->get_vars_iter(0))->name() == "array_proto");
        DBG(cerr << "*d_grid->get_vars_iter(0): " << hex << *d_grid->get_vars_iter(0) << endl);
        DBG(cerr << "d_array: " << hex << d_array << endl);
        CPPUNIT_ASSERT(*d_grid->get_vars_iter(0) == d_array);
    }

    void test_get_var_index_for_array()
    {
        DBG(cerr << "d_grid->get_var_index(0)->name(): " << d_grid->get_var_index(0)->name() << endl);
        CPPUNIT_ASSERT(d_grid->get_var_index(0)->name() == "array_proto");
        DBG(cerr << "d_grid->get_var_index(0): " << hex << d_grid->get_var_index(0) << endl);
        DBG(cerr << "d_array: " << hex << d_array << endl);
        CPPUNIT_ASSERT(d_grid->get_var_index(0) == d_array);
    }

    void test_get_vars_iter_for_map()
    {
        DBG(cerr << "(*d_grid->get_vars_iter(1))->name(): " << (*d_grid->get_vars_iter(1))->name() << endl);
        CPPUNIT_ASSERT((*d_grid->get_vars_iter(1))->name() == "map_proto");
        DBG(cerr << "*d_grid->get_vars_iter(1): " << hex << *d_grid->get_vars_iter(1) << endl);
        DBG(cerr << "d_map: " << hex << d_map << endl);
        CPPUNIT_ASSERT(*d_grid->get_vars_iter(1) == d_map);
    }

    void test_get_var_index_for_map()
    {
        DBG(cerr << "d_grid->get_var_index(1)->name(): " << d_grid->get_var_index(1)->name() << endl);
        CPPUNIT_ASSERT(d_grid->get_var_index(1)->name() == "map_proto");
        DBG(cerr << "d_grid->get_var_index(1): " << hex << d_grid->get_var_index(1) << endl);
        DBG(cerr << "d_map: " << hex << d_map << endl);
        CPPUNIT_ASSERT(d_grid->get_var_index(1) == d_map);
    }

    void test_get_map_iter()
    {
        DBG(cerr << "(*d_grid->get_map_iter(0))->name(): " << (*d_grid->get_map_iter(0))->name() << endl);
        CPPUNIT_ASSERT((*d_grid->get_map_iter(0))->name() == "map_proto");
        DBG(cerr << "*d_grid->get_map_iter(0): " << hex << *d_grid->get_map_iter(0) << endl);
        DBG(cerr << "d_array: " << hex << d_array << endl);
        CPPUNIT_ASSERT(*d_grid->get_map_iter(0) == d_map);
    }

    void test_map_begin()
    {
        DBG(cerr << "(*d_grid->map_begin())->name(): " << (*d_grid->map_begin())->name() << endl);
        CPPUNIT_ASSERT((*d_grid->map_begin())->name() == "map_proto");
        DBG(cerr << "*d_grid->map_begin(): " << hex << *d_grid->map_begin() << endl);
        DBG(cerr << "d_map: " << hex << d_map << endl);
        CPPUNIT_ASSERT(*d_grid->map_begin() == d_map);
    }

    // because there is only one map, the first map and the last map are the same
    void test_map_rbegin()
    {
        DBG(cerr << "(*d_grid->map_rbegin())->name(): " << (*d_grid->map_rbegin())->name() << endl);
        CPPUNIT_ASSERT((*d_grid->map_rbegin())->name() == "map_proto");
        DBG(cerr << "*d_grid->map_rbegin(): " << hex << *d_grid->map_rbegin() << endl);
        DBG(cerr << "d_map: " << hex << d_map << endl);
        CPPUNIT_ASSERT(*d_grid->map_rbegin() == d_map);
    }

#if 0
    Map_iter get_map_iter(int i);

    Map_riter map_rbegin() ;
    Map_iter map_begin() ;
#endif


#if 0
    Vars_iter get_vars_iter(int i);
    BaseType *get_var_index(int i);
#endif
};

CPPUNIT_TEST_SUITE_REGISTRATION (GridTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    int option_char;
    while ((option_char = getopt(argc, argv, "dh")) != EOF) {
        switch (option_char) {
            case 'd':
                debug = true;  // debug is a static global
                break;

            case 'h': {     // help - show test names
                cerr << "Usage: ArrayTest has the following tests:" << endl;
                const std::vector<Test *> &tests = libdap::GridTest::suite()->getTests();
                unsigned int prefix_len = libdap::GridTest::suite()->getName().append("::").length();
                for (std::vector<Test *>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                    cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
                }
                break;
            }

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
        int i = 0;
        while (i < argc) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = libdap::GridTest::suite()->getName().append("::").append(argv[i++]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
