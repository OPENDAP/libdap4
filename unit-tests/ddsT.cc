#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>

// #define DODS_DEBUG

#include "DDS.h"
//#include "Pix.h"
#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "TestArray.h"
#include "TestInt16.h"
#include "TestStr.h"
#include "TestTypeFactory.h"
//#include "ce_functions.h"
#include "util.h"
#include "debug.h"

#include "testFile.h"

#include "run_tests_cppunit.h"
#include "test_config.h"


using namespace CppUnit;
using namespace std;
using namespace libdap;



int test_variable_sleep_interval = 0; // Used in Test* classes for testing
// timeouts.
string cprint =
    "\
Dataset {\n\
    Int16 var1;\n\
    String var6;\n\
    Int16 var7;\n\
    Structure {\n\
        Structure {\n\
            Int16 var10;\n\
        } var9;\n\
    } var8;\n\
} Test%20Data%20Set;\n\
";

string pprint = "\
Dataset {\n\
    Int16 var1;\n\
    String var6;\n\
    Int16 var7;\n\
} Test%20Data%20Set;\n\
";

string nprint = "\
Dataset {\n\
} Test%20Data%20Set;\n\
";

string containerprint =
    "\
Dataset {\n\
    Structure {\n\
        Int16 c1var1;\n\
        Int16 c1var3;\n\
    } c1;\n\
    Structure {\n\
        String c2var2;\n\
        Structure {\n\
            Structure {\n\
                Int16 c2var3var1var1;\n\
                Int16 c2var3var1var2;\n\
            } c2var3var1;\n\
        } c2var3;\n\
    } c2;\n\
    Byte var1;\n\
    Float64 var2;\n\
} TestDDS;\n\
";

class ddsT: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(ddsT);
    CPPUNIT_TEST(ddsT_test);
    CPPUNIT_TEST(ddsT_containers);
    CPPUNIT_TEST(major_minor_test);
    CPPUNIT_TEST(parse_test);
    CPPUNIT_TEST(parse_2_test);
    CPPUNIT_TEST_SUITE_END();

private:
    /* TEST PRIVATE DATA */
    TestTypeFactory *factory;
    char a[1024];    

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

    void ddsT_test()
    {
        DDS dds(factory, "TestDDS");

        string dsn = dds.get_dataset_name();
        CPPUNIT_ASSERT(dsn == "TestDDS");

        dds.set_dataset_name("Test Data Set");
        dsn = dds.get_dataset_name();
        CPPUNIT_ASSERT(dsn == "Test Data Set");

        string fn = dds.filename();
        CPPUNIT_ASSERT(fn == "");

        dds.filename("dds_test.data");
        fn = dds.filename();
        CPPUNIT_ASSERT(fn == "dds_test.data");

        try {
            dds.add_var((BaseType *) NULL);
            CPPUNIT_FAIL("succeeded in adding a null var");
        }
        catch (InternalErr &e) {
        }

        try {
            BaseType *bt = factory->NewInt16("var1");
            CPPUNIT_ASSERT_THROW(dds.add_var_nocopy(0), InternalErr);
            dds.add_var(bt);
            delete bt;
            bt = factory->NewInt16("var2");
            dds.add_var(bt);
            delete bt;
            bt = factory->NewInt16("var3");
            dds.add_var(bt);
            delete bt;
            bt = factory->NewInt16("var4");
            dds.add_var(bt);
            delete bt;
            bt = factory->NewInt16("var5");
            dds.add_var(bt);
            delete bt;
            bt = factory->NewStr("var6");
            dds.add_var(bt);
            delete bt;
            BaseType *tbt = factory->NewInt16("");
            bt = factory->NewArray("var7", tbt);
            delete tbt;
            tbt = 0;
            dds.add_var(bt);
            delete bt;
            bt = factory->NewStructure("var8");
            Structure *s = (Structure *) bt;
            BaseType *bts1 = factory->NewStructure("var9");
            Structure *s1 = (Structure *) bts1;
            BaseType *bts2 = factory->NewInt16("var10");
            s1->add_var(bts2);
            delete bts2;
            bts2 = 0;
            s->add_var(bts1);
            delete bts1;
            bts1 = 0;
            dds.add_var(bt);
            delete bt;
            bt = 0;
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("failed to add a var");
        }

        DDS dds2(dds);
        CPPUNIT_ASSERT(dds2.num_var() == 8);
        DDS dds3(factory, "TestDDS3");
        dds3 = dds2;
        CPPUNIT_ASSERT(dds3.num_var() == 8);
        dds3 = dds3;
        CPPUNIT_ASSERT(dds3.num_var() == 8);
        
        int nv = dds.num_var();
        CPPUNIT_ASSERT(nv == 8);

        vector<string> vs;
        typedef std::vector<string>::const_iterator vs_citer;
        typedef std::vector<string>::iterator vs_iter;
        vs.push_back("var1");
        vs.push_back("var2");
        vs.push_back("var3");
        vs.push_back("var4");
        vs.push_back("var5");
        vs.push_back("var6");
        vs.push_back("var7");
        vs.push_back("var8");

        DDS::Vars_iter dvsc = dds.var_begin();
        vs_citer vsc = vs.begin();
        for (; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++) {
            CPPUNIT_ASSERT((*dvsc)->name() == *vsc);
        }
        CPPUNIT_ASSERT(dvsc == dds.var_end() && vsc == vs.end());
        if (dvsc != dds.var_end() && vsc == vs.end()) {
            CPPUNIT_FAIL("Too many vars");
        }
        else if (dvsc == dds.var_end() && vsc != vs.end()) {
            CPPUNIT_FAIL("Too few vars");
        }
        
        for (vsc = vs.begin(); vsc != vs.end(); vsc++) {
            if (*vsc == "var2") {
                vs_iter &vsi = (vs_iter &) vsc;
                vs.erase(vsi);
                break;
            }
        }

        int count = 0;
        for (DDS::Vars_riter rv = dds.var_rbegin(); rv != dds.var_rend(); rv++) 
            count++;
        CPPUNIT_ASSERT(count == 8);
        DDS::Vars_iter vi = dds.get_vars_iter(4);
        CPPUNIT_ASSERT((*vi)->name() == "var5");
        BaseType *v5 = dds.get_var_index(4);
        CPPUNIT_ASSERT(v5->name() == "var5");
        
        dvsc = dds.var_begin();
        vsc = vs.begin();
        for (; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++) {
            if ((*dvsc)->name() == "var2") {
                DDS::Vars_iter &dvsi = (DDS::Vars_iter &) dvsc;
                dds.del_var(dvsi);
            }
            CPPUNIT_ASSERT((*dvsc)->name() == *vsc);
        }

        nv = dds.num_var();
        CPPUNIT_ASSERT(nv == 7);
        if (nv != 7) {
            for (dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++) {
                DBG2( cerr << "    " << (*dvsc)->name() << endl ) ;
            }
        }

        for (vsc = vs.begin(); vsc != vs.end(); vsc++) {
            if (*vsc == "var3") {
                vs_iter &vsi = (vs_iter &) vsc;
                vs.erase(vsi);
                break;
            }
        }
        dvsc = dds.var_begin();
        vsc = vs.begin();
        for (; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++) {
            if ((*dvsc)->name() == "var3") {
                DDS::Vars_iter &dvsi = (DDS::Vars_iter &) dvsc;
                dds.del_var(dvsi);
            }
            CPPUNIT_ASSERT((*dvsc)->name() == *vsc);
        }

        nv = dds.num_var();
        CPPUNIT_ASSERT(nv == 6);
        if (nv != 6) {
            for (dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++) {
                DBG2( cerr << "    " << (*dvsc)->name() << endl ) ;
            }
        }

        for (vsc = vs.begin(); vsc != vs.end(); vsc++) {
            if (*vsc == "var4") {
                vs_citer vsc2 = vsc;
                vsc2++;
                vsc2++;
                vs_iter &vsi = (vs_iter &) vsc;
                vs_iter &vsi2 = (vs_iter &) vsc2;
                vs.erase(vsi, vsi2);
                break;
            }
        }
        dvsc = dds.var_begin();
        vsc = vs.begin();
        for (; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++) {
            if ((*dvsc)->name() == "var4") {
                DDS::Vars_iter dvsc2 = dvsc;
                dvsc2++;
                dvsc2++;
                DDS::Vars_iter &dvsi = (DDS::Vars_iter &) dvsc;
                DDS::Vars_iter &dvsi2 = (DDS::Vars_iter &) dvsc2;
                dds.del_var(dvsi, dvsi2);
            }
            CPPUNIT_ASSERT((*dvsc)->name() == *vsc);
        }

        nv = dds.num_var();
        CPPUNIT_ASSERT(nv == 4);
        if (nv != 4) {
            for (dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++) {
                DBG2( cerr << "    " << (*dvsc)->name() << endl ) ;
            }
        }

        BaseType *bt = dds.var("varnot");
        CPPUNIT_ASSERT(!bt);

        bt = dds.var("var6");
        CPPUNIT_ASSERT(bt);
        if (bt) {
            CPPUNIT_ASSERT(bt->name() == "var6");
        }

        string find_var = "var6";
        bt = dds.var(find_var);
        CPPUNIT_ASSERT(bt);
        if (bt) {
            CPPUNIT_ASSERT(bt->name() == "var6");
        }

        find_var = "var10";
        bt = dds.var(find_var);
        CPPUNIT_ASSERT(bt);
        if (bt) {
            CPPUNIT_ASSERT(bt->name() == "var10");
        }

        find_var = "var10";
        BaseType::btp_stack btps;
        bt = dds.var(find_var, &btps);
        CPPUNIT_ASSERT(bt);
        if (bt) {
            CPPUNIT_ASSERT(bt->name() == "var10");
            if (bt->name() == "var10") {
                CPPUNIT_ASSERT(btps.size() == 2);
                if (btps.size() == 2) {
                    CPPUNIT_ASSERT(btps.top()->name() == "var8");
                    btps.pop();
                    CPPUNIT_ASSERT(btps.top()->name() == "var9");
                    btps.pop();
                }
            }
        }

        find_var = "var8.var9.var10";
        bt = dds.var(find_var, &btps);
        CPPUNIT_ASSERT(bt);
        if (bt) {
            CPPUNIT_ASSERT(bt->name() == "var10");
            if (bt->name() == "var10") {
                CPPUNIT_ASSERT(btps.size() == 2);
                if (btps.size() == 2) {
                    CPPUNIT_ASSERT(btps.top()->name() == "var9");
                    btps.pop();
                    CPPUNIT_ASSERT(btps.top()->name() == "var8");
                    btps.pop();
                }
            }
        }

        {
            ostringstream sof;
            dds.print(sof);
            CPPUNIT_ASSERT(sof.str().find(cprint) != string::npos);
        }

        {
            FILE *fp;
            fp = fopen("ddsT_print.output", "w");
            dds.print(fp);
            fclose(fp);
            ifstream ifs("ddsT_print.output");
            while(!ifs.eof())
                ifs >> a;
            ifs.close();
            CPPUNIT_ASSERT(!strcmp(a, "Test%20Data%20Set;"));
        }
        
        {
            FILE *fp;
            fp = fopen("ddsT_print_constrained.output", "w");
            dds.print_constrained(fp);
            fclose(fp);
            ifstream ifs("ddsT_print_constrained.output");
            while(!ifs.eof())
                ifs >> a;
            ifs.close();
            CPPUNIT_ASSERT(!strcmp(a, "Test%20Data%20Set;"));
        }
        
        {
            ostringstream sof;
            dds.print_xml(sof, true, "ss");
            CPPUNIT_ASSERT(sof.str().find("<Dataset name=\"Test Data Set\"") != string::npos);
        }

        {
            FILE *fp;
            fp = fopen("ddsT_print_xml.output", "w");
            dds.print_xml(fp, true, " ");
            fclose(fp);
            ifstream ifs("ddsT_print_xml.output");
            while(!ifs.eof())
                ifs >> a;
            ifs.close();
            CPPUNIT_ASSERT(!strcmp(a, "</Dataset>"));
        }
        
        {
            ostringstream sof;
            dds.print_constrained(sof);
            CPPUNIT_ASSERT(sof.str().find(nprint) != string::npos);
        }

        dds.mark_all(true);

        {
            ostringstream sof;
            dds.print_constrained(sof);
            CPPUNIT_ASSERT(sof.str().find(cprint) != string::npos);
        }

        bool mark_ret = dds.mark("var8", false);
        CPPUNIT_ASSERT(mark_ret == true);

        {
            ostringstream sof;
            dds.print_constrained(sof);
            CPPUNIT_ASSERT(sof.str().find(pprint) != string::npos);
        }
    }

    void ddsT_containers()
    {
        DDS dds(factory, "TestDDS");

        // set the container to c1 and make sure the container is created
        dds.container_name("c1");
        CPPUNIT_ASSERT(dds.container_name() == "c1");

        Structure *c1 = dds.container();
        CPPUNIT_ASSERT(c1);
        CPPUNIT_ASSERT(c1->name() == "c1");

        CPPUNIT_ASSERT(dds.num_var() == 1);

        // set the container to c2 and make sure the container is created
        dds.container_name("c2");
        CPPUNIT_ASSERT(dds.container_name() == "c2");

        Structure *c2 = dds.container();
        CPPUNIT_ASSERT(c2);
        CPPUNIT_ASSERT(c2->name() == "c2");

        CPPUNIT_ASSERT(dds.num_var() == 2);

        // set back to c1, make sure new one not created
        dds.container_name("c1");
        CPPUNIT_ASSERT(dds.container_name() == "c1");

        c1 = dds.container();
        CPPUNIT_ASSERT(c1);
        CPPUNIT_ASSERT(c1->name() == "c1");

        CPPUNIT_ASSERT(dds.num_var() == 2);

        // start adding variables and make sure not added directly to dds.
        // Make sure can find those variables without referencing the
        // container.
        try {
            BaseType *bt = factory->NewInt16("c1var1");
            dds.add_var(bt);
            delete bt;
            bt = factory->NewStr("c1var2");
            dds.add_var(bt);
            delete bt;
            BaseType *tbt = factory->NewInt16("");
            bt = factory->NewArray("c1var3", tbt);
            delete tbt;
            tbt = 0;
            dds.add_var(bt);
            delete bt;
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("failed to add a var");
        }
        CPPUNIT_ASSERT(dds.num_var() == 2);
        CPPUNIT_ASSERT(c1->element_count() == 3);

        BaseType *bt = dds.var("c1var1");
        CPPUNIT_ASSERT(bt);
        CPPUNIT_ASSERT(bt->name() == "c1var1");
        Int16 *ibt = dynamic_cast<Int16 *>(bt);
        CPPUNIT_ASSERT(ibt);

        bt = dds.var("c1var2");
        CPPUNIT_ASSERT(bt);
        CPPUNIT_ASSERT(bt->name() == "c1var2");
        Str *strbt = dynamic_cast<Str *>(bt);
        CPPUNIT_ASSERT(strbt);

        dds.del_var("c1var2");
        bt = dds.var("c1var2");
        CPPUNIT_ASSERT(bt == 0);
        CPPUNIT_ASSERT(dds.num_var() == 2);
        CPPUNIT_ASSERT(c1->element_count() == 2);

        // set container to "", add vars, make sure added directly to dds.
        // Make sure can find those variables
        dds.container_name("");
        CPPUNIT_ASSERT(dds.container_name() == "");
        CPPUNIT_ASSERT(dds.container() == 0);
        try {
            BaseType *bt = factory->NewByte("var1");
            dds.add_var(bt);
            delete bt;
            bt = factory->NewFloat64("var2");
            dds.add_var(bt);
            delete bt;
            bt = factory->NewUInt32("var3");
            dds.add_var(bt);
            delete bt;
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("failed to add a var");
        }
        CPPUNIT_ASSERT(dds.num_var() == 5);
        CPPUNIT_ASSERT(c1->element_count() == 2);

        BaseType::btp_stack btps;
        bt = dds.var("c1var1", btps);
        CPPUNIT_ASSERT(bt && bt->name() == "c1var1");
        CPPUNIT_ASSERT(btps.size() == 1);
        BaseType *btp = btps.top();
        CPPUNIT_ASSERT(btp && btp->name() == "c1");
        btps.pop();

        bt = dds.var("var1");
        CPPUNIT_ASSERT(bt);
        CPPUNIT_ASSERT(bt->name() == "var1");
        Byte *bbt = dynamic_cast<Byte *>(bt);
        CPPUNIT_ASSERT(bbt);

        bt = c1->var("var1");
        CPPUNIT_ASSERT(bt == 0);

        dds.del_var("var3");
        bt = dds.var("var3");
        CPPUNIT_ASSERT(bt == 0);
        CPPUNIT_ASSERT(dds.num_var() == 4);
        CPPUNIT_ASSERT(c1->element_count() == 2);

        // set to c2, add variables and make sure not added directly to dds.
        // Make sure can find those variables without referencing the
        // container.
        dds.container_name("c2");
        CPPUNIT_ASSERT(dds.container_name() == "c2");
        c2 = dds.container();
        CPPUNIT_ASSERT(c2 && c2->name() == "c2");

        try {
            BaseType *bt = factory->NewInt32("c2var1");
            dds.add_var(bt);
            delete bt;
            bt = factory->NewStr("c2var2");
            dds.add_var(bt);
            delete bt;
            Structure *s = factory->NewStructure("c2var3");
            Structure *s1 = factory->NewStructure("c2var3var1");
            bt = factory->NewInt16("c2var3var1var1");
            s1->add_var(bt);
            delete bt;
            bt = 0;
            bt = factory->NewInt16("c2var3var1var2");
            s1->add_var(bt);
            delete bt;
            bt = 0;
            s->add_var(s1);
            delete s1;
            s1 = 0;
            dds.add_var(s);
            delete s;
            s = 0;
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("failed to add a var");
        }
        CPPUNIT_ASSERT(dds.num_var() == 4);
        CPPUNIT_ASSERT(c2->element_count() == 3);

        bt = dds.var("c2var1");
        CPPUNIT_ASSERT(bt && bt->name() == "c2var1");
        Int32 *i32bt = dynamic_cast<Int32 *>(bt);
        CPPUNIT_ASSERT(i32bt);

        bt = dds.var("c2var2");
        CPPUNIT_ASSERT(bt && bt->name() == "c2var2");
        strbt = dynamic_cast<Str *>(bt);
        CPPUNIT_ASSERT(strbt);

        bt = dds.var("c2var3");
        CPPUNIT_ASSERT(bt && bt->name() == "c2var3");
        Structure *sbt = dynamic_cast<Structure *>(bt);
        CPPUNIT_ASSERT(sbt);

        bt = dds.var("var1");
        CPPUNIT_ASSERT(bt == 0);

        bt = dds.var("var2");
        CPPUNIT_ASSERT(bt == 0);

        bt = dds.var("c2var3var1var2", btps);
        CPPUNIT_ASSERT(bt && bt->name() == "c2var3var1var2");
        CPPUNIT_ASSERT(btps.size() == 3);
        btp = btps.top();
        CPPUNIT_ASSERT(btp && btp->name() == "c2");
        btps.pop();
        btp = btps.top();
        CPPUNIT_ASSERT(btp && btp->name() == "c2var3");
        btps.pop();
        btp = btps.top();
        CPPUNIT_ASSERT(btp && btp->name() == "c2var3var1");
        btps.pop();

        dds.del_var("c2var1");
        bt = dds.var("c2var1");
        CPPUNIT_ASSERT(bt == 0);
        CPPUNIT_ASSERT(dds.num_var() == 4);
        CPPUNIT_ASSERT(c2->element_count() == 2);

        // print the dds and make sure it looks good.
        ostringstream sstrm;
        dds.print(sstrm);
        //cout << sstrm.str() << endl;
        CPPUNIT_ASSERT(sstrm.str() == containerprint);
    }

    void major_minor_test()
    {
        DDS dds(factory, "TestDDS");
        dds.set_dap_major(3);
        dds.set_dap_minor(4);
        CPPUNIT_ASSERT(dds.get_dap_version() == "3.4");

        // DDS does not protect against negatives, in spite of what
        // the code comments say. This has been entered as HYRAX issue
        // 650.
        // dds.set_dap_major(-3);
        // dds.set_dap_minor(-4);
        // cout<<dds.get_dap_version();
        // CPPUNIT_ASSERT(dds.get_dap_version() == "3.4");

        CPPUNIT_ASSERT_THROW(dds.set_dap_version("-1.1"), InternalErr);
        CPPUNIT_ASSERT_THROW(dds.set_dap_version("1m1"), InternalErr);
        CPPUNIT_ASSERT_THROW(dds.set_dap_version("1.-1"), InternalErr);
        CPPUNIT_ASSERT_THROW(dds.set_dap_version("5.0"), InternalErr);

        dds.set_dap_version(2.0);
        CPPUNIT_ASSERT(dds.get_dap_version() == "2.0");
    }

    void parse_test()
    {
        DDS dds(factory, "TestDDS");
        string file = (string)TEST_SRC_DIR + "/dds-testsuite/test.1";        
        CPPUNIT_ASSERT_THROW(dds.parse("not_a_file"), Error);
        dds.parse(file);
        CPPUNIT_ASSERT(dds.get_dataset_name() == "data1");
        dds.set_timeout(2);
#if USE_LOCAL_TIMEOUT_SCHEME
        CPPUNIT_ASSERT(dds.get_timeout() == 2);
#else
        CPPUNIT_ASSERT(dds.get_timeout() == 0);
#endif
    }

    void parse_2_test()
    {
        DDS dds(factory, "TestDDS");
        string file = (string)TEST_SRC_DIR + "/dds-testsuite/test.1";
        ostringstream strm;        
        int fp = open(file.c_str(), O_RDONLY);
        CPPUNIT_ASSERT_THROW(dds.parse(-1), InternalErr);
        CPPUNIT_ASSERT_THROW(dds.parse((FILE *)0), InternalErr);
        dds.parse(fp);
        close(fp);
        CPPUNIT_ASSERT(dds.get_dataset_name() == "data1");
        dds.dump(strm);
        CPPUNIT_ASSERT(strm.str().find("d_name: data1") != string::npos);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (ddsT);

/* NOTHING NEEDS TO BE CHANGED BELOW HERE */

int main(int argc, char*argv[])
{
    return run_tests<ddsT>(argc, argv) ? 0: 1;
}
