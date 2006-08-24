
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
#include "DAS.h"
#include "GeoConstraint.h"
#include "ce_functions.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

class CEFunctionsTest:public TestFixture {
  private:
    DDS * dds;
    BaseTypeFactory btf;
    ConstraintEvaluator ce;

    DDS *geo_dds;

  public:
     CEFunctionsTest() {
    } ~CEFunctionsTest() {
    }

    void setUp() {
        try {
            dds = new DDS(&btf);
            dds->parse("unit-tests/ce-functions-testsuite/two_grid.dds");
            DBG2(dds->print(stderr));
            // Load values into the grid variables
            Grid & a = dynamic_cast < Grid & >(*dds->var("a"));
            Array & m1 = dynamic_cast < Array & >(**a.map_begin());
            dods_float64 first_a[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

            m1.val2buf(first_a);
            m1.set_read_p(true);

            Grid & b = dynamic_cast < Grid & >(*dds->var("b"));
            Array & m2 = dynamic_cast < Array & >(**b.map_begin());
            dods_float64 first_b[10] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

            m2.val2buf(first_b);
            m2.set_read_p(true);
        }

        catch(Error & e) {
            cerr << "SetUp: " << e.get_error_message() << endl;
            throw;
        }
        
        // geo grid test data
        try {
            geo_dds = new DDS(&btf);
            geo_dds->parse("unit-tests/ce-functions-testsuite/geo_grid.dds");
            DAS das;
            das.parse("unit-tests/ce-functions-testsuite/geo_grid.das");
            geo_dds->transfer_attributes(&das);
            
            DBG2(geo_dds->print_xml(stderr, false, "No blob"));
        
            // Load values into the grid variables
            Grid & sst1 = dynamic_cast < Grid & >(*geo_dds->var("SST1"));
            Array & lon1 = dynamic_cast < Array & >(**sst1.map_begin());
            dods_float64 tmp_lon1[10] =
                { 0, 40, 80, 120, 160, 200, 240, 280, 320, 359 };
            lon1.val2buf(tmp_lon1);
            lon1.set_read_p(true);
        
            Array & lat1 = dynamic_cast < Array & >(**(sst1.map_begin() + 1));
            dods_float64 tmp_lat1[10] =
                { -40, -30, -20, -10, 0, 10, 20, 30, 40, 50 };
            lat1.val2buf(tmp_lat1);
            lat1.set_read_p(true);
        
            // Load values into the grid variables
            Grid & sst2 = dynamic_cast < Grid & >(*geo_dds->var("SST2"));
            Array & lon2 = dynamic_cast < Array & >(**sst2.map_begin());
            dods_float64 tmp_lon2[10] =
                { -179, -120, -80, -40, 0, 40, 80, 120, 160, 179 };
            lon2.val2buf(tmp_lon2);
            lon2.set_read_p(true);
        
            Array & lat2 = dynamic_cast < Array & >(**(sst2.map_begin() + 1));
            dods_float64 tmp_lat2[10] =
                { -40, -30, -20, -10, 0, 10, 20, 30, 40, 50 };
            lat2.val2buf(tmp_lat2);
            lat2.set_read_p(true);
        
            // Load values into the grid variables
            Grid & sst3 = dynamic_cast < Grid & >(*geo_dds->var("SST3"));
            Array & lon3 = dynamic_cast < Array & >(**sst3.map_begin());
            dods_float64 tmp_lon3[10] =
                { 20, 60, 100, 140, 180, 220, 260, 300, 340, 379 };
            lon3.val2buf(tmp_lon3);
            lon3.set_read_p(true);
        
            Array & lat3 = dynamic_cast < Array & >(**(sst3.map_begin() + 1));
            dods_float64 tmp_lat3[10] =
                { -40, -30, -20, -10, 0, 10, 20, 30, 40, 50 };
            lat3.val2buf(tmp_lat3);
            lat3.set_read_p(true);
        }
        
        catch(Error & e)
        {
            cerr << "SetUp: " << e.get_error_message() << endl;
            throw;
        }
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

    CPPUNIT_TEST(geoconstraint_find_lat_lon_maps_test);

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
    
    void geoconstraint_find_lat_lon_maps_test() {
        try {
        Grid *g = dynamic_cast<Grid*>(geo_dds->var("SST1"));
        CPPUNIT_ASSERT(g);
        GeoConstraint gc(g, *geo_dds);
        CPPUNIT_ASSERT(gc.find_lat_lon_maps());
        CPPUNIT_ASSERT(gc.d_longitude->name() == "lon");
        CPPUNIT_ASSERT(gc.d_latitude->name() == "lat");
        }
        catch (Error &e) {
            cerr << "find_lat_lon_maps: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"find_lat_lon_maps should not have thrown Error")'
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
