#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include <vector>
#include "AttrTable.h"
#include "GetOpt.h"

using std::cerr;
using std::endl;
using std::vector;

using namespace CppUnit;
using namespace libdap;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
// timeouts.

static bool debug = false;

class attrTableT: public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE (attrTableT);
    CPPUNIT_TEST(attrTableT_test);CPPUNIT_TEST_SUITE_END( )
    ;

private:
    /* TEST PRIVATE DATA */

public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void attrTableT_test()
    {
        AttrTable at;

        unsigned int at_size = at.get_size();
        CPPUNIT_ASSERT(at_size == 0);

        string at_name = at.get_name();
        CPPUNIT_ASSERT(at_name == "");

        at.set_name("My Attributes");
        at_name = at.get_name();
        CPPUNIT_ASSERT(at_name == "My Attributes");

        AttrTable *container = at.find_container("dummy_container");
        CPPUNIT_ASSERT(!container);

        AttrTable *dummy_at = 0;

        AttrTable::Attr_iter iter;
        at.find("dummy_attr", &dummy_at, &iter);
        CPPUNIT_ASSERT(iter == at.attr_end());

        string attr_name = "attr1";
        string attr_type = "string";
        string attr_value = "attr1Value1";
        at.append_attr(attr_name, attr_type, attr_value);

        attr_name = "attr2";
        attr_type = "string";
        attr_value = "attr2Value1";
        at.append_attr(attr_name, attr_type, attr_value);

        attr_name = "attr3";
        attr_type = "string";
        attr_value = "attr3Value1";
        at.append_attr(attr_name, attr_type, attr_value);

        at.append_attr("attr4", "string", "attr4Value1");

        at_size = at.get_size();
        CPPUNIT_ASSERT(at_size == 4);

        //at.print( stdout ) ;
        iter = at.attr_end();
        at.find("attr3", &dummy_at, &iter);
        CPPUNIT_ASSERT(iter != at.attr_end());

        iter = at.attr_end();
        at.find("dummy_attr", &dummy_at, &iter);
        CPPUNIT_ASSERT(iter == at.attr_end());

        attr_type = at.get_type("attr3");
        CPPUNIT_ASSERT(attr_type == "String");

        AttrType attr_type_enum = at.get_attr_type("attr3");
        CPPUNIT_ASSERT(attr_type_enum == Attr_string);

        unsigned int num_attrs = at.get_attr_num("attr3");
        CPPUNIT_ASSERT(num_attrs == 1);

        attr_value = at.get_attr("attr3");
        CPPUNIT_ASSERT(attr_value == "attr3Value1");

        at.append_attr("attr3", "string", "attr3Value2");
        at.append_attr("attr3", "string", "attr3Value3");
        at.append_attr("attr3", "string", "attr3Value4");

        attr_value = at.get_attr("attr3");
        CPPUNIT_ASSERT(attr_value == "attr3Value1");

        vector<string> sb;
        sb.push_back("attr3Value1");
        sb.push_back("attr3Value2");
        sb.push_back("attr3Value3");
        sb.push_back("attr3Value4");

        typedef vector<string>::const_iterator str_citer;
        typedef vector<string>::iterator str_iter;

        vector<string> *values = at.get_attr_vector("attr3");
        CPPUNIT_ASSERT(values);
        if (values) {
            str_citer vi = values->begin();
            str_citer sbi = sb.begin();
            for (; vi != values->end() && sbi != sb.end(); vi++, sbi++) {
                CPPUNIT_ASSERT((*vi) == (*sbi));
            }
            CPPUNIT_ASSERT(vi == values->end() && sbi == sb.end());
            if (vi == values->end() && sbi != sb.end()) {
                CPPUNIT_FAIL("not enough values");
            }
            else if (vi != values->end() && sbi == sb.end()) {
                CPPUNIT_FAIL("too many values");
            }
        }

        vector<string> attrs;
        attrs.push_back("attr1");
        attrs.push_back("attr2");
        attrs.push_back("attr3");
        attrs.push_back("attr4");

        str_citer ai = attrs.begin();
        AttrTable::Attr_iter i = at.attr_begin();
        // ai = attrs.begin() ;
        for (; i != at.attr_end() && ai != attrs.end(); i++, ai++) {
            CPPUNIT_ASSERT((*i)->name == (*ai));
        }
        CPPUNIT_ASSERT(i == at.attr_end() && ai == attrs.end());
        if (i != at.attr_end() && ai == attrs.end()) {
            CPPUNIT_FAIL("too many attributes");
        }
        else if (i == at.attr_end() && ai != attrs.end()) {
            CPPUNIT_FAIL("not enough attributes");
        }

        iter = at.attr_end();
        at.find("attr3", &dummy_at, &iter);
        CPPUNIT_ASSERT(iter != at.attr_end());

        attr_name = at.get_name(iter);
        CPPUNIT_ASSERT(attr_name == "attr3");

        bool isit = at.is_container(iter);
        CPPUNIT_ASSERT(isit == false);

        dummy_at = at.get_attr_table(iter);
        CPPUNIT_ASSERT(!dummy_at);

        attr_type = at.get_type(iter);
        CPPUNIT_ASSERT(attr_type == "String");

        attr_type_enum = at.get_attr_type(iter);
        CPPUNIT_ASSERT(attr_type_enum == Attr_string);

        attr_value = at.get_attr(iter);
        CPPUNIT_ASSERT(attr_value == "attr3Value1");

        attr_value = at.get_attr(iter, 1);
        CPPUNIT_ASSERT(attr_value == "attr3Value2");

        values = at.get_attr_vector(iter);
        CPPUNIT_ASSERT(values);
        if (values) {
            str_citer vi = values->begin();
            str_citer sbi = sb.begin();
            for (; vi != values->end() && sbi != sb.end(); vi++, sbi++) {
                CPPUNIT_ASSERT((*vi) == (*sbi));
            }
            CPPUNIT_ASSERT(vi == values->end() && sbi == sb.end());
            if (vi == values->end() && sbi != sb.end()) {
                CPPUNIT_FAIL("not enough values");
            }
            else if (vi != values->end() && sbi == sb.end()) {
                CPPUNIT_FAIL("too many values");
            }
        }

        {
            str_iter sbi = sb.begin();
            sbi++;
            sb.erase(sbi);
        }

        at.del_attr("attr3", 1);
        values = at.get_attr_vector(iter);
        CPPUNIT_ASSERT(values);
        if (values) {
            str_citer vi = values->begin();
            str_citer sbi = sb.begin();
            for (; vi != values->end() && sbi != sb.end(); vi++, sbi++) {
                CPPUNIT_ASSERT((*vi) == (*sbi));
            }
            CPPUNIT_ASSERT(vi == values->end() && sbi == sb.end());
            if (vi == values->end() && sbi != sb.end()) {
                CPPUNIT_FAIL("not enough values");
            }
            else if (vi != values->end() && sbi == sb.end()) {
                CPPUNIT_FAIL("too many values");
            }
        }

        at.del_attr("attr3");
        container = 0;
        try {
            container = at.append_container("attr2");
            CPPUNIT_FAIL("added container named attr2 successfully - already exists");
        }
        catch (Error &e) {
        }
        CPPUNIT_ASSERT(!container);

        try {
            container = at.append_container("attr5");
        }
        catch (Error &e) {
            CPPUNIT_FAIL("failed to add new container attr5");
        }
        CPPUNIT_ASSERT(container);
        if (container) {
            CPPUNIT_ASSERT(container->get_name() == "attr5");
        }

        container = at.find_container("attr5");
        CPPUNIT_ASSERT(container);
        if (container) {
            string container_name = container->get_name();
            CPPUNIT_ASSERT(container_name == "attr5");
        }

        iter = at.attr_end();
        at.find("attr5", &dummy_at, &iter);
        CPPUNIT_ASSERT(iter != at.attr_end());
        attr_name = at.get_name(iter);
        CPPUNIT_ASSERT(attr_name == "attr5");
        isit = at.is_container(iter);
        CPPUNIT_ASSERT(isit == true);
        container = at.get_attr_table(iter);
        CPPUNIT_ASSERT(container);
        attr_type = at.get_type(iter);
        CPPUNIT_ASSERT(attr_type == "Container");
        attr_type_enum = at.get_attr_type(iter);
        CPPUNIT_ASSERT(attr_type_enum == Attr_container);

        /* FIX: does append attr return anything? */
        container->append_attr("attr5-1", "string", "attr5.1Value1");
        container->append_attr("attr5-2", "string", "attr5.2Value1");
        container->append_attr("attr5-3", "string", "attr5.3Value1");
        container->append_attr("attr5-4", "string", "attr5.4Value1");
        iter = at.attr_end();
        at.find("attr5.attr5-3", &dummy_at, &iter);
        CPPUNIT_ASSERT(iter != at.attr_end());
        CPPUNIT_ASSERT(container == dummy_at);

        //at.print( stdout ) ;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(attrTableT);

/* NOTHING NEEDS TO BE CHANGED BELOW HERE */

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
            cerr << "Usage: attrTableT has the following tests:" << endl;
            const std::vector<Test*> &tests = attrTableT::suite()->getTests();
            unsigned int prefix_len = attrTableT::suite()->getName().append("::").length();
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
            test = attrTableT::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
