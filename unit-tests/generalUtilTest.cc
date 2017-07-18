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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// Tests for the util functions in util.cc and escaping.cc

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

// #define DODS_DEBUG

#include "debug.h"
#include "util.h"
#include "escaping.h"

#include <cstring>
#include <string>
#include "GetOpt.h"

using std::cerr;
using std::endl;
using std::string;

using namespace CppUnit;
using namespace libdap;

static bool debug = false;

class generalUtilTest: public TestFixture {
private:

public:
    generalUtilTest()
    {
    }
    ~generalUtilTest()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE (generalUtilTest);

    CPPUNIT_TEST (octal_to_hex_test);
    CPPUNIT_TEST (prune_spaces_test);
    CPPUNIT_TEST (path_to_filename_test);
    CPPUNIT_TEST (hexstring_test);
    CPPUNIT_TEST (unhexstring_test);
    CPPUNIT_TEST (id2www_test);
    CPPUNIT_TEST (www2id_test);
    CPPUNIT_TEST (ce_string_parse_test);
    CPPUNIT_TEST (escattr_test);
    CPPUNIT_TEST (unescattr_test);
    CPPUNIT_TEST (munge_error_message_test);
    CPPUNIT_TEST (id2xml_test);
    CPPUNIT_TEST (xml2id_test);

    CPPUNIT_TEST (glob_test_1);
    CPPUNIT_TEST (glob_test_2);
    CPPUNIT_TEST (glob_test_3);

    CPPUNIT_TEST_SUITE_END()
    ;

    // Tests for methods
    void glob_test_1()
    {
        string t = "This is a test";
        int status = glob("This is a test", t.c_str());
        CPPUNIT_ASSERT(status == 0);
    }
    void glob_test_2()
    {
        string t = "This is a test";
        int status = glob("This * test", t.c_str());
        CPPUNIT_ASSERT(status == 0);
    }
    void glob_test_3()
    {
        string t = "This is a test";
        int status = glob("* is * test", t.c_str());
        CPPUNIT_ASSERT(status == 0);
    }

    void octal_to_hex_test()
    {
        string hex;
        hex = octal_to_hex("000");
        CPPUNIT_ASSERT(hex == "00");

        hex = octal_to_hex("012");
        CPPUNIT_ASSERT(hex == "0a");

        hex = octal_to_hex("077");
        CPPUNIT_ASSERT(hex == "3f");
    }

    void prune_spaces_test()
    {
        string test_server = "http://test.opendap.org";
        CPPUNIT_ASSERT(prune_spaces(test_server) == test_server);

        string test_server_spaces = "   http://test.opendap.org";
        CPPUNIT_ASSERT(prune_spaces(test_server_spaces) == test_server);

        string test_server_ce = "http://test.opendap.org/file.txt?u,v";
        CPPUNIT_ASSERT(prune_spaces(test_server_ce) == test_server_ce);

        string test_server_ce_spaces = "http://test.opendap.org/file.txt? u,v";
        DBG(cerr << "Test Server CE Spaces: "
            << prune_spaces(test_server_ce_spaces) << endl);
        CPPUNIT_ASSERT(prune_spaces(test_server_ce_spaces) == test_server_ce);

        string hdf_two_var =
            "http://test.opendap.org/dap/data/hdf/S3096277.HDF.Z?Avg_Wind_Speed[0:5][0],RMS_Wind_Speed[0:5][0]";
        CPPUNIT_ASSERT(prune_spaces(hdf_two_var) == hdf_two_var);
    }

    void path_to_filename_test()
    {
        CPPUNIT_ASSERT(path_to_filename("/this/is/the/end/my.friend") == "my.friend");
        CPPUNIT_ASSERT(path_to_filename("this.dat") == "this.dat");
        CPPUNIT_ASSERT(path_to_filename("/this.dat") == "this.dat");
        CPPUNIT_ASSERT(path_to_filename("/this.dat/") == "");
    }

    void hexstring_test()
    {
        CPPUNIT_ASSERT(hexstring('[') == "5b");
        CPPUNIT_ASSERT(hexstring(']') == "5d");
        CPPUNIT_ASSERT(hexstring(' ') == "20");
        CPPUNIT_ASSERT(hexstring('%') == "25");
    }

    void unhexstring_test()
    {
        CPPUNIT_ASSERT(unhexstring("5b") == "[");
        CPPUNIT_ASSERT(unhexstring("5d") == "]");
        CPPUNIT_ASSERT(unhexstring("20") == " ");
        CPPUNIT_ASSERT(unhexstring("25") == "%");
        CPPUNIT_ASSERT(unhexstring("5B") == "[");
        CPPUNIT_ASSERT(unhexstring("5D") == "]");
    }

    void id2www_test()
    {
        CPPUNIT_ASSERT(id2www("this") == "this");
        CPPUNIT_ASSERT(id2www("This is a test") == "This%20is%20a%20test");
        CPPUNIT_ASSERT(id2www("This.is") == "This.is");
        CPPUNIT_ASSERT(id2www("This-is") == "This-is");
        CPPUNIT_ASSERT(id2www("This_is") == "This_is");
        CPPUNIT_ASSERT(id2www("This/is") == "This/is");
        CPPUNIT_ASSERT(id2www("This%is") == "This%25is");
        CPPUNIT_ASSERT(id2www("This&is") == "This%26is");
        CPPUNIT_ASSERT(id2www("%This&is") == "%25This%26is");
    }

    void www2id_test()
    {
        CPPUNIT_ASSERT(www2id("This_is_a_test") == "This_is_a_test");
        CPPUNIT_ASSERT(www2id("This is a test") == "This is a test");
        CPPUNIT_ASSERT(www2id("%5b") == "[");
        CPPUNIT_ASSERT(www2id("%5d") == "]");
        CPPUNIT_ASSERT(www2id("u%5b0%5d") == "u[0]");
        CPPUNIT_ASSERT(www2id("WVC%20Lat") == "WVC Lat");
        CPPUNIT_ASSERT(www2id("Grid.Data%20Fields[20][20]") == "Grid.Data Fields[20][20]");

        CPPUNIT_ASSERT(www2id("Grid.Data%3aFields[20][20]") == "Grid.Data:Fields[20][20]");

        CPPUNIT_ASSERT(www2id("Grid%3aData%20Fields%5b20%5d[20]", "%", "%20") == "Grid:Data%20Fields[20][20]");

        CPPUNIT_ASSERT(www2id("Grid%20Data%26Fields[20][20]", "%", "%20") == "Grid%20Data&Fields[20][20]");

        CPPUNIT_ASSERT(www2id("Grid%20Data%26Fields[20][20]", "%", "%20%26") == "Grid%20Data%26Fields[20][20]");
        //cerr << "www2id(\"%25This%26is\"): " << www2id("%25This%26is") << endl;
        CPPUNIT_ASSERT(www2id("%25This%26is") == "%This&is");
        //cerr << "www2id(\"OPF_MaxSpectralPixelsMissing%d4\"): " << www2id("OPF_MaxSpectralPixelsMissing%d4") << endl;
        //CPPUNIT_ASSERT(www2id("OPF_MaxSpectralPixelsMissing%d4") == "OPF_MaxSpectralPixelsMissing?");
    }

    // This is the code in expr.lex that removes enclosing double quotes and
    // %20 sequences from a string. I copied this here because that actual
    // function uses globals and would be hard to test. 7/11/2001 jhrg
    string *store_str(const char *text)
    {
        string *s = new string(www2id(string(text)));

        if (*s->begin() == '\"' && *(s->end() - 1) == '\"') {
            s->erase(s->begin());
            s->erase(s->end() - 1);
        }

        return s;
    }

    // The MS VC++ compiler does not like escapes in string arguments passed
    // to macros. 04/23/03 jhrg
    void ce_string_parse_test()
    {
        string *str = new string("testing");
        string *str1 = new string("testing");
        string *result = store_str(str->c_str());
        CPPUNIT_ASSERT(*result == *str1);
        delete result;
        *str = "\"testing\"";
        *str1 = "testing";
        result = store_str(str->c_str());
        CPPUNIT_ASSERT(*result == *str1);
        delete result;
        *str = "\"test%20ing\"";
        *str1 = "test ing";
        result = store_str(str->c_str());
        CPPUNIT_ASSERT(*result == *str1);
        delete result;
        delete str;
        str = 0;
        delete str1;
        str1 = 0;
    }

    void escattr_test()
    {
        // The backslash escapes the double quote; in the returned string the
        // first two backslashes are a single escaped bs, the third bs
        // escapes the double quote.
        string str = "this_contains a double quote (\")";
        string str1 = "this_contains a double quote (\\\")";
        CPPUNIT_ASSERT(escattr(str) == str1);

        str = "this_contains a backslash (\\)";
        str1 = "this_contains a backslash (\\\\)";
        CPPUNIT_ASSERT(escattr(str) == str1);
    }

    void unescattr_test()
    {
        CPPUNIT_ASSERT(unescattr("attr") == "attr");

        CPPUNIT_ASSERT(unescattr("\\\\attr") == "\\attr");

        DBG(cerr << "XXX" << unescattr("\\\"attr") << "XXX" << endl);
        CPPUNIT_ASSERT(unescattr("\\\"attr") == "\"attr");

        char A_200_177[4] = { (char) 128, 127, 'A', '\0' };
        DBG(cerr << "XXX" << unescattr("\\200\\177A") << "XXX" << endl);
        CPPUNIT_ASSERT(unescattr("\\200\\177A") == string(A_200_177));

        DBG(cerr << "XXX" << unescattr("\\\\200\\\\177A") << "XXX" << endl);
        CPPUNIT_ASSERT(unescattr("\\\\200\\\\177A") == string(A_200_177));

        DBG(cerr << "XXX" << unescattr("\\\\200\\\\177AZ$&") << "XXX" << endl);

        DBG(cerr << "XXX" << unescattr("\\\\200") << "XXX" << endl);
    }

    void munge_error_message_test()
    {
        string str = "An Error";
        string str1 = "\"An Error\"";
        DBG(cerr << "Munge: " << munge_error_message(str) << endl);
        CPPUNIT_ASSERT(munge_error_message(str) == str1);

        str = "\"An Error\"";
        str1 = "\"An Error\"";
        DBG(cerr << "Munge: " << munge_error_message(str) << endl);
        CPPUNIT_ASSERT(munge_error_message(str) == str1);

        str = "An \"E\"rror";
        str1 = "\"An \\\"E\\\"rror\"";
        DBG(cerr << "Munge: " << munge_error_message(str) << endl);
        CPPUNIT_ASSERT(munge_error_message(str) == str1);

        str = "An \\\"E\\\"rror";
        str1 = "\"An \\\"E\\\"rror\"";
        DBG(cerr << "Munge: " << munge_error_message(str) << endl);
        CPPUNIT_ASSERT(munge_error_message(str) == str1);
    }

#if 0
    // Moved function to HTTPConect.
    void get_tempfile_template_test() {
#ifdef WIN32
        if (_putenv("TMPDIR=C:\\") == 0) {
            DBG(cerr << "TMPDIR: " << getenv("TMPDIR") << endl);
            CPPUNIT_ASSERT(strcmp(get_tempfile_template("DODSXXXXXX"),
                    "C:\\DODSXXXXXX") == 0);
        }
        else
        cerr << "Did not test setting TMPDIR; no test" << endl;
#else
        if (setenv("TMPDIR", "/tmp", 1) == 0) {
            DBG(cerr << "TMPDIR: " << getenv("TMPDIR") << endl);
            CPPUNIT_ASSERT(strcmp(get_tempfile_template("DODSXXXXXX"),
                    "/tmp/DODSXXXXXX") == 0);
        }
        else
        cerr << "Did not test setting TMPDIR; no test" << endl;
#endif

#if !defined(WIN32) && defined(P_tmpdir)
        string tmplt = P_tmpdir;
        tmplt.append("/"); tmplt.append("DODSXXXXXX");
        putenv("TMPDIR=");
        CPPUNIT_ASSERT(strcmp(get_tempfile_template("DODSXXXXXX"),
                tmplt.c_str()) == 0);
#endif
    }
#endif

    void id2xml_test()
    {
        CPPUNIT_ASSERT(id2xml("abcdef") == "abcdef");
        CPPUNIT_ASSERT(id2xml("abc<def") == "abc&lt;def");
        CPPUNIT_ASSERT(id2xml("abc>def") == "abc&gt;def");
        CPPUNIT_ASSERT(id2xml("abc&def") == "abc&amp;def");
        CPPUNIT_ASSERT(id2xml("abc'def") == "abc&apos;def");
        CPPUNIT_ASSERT(id2xml("abc\"def") == "abc&quot;def");
        CPPUNIT_ASSERT(id2xml("abc<<def") == "abc&lt;&lt;def");
        CPPUNIT_ASSERT(id2xml("abc>>def>") == "abc&gt;&gt;def&gt;");
        CPPUNIT_ASSERT(id2xml("abc&def&") == "abc&amp;def&amp;");
        CPPUNIT_ASSERT(id2xml("'abc'def") == "&apos;abc&apos;def");
        CPPUNIT_ASSERT(id2xml("\"abc\"def\"") == "&quot;abc&quot;def&quot;");
        // To get '\\' in a string both the backslashes must be escaped.
        DBG(cerr << id2xml("octal escape: \\\\012") << endl);
        CPPUNIT_ASSERT(id2xml("octal escape: \\\\012") == "octal escape: \\\\012");
    }

    void xml2id_test()
    {
        CPPUNIT_ASSERT(xml2id("abcdef") == "abcdef");
        CPPUNIT_ASSERT(xml2id("abc&lt;def") == "abc<def");
        CPPUNIT_ASSERT(xml2id("abc&gt;def") == "abc>def");
        CPPUNIT_ASSERT(xml2id("abc&amp;def") == "abc&def");
        CPPUNIT_ASSERT(xml2id("abc&apos;def") == "abc'def");
        CPPUNIT_ASSERT(xml2id("abc&quot;def") == "abc\"def");
        CPPUNIT_ASSERT(xml2id("abc&lt;&lt;def") == "abc<<def");
        CPPUNIT_ASSERT(xml2id("abc&gt;&gt;def&gt;") == "abc>>def>");
        CPPUNIT_ASSERT(xml2id("abc&amp;def&amp;") == "abc&def&");
        CPPUNIT_ASSERT(xml2id("&apos;abc&apos;def") == "'abc'def");
        CPPUNIT_ASSERT(xml2id("&quot;abc&quot;def&quot;") == "\"abc\"def\"");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (generalUtilTest);

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: generalUtilTest has the following tests:" << endl;
            const std::vector<Test*> &tests = generalUtilTest::suite()->getTests();
            unsigned int prefix_len = generalUtilTest::suite()->getName().append("::").length();
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
            test = generalUtilTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    xmlMemoryDump();

    return wasSuccessful ? 0 : 1;
}

