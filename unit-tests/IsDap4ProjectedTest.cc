// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2022 OPeNDAP, Inc.
// Author: Samuel Lloyd <slloyd@opendap.org>
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

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <cppunit/extensions/HelperMacros.h>

#include "BaseType.h"
#include "Int8.h"
#include "Int64.h"
#include "UInt64.h"
#include "Byte.h"
#include "Float32.h"
#include "Array.h"
#include "Structure.h"
#include "D4Attributes.h"

#include "D4BaseTypeFactory.h"
#include "DMR.h"

#include "DapObj.h"
#include "D4Group.h"

#include "run_tests_cppunit.h"

using namespace std;

#define prolog std::string("D4ToolsTests::").append(__func__).append("() - ")

string truth(bool v) { return (v ? "true" : "false"); }

namespace libdap {

class IsDap4ProjectedTest: public CppUnit::TestFixture {

public:

    // Called once before everything gets tested
    IsDap4ProjectedTest() = default;

    // Called at the end of the test
    ~IsDap4ProjectedTest() override = default;

    void setUp() override {
        DBG(cerr << endl);
    }
    /////////////////////////////////////////////////////////////////
    /// DAP2 AND DDS TESTS
    /////////////////////////////////////////////


    /**
     * basic variable test - true
     * tests if dap4 variables returns true
     */
    void test_is_dap4_projected_int8() {
        Int8 var("ivar");
        var.set_send_p(true);

        vector<string> inv;
        bool is_projected = var.is_dap4_projected(inv);

        DBG(cerr << prolog << "var.is_dap4_projected(): " << truth(is_projected) << endl);
        CPPUNIT_ASSERT(is_projected == true);

        DBG(cerr << prolog << "             inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "      inv.at(0)->name(): " << inv.at(0)<< endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 ivar");
    }

    /**
     * basic variable test - false
     * tests if non-dap4 variables returns false
     */
    void test_is_dap4_projected_byte() {
        Byte var("bvar");
        var.set_send_p(true);

        vector<string> inv;
        bool is_projected = var.is_dap4_projected(inv);

        DBG(cerr << prolog << "var.is_dap4_projected(): " << truth(is_projected) << endl);
        CPPUNIT_ASSERT(is_projected == false);

        DBG(cerr << prolog << "            inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
     * array variable test - true
     * tests if array containing dap4 vars returns true
     */
    void test_is_dap4_projected_array_int8() {
        Int8 pvar("pvar");
        Array avar("avar", &pvar);
        avar.append_dim(5, "dim0");
        avar.append_dim(10, "dim1");
        avar.append_dim(15, "dim2");
        avar.set_send_p(true);

        if(debug) {
            cerr << prolog;
            avar.print_decl(cerr, " ", false, false, true);
            cerr << endl;
        }

        vector<string> inv;
        bool is_projected = avar.is_dap4_projected(inv);

        DBG(cerr << prolog << "array.is_dap4_projected(): " << truth(is_projected) << endl);
        CPPUNIT_ASSERT(is_projected == true);

        DBG(cerr << prolog << "               inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "        inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 pvar[dim0=5][dim1=10][dim2=15]");
    }

    /**
     * array variable test - false
     * test if array containing no dap4 vars returns false
     */
    void test_is_dap4_projected_array_byte() {
        Byte var("bvar");
        Array array("array", &var);
        array.set_send_p(false);

        vector<string> inv;
        bool is_projected = array.is_dap4_projected(inv);

        DBG(cerr << prolog << "array.is_dap4_projected(): " << truth(is_projected) << endl);
        CPPUNIT_ASSERT(is_projected == false);

        DBG(cerr << prolog << "              inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
     * struct variable test - true
     * test if struct containing dap4 vars returns true
     */
    void test_is_dap4_projected_struct_int8() {
        Structure svar("svar");
        Int8 pvar("pvar");
        auto *avar = new Array("avar", &pvar);
        avar->append_dim(1, "dim0");
        avar->append_dim(2, "dim1");
        avar->append_dim(3, "dim2");
        svar.add_var_nocopy(avar);
        svar.set_send_p(true);
        avar->set_send_p(true);

        vector<string> inv;
        bool result = svar.is_dap4_projected(inv);

        DBG(cerr << prolog << "structure.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "                   inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "            inv.at(0)->name(): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 svar.pvar[dim0=1][dim1=2][dim2=3]");
    }

    /**
     * struct variable test - false
     * test if struct containing no dap4 vars returns false
     */
    void test_is_dap4_projected_struct_byte() {
        Structure svar("svar");
        Byte bvar("bvar");
        auto *avar = new Array("avar", &bvar);
        svar.add_var_nocopy(avar);

        vector<string> inv;
        bool result = svar.is_dap4_projected(inv);

        DBG(cerr << prolog << "structure.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "                  inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }


    /**
     * basic variable attribute test - true
     * test if a basic var with dap4 attribute returns true
     */
    void test_is_dap4_projected_attr_true() {
        Byte bvar("bvar");
        auto *d4a = new D4Attribute("d4a", attr_int8_c);
        bvar.attributes()->add_attribute_nocopy(d4a);

        vector<string> inv;
        bool result = bvar.is_dap4_projected(inv);

        DBG(cerr << prolog << "byte.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        bvar.set_send_p(true);
        result = bvar.is_dap4_projected(inv);

        DBG(cerr << prolog << "byte.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0)->name(): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 bvar@d4a");
    }

    /**
    * basic variable attribute test - false
    * test if a basic var with no dap4 attribute returns false
    */
    void test_is_dap4_projected_attr_false() {
        Byte bvar("bvar");
        auto *d4a = new D4Attribute("d4a", attr_byte_c);
        bvar.attributes()->add_attribute_nocopy(d4a);

        vector<string> inv;
        bool result = bvar.is_dap4_projected(inv);

        DBG(cerr << prolog << "byte.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "             inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
    * array attribute test - true
    * test if an array with dap4 attribute returns true
    */
    void test_is_dap4_projected_attr_array_true() {

        Byte bvar("bvar");
        auto avar = unique_ptr<Array>(new Array("avar", &bvar));
        avar->append_dim(10,"dim1");

        auto d4a = new D4Attribute("d4a", attr_int8_c);
        avar->attributes()->add_attribute_nocopy(d4a);

        vector<string> inv;
        bool result = avar->is_dap4_projected(inv);

        DBG(cerr << prolog << "array.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        avar->set_send_p(true);
        result = avar->is_dap4_projected(inv);
        DBG(cerr << prolog << "array.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "               inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "        inv.at(0)->name(): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 bvar@d4a");
    }

    /**
    * array attribute test - false
    * test if an array with no dap4 attribute returns false
    */
    void test_is_dap4_projected_attr_array_false() {

        Byte bvar("bvar");
        auto avar = unique_ptr<Array>(new Array("avar", &bvar));

        auto *d4a = new D4Attribute("d4a", attr_byte_c);
        avar->attributes()->add_attribute_nocopy(d4a);

        vector<string> inv;
        bool result = avar->is_dap4_projected(inv);

        DBG(cerr << prolog << "array.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "              inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
    * struct attribute test - true
    * test if a struct with dap4 attribute returns true
    */
    void test_is_dap4_projected_attr_struct_true() {
        auto svar = unique_ptr<Structure>(new Structure("svar"));

        Byte bvar("bvar");
        auto avar = new Array("avar", &bvar);
        auto d4a = new D4Attribute("d4a", attr_int8_c);
        avar->attributes()->add_attribute_nocopy(d4a);
        svar->add_var_nocopy(avar);

        vector<string> inv;
        bool result = svar->is_dap4_projected(inv);

        DBG(cerr << prolog << "structure->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        avar->set_send_p(true);
        svar->set_send_p(true);
        result = svar->is_dap4_projected(inv);
        DBG(cerr << prolog << "structure->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "                    inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "             inv.at(0)->name(): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 svar.bvar@d4a");
    }

    /**
    * struct attribute test - false
    * test if a struct with no dap4 attribute returns false
    */
    void test_is_dap4_projected_attr_struct_false() {
        auto svar = unique_ptr<Structure>(new Structure("svar"));

        Byte bvar("bvar");
        auto avar = new Array("avar", &bvar);
        auto d4a = new D4Attribute("d4a", attr_byte_c);
        avar->attributes()->add_attribute_nocopy(d4a);
        svar->add_var_nocopy(avar);

        vector<string > inv;
        bool result = svar->is_dap4_projected(inv);

        DBG(cerr << prolog << "structure.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        avar->set_send_p(true);
        result = svar->is_dap4_projected(inv);

        DBG(cerr << prolog << "structure.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "                  inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }


    /////////////////////////////////////////////////////////////////
    /// DAP4 AND DMR TESTS
    /////////////////////////////////////////////

    /**
     * DMR Int8 variable test - true
     * test if DMR containing dap4 var [Int8] returns true
     */
    void test_is_dap4_projected_dmr_ddsint8() {

        Int8 *ivar = new Int8("ivar");
        ivar->set_send_p(true);

        BaseTypeFactory f;
        auto dds = unique_ptr<DDS>(new DDS(&f, "test_dds"));
        dds->add_var_nocopy(ivar);

        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, *dds));

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /ivar");

    }

    /**
     * DMR Int8 variable test - true
     * test if DMR containing dap4 var [Int8] returns true
     */
    void test_is_dap4_projected_dmr_ddsbyte() {

        auto bvar = new Byte("bvar");
        bvar->set_send_p(true);

        BaseTypeFactory f;
        auto dds = unique_ptr<DDS>(new DDS(&f, "test_dds"));
        dds->add_var_nocopy(bvar);

        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, *dds));

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "             inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }


    /**
     * DMR variable test - true
     * test if DMR containing a dap4 int8 var returns true
     */
    void test_is_dap4_projected_dmr_int8() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto *ivar = new Int8("ivar");
        ivar->set_send_p(true);
        dmr->root()->add_var_nocopy(ivar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /ivar");
    }

    /**
     * DMR variable test - true
     * test if DMR containing a dap4 int64 var returns true
     */
    void test_is_dap4_projected_dmr_int64() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto *ivar = new Int64("iVar");
        ivar->set_send_p(true);
        dmr->root()->add_var_nocopy(ivar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int64 /iVar");

    }

    /**
     * DMR variable test - false
     * test if DMR not containing a dap4 var returns false
     */
    void test_is_dap4_projected_dmr_byte() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto bvar = new Byte("bvar");
        bvar->set_send_p(true);
        dmr->root()->add_var_nocopy(bvar);

        vector<string> inv;

        bool result = dmr->is_dap4_projected(inv);
        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "             inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
     * dmr array variable test - true
     * tests if a dmr with an array containing a dap4 vars returns true
     */
    void test_is_dap4_projected_dmr_array_int8() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        Int8 ivar("ivar");
        auto avar = new Array("avar", &ivar);
        avar->set_send_p(true);
        dmr->root()->add_var_nocopy(avar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /ivar");

    }

    /**
     * dmr array variable test - false
     * tests if a dmr with an array not containing a dap4 var returns false
     */
    void test_is_dap4_projected_dmr_array_byte() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        Byte bvar("bvar");
        auto avar = new Array("avar", &bvar);
        avar->set_send_p(true);
        dmr->root()->add_var_nocopy(avar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "             inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
     * dmr struct variable test - true
     * test if a dmr with a struct containing dap4 vars returns true
     */
    void test_is_dap4_projected_dmr_struct_int8() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        Int8 ivar("ivar");
        auto avar = new Array("avar", &ivar);
        avar->append_dim(10,"dim1");
        avar->append_dim(20,"");
        avar->set_send_p(true);

        auto svar = new Structure("svar");
        svar->add_var_nocopy(avar);
        svar->set_send_p(true);

        dmr->root()->add_var_nocopy(svar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /svar.ivar[dim1=10][20]");
    }

    /**
     * dmr struct variable test - false
     * test if a dmr with a struct not containing dap4 vars returns false
     */
    void test_is_dap4_projected_dmr_struct_byte() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        Byte bvar("bvar");
        auto avar = new Array("avar", &bvar);
        avar->append_dim(10,"dim1");
        avar->set_send_p(true);

        auto svar = new Structure("svar");
        svar->set_send_p(true);
        svar->add_var_nocopy(avar);
        dmr->root()->add_var_nocopy(svar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "             inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);

    }

    /**
     * dmr basic variable attribute test - true
     * test if a dmr holding a basic var with dap4 attribute returns true
     */
    void test_is_dap4_projected_dmr_attr_true() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto bvar = new Byte("bvar");
        auto d4a = new D4Attribute("d4a", attr_int8_c);
        bvar->attributes()->add_attribute_nocopy(d4a);
        bvar->set_send_p(true);
        dmr->root()->add_var_nocopy(bvar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /bvar@d4a");
    }

    /**
    * dmr basic variable attribute test - false
    * test if a dmr holding a basic var without dap4 attribute returns false
    */
    void test_is_dap4_projected_dmr_attr_false() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto bvar = new Byte("bvar");
        auto d4a = new D4Attribute("d4a", attr_byte_c);
        bvar->attributes()->add_attribute_nocopy(d4a);
        bvar->set_send_p(true);
        dmr->root()->add_var_nocopy(bvar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "             inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
    * dmr array attribute test - true
    * test if a dmr holding an array with dap4 attribute returns true
    */
    void test_is_dap4_projected_dmr_attr_array_true() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        Byte bvar("bvar");
        auto avar = new Array("avar", &bvar);
        auto d4a = new D4Attribute("d4a", attr_int8_c);
        avar->attributes()->add_attribute_nocopy(d4a);
        avar->set_send_p(true);
        dmr->root()->add_var_nocopy(avar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /bvar@d4a");
    }

    /**
    * dmr array attribute test - false
    * test if a dmr holding an array without dap4 attribute returns false
    */
    void test_is_dap4_projected_dmr_attr_array_false() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        Byte bvar("bvar");
        auto avar = new Array("avar", &bvar);
        auto d4a = new D4Attribute("d4a", attr_byte_c);
        avar->attributes()->add_attribute_nocopy(d4a);
        avar->set_send_p(true);
        dmr->root()->add_var_nocopy(avar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "             inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
    * dmr struct attribute test - true
    * test if a dmr holding a struct with dap4 attribute returns true
    */
    void test_is_dap4_projected_dmr_attr_struct_true() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto d4a = new D4Attribute("d4a", attr_int8_c);
        Byte bvar("bvar");
        auto avar = new Array("avar", &bvar);
        avar->append_dim(7,"");
        avar->attributes()->add_attribute_nocopy(d4a);

        auto svar = new Structure("svar");
        svar->add_var_nocopy(avar);
        svar->set_send_p(true);
        dmr->root()->add_var_nocopy(svar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /svar.bvar@d4a");

    }

    /**
    * dmr struct attribute test - false
    * test if a dmr holding a struct without dap4 attribute returns false
    */
    void test_is_dap4_projected_dmr_attr_struct_false() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto d4a = new D4Attribute("d4a", attr_byte_c);
        Byte bvar("bvar");
        auto avar = new Array("avar", &bvar);
        avar->attributes()->add_attribute_nocopy(d4a);
        avar->set_send_p(true);

        auto svar = new Structure("svar");
        svar->add_var_nocopy(avar);
        svar->set_send_p(true);
        dmr->root()->add_var_nocopy(svar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        DBG(cerr << prolog << "             inv.empty(): " << truth(inv.empty()) << endl);
        CPPUNIT_ASSERT(inv.empty() == true);
    }

    /**
    * dmr subgroup variable test - true
    * test if a dmr holding a subgroup with dap4 vars returns true
    */
    void test_is_dap4_projected_dmr_subgroup_true() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto ivar = new Int8("ivar");
        ivar->set_send_p(true);

        auto subd4g = new D4Group("subgroup");
        subd4g->add_var_nocopy(ivar);
        subd4g->set_send_p(true);
        dmr->root()->add_group_nocopy(subd4g);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 2);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Group /subgroup/");

        DBG(cerr << prolog << "       inv.at(1)->name(): " << inv.at(1) << endl);
        CPPUNIT_ASSERT(inv.at(1) == "Int8 /subgroup/ivar");
    }

    /**
    * dmr subgroup variable test - false
    * test if a dmr holding a subgroup with no dap4 vars returns false
    */
    void test_is_dap4_projected_dmr_subgroup_true_too() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        auto bvar = new Byte("bvar");
        bvar->set_send_p(true);

        auto subd4g = new D4Group("subgroup");
        subd4g->add_var_nocopy(bvar);
        subd4g->set_send_p(true);
        dmr->root()->add_group_nocopy(subd4g);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "             inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Group /subgroup/");
    }

    /**
    * dmr subgroup variable test - false
    * test if a dmr holding a subgroup with no dap4 vars returns false
    */
    void test_is_dap4_projected_nested_group_array_attr() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        Float32 a_proto("fvar");
        auto avar = new Array("avar", &a_proto);
        auto d4a = new D4Attribute("d4a", attr_int8_c);
        avar->attributes()->add_attribute_nocopy(d4a);
        avar->set_send_p(true);

        auto cgroup = new D4Group("c_grp");
        cgroup->add_var_nocopy(avar);
        cgroup->set_send_p(true);

        auto bgroup = new D4Group("b_grp");
        bgroup->add_group_nocopy(cgroup);
        bgroup->set_send_p(true);
        dmr->root()->add_group_nocopy(bgroup);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "             inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 3);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Group /b_grp/");

        DBG(cerr << prolog << "       inv.at(1): " << inv.at(1) << endl);
        CPPUNIT_ASSERT(inv.at(1) == "Group /b_grp/c_grp/");

        DBG(cerr << prolog << "       inv.at(2): " << inv.at(2) << endl);
        CPPUNIT_ASSERT(inv.at(2) == "Int8 /b_grp/c_grp/fvar@d4a");
    }

    /**
     * dmr global dap4 attribute test.
     * true
     * test if a dmr with a dap4 attribute in the root group
     */
    void test_is_dap4_projected_global_d4_attr() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        Float32 a_proto("fvar");
        auto avar = new Array("avar", &a_proto);
        avar->set_send_p(true);

        auto cgroup = new D4Group("c_grp");
        cgroup->add_var_nocopy(avar);
        cgroup->set_send_p(true);

        auto bgroup = new D4Group("b_grp");
        bgroup->add_group_nocopy(cgroup);
        bgroup->set_send_p(true);
        dmr->root()->add_group_nocopy(bgroup);

        auto d4a = new D4Attribute("d4a", attr_int8_c);
        dmr->root()->attributes()->add_attribute_nocopy(d4a);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "      inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 3);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /@d4a");

        DBG(cerr << prolog << "       inv.at(1): " << inv.at(1) << endl);
        CPPUNIT_ASSERT(inv.at(1) == "Group /b_grp/");

        DBG(cerr << prolog << "       inv.at(2): " << inv.at(2) << endl);
        CPPUNIT_ASSERT(inv.at(2) == "Group /b_grp/c_grp/");

    }
    /**
     * dmr global dap4 attribute test.
     * true
     * test if a dmr with a dap4 attribute in the root group
     */
    void alternate_test() {
        D4BaseTypeFactory f4;
        DMR dmr(&f4, "test");

        Float32 a_proto("fvar");
        auto avar = new Array("avar", &a_proto);
        avar->set_send_p(true);

        auto cgroup = new D4Group("c_grp");
        cgroup->add_var_nocopy(avar);
        cgroup->set_send_p(true);

        auto bgroup = new D4Group("b_grp");
        bgroup->add_group_nocopy(cgroup);
        bgroup->set_send_p(true);

        dmr.root()->add_group_nocopy(bgroup);

        auto d4a = new D4Attribute("d4a", attr_int8_c);
        dmr.root()->attributes()->add_attribute_nocopy(d4a);

        vector<string> inv;
        bool result = dmr.is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "      inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 3);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /@d4a");

        DBG(cerr << prolog << "       inv.at(1): " << inv.at(1) << endl);
        CPPUNIT_ASSERT(inv.at(1) == "Group /b_grp/");

        DBG(cerr << prolog << "       inv.at(2): " << inv.at(2) << endl);
        CPPUNIT_ASSERT(inv.at(2) == "Group /b_grp/c_grp/");
    }


    /**
     * dmr global dap4 attribute test.
     * true
     * test if a dmr with a dap4 attribute in the root group
     */
    void test_is_dap4_projected_array_of_struct_with_d4() {
        D4BaseTypeFactory f4;
        auto dmr = unique_ptr<DMR>(new DMR(&f4, "test"));

        // auto svar_proto = new Structure("svar");
        Structure svar_proto("svar");
        auto fvar =  new Float32("fvar");
        svar_proto.add_var_nocopy(fvar);
        auto d4var = new UInt64("d4var");
        svar_proto.add_var_nocopy(d4var);

        auto avar = new Array("avar", &svar_proto);
        avar->append_dim(10,"dim1");
        avar->append_dim(20,"");
        avar->set_send_p(true);

        dmr->root()->add_var_nocopy(avar);

        vector<string> inv;
        bool result = dmr->is_dap4_projected(inv);

        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "      inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "UInt64 /svar.d4var");
    }

    /**
 * DMR Int8 variable test - true
 * test if DMR containing dap4 var [Int8] returns true
 */
    void test_dds_with_dap4_global_attr() {

        auto bvar = new Byte("bvar");
        bvar->set_send_p(true);

        BaseTypeFactory f;
        // These container variables don't need to be pointers. The things they contain
        // do, however. jhrg 1/30/23
        // auto dds = unique_ptr<DDS>(new DDS(&f, "test_dds"));
        DDS dds(&f, "test_dds");
        dds.add_var_nocopy(bvar);

        // Add an attribute that is a DAP4 type (Int8). NB: we don't care about the return
        // value here. jhrg 1/30/23
        (void) dds.get_attr_table().append_attr("ima_d4thing","Int8","0");

        vector<string> inv;
        bool result = dds.is_dap4_projected(inv);

        DBG(cerr << prolog << "dds->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /@ima_d4thing");
    }

    ///////////////////////////////////////////////////////
    /// DDS/DAP2 Tests

    CPPUNIT_TEST_SUITE( IsDap4ProjectedTest );

        CPPUNIT_TEST(test_is_dap4_projected_int8);
        CPPUNIT_TEST(test_is_dap4_projected_byte);

        CPPUNIT_TEST(test_is_dap4_projected_array_int8);
        CPPUNIT_TEST(test_is_dap4_projected_array_byte);

        CPPUNIT_TEST(test_is_dap4_projected_struct_int8);
        CPPUNIT_TEST(test_is_dap4_projected_struct_byte);

        CPPUNIT_TEST(test_is_dap4_projected_attr_true);
        CPPUNIT_TEST(test_is_dap4_projected_attr_false);

        CPPUNIT_TEST(test_is_dap4_projected_attr_array_true);
        CPPUNIT_TEST(test_is_dap4_projected_attr_array_false);

        CPPUNIT_TEST(test_is_dap4_projected_attr_struct_true);
        CPPUNIT_TEST(test_is_dap4_projected_attr_struct_false);

        ///////////////////////////////////////////////////////
        /// DMR/DAP4 Tests

        CPPUNIT_TEST(test_is_dap4_projected_dmr_ddsint8);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_ddsbyte);

        CPPUNIT_TEST(test_is_dap4_projected_dmr_int8);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_int64);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_byte);

        CPPUNIT_TEST(test_is_dap4_projected_dmr_array_int8);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_array_byte);

        CPPUNIT_TEST(test_is_dap4_projected_dmr_struct_int8);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_struct_byte);

        CPPUNIT_TEST(test_is_dap4_projected_dmr_attr_true);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_attr_false);

        CPPUNIT_TEST(test_is_dap4_projected_dmr_attr_array_true);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_attr_array_false);

        CPPUNIT_TEST(test_is_dap4_projected_dmr_attr_struct_true);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_attr_struct_false);

        CPPUNIT_TEST(test_is_dap4_projected_dmr_subgroup_true);
        CPPUNIT_TEST(test_is_dap4_projected_dmr_subgroup_true_too);
        CPPUNIT_TEST(test_is_dap4_projected_nested_group_array_attr);
        CPPUNIT_TEST(test_is_dap4_projected_global_d4_attr);
        CPPUNIT_TEST(alternate_test);
        CPPUNIT_TEST(test_is_dap4_projected_array_of_struct_with_d4);
        CPPUNIT_TEST(test_dds_with_dap4_global_attr);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(IsDap4ProjectedTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    return run_tests<libdap::IsDap4ProjectedTest>(argc, argv) ? 0: 1;
}

