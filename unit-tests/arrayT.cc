
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#if 0

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
// #include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#endif

#include "TestArray.h"
#include "TestInt16.h"
#include "TestTypeFactory.h"

#include "run_tests_cppunit.h"

using std::cerr;
using std::endl;

using namespace CppUnit;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
// timeouts.

class arrayT : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(arrayT);

    CPPUNIT_TEST(arrayT_test);

    CPPUNIT_TEST_SUITE_END();

private:
    /* TEST PRIVATE DATA */
    TestTypeFactory *factory{nullptr};

public:
    void setUp() override { factory = new TestTypeFactory; }

    void tearDown() override {
        delete factory;
        factory = nullptr;
    }

    void arrayT_test() {
        BaseType *bt = factory->NewInt16();

        TestArray ar("My Array", bt);

        int l = ar.length();
        CPPUNIT_ASSERT(l == -1);

        try {
            unsigned int w = ar.width(true);
            DBG(cerr << "w = " << (int)w << endl);
            DBG(cerr << "bt->width() " << bt->width() << endl);
            DBG(cerr << "L " << l << endl);
            CPPUNIT_ASSERT(w == (l * bt->width()));
        } catch (const InternalErr &e) {
            CPPUNIT_FAIL(string("Unable to retrieve width: ") + e.get_error_message());
        }

        ar.append_dim(4, "dim1");

        l = ar.length();
        CPPUNIT_ASSERT(l == 4);

        try {
            unsigned int w = ar.width();
            CPPUNIT_ASSERT(w == (l * bt->width()));
        } catch (const InternalErr &e) {
            CPPUNIT_FAIL(string("Unable to retrieve width: ") + e.get_error_message());
        }

        ar.append_dim(3, "dim2");

        l = ar.length();
        CPPUNIT_ASSERT(l == 12);

        try {
            unsigned int w = ar.width();
            CPPUNIT_ASSERT(w == (l * bt->width()));
        } catch (const InternalErr &e) {
            CPPUNIT_FAIL(string("Unable to retrieve width: ") + e.get_error_message());
        }

        ar.append_dim(2, "dim3");

        l = ar.length();
        CPPUNIT_ASSERT(l == 24);

        try {
            int w = ar.width();
            CPPUNIT_ASSERT(w == (l * (int)bt->width()));
        } catch (InternalErr &e) {
            CPPUNIT_FAIL(string("Unable to retrieve width: ") + e.get_error_message());
        }

        vector<string> dims;
        // using citer = vector<string>::const_iterator;
        dims.emplace_back("dim1");
        dims.emplace_back("dim2");
        dims.emplace_back("dim3");

        vector<int> dimsize;
        // using dsiter = vector<int>::const_iterator;
        dimsize.push_back(4);
        dimsize.push_back(3);
        dimsize.push_back(2);

#if 0
        citer i = dims.begin();
        dsiter d = dimsize.begin();
#endif
        auto diter = ar.dim_begin();
        auto i = dims.begin();
        auto d = dimsize.begin();
        for (; diter != ar.dim_end() && i != dims.end(); diter++, i++, d++) {
            CPPUNIT_ASSERT(ar.dimension_name(diter) == (*i));
            if (ar.dimension_name(diter) == (*i)) {
                CPPUNIT_ASSERT(ar.dimension_size(diter) == (*d));
                CPPUNIT_ASSERT(ar.dimension_start(diter) == 0);
                CPPUNIT_ASSERT(ar.dimension_stop(diter) == (*d) - 1);
                CPPUNIT_ASSERT(ar.dimension_stride(diter) == 1);
            }
        }
        if (diter != ar.dim_end() && i == dims.end()) {
            CPPUNIT_FAIL("too many dimensions");
        } else if (diter == ar.dim_end() && i != dims.end()) {
            CPPUNIT_FAIL("not enough dimensions");
        }

        unsigned int numdims = ar.dimensions();
        CPPUNIT_ASSERT(numdims == 3);

        diter = ar.dim_begin();
        ar.add_constraint(diter, 0, 2, 3);

        l = ar.length();
        CPPUNIT_ASSERT(l == 12);

        diter = ar.dim_begin();
        diter++;
        ar.add_constraint(diter, 0, 2, 2);

        l = ar.length();
        CPPUNIT_ASSERT(l == 8);

        ar.reset_constraint();

        l = ar.length();
        CPPUNIT_ASSERT(l == 24);

        ar.rename_dim("dim3", "myDim");

        bool is_read = ar.read();
        CPPUNIT_ASSERT(is_read == true);

        cout << ar << endl;

        delete bt;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(arrayT);

int main(int argc, char *argv[]) { return run_tests<arrayT>(argc, argv) ? 0 : 1; }
