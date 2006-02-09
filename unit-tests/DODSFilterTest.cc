
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

#include <sstream>

#include "DODSFilter.h"
#include "DAS.h"
#include "GNURegex.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

class DODSFilterTest : public TestFixture {
private:
    DODSFilter *df, *df1, *df2, *df3, *df4, *df5, *df6;

    AttrTable *cont_a;
    DAS *das;
    ostringstream oss;
    time_t now;
    char now_array[256];

public: 
    DODSFilterTest() {
	now = time(0);
	ostringstream time_string;
	time_string << (int)now;
	strncpy(now_array, time_string.str().c_str(), 255);
	now_array[255] = '\0';
    }

    ~DODSFilterTest() {}

    void setUp() {
	// Test pathname
	char *argv_1[] = {"test_case", "server-testsuite/bears.data"};
	df = new DODSFilter(2, argv_1);

	// Test missing file
	argv_1[1] = "no-such-file";
	df1 = new DODSFilter(2, argv_1);

	// Test files in CWD. Note that the time is the GM time : Tue, 01 May
	// 2001 01:08:14 -0700
	argv_1[1] = "Makefile.in";
	df2 = new DODSFilter(2, argv_1);

	// This file has an ancillary DAS in the server-testsuite dir.
	// df3 is also used to test escaping stuff in URLs. 5/4/2001 jhrg
	char *argv_2[8];
	argv_2[0] = "test_case";
	argv_2[1] = "server-testsuite/coads.data";
	argv_2[2] = "-l";
	argv_2[3] = &now_array[0];
	argv_2[4] = "-e";
	argv_2[5] = "u,x,z[0]&grid(u,\"lat<10.0\")";
	argv_2[6] = "-t";
	argv_2[7] = "1";
	df3 = new DODSFilter(6, argv_2);

	// Go back to this data source to test w/o an ancillary DAS.
	argv_2[0] = "test_case";
	argv_2[1] = "server-testsuite/bears.data";
	argv_2[2] = "-l";
	argv_2[3] = &now_array[0];
	argv_2[4] = "-e";
	argv_2[5] = "u,x,z[0]&grid(u,\"lat<10.0\")";
	argv_2[6] = "-t";
	argv_2[7] = "1";
	df4 = new DODSFilter(6, argv_2);

	// Test escaping stuff. 5/4/2001 jhrg
	char *argv_3[]={"test_case", "nowhere%5Bmydisk%5Dmyfile", "-e",
			"u%5B0%5D"};
	df5 = new DODSFilter(4, argv_3);

	char *argv_4[]={"test_case", "nowhere%3a%5bmydisk%5dmyfile", "-e",
			"Grid%20field%3au%5b0%5d,Grid%20field%3av"};
	df6 = new DODSFilter(4, argv_4);

	cont_a = new AttrTable;
	cont_a->append_attr("size", "Int32", "7");
	cont_a->append_attr("type", "String", "cars");
	das = new DAS(cont_a, "a");

	// This AttrTable looks like:
	//      Attributes {
	//          a {
	//              Int32 size 7;
	//              String type cars;
	//          }
	//      }
    }

    void tearDown() {
	delete df; df = 0;
	delete df1; df1 = 0;
	delete df2; df2 = 0;
	delete df3; df3 = 0;
	delete df4; df4 = 0;
	delete df5; df5 = 0;
	delete df6; df6 = 0;

	delete das; das = 0;
    }

    void reset_oss() {
	oss.str("");
    }	

    bool re_match(Regex &r, const string &s) {
	DBG(cerr << "s.length(): " << s.length() << endl);
	int pos = r.match(s.c_str(), s.length());
	DBG(cerr << "r.match(s): " << pos << endl);
	return pos > 0 && static_cast<unsigned>(pos) == s.length();
    }

    // Tests for methods
    void read_ancillary_das_test() {
    }

    void read_ancillary_dds_test() {
    }

    void get_dataset_last_modified_time_test() {
	time_t t = time(0);
	CPPUNIT_ASSERT(df1->get_dataset_last_modified_time() == t);

	struct stat st;
	stat("server-testsuite/bears.data", &st);
	CPPUNIT_ASSERT(df->get_dataset_last_modified_time() == st.st_mtime);

	stat("Makefile.in", &st);
	CPPUNIT_ASSERT(df2->get_dataset_last_modified_time() == st.st_mtime);

	stat("server-testsuite/coads.data", &st);
	CPPUNIT_ASSERT(df3->get_dataset_last_modified_time() == st.st_mtime);
    }

    void get_das_last_modified_time_test() {
	// the dataset pointed to by df has no anc. DAS
	struct stat st;
	stat("server-testsuite/bears.data", &st);
	CPPUNIT_ASSERT(df->get_das_last_modified_time() == st.st_mtime);

	// the dataset pointed by df3 has an anc. DAS
	stat("server-testsuite/coads.data.das", &st);
	CPPUNIT_ASSERT(df3->get_das_last_modified_time() == st.st_mtime);
    }

#if 0
    void send_das_test() {
#if 0
	// I'm pretty sure I've fixed this... It should always work now. I
	// set the time in df3 to 'now' so that should always be later than
	// whenever the das was pulled from cvs. 09/05/03 jhrg
	cerr << endl
	     << "Note: the send_das() tests depend, in part, on having data\r\n\
 sources written on a certain date. These work with my copies of the files,\r\n\
 but these tests probably will not work with files checked out of CVS."
	     << endl;
#endif

	Regex r1("HTTP/1.0 200 OK\r\n\
XDODS-Server:.*\r\n\
XOPeNDAP-Server: dods-test/0.00\r\n\
XDAP: .*\r\n\
Date: .*\r\n\
Last-Modified: .*\r\n\
Content-type: text/plain\r\n\
Content-Description: dods_das\r\n\
\r\n\
Attributes \\{\r\n\
    a \\{\r\n\
        Int32 size 7;\r\n\
        String type cars;\r\n\
    \\}\r\n\
\\}.*\n");
	df->send_das(oss, *das);

	DBG(cerr << "DAS: " << oss.str() << endl);

	CPPUNIT_ASSERT(re_match(r1, oss.str()));
	reset_oss();

	Regex r2("HTTP/1.0 304 NOT MODIFIED\r\n\
Date: .*\r\n\
\r\n\
");
	df3->send_das(oss, *das);
	CPPUNIT_ASSERT(re_match(r2, oss.str()));
    }	
#endif

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

	CPPUNIT_ASSERT(df3->get_dataset_name() == "server-testsuite/coads.data");
	CPPUNIT_ASSERT(df3->get_ce() == "u,x,z[0]&grid(u,\"lat<10.0\")");

	// The DODSFIlter instance is feed escaped values; they should be
	// unescaped by the ctor and the mutators. 5/4/2001 jhrg

	DBG(cerr << df5->get_dataset_name() << endl);
	DBG(cerr << df5->get_ce() << endl);

	CPPUNIT_ASSERT(df5->get_dataset_name() == "nowhere[mydisk]myfile");
	CPPUNIT_ASSERT(df5->get_ce() == "u[0]");

	df5->set_ce("u%5B0%5D");
	CPPUNIT_ASSERT(df5->get_ce() == "u[0]");

	CPPUNIT_ASSERT(df6->get_dataset_name() == "nowhere:[mydisk]myfile");
	CPPUNIT_ASSERT(df6->get_ce() == "Grid%20field:u[0],Grid%20field:v");
	df5->set_ce("Grid%20u%5B0%5D");
	CPPUNIT_ASSERT(df5->get_ce() == "Grid%20u[0]");
    }

    // This tests reading the timeout value from argv[].
    void timeout_test() {
	CPPUNIT_ASSERT(df3->get_timeout() == 1);
	CPPUNIT_ASSERT(df1->get_timeout() == 0);
    }

    CPPUNIT_TEST_SUITE( DODSFilterTest );

    CPPUNIT_TEST(get_dataset_last_modified_time_test);
    CPPUNIT_TEST(get_das_last_modified_time_test);
    // Removed until I can figure out how to test using the FILE-based 
    // methods. jhrg 1/18/06
#if 0
    CPPUNIT_TEST(send_das_test);
#endif
    CPPUNIT_TEST(is_conditional_test);
    CPPUNIT_TEST(get_request_if_modified_since_test);
    CPPUNIT_TEST(escape_code_test);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DODSFilterTest);

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    return wasSuccessful ? 0 : 1;
}



