// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
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

// Tests for the AISResources class.

#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <algorithm>
#include <stdexcept>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "AISResources.h"
#include "debug.h"
#include <test_config.h>

using namespace CppUnit;

namespace libdap {

class AISResourcesTest: public TestFixture {
private:
    AISResources *ais;
    string fnoc1, fnoc2, fnoc3, regexp, bears, three_fnoc, one_2_3;
    string fnoc1_ais, fnoc2_ais, fnoc3_ais, digit_ais, one_2_3_ais;

public:
    AISResourcesTest()
    {
    }
    ~AISResourcesTest()
    {
    }

    void setUp()
    {
        fnoc1 = "http://test.opendap.org/opendap/data/nc/fnoc1.nc";
        fnoc2 = "http://test.opendap.org/opendap/data/nc/fnoc2.nc";
        fnoc3 = "http://test.opendap.org/opendap/data/nc/fnoc3.nc";

        regexp = "http://test.opendap.org/opendap/data/nc/[0-9]+.*\\.nc";
        bears = "http://test.opendap.org/opendap/data/nc/123bears.nc";
        three_fnoc = "http://test.opendap.org/opendap/data/nc/3fnoc.nc";
        one_2_3 = "http://test.opendap.org/opendap/data/nc/123.nc";

        fnoc1_ais = "http://test.opendap.org/ais/fnoc1.nc.das";
        fnoc2_ais = (string) TEST_SRC_DIR + "/ais_testsuite/fnoc2_replace.das";
        fnoc3_ais = "http://test.opendap.org/ais/fnoc3_fallback.das";
        digit_ais = (string) TEST_SRC_DIR + "/ais_testsuite/starts_with_number.das";
        one_2_3_ais = (string) TEST_SRC_DIR + "/ais_testsuite/123.das";

        ais = new AISResources;

        Resource r1(fnoc1_ais);
        ais->add_url_resource(fnoc1, r1);

        Resource r2(fnoc2_ais, Resource::fallback);
        ais->add_url_resource(fnoc2, r2);

        ais->add_url_resource(fnoc3, r1);
        ais->add_url_resource(fnoc3, r2);

        Resource r3(digit_ais);
        ais->add_regexp_resource(regexp, r3);

        Resource r4(one_2_3_ais);
        ais->add_url_resource(one_2_3, r4);
    }

    void tearDown()
    {
        delete ais;
        ais = 0;
    }

    CPPUNIT_TEST_SUITE (AISResourcesTest);

    CPPUNIT_TEST (add_url_resource_test);
    CPPUNIT_TEST (add_regexp_resource_test);
    CPPUNIT_TEST (add_url_resource_vector_test);
    CPPUNIT_TEST (has_resource_test);
    CPPUNIT_TEST (get_resource_test);
    CPPUNIT_TEST (read_database_test);
    CPPUNIT_TEST (write_database_test);

    CPPUNIT_TEST_SUITE_END();

    void add_url_resource_test()
    {
        // setUp() makes the add_url_resource calls. 02/13/03 jhrg
        CPPUNIT_ASSERT(ais->d_db.find(fnoc1) != ais->d_db.end());
        CPPUNIT_ASSERT(ais->d_db.find(fnoc1)->second.size() == 1);
        CPPUNIT_ASSERT(ais->d_db.find(fnoc1)->second[0].get_url() == fnoc1_ais);
        CPPUNIT_ASSERT(ais->d_db.find(fnoc1)->second[0].get_rule() == Resource::overwrite);

        CPPUNIT_ASSERT(ais->d_db.find(fnoc2) != ais->d_db.end());
        CPPUNIT_ASSERT(ais->d_db.find(fnoc2)->second.size() == 1);
        CPPUNIT_ASSERT(ais->d_db.find(fnoc2)->second[0].get_url() == fnoc2_ais);
        CPPUNIT_ASSERT(ais->d_db.find(fnoc2)->second[0].get_rule() == Resource::fallback);

        CPPUNIT_ASSERT(ais->d_db.find(fnoc3) != ais->d_db.end());
        CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second.size() == 2);
        CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second[0].get_url() == fnoc1_ais);
        CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second[1].get_url() == fnoc2_ais);
        CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second[0].get_rule() == Resource::overwrite);
        CPPUNIT_ASSERT(ais->d_db.find(fnoc3)->second[1].get_rule() == Resource::fallback);
    }

    void add_regexp_resource_test()
    {
        AISResources::ResourceRegexpsIter pos;
        pos = find_if(ais->d_re.begin(), ais->d_re.end(), AISResources::FindRegexp(regexp));
        CPPUNIT_ASSERT(pos != ais->d_re.end());
        CPPUNIT_ASSERT(pos->second.size() == 1);
        CPPUNIT_ASSERT(pos->second[0].get_url() == digit_ais);
        CPPUNIT_ASSERT(pos->second[0].get_rule() == Resource::overwrite);
    }

    void add_url_resource_vector_test()
    {
        AISResources *ais2 = new AISResources;

        Resource r1(fnoc1_ais);
        ResourceVector rv1(1, r1);
        ais2->add_url_resource(fnoc2, rv1);

        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2) != ais2->d_db.end());
        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second.size() == 1);
        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[0].get_url() == fnoc1_ais);
        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[0].get_rule() == Resource::overwrite);

        Resource r2(fnoc2_ais, Resource::fallback);
        ResourceVector rv2(1, r2);
        ais2->add_url_resource(fnoc2, rv2);

        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2) != ais2->d_db.end());
        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second.size() == 2);
        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[0].get_url() == fnoc1_ais);
        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[1].get_url() == fnoc2_ais);
        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[0].get_rule() == Resource::overwrite);
        CPPUNIT_ASSERT(ais2->d_db.find(fnoc2)->second[1].get_rule() == Resource::fallback);
    }

    void has_resource_test()
    {
        CPPUNIT_ASSERT(ais->has_resource(fnoc1));
        CPPUNIT_ASSERT(ais->has_resource(fnoc2));
        CPPUNIT_ASSERT(ais->has_resource(fnoc3));
        CPPUNIT_ASSERT(ais->has_resource(bears));
        CPPUNIT_ASSERT(ais->has_resource(three_fnoc));
    }

    void get_resource_test()
    {
        ResourceVector trv1 = ais->get_resource(fnoc1);
        CPPUNIT_ASSERT(trv1.size() == 1);
        CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
        CPPUNIT_ASSERT(trv1[0].get_rule() == Resource::overwrite);

        ResourceVector trv2 = ais->get_resource(fnoc2);
        CPPUNIT_ASSERT(trv2.size() == 1);
        CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
        CPPUNIT_ASSERT(trv2[0].get_rule() == Resource::fallback);

        ResourceVector trv3 = ais->get_resource(fnoc3);
        CPPUNIT_ASSERT(trv3.size() == 2);
        CPPUNIT_ASSERT(trv3[0].get_url() == fnoc1_ais);
        CPPUNIT_ASSERT(trv3[0].get_rule() == Resource::overwrite);
        CPPUNIT_ASSERT(trv3[1].get_url() == fnoc2_ais);
        CPPUNIT_ASSERT(trv3[1].get_rule() == Resource::fallback);

        ResourceVector trv4 = ais->get_resource(bears);
        CPPUNIT_ASSERT(trv4.size() == 1);
        CPPUNIT_ASSERT(trv4[0].get_url() == digit_ais);
        CPPUNIT_ASSERT(trv4[0].get_rule() == Resource::overwrite);

        ResourceVector trv5 = ais->get_resource(three_fnoc);
        CPPUNIT_ASSERT(trv5.size() == 1);
        CPPUNIT_ASSERT(trv5[0].get_url() == digit_ais);
        CPPUNIT_ASSERT(trv5[0].get_rule() == Resource::overwrite);

        ResourceVector trv6 = ais->get_resource(one_2_3);
        CPPUNIT_ASSERT(trv6.size() == 2);
        CPPUNIT_ASSERT(
            (trv6[0].get_url() == one_2_3_ais && trv6[1].get_url() == digit_ais)
                || (trv6[1].get_url() == one_2_3_ais && trv6[0].get_url() == digit_ais));
        CPPUNIT_ASSERT(trv6[0].get_rule() == Resource::overwrite);
        CPPUNIT_ASSERT(trv6[1].get_rule() == Resource::overwrite);

        try {
            ResourceVector trv4 = ais->get_resource("http://never");
            CPPUNIT_ASSERT(!"get_resource() failed to throw NoSuchPrimaryResource");
        }
        catch (NoSuchPrimaryResource &nspr) {
            CPPUNIT_ASSERT("get_resource() correctly threw NoSuchPrimaryResource");
        }
    }

    void read_database_test()
    {
        try {
            AISResources *ais2 = new AISResources;
            ais2->read_database("ais_testsuite/ais_database.xml");

            ResourceVector trv1 = ais2->get_resource(fnoc1);
            CPPUNIT_ASSERT(trv1.size() == 1);
            CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
            CPPUNIT_ASSERT(trv1[0].get_rule() == Resource::overwrite);

            ResourceVector trv2 = ais2->get_resource(fnoc2);
            CPPUNIT_ASSERT(trv2.size() == 1);
            CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
            CPPUNIT_ASSERT(trv2[0].get_rule() == Resource::replace);

            ResourceVector trv3 = ais2->get_resource(fnoc3);
            CPPUNIT_ASSERT(trv3.size() == 1);
            CPPUNIT_ASSERT(trv3[0].get_url() == fnoc3_ais);
            CPPUNIT_ASSERT(trv3[0].get_rule() == Resource::fallback);

            ResourceVector trv6 = ais->get_resource(one_2_3);
            CPPUNIT_ASSERT(trv6.size() == 2);
            CPPUNIT_ASSERT(
                (trv6[0].get_url() == one_2_3_ais && trv6[1].get_url() == digit_ais)
                    || (trv6[1].get_url() == one_2_3_ais && trv6[0].get_url() == digit_ais));
            CPPUNIT_ASSERT(trv6[0].get_rule() == Resource::overwrite);
            CPPUNIT_ASSERT(trv6[1].get_rule() == Resource::overwrite);
        }
        catch (AISDatabaseReadFailed &adrf) {
            CPPUNIT_ASSERT(!"Document not well formed and/or valid!");
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Caught Error");
        }
        catch (std::exception &e) {
            cerr << "std::exception: " << e.what() << endl;
            CPPUNIT_ASSERT(!"Caught exception");
        }
    }

    void write_database_test()
    {
        try {
            ais->write_database("dummy.xml");
            AISResources *ais2 = new AISResources;

            ais2->read_database("dummy.xml");

            ResourceVector trv1 = ais2->get_resource(fnoc1);
            CPPUNIT_ASSERT(trv1.size() == 1);
            CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
            CPPUNIT_ASSERT(trv1[0].get_rule() == Resource::overwrite);

            ResourceVector trv2 = ais2->get_resource(fnoc2);
            CPPUNIT_ASSERT(trv2.size() == 1);
            CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
            CPPUNIT_ASSERT(trv2[0].get_rule() == Resource::fallback);

            ResourceVector trv3 = ais2->get_resource(fnoc3);
            CPPUNIT_ASSERT(trv3.size() == 2);
            CPPUNIT_ASSERT(trv3[0].get_url() == fnoc1_ais);
            CPPUNIT_ASSERT(trv3[0].get_rule() == Resource::overwrite);
            CPPUNIT_ASSERT(trv3[1].get_url() == fnoc2_ais);
            CPPUNIT_ASSERT(trv3[1].get_rule() == Resource::fallback);

            ResourceVector trv6 = ais->get_resource(one_2_3);
            CPPUNIT_ASSERT(trv6.size() == 2);
            CPPUNIT_ASSERT(
                (trv6[0].get_url() == one_2_3_ais && trv6[1].get_url() == digit_ais)
                    || (trv6[1].get_url() == one_2_3_ais && trv6[0].get_url() == digit_ais));
            CPPUNIT_ASSERT(trv6[0].get_rule() == Resource::overwrite);
            CPPUNIT_ASSERT(trv6[1].get_rule() == Resource::overwrite);
        }
        catch (AISDatabaseReadFailed &adrf) {
            CPPUNIT_ASSERT(!"Document not well formed and/or valid!");
        }
        catch (AISDatabaseWriteFailed &adwf) {
            CPPUNIT_ASSERT(!"Write failed!");
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (AISResourcesTest);

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
            cerr << "Usage: AISResourcesTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::AISResourcesTest::suite()->getTests();
            unsigned int prefix_len = libdap::AISResourcesTest::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
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
            test = libdap::AISResourcesTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
