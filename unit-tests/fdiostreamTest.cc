// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2009 OPeNDAP, Inc.
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

//#define DODS_DEBUG

#include <fcntl.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>

#include "fdiostream.h"
#include "test_config.h"
#include "debug.h"

using namespace std;
using namespace CppUnit;
using namespace libdap;

class fdiostreamTest: public TestFixture {
private:
    string fdiostream_txt;
    string ff_test1_data;
public:
    fdiostreamTest()
    {
        //data = "Output from fdiostream";
    }
    ~fdiostreamTest()
    {
    }

    void setUp()
    {
        fdiostream_txt = (string) TEST_SRC_DIR + "/fdiostream.txt";
        ff_test1_data = (string) TEST_SRC_DIR + "/server-testsuite/ff_test1_ce1.data";
    }

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE (fdiostreamTest);

    CPPUNIT_TEST (write_file);
    CPPUNIT_TEST (read_test);
    CPPUNIT_TEST (readsome_test);
    CPPUNIT_TEST (readsome_test2);
    CPPUNIT_TEST (read_strings);
#if 1
    CPPUNIT_TEST (read_test_file_ptr);
    CPPUNIT_TEST (read_test_file_ptr_2);
    CPPUNIT_TEST (read_test_unget_file_descriptor);
    CPPUNIT_TEST (read_test_unget_file_ptr);
    CPPUNIT_TEST (large_read_test);
    CPPUNIT_TEST (large_read_test_file_pointer);
#endif
    CPPUNIT_TEST_SUITE_END();

    void write_file()
    {
        int fd = open("tmp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
        CPPUNIT_ASSERT("write_file open tmp.txt" && fd != -1);
        fdostream out(fd);
        out << "Output from fdiostream";
        out.flush();
        close(fd);

        // now read and compare to the baseline

        ifstream ifs("tmp.txt");

        char buf[1024];
        int num = ifs.readsome(buf, 1023);
        buf[num] = '\0';
        DBG(cerr << "buf: " << buf << endl);

        CPPUNIT_ASSERT(string(buf) == "Output from fdiostream");
    }

    void read_test()
    {
        int fd = open(fdiostream_txt.c_str(), O_RDONLY);
        CPPUNIT_ASSERT("read_test open fdiostream.txt" && fd != -1);
        fdistream in(fd, true);

        char buf[1024];
        in.read(buf, 1023);
        streamsize num = in.gcount();
        DBG(cerr << "num: " << num << endl);
        buf[num] = '\0';
        DBG(cerr << "buf: " << buf << endl);

        CPPUNIT_ASSERT(string(buf) == "Output from fdiostream");
    }

    // readsome() is designed to work with terminals and the like. It returns
    // only characters in the stream's buffer and does not initiate a read
    // from the underlying input source. So, unless some other call is used
    // to fill the buffer, readsome() returns zero. See the next test for
    // more info.
    void readsome_test()
    {
        int fd = open(fdiostream_txt.c_str(), O_RDONLY);
        CPPUNIT_ASSERT("readsome_test open fdiostream.txt" && fd != -1);
        fdistream in(fd, true);

        char buf[1024];
        int num = in.readsome(buf, 1023);
        DBG(cerr << "num: " << num << endl);
        buf[num] = '\0';
        DBG(cerr << "buf: " << buf << endl);

        CPPUNIT_ASSERT(num == 0);
    }

    // In this test, get() is used to get one character from the input source
    // and readsome() is used to read teh remaining chars. This works inpart
    // because the input buffer is bigger than the input, so the initial call
    // to get() triggers a read in underflow that grabs all the characters.
    // Given that the stream's input buffer has them all, readsome() returns
    // them.
    void readsome_test2()
    {
        int fd = open(fdiostream_txt.c_str(), O_RDONLY);
        CPPUNIT_ASSERT("readsome_test2 open fdiostream.txt" && fd != -1);
        fdistream in(fd, true);

        char buf[1024];
        buf[0] = in.get();
        int num = in.readsome(buf + 1, 1023);
        DBG(cerr << "num: " << num << endl);
        buf[num + 1] = '\0';
        DBG(cerr << "buf: " << buf << endl);

        CPPUNIT_ASSERT(string(buf) == "Output from fdiostream");
    }

    void read_strings()
    {
        int fd = open(fdiostream_txt.c_str(), O_RDONLY);
        CPPUNIT_ASSERT("read_strings open fdiostream.txt" && fd != -1);
        fdistream in(fd, true);

        // Strings read as space-separated tokens
        string test;
        in >> test;
        DBG(cerr << "test: " << test << endl);
        CPPUNIT_ASSERT(test == "Output");

        in >> test;
        DBG(cerr << "test: " << test << endl);
        CPPUNIT_ASSERT(test == "from");

        in >> test;
        DBG(cerr << "test: " << test << endl);
        CPPUNIT_ASSERT(test == "fdiostream");
    }

    // first read from the FILE * and then read the remaining using fdistream
    // This is sort of a degenerate case; it's much better to use fpistream
    // than to hack fdistream, but I'm including this test just a reminder of
    // how the file descriptor can be reset to counter the buffering done by
    // FILE pointers.
    void read_test_file_ptr()
    {
        FILE *fp = fopen(fdiostream_txt.c_str(), "r");
        CPPUNIT_ASSERT("read_test_file_ptr fopen fdiostream.txt" && fp != NULL);
        char word[7];
        int num = fread(&word[0], 1, 6, fp);
        word[6] = '\0';
        DBG(cerr << "first word (" << num << "): " << word << endl);
        CPPUNIT_ASSERT(string(word) == "Output");

        int pos = ftell(fp);
        int fd = fileno(fp);
        if (lseek(fd, pos, SEEK_SET) < 0) DBG(cerr << "lseek error: Could not seek to " << pos << endl);

        fdistream in(fd, true);
        char buf[1024];
        in.read(buf, 1023);
        streamsize size = in.gcount();
        DBG(cerr << "size: " << size << endl);
        buf[size] = '\0';
        DBG(cerr << "buf: " << buf << endl);

        CPPUNIT_ASSERT(string(buf) == " from fdiostream");
    }

    // now test FILE*s using fpistream (not fd...)
    void read_test_file_ptr_2()
    {
        FILE *fp = fopen(fdiostream_txt.c_str(), "r");
        CPPUNIT_ASSERT("read_test_file_ptr_2 fopen fdiostream.txt" && fp != NULL);
        char word[7];
        int num = fread(&word[0], 1, 6, fp);
        word[6] = '\0';
        DBG(cerr << "first word (" << num << "): " << word << endl);
        CPPUNIT_ASSERT(string(word) == "Output");

        fpistream in(fp, true);

        char buf[1024];
        in.read(buf, 1023);
        streamsize size = in.gcount();
        DBG(cerr << "size: " << size << endl);
        buf[size] = '\0';
        DBG(cerr << "buf: " << buf << endl);

        CPPUNIT_ASSERT(string(buf) == " from fdiostream");
    }

    void read_test_unget_file_descriptor()
    {
        int fd = open(fdiostream_txt.c_str(), O_RDONLY);
        CPPUNIT_ASSERT("read_test_unget_file_descriptor open fdiostream.txt" && fd != -1);
        fdistream in(fd, true);

        string test;
        in >> test;
        CPPUNIT_ASSERT(test == "Output");

        for (int i = 0; i < 6; ++i)
            in.unget();

        char buf[1024];
        in.read(buf, 1023);
        streamsize num = in.gcount();
        DBG(cerr << "num: " << num << endl);
        buf[num] = '\0';
        DBG(cerr << "buf: " << buf << endl);

        CPPUNIT_ASSERT(string(buf) == "Output from fdiostream");
    }

    void read_test_unget_file_ptr()
    {
        FILE *fp = fopen(fdiostream_txt.c_str(), "r");
        CPPUNIT_ASSERT("read_test_unget_file_ptr open fdiostream.txt" && fp != NULL);
        fpistream in(fp, true);

        string test;
        in >> test;
        CPPUNIT_ASSERT(test == "Output");

        for (int i = 0; i < 6; ++i)
            in.unget();

        char buf[1024];
        in.read(buf, 1023);
        streamsize size = in.gcount();
        DBG(cerr << "size: " << size << endl);
        buf[size] = '\0';
        DBG(cerr << "buf: " << buf << endl);

        CPPUNIT_ASSERT(string(buf) == "Output from fdiostream");
    }

    void large_read_test()
    {
        int fd = open(ff_test1_data.c_str(), O_RDONLY);
        CPPUNIT_ASSERT("large_read_test open server-testsuite/ff_test1_ce1.data" && fd != -1);
        fdistream in(fd, true);

        char buf[55000]; // actual size 54,351 characters/bytes
        in.read(buf, 55000);
        streamsize num = in.gcount();
        DBG(cerr << "num: " << num << endl);
        CPPUNIT_ASSERT(num == 54351);
        DBG(cerr << "buf: " << buf << endl);

        buf[9] = '\0';
        CPPUNIT_ASSERT(string(buf) == "The data:");

        buf[num - 2] = '\0'; // ends with CR NL
        DBG(cerr << "End of buf: '" << string(buf + num - 13) << "'" << endl);
        CPPUNIT_ASSERT(string(buf + num - 13) == "{ 1995 } };");
    }

    void large_read_test_file_pointer()
    {
        FILE *fp = fopen(ff_test1_data.c_str(), "r");
        CPPUNIT_ASSERT("large_read_test_file_pointer open server-testsuite/ff_test1_ce1.data" && fp != NULL);
        fpistream in(fp, true);

        char buf[55000]; // actual size 54,351 characters/bytes
        in.read(buf, 55000);
        streamsize num = in.gcount();
        DBG(cerr << "num: " << num << endl);
        CPPUNIT_ASSERT(num == 54351);
        DBG(cerr << "buf: " << buf << endl);

        buf[9] = '\0';
        CPPUNIT_ASSERT(string(buf) == "The data:");

        buf[num - 2] = '\0'; // ends with CR NL
        DBG(cerr << "End of buf: '" << string(buf + num - 13) << "'" << endl);
        CPPUNIT_ASSERT(string(buf + num - 13) == "{ 1995 } };");
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (fdiostreamTest);

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
            cerr << "Usage: fdiostreamTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::fdiostreamTest::suite()->getTests();
            unsigned int prefix_len = libdap::fdiostreamTest::suite()->getName().append("::").length();
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
            test = libdap::fdiostreamTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    xmlMemoryDump();

    return wasSuccessful ? 0 : 1;
}
