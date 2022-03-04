#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include "TestSequence.h"
#include "TestInt16.h"
#include "TestStr.h"
#include "TestTypeFactory.h"
#include "util.h"
#include "run_tests_cppunit.h"
#include "test_config.h"

using std::cerr;
using std::endl;

using namespace libdap;
using namespace CppUnit;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
// timeouts.

class sequenceT: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE (sequenceT);
    CPPUNIT_TEST (sequenceT_test);CPPUNIT_TEST_SUITE_END( );

private:
    /* TEST PRIVATE DATA */
    TestTypeFactory *factory;

public:
    void setUp()
    {
        factory = new TestTypeFactory;
    }

    void tearDown()
    {
        delete factory;
        factory = 0;
    }

    void sequenceT_test()
    {
        TestSequence s("Supporters");

        BaseType *nm = factory->NewStr("Name");
        s.add_var(nm);

        BaseType *age = factory->NewInt16("Age");
        s.add_var(age);

        TestSequence *friends = (TestSequence *) factory->NewSequence("Friends");
        friends->add_var(nm);
        delete nm;
        nm = 0;
        friends->add_var(age);
        delete age;
        age = 0;
        s.add_var(friends);
        delete friends;
        friends = 0;

        BaseType *bt = s.var("Age");
        CPPUNIT_ASSERT(bt->name() == "Age");

        bt = s.var("Age", false);
        CPPUNIT_ASSERT(bt->name() == "Age");

        BaseType::btp_stack btps;
        bt = s.var("Friends.Age", btps);
        CPPUNIT_ASSERT(bt->name() == "Age");
        CPPUNIT_ASSERT(btps.size() == 2);

        vector<string> names;
        names.push_back("Friends");
        names.push_back("Supporters");
        typedef vector<string>::const_iterator names_iter;
        names_iter n = names.begin();
        while (!btps.empty() && n != names.end()) {
            BaseType *curr = btps.top();
            CPPUNIT_ASSERT(curr->name() == (*n));
            btps.pop();
            n++;
        }

        int num_elems = s.element_count(false);
        CPPUNIT_ASSERT(num_elems == 3);

        num_elems = s.element_count(true);
        CPPUNIT_ASSERT(num_elems == 4);

#if 0
        // This is only true on 32 bit machines, on a 64bit machine is will
        // probably be 24.
        unsigned int w = s.width( );
        CPPUNIT_ASSERT( w == 12 );
#endif

        vector<string> elems;
        elems.push_back("Name");
        elems.push_back("Age");
        elems.push_back("Friends");
        typedef vector<string>::const_iterator elems_iter;

        Sequence::Vars_iter v = s.var_begin();
        elems_iter e = elems.begin();
        for (; v != s.var_end() && e != elems.end(); v++, e++) {
            CPPUNIT_ASSERT((*v)->name() == (*e));
        }
        CPPUNIT_ASSERT(v == s.var_end() && e == elems.end());
        if (v != s.var_end() && e == elems.end()) {
            CPPUNIT_FAIL("Too many elements");
        }
        else if (v == s.var_end() && e != elems.end()) {
            CPPUNIT_FAIL("Too few elements");
        }

        int num_rows = s.number_of_rows();
        CPPUNIT_ASSERT(num_rows == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (sequenceT);

/* NOTHING NEEDS TO BE CHANGED BELOW HERE */

int main(int argc, char*argv[])
{
    return run_tests<sequenceT>(argc, argv) ? 0: 1;
}
