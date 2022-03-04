#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>

// #define DODS_DEBUG

#include "DAS.h"

#include "testFile.h"

#include "InternalErr.h"
#include "Error.h"
#include "run_tests_cppunit.h"
#include "test_config.h"


using namespace std;
using namespace libdap;
using namespace CppUnit;



string dprint =
    "\
Attributes {\n\
    c1 {\n\
        v1 {\n\
            String v1a1 \"v1a1val\";\n\
            String v1a2 \"v1a2val\";\n\
            String v1a3 \"v1a3val\";\n\
            String v1a4 \"v1a4val\";\n\
            v1v1 {\n\
                String v1v1a1 \"v1v1a1val\";\n\
                String v1v1a2 \"v1v1a2val\";\n\
            }\n\
        }\n\
    }\n\
    c2 {\n\
        v2 {\n\
            String v2a1 \"v2a1val\";\n\
            String v2a2 \"v2a2val\";\n\
        }\n\
        v3 {\n\
            String v3a1 \"v3a1val\";\n\
            String v3a2 \"v3a2val\";\n\
            v3v1 {\n\
                String v3v1a1 \"v3v1a1val\";\n\
            }\n\
        }\n\
    }\n\
}\n\
";

class dasT: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(dasT);
    CPPUNIT_TEST(dasT_test);
    CPPUNIT_TEST(das_file_test);
    CPPUNIT_TEST(das_file_2_test);
    CPPUNIT_TEST(das_dump_test);
    CPPUNIT_TEST(das_dump_2_test);
    CPPUNIT_TEST(das_erase_test);
    CPPUNIT_TEST_SUITE_END();

private:
    /* TEST PRIVATE DATA */

public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void dasT_test()
    {
        DAS das;
        CPPUNIT_ASSERT(das.get_size() == 0);
        CPPUNIT_ASSERT(das.container_name().empty());
        CPPUNIT_ASSERT(das.container() == 0);

        // set container to c1 and make sure set correctly
        das.container_name("c1");
        CPPUNIT_ASSERT(das.container_name() == "c1");
        CPPUNIT_ASSERT(das.container());
        CPPUNIT_ASSERT(das.container()->get_name() == "c1");
        CPPUNIT_ASSERT(das.get_size() == 0);

        // set back to upermost attribute table and test
        das.container_name("");
        CPPUNIT_ASSERT(das.container_name().empty());
        CPPUNIT_ASSERT(!das.container());
        CPPUNIT_ASSERT(das.get_size() == 1);

        // change to c2
        das.container_name("c2");
        CPPUNIT_ASSERT(das.container_name() == "c2");
        CPPUNIT_ASSERT(das.container());
        CPPUNIT_ASSERT(das.container()->get_name() == "c2");
        CPPUNIT_ASSERT(das.get_size() == 0);

        // set back to upermost attribute table and test
        das.container_name("");
        CPPUNIT_ASSERT(das.container_name().empty());
        CPPUNIT_ASSERT(!das.container());
        CPPUNIT_ASSERT(das.get_size() == 2);

        // change back to c1, make sure not another c1 added
        das.container_name("c1");
        CPPUNIT_ASSERT(das.container_name() == "c1");
        CPPUNIT_ASSERT(das.container());
        CPPUNIT_ASSERT(das.container()->get_name() == "c1");
        CPPUNIT_ASSERT(das.get_size() == 0);

        // set back to upermost attribute table and test
        das.container_name("");
        CPPUNIT_ASSERT(das.container_name().empty());
        CPPUNIT_ASSERT(!das.container());
        CPPUNIT_ASSERT(das.get_size() == 2);

        // add stuff to das, should go to c1
        das.container_name("c1");
        AttrTable *v1 = new AttrTable;
        das.add_table("v1", v1);
        v1->append_attr("v1a1", "String", "v1a1val");
        v1->append_attr("v1a2", "String", "v1a2val");
        v1->append_attr("v1a3", "String", "v1a3val");
        v1->append_attr("v1a4", "String", "v1a4val");
        AttrTable *v1v1 = v1->append_container("v1v1");
        v1v1->append_attr("v1v1a1", "String", "v1v1a1val");
        v1v1->append_attr("v1v1a2", "String", "v1v1a2val");

        // check container and das size
        CPPUNIT_ASSERT(das.get_size() == 1);
        das.container_name("");
        CPPUNIT_ASSERT(das.get_size() == 2);

        // add stuff to das, should go to c2
        das.container_name("c2");
        AttrTable *v2 = new AttrTable;
        das.add_table("v2", v2);
        v2->append_attr("v2a1", "String", "v2a1val");
        v2->append_attr("v2a2", "String", "v2a2val");
        AttrTable *v3 = new AttrTable;
        das.add_table("v3", v3);
        v3->append_attr("v3a1", "String", "v3a1val");
        v3->append_attr("v3a2", "String", "v3a2val");
        AttrTable *v3v1 = v3->append_container("v3v1");
        v3v1->append_attr("v3v1a1", "String", "v3v1a1val");

        // check container and das size
        CPPUNIT_ASSERT(das.get_size() == 2);
        das.container_name("");
        CPPUNIT_ASSERT(das.get_size() == 2);

        // print to stream and compare results
        ostringstream strm;
        das.print(strm);
        //cout << strm.str() << endl;
        //cout << dprint << endl;
        CPPUNIT_ASSERT(strm.str() == dprint);
    }

    void das_file_test()
    {
        DAS d;
        FILE *fp;
        string file = (string)TEST_SRC_DIR + "/dds-testsuite/test.1.das";
        fp = fopen(file.c_str(), "r");
        d.parse(fp);
        fclose(fp);
        CPPUNIT_ASSERT(d.get_size() == 2);
        CPPUNIT_ASSERT_THROW(d.parse((FILE *)0), InternalErr);
        CPPUNIT_ASSERT_THROW(d.parse(""), Error);
    }

    void das_file_2_test()
    {
        DAS d;
        int fp;
        string file = (string)TEST_SRC_DIR + "/dds-testsuite/test.1.das";
        fp = open(file.c_str(), O_RDONLY);
        CPPUNIT_ASSERT_THROW(d.parse(-1), InternalErr);
        d.parse(fp);
        close(fp);
        CPPUNIT_ASSERT(d.get_size() == 2);
    }

    void das_dump_test()
    {
        DAS das;
        ostringstream strm;        
        das.container_name("c1");
        das.dump(strm);
        CPPUNIT_ASSERT(strm.str().find("current container: c1") != string::npos);
    }

    void das_dump_2_test()
    {
        DAS das;
        ostringstream strm;        
        das.dump(strm);
        CPPUNIT_ASSERT(strm.str().find("current container: NONE") != string::npos);
    }

    void das_erase_test()
    {
        DAS das;
        das.container_name("c1");
        AttrTable *v1 = new AttrTable;
        das.add_table("v1", v1);
        v1->append_attr("v1a1", "String", "v1a1val");
        CPPUNIT_ASSERT(das.get_size() == 1);
        das.erase();
        CPPUNIT_ASSERT(das.get_size() == 0);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (dasT);

/* NOTHING NEEDS TO BE CHANGED BELOW HERE */

int main(int argc, char*argv[])
{
    return run_tests<dasT>(argc, argv) ? 0: 1;
}
