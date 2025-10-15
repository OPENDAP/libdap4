// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2014 OPeNDAP, Inc.
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
#include <cppunit/extensions/HelperMacros.h>

#include <sstream>

#include "Array.h"
#include "Byte.h"
#include "D4BaseTypeFactory.h"
#include "D4ParserSax2.h"
#include "DDS.h"
#include "DMR.h"
#include "Sequence.h"
#include "XMLWriter.h"

#include "GNURegex.h"
#include "util.h"

#include "run_tests_cppunit.h"
#include "testFile.h"
#include "test_config.h"

static bool debug2 = false;

#undef DBG
#define DBG(x)                                                                                                         \
    do {                                                                                                               \
        if (debug) {                                                                                                   \
            x;                                                                                                         \
        }                                                                                                              \
    } while (false)
#undef DBG2
#define DBG2(x)                                                                                                        \
    do {                                                                                                               \
        if (debug2) {                                                                                                  \
            x;                                                                                                         \
        }                                                                                                              \
    } while (false)

using namespace CppUnit;
using namespace std;
using namespace libdap;

static string THE_TESTS_DIR("/dmr-rt-testsuite/");

class DmrRoundTripTest : public TestFixture {
private:
    string d_prefix = string(TEST_SRC_DIR) + THE_TESTS_DIR;
    string d_build_prefix = string(TEST_BUILD_DIR) + "/";

public:
    DmrRoundTripTest() = default;
    ~DmrRoundTripTest() override = default;

    static bool re_match(Regex &r, const string &s) {
        const int match = r.match(s.c_str(), static_cast<int>(s.length()));
        DBG(cerr << "Match: " << match << " should be: " << s.length() << endl);
        return match == static_cast<int>(s.length());
    }

    /**
     * Given the name of a DDS and optional DAS file, build a DMR using the
     * hackery known as transform_to_dap4 and the new DMR ctor.
     *
     * @param dds_file
     * @param das_file
     * @return A pointer to the new DMR; caller must delete
     */
    DMR *build_dmr(const string &dds_file, const string &das_file = "") const {
        DBG(cerr << __func__ << "() - BEGIN" << endl);
        DBG(cerr << __func__ << "() - dds_file: " << dds_file << endl);
        DBG(cerr << __func__ << "() - das_file: " << das_file << endl);

        try {
            BaseTypeFactory factory;
            DDS dds(&factory, dds_file);
            dds.parse(d_prefix + dds_file);
            DBG2(cerr << "SOURCE DDS: " << d_prefix + dds_file << endl; dds.print(cerr));

            if (!das_file.empty()) {
                DAS das;
                das.parse(d_prefix + das_file);
                dds.transfer_attributes(&das);
                DBG2(cerr << "SOURCE DAS: " << d_prefix + das_file << endl; das.print(cerr));

                DBG2(cerr << "dds.print_das(): " << endl; dds.print_das(cerr));
            }

            D4BaseTypeFactory d4_factory;
            DBG(cerr << __func__ << "() - END" << endl);
            return new DMR(&d4_factory, dds);
        } catch (Error &e) {
            CPPUNIT_FAIL(string("Caught Error: ") + e.get_error_message());
        }
    }

    /**
     * @brief Build a DMR object from a DMR response XML file.
     * @param dmr_file The XML file
     * @param print Defaults to false; if true, print the built DMR
     * @return A unique_ptr<DMR>
     */
    static unique_ptr<DMR> parse_dmr(const string &dmr_file, bool print = false) {
        DBG(cerr << __func__ << "() - dmr_file: " << dmr_file << endl);
        D4BaseTypeFactory factory;
        auto dataset = make_unique<DMR>(&factory, path_to_filename(dmr_file));
        dataset->use_checksums(false);

        if (!dmr_file.empty()) {
            fstream in(dmr_file, ios_base::in);
            D4ParserSax2 parser;
            parser.intern(in, dataset.get(), debug);
        }

        DBG(cerr << __func__ << "() - Parse successful" << endl);
        if (print) {
            XMLWriter xml("    ");
            dataset->print_dap4(xml, false);
            cout << xml.get_doc() << endl;
        }

        return dataset;
    }

    // This will now write out the 'result' responses to file when debug is true. jhrg 8/1/25
    void test_roundtrip_template(const string &dds_file, const string &dmr_baseline,
                                 const string &das_file = "") const {
        DBG(cerr << __func__ << "() - BEGIN" << endl);
        unique_ptr<DMR> dmr = nullptr;
        try {
            dmr.reset(build_dmr(dds_file, das_file));
            XMLWriter xml;
            dmr->print_dap4(xml);

            string result_dmr(xml.get_doc());
            string baseline_dmr = read_test_baseline(d_prefix + dmr_baseline);

            DBG(cerr << "BASELINE DMR(" << baseline_dmr.size() << " chars): " << d_prefix + dmr_baseline << endl
                     << baseline_dmr << endl);
            DBG(cerr << "RESULT DMR(" << result_dmr.size() << " chars): " << endl << result_dmr << endl);
            if (debug && result_dmr != baseline_dmr) {
                write_test_result(d_build_prefix + "result_" + dmr_baseline, result_dmr);
            }

            CPPUNIT_ASSERT(result_dmr == baseline_dmr);

            unique_ptr<DDS> dds = nullptr;
            dds.reset(dmr->getDDS(true)); // This is the default. jhrg 8/4/25

            std::ostringstream result_dds;
            dds->print(result_dds);

            string source_dds = read_test_baseline(d_prefix + dds_file);
            DBG(cerr << "SOURCE DDS(" << source_dds.size() << " chars): " << d_prefix + dds_file << endl
                     << source_dds << endl);
            DBG(cerr << "RESULT DDS(" << result_dds.str().size() << " chars): " << endl << result_dds.str() << endl);
            if (debug && result_dds.str() != source_dds) {
                write_test_result(d_build_prefix + "result_" + dds_file, result_dds.str());
            }

            CPPUNIT_ASSERT(result_dds.str() == source_dds);

            if (!das_file.empty()) {
                std::ostringstream result_das;
                dds->print_das(result_das);

                string source_das = read_test_baseline(d_prefix + das_file);
                DBG(cerr << "SOURCE DAS(" << source_das.size() << " chars): " << d_prefix + das_file << endl
                         << source_das << endl);
                DBG(cerr << "RESULT DAS(" << result_das.str().size() << " chars): " << endl
                         << result_das.str() << endl);
                if (debug && result_das.str() != source_das) {
                    write_test_result(d_build_prefix + "result_" + das_file, result_das.str());
                }

                CPPUNIT_ASSERT(result_das.str() == source_das);
            }
        } catch (const Error &e) {
            CPPUNIT_FAIL(string("Caught Error: ") + e.get_error_message());
        }
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dds_to_dmr_to_dds_1() {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_roundtrip_template("attr_test_00.dds", "attr_test_00.dmr", "attr_test_00.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dds_to_dmr_to_dds_2() {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_roundtrip_template("test.1", "test.1.full.dmr", "test.1.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dds_to_dmr_to_dds_3() {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_roundtrip_template("fnoc1.nc.dds", "fnoc1.nc.dmr.xml", "fnoc1.nc.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dds_to_dmr_to_dds_4() {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_roundtrip_template("3B42.980909.5.HDF.dds", "3B42.980909.5.HDF.full.dmr", "3B42.980909.5.HDF.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_dds_to_dmr_to_dds_5() {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        test_roundtrip_template("S2000415.HDF.dds", "S2000415.HDF.full.dmr", "S2000415.HDF.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_grid_rt_01() {
        DBG(cerr << endl
                 << __func__ << "() - BEGIN: " << "Testing simple Grid->D4Array->Grid with 'correct' DAS." << endl);
        //       i_am_broken(__func__);
        test_roundtrip_template("attr_test_01.dds", "attr_test_01.dmr", "attr_test_01.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_grid_rt_02() {
        DBG(cerr << endl << __func__ << "() - BEGIN: " << "Testing Grid->D4Array->Grid with 'correct' DAS." << endl);
        // Replaced "coads_climatology.nc.full.dmr" with "coads_climatology.nc.dmr". The
        // difference is only in the order of the variables. jhrg 8/1/25
        // Replaced "coads_climatology.nc.correct.das" with "coads_climatology.nc.das". The 'correct'
        // DAS seems to be one without the attributes for the 'extra' copies of the Dimensions.
        // This was a long-running debate, but those appear to be the default output of our server.
        // I'm putting them back into this test. jhrg 8/1/25
        test_roundtrip_template("coads_climatology.nc.dds", "coads_climatology.nc.dmr", "coads_climatology.nc.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_grid_rt_03() {
        DBG(cerr << endl << __func__ << "() - BEGIN: " << "Testing Grid->D4Array->Grid with flat DAS." << endl);
        // Replaced "coads_climatology.nc.full.dmr" with "coads_climatology.nc.dmr". jhrg 8/1/25
        test_roundtrip_template("coads_climatology.nc.dds", "coads_climatology.nc.dmr", "coads_climatology.nc.das");
        DBG(cerr << __func__ << "() - END" << endl);
    }

    void test_coads_dmr_to_dds() {
        DBG(cerr << endl << __func__ << "() - BEGIN" << endl);
        // When debug is true, the DMR will print. jhrg 8/1/25
        const auto dmr = parse_dmr(d_prefix + "coads_climatology.nc.dmr", debug);
        const unique_ptr<DDS> dds(dmr->getDDS());
        ostringstream oss;
        dds->print(oss);
        const string baseline_dds = read_test_baseline(d_prefix + "coads_climatology.nc.dds");
        CPPUNIT_ASSERT_MESSAGE("The DDS build from the DMR should match the baseline.", baseline_dds == oss.str());
    }

    CPPUNIT_TEST_SUITE(DmrRoundTripTest);

    CPPUNIT_TEST(test_dds_to_dmr_to_dds_1);
    CPPUNIT_TEST(test_dds_to_dmr_to_dds_2);
    CPPUNIT_TEST(test_dds_to_dmr_to_dds_3);
    CPPUNIT_TEST(test_dds_to_dmr_to_dds_4);
    CPPUNIT_TEST(test_dds_to_dmr_to_dds_5);
    CPPUNIT_TEST(test_grid_rt_01);

    CPPUNIT_TEST(test_grid_rt_02);
    CPPUNIT_TEST(test_grid_rt_03);

    CPPUNIT_TEST(test_coads_dmr_to_dds);

    CPPUNIT_TEST_SUITE_END();
};

// Temporarily turn off this test to reflect the change of escaping special characters.
// Need to re-visit in the future. KY 2022-08-25
// I started work on this again. jhrg 8/1/25
CPPUNIT_TEST_SUITE_REGISTRATION(DmrRoundTripTest);

int main(int argc, char *argv[]) { return run_tests<DmrRoundTripTest>(argc, argv) ? 0 : 1; }
