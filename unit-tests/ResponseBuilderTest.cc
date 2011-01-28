
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>  // for stat

#include <cstring>
#include <sstream>

//#define DODS_DEBUG

#include "ResponseBuilder.h"
#include "DAS.h"
#include "DDS.h"
#include "GNURegex.h"
#include "debug.h"

#include "../tests/TestTypeFactory.h"
#include "../tests/TestByte.h"

#include <test_config.h>


using namespace CppUnit;
using namespace std;
using namespace libdap;

int test_variable_sleep_interval = 0;

namespace libdap {

class ResponseBuilderTest : public TestFixture {
private:
    ResponseBuilder *df, *df_conditional, *df1, *df2, *df3, *df4, *df5, *df6;

    AttrTable *cont_a;
    DAS *das;
    DDS *dds;
    ostringstream oss;
    time_t now;
    char now_array[256];

public:
    ResponseBuilderTest() {
	now = time(0);
	ostringstream time_string;
	time_string << (int)now;
	strncpy(now_array, time_string.str().c_str(), 255);
	now_array[255] = '\0';
    }

    ~ResponseBuilderTest() {}

    void setUp() {
	// Test pathname
	string test_file = (string)TEST_SRC_DIR + "/server-testsuite/bears.data" ;
	char *argv_1[] = {(char*)"test_case", (char *)test_file.c_str()};
	df = new ResponseBuilder(2, argv_1);

	char *argv_1_1[] = {(char*)"test_case", (char *)test_file.c_str(), (char*)"-l", &now_array[0]};
	df_conditional = new ResponseBuilder(4, argv_1_1);

	// Test missing file
	argv_1[1] = (char*)"no-such-file";
	df1 = new ResponseBuilder(2, argv_1);

	// Test files in CWD. Note that the time is the GM time : Tue, 01 May
	// 2001 01:08:14 -0700
	argv_1[1] = (char*)"test_config.h";
	df2 = new ResponseBuilder(2, argv_1);

	// This file has an ancillary DAS in the server-testsuite dir.
	// df3 is also used to test escaping stuff in URLs. 5/4/2001 jhrg
	char *argv_2[8];
	argv_2[0] = (char*)"test_case";
	test_file = (string)TEST_SRC_DIR + "/server-testsuite/coads.data";
	argv_2[1] = (char *)test_file.c_str();
	argv_2[2] = (char*)"-l";
	argv_2[3] = &now_array[0];
	argv_2[4] = (char*)"-e";
	argv_2[5] = (char*)"u,x,z[0]&grid(u,\"lat<10.0\")";
	argv_2[6] = (char*)"-t";
	argv_2[7] = (char*)"1";
	df3 = new ResponseBuilder(6, argv_2);

	// Go back to this data source to test w/o an ancillary DAS.
	argv_2[0] = (char*)"test_case";
	test_file = (string)TEST_SRC_DIR + "/server-testsuite/bears.data";
	argv_2[1] = (char *)test_file.c_str();
	argv_2[2] = (char*)"-l";
	argv_2[3] = &now_array[0];
	argv_2[4] = (char*)"-e";
	argv_2[5] = (char*)"u,x,z[0]&grid(u,\"lat<10.0\")";
	argv_2[6] = (char*)"-t";
	argv_2[7] = (char*)"1";
	df4 = new ResponseBuilder(6, argv_2);

	// Test escaping stuff. 5/4/2001 jhrg
	char *argv_3[]={(char*)"test_case", (char*)"nowhere%5Bmydisk%5Dmyfile", (char*)"-e", (char*)"u%5B0%5D"};
	df5 = new ResponseBuilder(4, argv_3);

	char *argv_4[]={(char*)"test_case", (char*)"nowhere%3a%5bmydisk%5dmyfile", (char*)"-e", (char*)"Grid%20field%3au%5b0%5d,Grid%20field%3av"};
	df6 = new ResponseBuilder(4, argv_4);

	cont_a = new AttrTable;
	cont_a->append_attr("size", "Int32", "7");
	cont_a->append_attr("type", "String", "cars");
	das = new DAS;
	das->add_table( "a", cont_a ) ;

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
	delete df; df = 0;
	delete df_conditional; df_conditional = 0;
	delete df1; df1 = 0;
	delete df2; df2 = 0;
	delete df3; df3 = 0;
	delete df4; df4 = 0;
	delete df5; df5 = 0;
	delete df6; df6 = 0;

	delete das; das = 0;
    }

    bool re_match(Regex &r, const string &s) {
	DBG(cerr << "s.length(): " << s.length() << endl);
	int pos = r.match(s.c_str(), s.length());
	DBG(cerr << "r.match(s): " << pos << endl);
	return pos > 0 && static_cast<unsigned>(pos) == s.length();
    }

    bool re_match_binary(Regex &r, const string &s) {
	DBG(cerr << "s.length(): " << s.length() << endl);
	int pos = r.match(s.c_str(), s.length());
	DBG(cerr << "r.match(s): " << pos << endl);
	return pos > 0;
    }

    void add_keyword_test() {
	ResponseBuilder tdf;
	tdf.add_keyword("test");
	CPPUNIT_ASSERT(tdf.d_keywords.find("test") != tdf.d_keywords.end());
	CPPUNIT_ASSERT(tdf.d_keywords.find("test") == tdf.d_keywords.begin());
	CPPUNIT_ASSERT(*(tdf.d_keywords.find("test")) == string("test"));
	tdf.add_keyword("dap3.3");
	CPPUNIT_ASSERT(tdf.d_keywords.size() == 2);
    }

    void is_keyword_test() {
	ResponseBuilder tdf;
	tdf.add_keyword("test");
	tdf.add_keyword("dap3.3");
	CPPUNIT_ASSERT(tdf.is_keyword("test"));
	CPPUNIT_ASSERT(!tdf.is_keyword("TEST"));
    }

    void get_keywords() {
	ResponseBuilder tdf;
	tdf.add_keyword("test");
	tdf.add_keyword("dap2.0");
	tdf.add_keyword("dap4.0");
	CPPUNIT_ASSERT(tdf.d_keywords.size() == 3);

	ostringstream oss;
	list<string> kwds = tdf.get_keywords();
	list<string>::iterator i = kwds.begin();
	while (i != kwds.end())
	    oss << *i++;
	//cerr << oss.str() << endl;

	CPPUNIT_ASSERT(oss.str().find("test") != string::npos);
	CPPUNIT_ASSERT(oss.str().find("dap2.0") != string::npos);
	CPPUNIT_ASSERT(oss.str().find("dap4.0") != string::npos);
    }

    void get_dataset_last_modified_time_test() {
	time_t t = time(0);
	CPPUNIT_ASSERT(df1->get_dataset_last_modified_time() == t);

	struct stat st;
	string test_file = (string)TEST_SRC_DIR
	                   + "/server-testsuite/bears.data";
	stat(test_file.c_str(), &st);
	CPPUNIT_ASSERT(df->get_dataset_last_modified_time() == st.st_mtime);

	stat("test_config.h", &st);
	CPPUNIT_ASSERT(df2->get_dataset_last_modified_time() == st.st_mtime);

	test_file = (string)TEST_SRC_DIR + "/server-testsuite/coads.data";
	stat(test_file.c_str(), &st);
	CPPUNIT_ASSERT(df3->get_dataset_last_modified_time() == st.st_mtime);
    }

    void get_das_last_modified_time_test() {
	// the dataset pointed to by df has no anc. DAS
	struct stat st;
	string test_file = (string)TEST_SRC_DIR
	                   + "/server-testsuite/bears.data";
	stat(test_file.c_str(), &st);
	CPPUNIT_ASSERT(df->get_das_last_modified_time() == st.st_mtime);

	// the dataset pointed by df3 has an anc. DAS
	test_file = (string)TEST_SRC_DIR + "/server-testsuite/coads.data.das";
	stat(test_file.c_str(), &st);
	CPPUNIT_ASSERT(df3->get_das_last_modified_time() == st.st_mtime);
    }

    void send_das_test() {
	Regex r1("HTTP/1.0 200 OK\r\n\
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

	Regex r2("HTTP/1.0 304 NOT MODIFIED\r\n\
Date: .*\r\n\
\r\n");

	df_conditional->send_das(oss, *das);
	CPPUNIT_ASSERT(re_match(r2, oss.str()));
    }

    void send_dds_test() {
	Regex r1("HTTP/1.0 200 OK\r\n\
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

	Regex r2("HTTP/1.0 304 NOT MODIFIED\r\n\
Date: .*\r\n\
\r\n");

	df_conditional->send_dds(oss, *dds, ce);
	CPPUNIT_ASSERT(re_match(r2, oss.str()));
    }

    void send_ddx_test() {
	Regex r1("HTTP/1.0 200 OK\r\n\
XDODS-Server: .*\
XOPeNDAP-Server: .*\
XDAP: .*\
Date: .*\
Last-Modified: .*\
Content-Type: text/xml\r\n\
Content-Description: dap4-ddx\r\n\
\r\n\
<\\?xml version=\"1.0\" encoding=\"UTF-8\"\\?>.*\
<Dataset name=\"test\".*\
.*\
.*\
.*\
.*\
.*\
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
</Dataset>.*");

	ConstraintEvaluator ce;

	try {
	    df->send_ddx(*dds, ce, oss);

	    DBG(cerr << "DDX: " << oss.str() << endl);

	    CPPUNIT_ASSERT(re_match(r1, oss.str()));
	    oss.str("");

	    Regex r2("HTTP/1.0 304 NOT MODIFIED\r\n\
Date: .*\r\n\
\r\n");

	    df_conditional->send_ddx(*dds, ce, oss);
	    CPPUNIT_ASSERT(re_match(r2, oss.str()));
	}
	catch(Error &e) {
	    cerr << "Error (line 306): " << e.get_error_message() << endl;
	}
    }

    void send_data_ddx_test() {
	Regex r1("HTTP/1.0 200 OK\r\n\
.*\
XDAP:.*\r\n\
.*\
Content-Type: Multipart/Related; boundary=boundary; start=\"<start@opendap.org>\"; type=\"Text/xml\"\r\n\
Content-Description: dap4-data-ddx\r\n\
\r\n\
--boundary\r\n\
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

	// I do not look for the closing '--boundary' because the binary
	// data breaks the regex functions in the c library WRT subsequent
	// pattern matches. jhrg
	//--boundary--\r\n");

	ConstraintEvaluator ce;

	try {
	    df->send_data_ddx(*dds, ce, oss, "start@opendap.org", "boundary",
		    "", true);

	    DBG(cerr << "DataDDX: " << oss.str() << endl);

	    CPPUNIT_ASSERT(re_match_binary(r1, oss.str()));
	    oss.str("");

	    Regex r2("HTTP/1.0 304 NOT MODIFIED\r\n\
Date: .*\r\n\
\r\n");

	    df_conditional->send_data_ddx(*dds, ce, oss, "start@opendap.org",
		    "boundary", "", true);
	    CPPUNIT_ASSERT(re_match(r2, oss.str()));
	}
	catch(Error &e) {
	    cerr << "Error (line 306): " << e.get_error_message() << endl;
	}
    }

    void send_data_ddx_test2() {
	Regex r1("--boundary\r\n\
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
	    df->send_data_ddx(*dds, ce, oss, "start@opendap.org", "boundary",
		    "", false);
	    DBG(cerr << "DataDDX: " << oss.str() << endl);
	    CPPUNIT_ASSERT(re_match_binary(r1, oss.str()));

	    // Unlike the test where the full headers are generated, there's
	    // no check for a conditional response here because that feature
	    // of ResponseBuilder is only supported when MIME headers are built by
	    // the class. In order to return a '304' response, headers must be
	    // built.
	}
	catch(Error &e) {
	    cerr << "Error (line 306): " << e.get_error_message() << endl;
	}
    }

    void is_conditional_test() {
	CPPUNIT_ASSERT(df->is_conditional() == false);
	CPPUNIT_ASSERT(df3->is_conditional() == true);
    }

    void get_request_if_modified_since_test() {
	CPPUNIT_ASSERT(df->get_request_if_modified_since() == -1);
	CPPUNIT_ASSERT(df3->get_request_if_modified_since() == now);
    }

    void escape_code_test() {
	// These should NOT be escaped.

	DBG(cerr << df3->get_dataset_name() << endl);
	DBG(cerr << df3->get_ce() << endl);

	CPPUNIT_ASSERT(df3->get_dataset_name() == (string)TEST_SRC_DIR + "/server-testsuite/coads.data");
	CPPUNIT_ASSERT(df3->get_ce() == "u,x,z[0]&grid(u,\"lat<10.0\")");

	// The DODSFIlter instance is feed escaped values; they should be
	// unescaped by the ctor and the mutators. 5/4/2001 jhrg

	DBG(cerr << df5->get_dataset_name() << endl);
	DBG(cerr << df5->get_ce() << endl);

	CPPUNIT_ASSERT(df5->get_dataset_name() == "nowhere[mydisk]myfile");
	CPPUNIT_ASSERT(df5->get_ce() == "u[0]");

	df5->set_ce("u%5B0%5D");
	CPPUNIT_ASSERT(df5->get_ce() == "u[0]");

	DBG(cerr << df6->get_dataset_name() << endl);
	DBG(cerr << df6->get_ce() << endl);
#if 0
	CPPUNIT_ASSERT(df6->get_dataset_name() == "nowhere:[mydisk]myfile");
	CPPUNIT_ASSERT(df6->get_ce() == "Grid%20field:u[0],Grid%20field:v");
#endif
	df5->set_ce("Grid%20u%5B0%5D");
	CPPUNIT_ASSERT(df5->get_ce() == "Grid%20u[0]");
    }

    // This tests reading the timeout value from argv[].
    void timeout_test() {
	CPPUNIT_ASSERT(df3->get_timeout() == 1);
	CPPUNIT_ASSERT(df1->get_timeout() == 0);
    }

    CPPUNIT_TEST_SUITE( ResponseBuilderTest );
    CPPUNIT_TEST(add_keyword_test);
    CPPUNIT_TEST(is_keyword_test);
    CPPUNIT_TEST(get_keywords);
#if 1
    CPPUNIT_TEST(get_dataset_last_modified_time_test);
    CPPUNIT_TEST(get_das_last_modified_time_test);

    CPPUNIT_TEST(send_das_test);
    CPPUNIT_TEST(send_dds_test);
#endif
    CPPUNIT_TEST(send_ddx_test);
    CPPUNIT_TEST(send_data_ddx_test);
    CPPUNIT_TEST(send_data_ddx_test2);
#if 1
    CPPUNIT_TEST(is_conditional_test);
    CPPUNIT_TEST(get_request_if_modified_since_test);
    CPPUNIT_TEST(escape_code_test);
#endif
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ResponseBuilderTest);
}

int
main( int, char** )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    return wasSuccessful ? 0 : 1;
}



