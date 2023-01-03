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

string truth(bool v){ return (v?"true":"false"); }

namespace libdap {

class IsDap4ProjectedTest: public CppUnit::TestFixture {

private:

public:

    // Called once before everything gets tested
    IsDap4ProjectedTest() = default;

    // Called at the end of the test
    ~IsDap4ProjectedTest() = default;

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
        vector<string> inv;
        Int8 var("ivar");
        var.set_send_p(true);

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
        vector<string> inv;
        Byte var("bvar");
        var.set_send_p(true);

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

        // void Array::print_decl(ostream &out, string space, bool print_semi, bool constraint_info, bool constrained)

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
        Byte var("byte");
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
        auto *avar = new Array("avar", new Int8("pvar"));
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
        Structure structure("struct");
        auto *array = new Array("array", new Byte("byte"));
        structure.add_var_nocopy(array);
        vector<string> inv;

        bool result = structure.is_dap4_projected(inv);
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
        Byte byte("bvar");
        auto *d4a = new D4Attribute("d4a", attr_int8_c);
        byte.attributes()->add_attribute(d4a);
        vector<string> inv;

        bool result = byte.is_dap4_projected(inv);
        DBG(cerr << prolog << "byte.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        byte.set_send_p(true);
        result = byte.is_dap4_projected(inv);
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
        Byte byte("byte");
        auto *d4a = new D4Attribute("d4a", attr_byte_c);
        byte.attributes()->add_attribute(d4a);
        vector<string> inv;

        bool result = byte.is_dap4_projected(inv);
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
        Array avar("avar", &bvar);
        D4Attribute *d4a = new D4Attribute("d4a", attr_int8_c);
        avar.attributes()->add_attribute(d4a);
        vector<string> inv;

        bool result = avar.is_dap4_projected(inv);
        DBG(cerr << prolog << "array.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        avar.set_send_p(true);
        result = avar.is_dap4_projected(inv);
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
        Byte byte("byte");
        Array array("array", &byte);
        auto *d4a = new D4Attribute("d4a", attr_byte_c);
        array.attributes()->add_attribute(d4a);
        vector<string> inv;

        bool result = array.is_dap4_projected(inv);
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
        Structure svar("svar");
        auto *avar = new Array("avar", new Byte("byte"));
        auto *d4a = new D4Attribute("d4a", attr_int8_c);
        avar->attributes()->add_attribute(d4a);
        svar.add_var_nocopy(avar);
        vector<string> inv;

        bool result = svar.is_dap4_projected(inv);
        DBG(cerr << prolog << "structure.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        avar->set_send_p(true);
        svar.set_send_p(true);
        result = svar.is_dap4_projected(inv);
        DBG(cerr << prolog << "structure.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "                   inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "            inv.at(0)->name(): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 svar.byte@d4a");
    }

    /**
    * struct attribute test - false
    * test if a struct with no dap4 attribute returns false
    */
    void test_is_dap4_projected_attr_struct_false() {
        Structure structure("struct");
        Array *array = new Array("array", new Byte("byte"));
        D4Attribute *d4a = new D4Attribute("d4a", attr_byte_c);
        array->attributes()->add_attribute(d4a);
        structure.add_var_nocopy(array);
        vector<string > inv;

        bool result = structure.is_dap4_projected(inv);
        DBG(cerr << prolog << "structure.is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == false);

        array->set_send_p(true);
        result = structure.is_dap4_projected(inv);
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
        DDS *dds = new DDS(&f);
        dds->add_var_nocopy(ivar);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, *dds);

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
        Byte *byte = new Byte("byte");
        byte->set_send_p(true);

        BaseTypeFactory f;
        DDS *dds = new DDS(&f);
        dds->add_var_nocopy(byte);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, *dds);

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
        Int8 *ivar = new Int8("ivar");
        ivar->set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(ivar);

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
        auto *ivar = new Int64("iVar");
        ivar->set_send_p(true);
        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");
        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(ivar);
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

        Byte *byte = new Byte("byte");
        byte->set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(byte);

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
        Array *avar = new Array("avar", new Int8("ivar"));
        avar->set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(avar);

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
        Array *array = new Array("array", new Byte("byte"));
        array->set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(array);

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
        Structure svar("svar");
        Array *avar = new Array("avar", new Int8("ivar"));
        svar.add_var_nocopy(avar);
        svar.set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(&svar);

        vector<string> inv;

        bool result = dmr->is_dap4_projected(inv);
        DBG(cerr << prolog << "dmr->is_dap4_projected(): " << truth(result) << endl);
        CPPUNIT_ASSERT(result == true);

        DBG(cerr << prolog << "              inv.size(): " << inv.size() << endl);
        CPPUNIT_ASSERT(inv.size() == 1);

        DBG(cerr << prolog << "       inv.at(0): " << inv.at(0) << endl);
        CPPUNIT_ASSERT(inv.at(0) == "Int8 /svar.ivar");
    }

    /**
     * dmr struct variable test - false
     * test if a dmr with a struct not containing dap4 vars returns false
     */
    void test_is_dap4_projected_dmr_struct_byte() {
        Structure structure("struct");
        Array *array = new Array("array", new Byte("byte"));
        structure.set_send_p(true);
        structure.add_var_nocopy(array);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(array);

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
        Byte bvar("bvar");
        D4Attribute *d4a = new D4Attribute("d4a", attr_int8_c);
        bvar.attributes()->add_attribute(d4a);
        bvar.set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(&bvar);

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
        Byte byte("byte");
        D4Attribute *d4a = new D4Attribute("d4a", attr_byte_c);
        byte.attributes()->add_attribute(d4a);
        byte.set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(&byte);

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
        Byte bvar("bvar");
        Array avar("avar", &bvar);
        D4Attribute *d4a = new D4Attribute("d4a", attr_int8_c);
        avar.attributes()->add_attribute(d4a);
        avar.set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(&avar);

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
        Byte byte("byte");
        Array array("array", &byte);
        D4Attribute *d4a = new D4Attribute("d4a", attr_byte_c);
        array.attributes()->add_attribute(d4a);
        array.set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(&array);

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
        Structure svar("svar");
        Array *avar = new Array("avar", new Byte("bvar"));
        avar->append_dim(7,"");
        D4Attribute *d4a = new D4Attribute("d4a", attr_int8_c);
        avar->attributes()->add_attribute(d4a);
        svar.add_var_nocopy(avar);
        svar.set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(&svar);

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
        Structure structure("struct");
        Array *array = new Array("array", new Byte("byte"));
        D4Attribute *d4a = new D4Attribute("d4a", attr_byte_c);
        array->attributes()->add_attribute(d4a);
        structure.add_var_nocopy(array);
        structure.set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *d4g = dmr->root();
        d4g->add_var_nocopy(&structure);

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
        DMR *dmr = new DMR(&f4, "test");
        D4Group *rootg = dmr->root();

        Int8 ivar("ivar");
        D4Group *subd4g = new D4Group("subgroup");
        subd4g->add_var_nocopy(&ivar);
        rootg->add_group_nocopy(subd4g);

        ivar.set_send_p(true);
        subd4g->set_send_p(true);

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
        Byte bvar("bvar");
        bvar.set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *subd4g = new D4Group("subgroup");
        subd4g->add_var_nocopy(&bvar);
        subd4g->set_send_p(true);

        D4Group *d4g = dmr->root();
        d4g->add_group_nocopy(subd4g);

        vector<string> inv;

        // @TODO What is the correct response here?
        //   Because the child group subd4g is marked to send.
        //   And even if it only contains DAP2 things, it's still a Group,
        //   and that's DAP4. The test came in expecting that this would
        //   be not projected.

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
        Float32 a_proto("fvar");
        Array *avar = new Array("avar", &a_proto);
        D4Attribute *d4a = new D4Attribute("d4a", attr_int8_c);
        avar->attributes()->add_attribute(d4a);
        avar->set_send_p(true);

        D4BaseTypeFactory f4;
        DMR *dmr = new DMR(&f4, "test");

        D4Group *bgroup = new D4Group("b_grp");
        D4Group *cgroup = new D4Group("c_grp");
        cgroup->add_var_nocopy(avar);
        cgroup->set_send_p(true);
        bgroup->add_group_nocopy(cgroup);
        bgroup->set_send_p(true);

        D4Group *d4g = dmr->root();
        d4g->add_group_nocopy(bgroup);

        vector<string> inv;

        // @TODO What is the correct response here?
        //   Because the child group subd4g is marked to send.
        //   And even if it only contains DAP2 things, it's still a Group,
        //   and that's DAP4. The test came in expecting that this would
        //   be not projected.

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

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(IsDap4ProjectedTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    return run_tests<libdap::IsDap4ProjectedTest>(argc, argv) ? 0: 1;
}

