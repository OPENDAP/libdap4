
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
 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <fstream>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

// #define DODS_DEBUG
#include "RCReader.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

class RCReaderTest : public TestFixture {
private:
    RCReader *rcr;

protected:

public:
    RCReaderTest() : rcr(RCReader::instance()) {}

    void setUp () {}

    void tearDown() {}

    CPPUNIT_TEST_SUITE(RCReaderTest);

    CPPUNIT_TEST(check_env_var_test1);
    CPPUNIT_TEST(check_env_var_test2);
    CPPUNIT_TEST(check_env_var_test3);
    CPPUNIT_TEST(check_env_var_test4);
    CPPUNIT_TEST(check_env_var_test5);
    CPPUNIT_TEST(instance_test1);
    CPPUNIT_TEST(instance_test2);
    CPPUNIT_TEST(proxy_test1);
    CPPUNIT_TEST(proxy_test2);
    CPPUNIT_TEST(proxy_test3);

    CPPUNIT_TEST_SUITE_END();

    void check_env_var_test1() {
	putenv("DODS_CONF=");
	CPPUNIT_ASSERT(rcr->check_env_var("DODS_CONF") == "");
    }

    void check_env_var_test2() {
	putenv("DODS_CONF=Nothing_sensible");
	CPPUNIT_ASSERT(rcr->check_env_var("DODS_CONF") == "");
    }

    void check_env_var_test3() {
	putenv("DODS_CONF=/etc/passwd");
	CPPUNIT_ASSERT(rcr->check_env_var("DODS_CONF") == "/etc/passwd");
    }

    void check_env_var_test4() {
	// Set DODS_CONF to the CWD plus .dodsrc, create file called .dodsrc
	// in the CWD and test to see if check_env_var finds it.
	char cwd[1024];
	getcwd(cwd, 1024);

	string rc = string(cwd) + string("/.dodsrc");
	ifstream ifp(rc.c_str()); // This should create .dodsrc in the CWD
	string dc = string("DODS_CONF=") + string(cwd);
	putenv(const_cast<char*>(dc.c_str()));

	// Return existing file
	CPPUNIT_ASSERT(rcr->check_env_var("DODS_CONF") == rc);
	remove(rc.c_str());
    }

    void check_env_var_test5() {
	// In this test we *don't* create the file, just set DODS_CONF to the
	// directory and see if check_env_var() makes the RC file.
	char cwd[1024];
	getcwd(cwd, 1024);

	string rc = string(cwd) + string("/.dodsrc");
	string dc = string("DODS_CONF=") + string(cwd);
	putenv(const_cast<char*>(dc.c_str()));

	// Create the file.
	CPPUNIT_ASSERT(rcr->check_env_var("DODS_CONF") == rc);
	struct stat stat_info;
	CPPUNIT_ASSERT(stat(rc.c_str(), &stat_info) == 0 &&
		       S_ISREG(stat_info.st_mode));
	remove(rc.c_str());
    }

    void instance_test1() {
	// This test assumes that HOME *is* defined. We should find the
	// .dodsrc there. If it's not there, we should create one there.
	putenv("DODS_CONF=");
	string home = getenv("HOME");
	if (*home.rbegin() != '/')
	    home += "/";
	RCReader::delete_instance();
	RCReader::initialize_instance();
	RCReader *reader = RCReader::instance();
	CPPUNIT_ASSERT(reader->d_rc_file_path == home + string(".dodsrc"));
	DBG(cerr << "Cache root: " << reader->get_dods_cache_root() << endl);
	CPPUNIT_ASSERT(reader->get_dods_cache_root() 
		       == home + string(".dods_cache/"));
    }

    void instance_test2() {
	// Set DODS_CONF to create a .dodsrc in the CWD, then check to make
	// sure that .dodsrc has the correct cache root.
	char cwd[1024];
	getcwd(cwd, 1024);
	DBG(cerr << "CWD: " << cwd << endl);
	string rc = cwd;
	rc += "/.dodsrc";
	DBG(cerr << "RC: " << rc << endl);
	remove(rc.c_str());	// make sure the RC does not exist

	char dc[1024];
	strncpy(dc, "DODS_CONF=", 1024);
	strncat(dc, cwd, 1024-strlen("DODS_CONF="));
	dc[1023] = '\0';
	DBG(cerr << "dc: " << dc << endl);
	putenv(dc);

	RCReader::delete_instance();
	RCReader::initialize_instance();
	RCReader *reader = RCReader::instance();
	DBG(cerr << "RC path: " << reader->d_rc_file_path << endl);
	CPPUNIT_ASSERT(reader->d_rc_file_path 
		       == string(cwd) + string("/.dodsrc"));
	DBG(cerr << "Cache root: " << reader->get_dods_cache_root() << endl);
	CPPUNIT_ASSERT(reader->get_dods_cache_root() 
		       == string(cwd) + string("/.dods_cache/"));
	
    }

    // Read the proxy info from rcreader-testsuite/test1.rc
    void proxy_test1() {
	char rc[] = { "DODS_CONF=rcreader-testsuite/test1.rc" };
	DBG(cerr << "rc: " << rc << endl);
	putenv(rc);

	RCReader::delete_instance();
	RCReader::initialize_instance();
	RCReader *reader = RCReader::instance();
	DBG(cerr << "RC path: " << reader->d_rc_file_path << endl);
	CPPUNIT_ASSERT(reader->d_rc_file_path 
		       == "rcreader-testsuite/test1.rc");
	CPPUNIT_ASSERT(reader->get_proxy_server_protocol() == "http");

	string proxy = reader->get_proxy_server_host_url();
	CPPUNIT_ASSERT(proxy == "jimg:mypass@proxy.local.org:8080");

	CPPUNIT_ASSERT(reader->get_proxy_server_host() == "proxy.local.org");
	CPPUNIT_ASSERT(reader->get_proxy_server_port() == 8080);
	CPPUNIT_ASSERT(reader->get_proxy_server_userpw() == "jimg:mypass");

	CPPUNIT_ASSERT(reader->is_no_proxy_for_used());
	CPPUNIT_ASSERT(reader->get_no_proxy_for_protocol() == "http");
	CPPUNIT_ASSERT(reader->get_no_proxy_for_host() == "local.org");
    }

    void proxy_test2() {
	char rc[] = { "DODS_CONF=rcreader-testsuite/test2.rc" };
	DBG(cerr << "rc: " << rc << endl);
	putenv(rc);

	RCReader::delete_instance();
	RCReader::initialize_instance();
	RCReader *reader = RCReader::instance();
	DBG(cerr << "RC path: " << reader->d_rc_file_path << endl);
	CPPUNIT_ASSERT(reader->d_rc_file_path 
		       == "rcreader-testsuite/test2.rc");
	CPPUNIT_ASSERT(reader->get_proxy_server_protocol() == "http");

	string proxy = reader->get_proxy_server_host_url();
	DBG(cerr << "get_proxy_server_host_url(): " << proxy << endl);
	CPPUNIT_ASSERT(proxy == "proxy.local.org:80");

	CPPUNIT_ASSERT(reader->get_proxy_server_host() == "proxy.local.org");
	CPPUNIT_ASSERT(reader->get_proxy_server_port() == 80);
	CPPUNIT_ASSERT(reader->get_proxy_server_userpw() == "");
    }

    void proxy_test3() {
	char rc[] = { "DODS_CONF=rcreader-testsuite/test3.rc" };
	DBG(cerr << "rc: " << rc << endl);
	putenv(rc);

	try {
	    RCReader::delete_instance();
	    RCReader::initialize_instance();
	    CPPUNIT_ASSERT(!"initialize_instance() should throw Error.");
	}
	catch(Error &e) {
	    DBG(cerr << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(e.get_error_message() != "");
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(RCReaderTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: RCReaderTest.cc,v $
// Revision 1.5  2004/11/16 22:50:20  jimg
// Fixed tests. Also fixed a bug intorduced in Vector where a template
// with no name caused some software (any code which depends on the
// template having the same name as the array) to fail.
//
// Revision 1.4  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.1.2.4  2004/06/21 20:52:18  jimg
// Added tests for th new proxy server methods.
//
// Revision 1.3  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.1.2.3  2004/02/12 23:34:57  jimg
// Fixed two tests that came back as using uninitialized memory. I'm not sure it
// if was a real problem, but valgrind now reports zero errors for all the unit
// tests.
//
// Revision 1.1.2.2  2004/02/11 17:35:01  jimg
// Updated for new instance initialization methods.
//
// Revision 1.2  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.1.2.1  2003/09/19 22:30:28  jimg
// Added
//
// Revision 1.9.2.3  2003/09/06 22:11:02  jimg
// Modified so that localhost is used instead of remote hosts. This means
// that the tests don't require Internet access but do require that the
// local machine runs httpd and has it correctly configured.
//
// Revision 1.9.2.2  2003/08/17 20:37:15  rmorris
// Mod's to more-uniformly and simply account for sleep(secs) vs. Sleep(millisecs)
// difference between unix/Linux/OSX and win32.  The ifdef to make win32
// match unixes is now in one single win32-specific place.
//
// Revision 1.9.2.1  2003/05/06 06:45:29  jimg
// Fixed HTTPCacheTest to work with the new HTTPConnect and Response
// classes. These use local copies of the response headers managed by the
// Response class.
//
// Revision 1.9  2003/05/01 22:52:46  jimg
// Corrected the get_conditional_response_headers_test() given the latest mods
// to HTTPCache.cc.
//
// Revision 1.8  2003/04/23 21:33:53  jimg
// Changes for the unit tests. This involved merging Rob's VC++ changes
// and fixing a bug in escaping.cc (a call to string::insert invalidated
// an iterator in a loop).
//
// Revision 1.7  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.3.2.2  2003/04/18 00:54:24  rmorris
// Ported dap unit tests to win32.
//
// Revision 1.6  2003/03/04 23:19:37  jimg
// Fixed some of the unit tests.
//
// Revision 1.5  2003/03/04 17:30:04  jimg
// Now uses Response objects.
//
// Revision 1.4  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.5  2002/12/24 00:51:04  jimg
// Updated for the new (streamlined) HTTPConnect interface.
//
// Revision 1.1.2.4  2002/10/18 23:02:22  jimg
// Massive additions for the latest version of HTTPCache.
//
// Revision 1.1.2.3  2002/10/11 00:19:38  jimg
// Latest version. 22 tests. Note that these tests don't make a cache that
// behaves correctly; all of the test caches grown in size and need to be
// cleaned up. All can be removed *except* cache-testsuite/dods_cache. The
// latter has some files which should not be removed.
//
// Revision 1.1.2.2  2002/10/08 05:24:45  jimg
// This checking matches the version of HTTPCache.cc,h of the same date.
// All the tests pass but they rely on the current contents of
// cache-testsuite.
//
// Revision 1.1.2.1  2002/09/17 00:19:29  jimg
// New tests for cache root and lock code.
//
// Revision 1.1.2.5  2002/06/19 06:05:36  jimg
// Built a single file version of the cgi_util tests. This should be easy to
// clone for other test suites.
//
