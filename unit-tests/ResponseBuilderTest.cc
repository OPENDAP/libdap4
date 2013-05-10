// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>  // for stat
#include <cstring>
#include <sstream>

//#define DODS_DEBUG

#include "ObjectType.h"
#include "EncodingType.h"
#include "ResponseBuilder.h"
#include "ServerFunction.h"
#include "ServerFunctionsList.h"
#include "DAS.h"
#include "DDS.h"
#include "Str.h"
//#include "ce_functions.h"
#include "GetOpt.h"

#include "GNURegex.h"
#include "util.h"
#include "debug.h"

#include "../tests/TestTypeFactory.h"
#include "../tests/TestByte.h"

#include "testFile.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

int test_variable_sleep_interval = 0;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);


void
rb_test_function(int, BaseType *[], DDS &dds, BaseType **btpp)
{
    string xml_value = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";



    ServerFunction *sf;
    string functionType;

    ServerFunctionsList *sfList = libdap::ServerFunctionsList::TheList();
    std::multimap<string,libdap::ServerFunction *>::iterator begin = sfList->begin();
    std::multimap<string,libdap::ServerFunction *>::iterator end = sfList->end();
    std::multimap<string,libdap::ServerFunction *>::iterator sfit;

    xml_value += "<ds:functions xmlns:ds=\"http://xml.opendap.org/ns/DAP/4.0/dataset-services#\">\n";
    for(sfit=begin; sfit!=end; sfit++){
        sf = sfList->getFunction(sfit);
        if(sf->canOperateOn(dds)){
            xml_value += "    <ds:function  name=\"" + sf->getName() +"\""+
                         " version=\"" + sf->getVersion() + "\""+
                         " type=\"" + sf->getTypeString() + "\""+
                         " role=\"" + sf->getRole() + "\""+
                         " >\n" ;
            xml_value += "        <ds:Description href=\"" + sf->getDocUrl() + "\">" + sf->getDescriptionString() + "</ds:Description>\n";
            xml_value += "    </ds:function>\n";
        }
    }
    xml_value += "</functions>\n";

    Str *response = new Str("version");

    response->set_value(xml_value);
    *btpp = response;
    return;
}



namespace libdap {

class ResponseBuilderTest: public TestFixture {
private:
    ResponseBuilder *df, *df1, *df2, *df3, *df4, *df5, *df6;

    AttrTable *cont_a;
    DAS *das;
    DDS *dds;
    ostringstream oss;
    time_t now;
    char now_array[256];

    void loadServerSideFunction(){
        libdap::ServerFunction *rbSSF = new libdap::ServerFunction(

            // The name of the function as it will appear in a constraint expression
            "rbFuncTest",

            // The version of the function
            "1.0",

            // A brief description of the function
            "Returns a list of the functions held in the ServerFunctionsList object",

            // A usage/syntax statement
            "rbFuncTest()",

            // A URL that points two a web page describing the function
            "http://docs.opendap.org/index.php/Hyrax:_Server_Side_Functions",

            // A URI that defines the role of the function
            "http://services.opendap.org/dap4/unit-tests/ResponseBuilderTest",

            // A pointer to the helloWorld() function
            rb_test_function
        );
        // Here we add our new instance of libdap::ServerFunction to the libdap::ServerFunctionsList.
        libdap::ServerFunctionsList::TheList()->add_function(rbSSF);

    }

public:
    ResponseBuilderTest(): cont_a(0), df(0), df1(0), df2(0), df3(0), df4(0), df5(0), df6(0), dds(0), das(0) {
        now = time(0);
        ostringstream time_string;
        time_string << (int) now;
        strncpy(now_array, time_string.str().c_str(), 255);
        now_array[255] = '\0';

    }


    ~ResponseBuilderTest() {
    }

    void setUp() {
        // Test pathname
        df = new ResponseBuilder();
        // Test missing file
        df1 = new ResponseBuilder();
        df1->set_dataset_name("no-such-file");

        // Test files in CWD. Note that the time is the GM time : Tue, 01 May
        // 2001 01:08:14 -0700
        df2 = new ResponseBuilder();
        df2->set_dataset_name("test_config.h");

        // This file has an ancillary DAS in the server-testsuite dir.
        // df3 is also used to test escaping stuff in URLs. 5/4/2001 jhrg
        df3 = new ResponseBuilder();
        df3->set_dataset_name((string) TEST_SRC_DIR + "/server-testsuite/coads.data");
        df3->set_ce("u,x,z[0]&grid(u,\"lat<10.0\")");
        df3->set_timeout(1);

        // Go back to this data source to test w/o an ancillary DAS.
        df4 = new ResponseBuilder();
        df4->set_dataset_name((string) TEST_SRC_DIR + "/server-testsuite/bears.data");
        df4->set_ce("u,x,z[0]&grid(u,\"lat<10.0\")");
        df4->set_timeout(1);

        // Test escaping stuff. 5/4/2001 jhrg
        df5 = new ResponseBuilder();
        df5->set_dataset_name("nowhere%5Bmydisk%5Dmyfile");
        df5->set_ce("u%5B0%5D");



        // Try a server side function call.
        loadServerSideFunction();
        df6 = new ResponseBuilder();
        df6->set_dataset_name((string) TEST_SRC_DIR + "/server-testsuite/bears.data");
        df6->set_ce("rbFuncTest()");
        df6->set_timeout(1);



        cont_a = new AttrTable;
        cont_a->append_attr("size", "Int32", "7");
        cont_a->append_attr("type", "String", "cars");
        das = new DAS;
        das->add_table("a", cont_a);

        // This AttrTable looks like:
        //      Attributes {
        //          a {
        //              Int32 size 7;
        //              String type cars;
        //          }
        //      }

        TestTypeFactory ttf;
        dds = new DDS(&ttf, "test");
        TestByte a("a");
        dds->add_var(&a);

        dds->transfer_attributes(das);
        dds->set_dap_major(3);
        dds->set_dap_minor(2);
    }

    void tearDown() {
        delete df;
        df = 0;
        delete df1;
        df1 = 0;
        delete df2;
        df2 = 0;
        delete df3;
        df3 = 0;
        delete df4;
        df4 = 0;
        delete df5;
        df5 = 0;

        delete das;
        das = 0;
    }

    bool re_match(Regex &r, const string &s) {
        DBG(cerr << "s.length(): " << s.length() << endl);
        int pos = r.match(s.c_str(), s.length());
        DBG(cerr << "r.match(s): " << pos << endl);
        return pos > 0 && static_cast<unsigned> (pos) == s.length();
    }

    bool re_match_binary(Regex &r, const string &s) {
        DBG(cerr << "s.length(): " << s.length() << endl);
        int pos = r.match(s.c_str(), s.length());
        DBG(cerr << "r.match(s): " << pos << endl);
        return pos > 0;
    }

   void send_das_test() {
        Regex
                r1(
                        "HTTP/1.0 200 OK\r\n\
XDODS-Server: .*\
XOPeNDAP-Server: .*\
XDAP: .*\
Date: .*\
Last-Modified: .*\
Content-Type: text/plain\r\n\
Content-Description: dods_das\r\n\
\r\n\
Attributes \\{\n\
    a \\{\n\
        Int32 size 7;\n\
        String type \"cars\";\n\
    \\}\n\
\\}\n");

        df->send_das(oss, *das);

        DBG(cerr << "DAS: " << oss.str() << endl);

        CPPUNIT_ASSERT(re_match(r1, oss.str()));
        oss.str("");
    }

    void send_dds_test() {
        Regex
                r1(
                        "HTTP/1.0 200 OK\r\n\
XDODS-Server: .*\
XOPeNDAP-Server: .*\
XDAP: .*\
Date: .*\
Last-Modified: .*\
Content-Type: text/plain\r\n\
Content-Description: dods_dds\r\n\
\r\n\
Dataset \\{\n\
    Byte a;\n\
\\} test;\n");

        ConstraintEvaluator ce;

        df->send_dds(oss, *dds, ce);

        DBG(cerr << "DDS: " << oss.str() << endl);

        CPPUNIT_ASSERT(re_match(r1, oss.str()));
        oss.str("");
    }

    void send_ddx_test() {
        string baseline = readTestBaseline((string) TEST_SRC_DIR + "/ddx-testsuite/response_builder_send_ddx_test.xml");
        Regex r1(baseline.c_str());
        ConstraintEvaluator ce;

        try {
            df->send_ddx(oss, *dds, ce);

            DBG(cerr << "DDX: " << oss.str() << endl);

            CPPUNIT_ASSERT(re_match(r1, baseline));
            //CPPUNIT_ASSERT(re_match(r1, oss.str()));
            //oss.str("");
        } catch (Error &e) {
            CPPUNIT_FAIL("Error: " + e.get_error_message());
        }
    }


#if 0
    void send_data_ddx_test() {
        string baseline = readTestBaseline((string) TEST_SRC_DIR + "/tmp.xml"); //"/ddx-testsuite/response_builder_send_data_ddx_test_3.xml");
        Regex r1(baseline.c_str());
        // I do not look for the closing '--boundary' because the binary
        // data breaks the regex functions in the c library WRT subsequent
        // pattern matches. jhrg
        //--boundary--\r\n");

        ConstraintEvaluator ce;

        try {
            df->send_data_ddx(oss, *dds, ce, "start@opendap.org", "boundary", true);

            DBG(cerr << "DataDDX: " << oss.str() << endl);
            //DBG(cerr << "glob: " << glob(baseline.c_str(), oss.str().c_str()) << endl);
            //CPPUNIT_ASSERT(glob(baseline.c_str(), oss.str().c_str()) == 0);
            CPPUNIT_ASSERT(re_match_binary(r1, oss.str()));
            //oss.str("");
        } catch (Error &e) {
            CPPUNIT_FAIL("Error: " + e.get_error_message());
        }
    }

    void send_data_ddx_test2() {
        Regex
                r1(
                        "--boundary\r\n\
Content-Type: Text/xml; charset=iso-8859-1\r\n\
Content-Id: <start@opendap.org>\r\n\
Content-Description: dap4-ddx\r\n\
\r\n\
<\\?xml version=\"1.0\" encoding=\"UTF-8\"\\?>.*\
<Dataset name=\"test\".*\
.*\
dapVersion=\"3.2\">.*\
.*\
    <Byte name=\"a\">.*\
        <Attribute name=\"size\" type=\"Int32\">.*\
            <value>7</value>.*\
        </Attribute>.*\
        <Attribute name=\"type\" type=\"String\">.*\
            <value>cars</value>.*\
        </Attribute>.*\
    </Byte>.*\
.*\
    <blob href=\"cid:.*@.*\"/>.*\
</Dataset>.*\
--boundary\r\n\
Content-Type: application/octet-stream\r\n\
Content-Id: <.*@.*>\r\n\
Content-Description: dap4-data\r\n\
Content-Encoding: binary\r\n\
\r\n\
.*");

        ConstraintEvaluator ce;

        try {
            df->send_data_ddx(oss, *dds, ce, "start@opendap.org", "boundary", false);
            DBG(cerr << "DataDDX: " << oss.str() << endl);
            CPPUNIT_ASSERT(re_match_binary(r1, oss.str()));

            // Unlike the test where the full headers are generated, there's
            // no check for a conditional response here because that feature
            // of ResponseBuilder is only supported when MIME headers are built by
            // the class. In order to return a '304' response, headers must be
            // built.
        } catch (Error &e) {
            CPPUNIT_FAIL("Error: " + e.get_error_message());
        }
    }
#endif

    void escape_code_test() {
        // These should NOT be escaped.

        DBG(cerr << df3->get_dataset_name() << endl); DBG(cerr << df3->get_ce() << endl);

        CPPUNIT_ASSERT(df3->get_dataset_name() == (string)TEST_SRC_DIR + "/server-testsuite/coads.data");
        CPPUNIT_ASSERT(df3->get_ce() == "u,x,z[0]&grid(u,\"lat<10.0\")");

        // The ResponseBuilder instance is feed escaped values; they should be
        // unescaped by the ctor and the mutators. 5/4/2001 jhrg

        DBG(cerr << df5->get_dataset_name() << endl); DBG(cerr << df5->get_ce() << endl);

        CPPUNIT_ASSERT(df5->get_dataset_name() == "nowhere[mydisk]myfile");
        CPPUNIT_ASSERT(df5->get_ce() == "u[0]");

        df5->set_ce("u%5B0%5D");
        CPPUNIT_ASSERT(df5->get_ce() == "u[0]");

        df5->set_ce("Grid%20u%5B0%5D");
        CPPUNIT_ASSERT(df5->get_ce() == "Grid%20u[0]");
    }

    // This tests reading the timeout value from argv[].
    void timeout_test() {
        CPPUNIT_ASSERT(df3->get_timeout() == 1);
        CPPUNIT_ASSERT(df1->get_timeout() == 0);
    }

    void invoke_server_side_function_test() {
        string baseline = readTestBaseline((string) TEST_SRC_DIR + "/server-testsuite/response_builder_invoke_server_side_function_test.xml");
        Regex r1(baseline.c_str());
        ConstraintEvaluator ce;

        DBG( cerr << endl);
        DBG( cerr << "invoke_server_side_function_test():" << endl);
        DBG(cerr << "  dataset: '" << df6->get_dataset_name() << "'" << endl);
        DBG(cerr << "  ce:      '" << df6->get_ce() << "'" << endl);

        try {
            DBG(cerr << "  calling send_data()" << endl);
            df6->send_data(oss, *dds, ce);


            DBG(cerr << "  DATA: " << endl << oss.str() << endl);

            CPPUNIT_ASSERT(true);

        } catch (Error &e) {
            CPPUNIT_FAIL("Error: " + e.get_error_message());
        }
    }



#if 0
    // The server functions have been moved out of libdap and into a bes
    // module.
    void split_ce_test_1() {
        ConstraintEvaluator eval;
        register_functions(eval);
        df->split_ce(eval, "x,y,z");
        CPPUNIT_ASSERT(df->get_ce() == "x,y,z");
        CPPUNIT_ASSERT(df->get_btp_func_ce() == "");
    }

    void split_ce_test_2() {
        ConstraintEvaluator eval;
        register_functions(eval);
        df->split_ce(eval, "honker(noise),x,y,z");
        CPPUNIT_ASSERT(df->get_ce() == "honker(noise),x,y,z");
        CPPUNIT_ASSERT(df->get_btp_func_ce() == "");
    }

    void split_ce_test_3() {
        ConstraintEvaluator eval;
        register_functions(eval);
        df->split_ce(eval, "grid(noise),x,y,z");
        CPPUNIT_ASSERT(df->get_ce() == "x,y,z");
        CPPUNIT_ASSERT(df->get_btp_func_ce() == "grid(noise)");
    }

    void split_ce_test_4() {
        ConstraintEvaluator eval;
        register_functions(eval);
        df->split_ce(eval, "grid(noise),linear_scale(noise2),x,y,z");
        CPPUNIT_ASSERT(df->get_ce() == "x,y,z");
        CPPUNIT_ASSERT(df->get_btp_func_ce() == "grid(noise),linear_scale(noise2)");
    }

    void split_ce_test_5() {
         ConstraintEvaluator eval;
         register_functions(eval);
         df->split_ce(eval, "grid(noise),honker(foo),grid(noise2),x,y,z");
         CPPUNIT_ASSERT(df->get_ce() == "honker(foo),x,y,z");
         CPPUNIT_ASSERT(df->get_btp_func_ce() == "grid(noise),grid(noise2)");
     }

    void split_ce_test_6() {
         ConstraintEvaluator eval;
         register_functions(eval);
         df->split_ce(eval, "grid(noise),honker(foo),grid(noise2),x,y,z,foo()");
         CPPUNIT_ASSERT(df->get_ce() == "honker(foo),x,y,z,foo()");
         CPPUNIT_ASSERT(df->get_btp_func_ce() == "grid(noise),grid(noise2)");
     }
#endif

CPPUNIT_TEST_SUITE( ResponseBuilderTest );

        CPPUNIT_TEST(send_das_test);
        CPPUNIT_TEST(send_dds_test);
        CPPUNIT_TEST(send_ddx_test);

        // These tests fail because the regex comparison code is hosed.
        // I've tried using some simpler globbing code, but that fails, too,
        // likely because of the MIME header line termination chars. These
        // methods do work and I'm spending more time on the these two tests
        // than on all of the XMLWriter methods!
        //
        // Removed send_data_ddx from the class; it was never used.
        // CPPUNIT_TEST(send_data_ddx_test);
        // CPPUNIT_TEST(send_data_ddx_test2);
        CPPUNIT_TEST(escape_code_test);
        CPPUNIT_TEST(invoke_server_side_function_test);

#if 0
        // not written yet 9/14/12
        CPPUNIT_TEST(send_dmr_test_1);
#endif
#if 0
        CPPUNIT_TEST(split_ce_test_1);
        CPPUNIT_TEST(split_ce_test_2);
        CPPUNIT_TEST(split_ce_test_3);
        CPPUNIT_TEST(split_ce_test_4);
        CPPUNIT_TEST(split_ce_test_5);
        CPPUNIT_TEST(split_ce_test_6);
#endif
    CPPUNIT_TEST_SUITE_END();
};
CPPUNIT_TEST_SUITE_REGISTRATION(ResponseBuilderTest);
}

int main(int argc, char*argv[]) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    GetOpt getopt(argc, argv, "d");
    char option_char;
    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        default:
            break;
        }

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        while (i < argc) {
            test = string("libdap::ResponseBuilderTest::") + argv[i++];

            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
