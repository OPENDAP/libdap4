
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
 
#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "DODSFilter.h"
#include "DAS.h"
#include "Regex.h"
#include <strstream>

class DODSFilterTest : public TestCase {
private:
    DODSFilter *df, *df1, *df2, *df3, *df4, *df5, *df6;

    AttrTable *cont_a;
    DAS *das;
    ostrstream oss;
    streampos start_oss;

public: 
    DODSFilterTest (string name) : TestCase (name) {}

    void setUp() {
	start_oss = oss.tellp();

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
	char *argv_2[] = {"test_case", "server-testsuite/coads.data", "-l",
			  "988679294", "-e", "u,x,z[0]&grid(u,\"lat<10.0\")"};
	df3 = new DODSFilter(6, argv_2);

	// Go back to this data source to test w/o an ancillary DAS.
	argv_2[1] = "server-testsuite/bears.data";
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
	oss.freeze(0);
	oss.seekp(start_oss);
    }	

    bool re_match(Regex &r, const char *s) {
#if 0
	cerr << "strlen(s): " << (int)strlen(s) << endl;
	cerr << "r.match(s): " << r.match(s, strlen(s)) << endl;
#endif
	return r.match(s, strlen(s)) == (int)strlen(s);
    }

    // Tests for methods
    void read_ancillary_das_test() {
	// Tests using assert.
    }

    void read_ancillary_dds_test() {
	// Tests using assert.
    }

    void get_dataset_last_modified_time_test() {
	time_t t = time(0);
	assert(df1->get_dataset_last_modified_time() == t);

	struct stat st;
	stat("server-testsuite/bears.data", &st);
	assert(df->get_dataset_last_modified_time() == st.st_mtime);

	stat("Makefile.in", &st);
	assert(df2->get_dataset_last_modified_time() == st.st_mtime);

	stat("server-testsuite/coads.data", &st);
	assert(df3->get_dataset_last_modified_time() == st.st_mtime);
    }

    void get_das_last_modified_time_test() {
	// the dataset pointed to by df has no anc. DAS
	struct stat st;
	stat("server-testsuite/bears.data", &st);
	assert(df->get_das_last_modified_time() == st.st_mtime);

	// the dataset pointed by df3 has an anc. DAS
	stat("server-testsuite/coads.data.das", &st);
	assert(df3->get_das_last_modified_time() == st.st_mtime);
    }

    void send_das_test() {
	cerr << endl
	     << "Note: the send_das() tests depend, in part, on having data
 sources written on a certain date. These work with my copies of the files,
 but these tests probably will not work with files checked out of CVS."
	     << endl;

	Regex r1("HTTP/1.0 200 OK
XDODS-Server:.*
Date: \\(.*\\)
Last-Modified: Thu, 29 Apr 1999 02:29:40 GMT
Content-type: text/plain
Content-Description: dods_das

Attributes {
    a {
        Int32 size 7;
        String type cars;
    }
}.*
");
	df->send_das(oss, *das); oss << ends;
#if 0
	cerr << "DAS: " << oss.str() << endl;
#endif
	assert(re_match(r1, oss.str()));
	reset_oss();

	Regex r2("HTTP/1.0 304 NOT MODIFIED
Date: .*

");
	df3->send_das(oss, *das); oss << ends;
	assert(re_match(r2, oss.str()));
    }	

    void is_conditional_test() {
	assert(df->is_conditional() == false);
	assert(df3->is_conditional() == true);
    }

    void get_request_if_modified_since_test() {
	assert(df->get_request_if_modified_since() == -1);
	assert(df3->get_request_if_modified_since() == 988679294);
    }

    void escape_code_test() {
	// These should NOT be escaped.
#if 0
	cerr << df3->get_dataset_name() << endl;
	cerr << df3->get_ce() << endl;
#endif
	assert(df3->get_dataset_name() == "server-testsuite/coads.data");
	assert(df3->get_ce() == "u,x,z[0]&grid(u,\"lat<10.0\")");

	// The DODSFIlter instance is feed escaped values; they should be
	// unescaped by the ctor and the mutators. 5/4/2001 jhrg

#if 0
	cerr << df5->get_dataset_name() << endl;
	cerr << df5->get_ce() << endl;
#endif
	assert(df5->get_dataset_name() == "nowhere[mydisk]myfile");
	assert(df5->get_ce() == "u[0]");

	df5->set_ce("u%5B0%5D");
	assert(df5->get_ce() == "u[0]");

	assert(df6->get_dataset_name() == "nowhere:[mydisk]myfile");
	assert(df6->get_ce() == "Grid%20field:u[0],Grid%20field:v");
	df5->set_ce("Grid%20u%5B0%5D");
	assert(df5->get_ce() == "Grid%20u[0]");
    }

    static Test *suite ()  {
	TestSuite *s = new TestSuite("DODSFilterTest");
	s->addTest(new TestCaller<DODSFilterTest>
		   ("escape_code_test", &DODSFilterTest::escape_code_test));

	s->addTest(new TestCaller<DODSFilterTest>
		   ("read_ancillary_das_test",
		    &DODSFilterTest::read_ancillary_das_test));

	s->addTest(new TestCaller<DODSFilterTest>
		   ("read_ancillary_dds_test",
		    &DODSFilterTest::read_ancillary_dds_test));

	s->addTest(new TestCaller<DODSFilterTest>
		   ("get_dataset_last_modified_time_test", 
		    &DODSFilterTest::get_dataset_last_modified_time_test));

	s->addTest(new TestCaller<DODSFilterTest>
		   ("get_das_last_modified_time_test", 
		    &DODSFilterTest::get_das_last_modified_time_test));

	s->addTest(new TestCaller<DODSFilterTest>
		   ("send_das_test", &DODSFilterTest::send_das_test));

	s->addTest(new TestCaller<DODSFilterTest>
		   ("is_conditional_test", 
		    &DODSFilterTest::is_conditional_test));

	s->addTest(new TestCaller<DODSFilterTest>
		   ("get_request_if_modified_since_test", 
		    &DODSFilterTest::get_request_if_modified_since_test));
	return s;
    }
};




