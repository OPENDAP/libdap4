
// -*- C++ -*-

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <algorithm>

#include "Regex.h"

#include "HTTPConnect.h"
#include "RCReader.h"
// #define DODS_DEBUG 1
#include "debug.h"

using namespace CppUnit;

class HTTPConnectTest : public TestFixture {
private:
    HTTPConnect *http;
    string dodsdev_url;
    string etag;
    string lm;
    string dsp_das_url;

protected:
    bool re_match(Regex &r, const char *s) {
	return r.match(s, strlen(s)) == (int)strlen(s);
    }

public:
    HTTPConnectTest() {}
    ~HTTPConnectTest() {}
    
    void setUp () {
	putenv("DODS_CACHE_INIT=cache-testsuite/dodsrc");
	http = new HTTPConnect(RCReader::instance());

	dodsdev_url = "http://dodsdev.gso.uri.edu/test.html";
	// Two request header values that will generate a 304 response to the
	// above URL.
	etag = "\"6c054-2bf-3d9a1d42\"";
	lm = "Tue, 02 Oct 2002 22:10:10 GMT";
	dsp_das_url = "http://eddy.gso.uri.edu/cgi-bin/nph-dods/avhrr/2001/4/d01093165826.pvu.Z.das";
    }

    void tearDown() {
	delete http;
    }

    CPPUNIT_TEST_SUITE( HTTPConnectTest );

    CPPUNIT_TEST(read_url_test);
    CPPUNIT_TEST(fetch_url_test);
    CPPUNIT_TEST(get_response_headers_test);
    CPPUNIT_TEST(server_version_test);
    CPPUNIT_TEST(type_test);
#if 0
    CPPUNIT_TEST(is_reponse_present_test); // deprecated method
#endif
    CPPUNIT_TEST(cache_test);

    CPPUNIT_TEST_SUITE_END();

    void read_url_test() {
	try {
	    FILE *dump = fopen("/dev/null", "w");
	    long status = http->read_url(dodsdev_url, dump);
	    CPPUNIT_ASSERT(status == 200); 

	    vector<string> headers;

	    // First test using a time with if-modified-since
	    headers.push_back(string("If-Modified-Since: ") + lm);
	    status = http->read_url(dodsdev_url, dump, &headers);
	    CPPUNIT_ASSERT(status == 304); 

	    // Now test an etag
	    headers.clear();
	    headers.push_back(string("If-None-Match: ") + etag);
	    status = http->read_url(dodsdev_url, dump, &headers);
	    CPPUNIT_ASSERT(status == 304); 

	    // now test a combined etag and time4
	    headers.push_back(string("If-Modified-Since: ") + lm);
	    status = http->read_url(dodsdev_url, dump, &headers);
	    CPPUNIT_ASSERT(status == 304); 

	}
	catch(Error &e) {
	    cerr << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false && "Should not get an Error");
	}
    }

    void fetch_url_test() {
	char c;
	try {
	    FILE *stuff = http->fetch_url("http://dcz.dods.org/");
	    CPPUNIT_ASSERT(fread(&c, 1, 1, stuff) == 1 && !ferror(stuff) 
			   && !feof(stuff));
	    fclose(stuff);
	    stuff = http->fetch_url(dsp_das_url);
	    CPPUNIT_ASSERT(fread(&c, 1, 1, stuff) == 1 && !ferror(stuff) 
			   && !feof(stuff));
	    fclose(stuff);
	}
	catch (Error &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Caught a DODS exception from fetch_url");
	}
	catch (InternalErr &e) {
	    cerr << "InternalErr: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Caught a DODS exception from fetch_url");
	}
#if 0
	catch (...) {
	    CPPUNIT_ASSERT(!"Caught an unknown exception from fetch_url");
	}
#endif
    }

    void get_response_headers_test() {
	http->fetch_url(dsp_das_url);

	try {
	    vector<string> h = http->get_response_headers();

	    DBG(copy(h.begin(), h.end(), 
		     ostream_iterator<string>(cerr, "\n")));

	    // Should get five headers back.
	    Regex header("XDODS-Server: DAP/.*");
	    CPPUNIT_ASSERT(re_match(header, h[0].c_str()));
	    CPPUNIT_ASSERT(h[4] == "Content-Description: dods_das");
	    CPPUNIT_ASSERT(h.size() == 5);
	}
	catch (InternalErr &e) {
	    CPPUNIT_ASSERT(!"Caught an exception from get_response_headers");
	}
    }

    void server_version_test() {
	http->fetch_url(dsp_das_url);
	Regex version("dap/[0-9]+\\.[0-9]+\\.[0-9]+");
	try {
	    CPPUNIT_ASSERT(re_match(version, http->server_version().c_str()));
	}
	catch (InternalErr &e) {
	    CPPUNIT_ASSERT(!"Caught an exception from server_version");
	}
	
    }

    void type_test() {
	http->fetch_url(dsp_das_url);
	try {
	    CPPUNIT_ASSERT(http->type() == dods_das);
	}
	catch (InternalErr &e) {
	    CPPUNIT_ASSERT(!"Caught an exception from type()");
	}

    }

    void set_credentials_test() {
	http->set_credentials("jimg", "was_quit");

	try {
	    char c;
	    FILE *stuff = http->fetch_url("http://dcz.dods.org/secret");
	    CPPUNIT_ASSERT(fread(&c, 1, 1, stuff) == 1 && !ferror(stuff) 
			   && !feof(stuff));
	}
	catch (InternalErr &e) {
	    CPPUNIT_ASSERT(!"Caught exception from output");
	}
    }

#if 0
    // This method is deprecated.
    void is_reponse_present_test() {
	FILE *resp = http->fetch_url("http://dcz.dods.org/");
	CPPUNIT_ASSERT(http->is_response_present());
	fclose(resp);
	CPPUNIT_ASSERT(!http->is_response_present());
	try {
	    http->get_response_headers();
	    CPPUNIT_ASSERT(!"Didn't throw exception as expected");
	}
	catch (InternalErr &e) {
	    CPPUNIT_ASSERT("Caught exception as expected");
	}
    }
#endif

    void cache_test() {
	// The cache-testsuite/dodsrc file turns this off; all the other
	// params are set up. 
	http->d_http_cache->set_cache_enabled(true);

	fetch_url_test();
	get_response_headers_test();
	server_version_test();
	type_test();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPConnectTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: HTTPConnectTest.cc,v $
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.5  2002/12/24 02:39:48  jimg
// Added cache test back into code.
//
// Revision 1.1.2.4  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.1.2.3  2002/10/18 22:53:17  jimg
// Massive cahnges. Now tests read_url and also includes tests for accesses that use the HTTPCache class (as well as tests that force *not* using the cache).
//
// Revision 1.1.2.2  2002/09/14 03:41:31  jimg
// Added tests for get_response_headers and is_response_present.
//
// Revision 1.1.2.1  2002/07/25 13:24:44  jimg
// Added.
//

