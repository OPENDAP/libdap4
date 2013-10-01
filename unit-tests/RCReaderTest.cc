
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2006 OPeNDAP, Inc.
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <cstdlib>
#include <cstring>
#include <cstdlib>

#include <string>
#include <iostream>
#include <fstream>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

//#define DODS_DEBUG
#include "RCReader.h"
#include "debug.h"
#include <test_config.h>

using namespace CppUnit;
using namespace std;

namespace libdap
{

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
    CPPUNIT_TEST(proxy_test4);
    CPPUNIT_TEST(proxy_test5);
    CPPUNIT_TEST(validate_ssl_test);
    
    CPPUNIT_TEST_SUITE_END();

    void check_env_var_test1() {
	putenv((char *)"DODS_CONF=");
	CPPUNIT_ASSERT(rcr->check_env_var("DODS_CONF") == "");
    }

    void check_env_var_test2() {
	putenv((char*)"DODS_CONF=Nothing_sensible");
	CPPUNIT_ASSERT(rcr->check_env_var("DODS_CONF") == "");
    }

    void check_env_var_test3() {
	putenv((char*)"DODS_CONF=/etc/passwd");
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
	putenv((char*)"DODS_CONF=");
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
	string rc = (string)"DODS_CONF=" + TEST_SRC_DIR + "/rcreader-testsuite/test1.rc" ;
	DBG(cerr << "rc: " << rc << endl);
	putenv((char *)rc.c_str());

	RCReader::delete_instance();
	RCReader::initialize_instance();
	RCReader *reader = RCReader::instance();
	DBG(cerr << "RC path: " << reader->d_rc_file_path << endl);
	CPPUNIT_ASSERT(reader->d_rc_file_path 
		       == (string)TEST_SRC_DIR + "/rcreader-testsuite/test1.rc");
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
	string rc = (string)"DODS_CONF=" + TEST_SRC_DIR + "/rcreader-testsuite/test2.rc" ;
	DBG(cerr << "rc: " << rc << endl);
	putenv((char *)rc.c_str());

	RCReader::delete_instance();
	RCReader::initialize_instance();
	RCReader *reader = RCReader::instance();
	DBG(cerr << "RC path: " << reader->d_rc_file_path << endl);
	CPPUNIT_ASSERT(reader->d_rc_file_path 
		       == (string)TEST_SRC_DIR + "/rcreader-testsuite/test2.rc");
	CPPUNIT_ASSERT(reader->get_proxy_server_protocol() == "http");

	string proxy = reader->get_proxy_server_host_url();
	DBG(cerr << "get_proxy_server_host_url(): " << proxy << endl);
	CPPUNIT_ASSERT(proxy == "proxy.local.org:80");

	CPPUNIT_ASSERT(reader->get_proxy_server_host() == "proxy.local.org");
	CPPUNIT_ASSERT(reader->get_proxy_server_port() == 80);
	CPPUNIT_ASSERT(reader->get_proxy_server_userpw() == "");
    }

    void proxy_test3() {
	string rc = (string)"DODS_CONF=" + TEST_SRC_DIR + "/rcreader-testsuite/test3.rc" ;
	DBG(cerr << "rc: " << rc << endl);
	putenv((char *)rc.c_str());

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
    
    void proxy_test4() {
	string rc = (string)"DODS_CONF=" + TEST_SRC_DIR + "/rcreader-testsuite/test4.rc" ;
	DBG(cerr << "rc: " << rc << endl);
	putenv((char *)rc.c_str());

	try {
	    RCReader::delete_instance();
	    RCReader::initialize_instance();
	    RCReader *reader = RCReader::instance();
	    DBG(cerr << "RC path: " << reader->d_rc_file_path << endl);
	    CPPUNIT_ASSERT(reader->d_rc_file_path 
			   == (string)TEST_SRC_DIR + "/rcreader-testsuite/test4.rc");
	    CPPUNIT_ASSERT(reader->get_proxy_server_protocol() == "http");

	    string proxy = reader->get_proxy_server_host_url();
	    DBG(cerr << "get_proxy_server_host_url(): " << proxy << endl);
	    CPPUNIT_ASSERT(proxy == "jimg:test@proxy.local.org:3128");

	    CPPUNIT_ASSERT(reader->get_proxy_server_host() == "proxy.local.org");
	    CPPUNIT_ASSERT(reader->get_proxy_server_port() == 3128);
	    CPPUNIT_ASSERT(reader->get_proxy_server_userpw() == "jimg:test");
	}
	catch(Error &e) {
	    DBG(cerr << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(e.get_error_message() != "");
	}
    }
    
    void proxy_test5() {
	string rc = (string)"DODS_CONF=" + TEST_SRC_DIR + "/rcreader-testsuite/test5.rc" ;
	DBG(cerr << "rc: " << rc << endl);
	putenv((char *)rc.c_str());

	try {
	    RCReader::delete_instance();
	    RCReader::initialize_instance();
	    RCReader *reader = RCReader::instance();
	    DBG(cerr << "RC path: " << reader->d_rc_file_path << endl);
	    CPPUNIT_ASSERT(reader->d_rc_file_path 
			   == (string)TEST_SRC_DIR + "/rcreader-testsuite/test5.rc");
	    string proxy = reader->get_proxy_server_host_url();
	    DBG(cerr << "get_proxy_server_host_url(): " << proxy << endl);
	    CPPUNIT_ASSERT(reader->get_proxy_server_protocol() == "http");

	    CPPUNIT_ASSERT(proxy == "jimg:test@proxy.local.org:3128");

	    CPPUNIT_ASSERT(reader->get_proxy_server_host() == "proxy.local.org");
	    CPPUNIT_ASSERT(reader->get_proxy_server_port() == 3128);
	    CPPUNIT_ASSERT(reader->get_proxy_server_userpw() == "jimg:test");
	}
	catch(Error &e) {
	    DBG(cerr << e.get_error_message() << endl);
	    CPPUNIT_ASSERT(e.get_error_message() != "");
	}
    }
    

    // This simple test checks to see that the VALIDATE_SSL parameter is 
    // read correctly.
    void validate_ssl_test() {
	string rc = (string)"DODS_CONF=" + TEST_SRC_DIR + "/rcreader-testsuite/dodssrc_ssl_1" ;
	DBG(cerr << "rc: " << rc << endl);
	putenv((char *)rc.c_str());

        RCReader::delete_instance();
        RCReader::initialize_instance();
        RCReader *reader = RCReader::instance();
        // No param set in file
        DBG(cerr << "reader->get_validate_ssl(): " << reader->get_validate_ssl()
            << endl);
        CPPUNIT_ASSERT(reader->get_validate_ssl() == 1);

        // Param set in file
	rc = (string)"DODS_CONF=" + TEST_SRC_DIR + "/rcreader-testsuite/dodssrc_ssl_2" ;
	DBG(cerr << "rc: " << rc << endl);
	putenv((char *)rc.c_str());

        RCReader::delete_instance();
        RCReader::initialize_instance();
        reader = RCReader::instance();
        DBG(cerr << "reader->check_env_var(\"DODS_CONF\"): " 
            << reader->check_env_var("DODS_CONF") << endl);
        DBG(cerr << "reader->get_validate_ssl(): " << reader->get_validate_ssl()
            << endl);
        CPPUNIT_ASSERT(reader->get_validate_ssl() == 1);

        // Param cleared in file 
	rc = (string)"DODS_CONF=" + TEST_SRC_DIR + "/rcreader-testsuite/dodsrc_ssl_3" ;
	DBG(cerr << "rc: " << rc << endl);
	putenv((char *)rc.c_str());

        RCReader::delete_instance();
        RCReader::initialize_instance();
        reader = RCReader::instance();
        DBG(cerr << "reader->get_validate_ssl(): " << reader->get_validate_ssl()
            << endl);
        CPPUNIT_ASSERT(reader->get_validate_ssl() == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(RCReaderTest);

} // namespace libdap

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    return wasSuccessful ? 0 : 1;
}
