
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2006 OPeNDAP, Inc.
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

#include "BaseType.h"
#include "Str.h"
#include "Array.h"
#include "Grid.h"
#include "DDS.h"
#include "ce_functions.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

class CEFunctionsTest:public TestFixture {
private:
    DDS *dds;
    BaseTypeFactory btf;
    ConstraintEvaluator ce;
    
public:
    CEFunctionsTest() {} 
    ~CEFunctionsTest() {} 

    void setUp() {
        dds = new DDS(&btf);
        dds->parse("ce-functions-testsuite/two_grid.dds");
        DBG2(dds->print(stderr));
        // Load values into the grid variables
        Grid &a = dynamic_cast<Grid&>(*dds->var("a"));
        Array &m1 = dynamic_cast<Array&>(**a.map_begin());
        dods_float64 first_a[10] = {0,1,2,3,4,5,6,7,8,9};
        m1.val2buf(first_a);
        m1.set_read_p(true);
        
        Grid &b = dynamic_cast<Grid&>(*dds->var("b"));
        Array &m2 = dynamic_cast<Array&>(**b.map_begin());
        dods_float64 first_b[10] = {9,8,7,6,5,4,3,2,1,0};
        m2.val2buf(first_b);
        m2.set_read_p(true);
    } 

    void tearDown() {
        delete dds;
    }

    CPPUNIT_TEST_SUITE( CEFunctionsTest );

    // Test void projection_function_grid(int argc, BaseType *argv[], DDS &dds)

    CPPUNIT_TEST(no_arguments_test);
    CPPUNIT_TEST(one_argument_test);
    CPPUNIT_TEST(one_argument_not_a_grid_test);
    CPPUNIT_TEST(map_not_in_grid_test);
    CPPUNIT_TEST(one_dim_grid_test);
    CPPUNIT_TEST(one_dim_grid_two_expressions_test);
    CPPUNIT_TEST(one_dim_grid_noninclusive_values_test);
    CPPUNIT_TEST(one_dim_grid_descending_test);
    CPPUNIT_TEST(one_dim_grid_two_expressions_descending_test);
#if 0
    // grid() is not required to handle this case.
    CPPUNIT_TEST(values_outside_map_range_test);
#endif

    CPPUNIT_TEST_SUITE_END();
        
    void no_arguments_test() {
        try {
            projection_function_grid(0, 0, *dds, ce);
            CPPUNIT_ASSERT(!"no_arguments_test() should have failed");
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(true);
        }
    }
    
    void one_argument_test() {
        try {
            BaseType *argv[1];
            argv[0] = dds->var("a");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this");
            projection_function_grid(1, argv, *dds, ce);
            CPPUNIT_ASSERT("one_argument_not_a_grid_test() should work");
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(!"one_argument_test should not fail");
        }
    }

    void one_argument_not_a_grid_test() {
        try {
            BaseType *argv[1];
            argv[0] = dds->var("lat");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this, although it is not a grid");
            projection_function_grid(1, argv, *dds, ce);
            CPPUNIT_ASSERT(!"one_argument_not_a_grid_test() should have failed");
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(true);
        }
    }
    
    void map_not_in_grid_test() {
        try {
            BaseType *argv[2];
            argv[0] = dds->var("a");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this");
            argv[1] = new Str;
            string expression = "3<second<=7";
            dynamic_cast<Str*>(argv[1])->val2buf(&expression);
            dynamic_cast<Str*>(argv[1])->set_read_p(true);
            projection_function_grid(2, argv, *dds, ce);
            CPPUNIT_ASSERT(!"map_not_in_grid_test() should have failed");
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(true);
        }
    }
    
    void one_dim_grid_test() {
        try {
            BaseType *argv[2];
            argv[0] = dds->var("a");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this");
            argv[1] = new Str;
            string expression = "3<first<=7";
            dynamic_cast<Str*>(argv[1])->val2buf(&expression);
            dynamic_cast<Str*>(argv[1])->set_read_p(true);
            
            projection_function_grid(2, argv, *dds, ce);
            
            Grid &g = dynamic_cast<Grid&>(*argv[0]);
            Array &m = dynamic_cast<Array&>(**g.map_begin());
            CPPUNIT_ASSERT(m.dimension_start(m.dim_begin(), true) == 4);
            CPPUNIT_ASSERT(m.dimension_stop(m.dim_begin(), true) == 7);
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(!"one_dim_grid_test() should have worked");
        }
    }
    
    void one_dim_grid_two_expressions_test() {
        try {
            BaseType *argv[3];
            argv[0] = dds->var("a");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this");
            
            argv[1] = new Str;
            string expression = "first>3";
            dynamic_cast<Str*>(argv[1])->val2buf(&expression);
            dynamic_cast<Str*>(argv[1])->set_read_p(true);

            argv[2] = new Str;
            expression = "first<=7";
            dynamic_cast<Str*>(argv[2])->val2buf(&expression);
            dynamic_cast<Str*>(argv[2])->set_read_p(true);

            projection_function_grid(3, argv, *dds, ce);
            
            Grid &g = dynamic_cast<Grid&>(*argv[0]);
            Array &m = dynamic_cast<Array&>(**g.map_begin());
            CPPUNIT_ASSERT(m.dimension_start(m.dim_begin(), true) == 4);
            CPPUNIT_ASSERT(m.dimension_stop(m.dim_begin(), true) == 7);
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(!"one_dim_grid_two_expressions_test() should have worked");
        }
    }
    
    void one_dim_grid_descending_test() {
        try {
            BaseType *argv[2];
            argv[0] = dds->var("b");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this");
            argv[1] = new Str;
            string expression = "3<first<=7";
            dynamic_cast<Str*>(argv[1])->val2buf(&expression);
            dynamic_cast<Str*>(argv[1])->set_read_p(true);
            
            projection_function_grid(2, argv, *dds, ce);

            Grid &g = dynamic_cast<Grid&>(*argv[0]);
            Array &m = dynamic_cast<Array&>(**g.map_begin());
            CPPUNIT_ASSERT(m.dimension_start(m.dim_begin(), true) == 2);
            CPPUNIT_ASSERT(m.dimension_stop(m.dim_begin(), true) == 5);
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(!"one_dim_grid_test() should have worked");
        }
    }
    
    void one_dim_grid_two_expressions_descending_test() {
        try {
            BaseType *argv[3];
            argv[0] = dds->var("b");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this");
            
            argv[1] = new Str;
            string expression = "first>3";
            dynamic_cast<Str*>(argv[1])->val2buf(&expression);
            dynamic_cast<Str*>(argv[1])->set_read_p(true);

            argv[2] = new Str;
            expression = "first<=7";
            dynamic_cast<Str*>(argv[2])->val2buf(&expression);
            dynamic_cast<Str*>(argv[2])->set_read_p(true);

            projection_function_grid(3, argv, *dds, ce);

            Grid &g = dynamic_cast<Grid&>(*argv[0]);
            Array &m = dynamic_cast<Array&>(**g.map_begin());
            CPPUNIT_ASSERT(m.dimension_start(m.dim_begin(), true) == 2);
            CPPUNIT_ASSERT(m.dimension_stop(m.dim_begin(), true) == 5);
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(!"one_dim_grid_two_expressions_test() should have worked");
        }
    }
    
    void one_dim_grid_noninclusive_values_test() {
        try {
            BaseType *argv[2];
            argv[0] = dds->var("a");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this");
            argv[1] = new Str;
            string expression = "7<first<=3";
            dynamic_cast<Str*>(argv[1])->val2buf(&expression);
            dynamic_cast<Str*>(argv[1])->set_read_p(true);
            
            projection_function_grid(2, argv, *dds, ce);

            CPPUNIT_ASSERT(!"one_dim_grid_noninclusive_values_test() should not have worked");
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(true);
        }
    }
    
    // grid() is not required to handle this case. This test is not used.
    void values_outside_map_range_test() {
        try {
            BaseType *argv[2];
            argv[0] = dds->var("a");
            CPPUNIT_ASSERT(argv[0] && "dds->var should find this");
            argv[1] = new Str;
            string expression = "3<=first<20";
            dynamic_cast<Str*>(argv[1])->val2buf(&expression);
            dynamic_cast<Str*>(argv[1])->set_read_p(true);
            
            projection_function_grid(2, argv, *dds, ce);

            CPPUNIT_ASSERT(!"values_outside_map_range_test() should not have worked");
        }
        catch (Error &e) {
            DBG(cerr << e.get_error_message() << endl);
            CPPUNIT_ASSERT(true);
        }        
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CEFunctionsTest);

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    return wasSuccessful ? 0 : 1;
}
