// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <string>
#include <vector>

#include "DAPCache3.h"
#include "InternalErr.h"
#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace libdap;
using namespace std;

namespace {

bool file_exists(const string &path) { return !path.empty() && (::access(path.c_str(), F_OK) == 0); }

string make_temp_dir() {
    string tmpl = string(TEST_BUILD_DIR) + "/dapcache3test.XXXXXX";
    vector<char> buf(tmpl.begin(), tmpl.end());
    buf.push_back('\0');
    char *dir = ::mkdtemp(buf.data());
    if (!dir)
        throw InternalErr(__FILE__, __LINE__, string("mkdtemp failed: ") + ::strerror(errno));
    return string(dir);
}

void cleanup_cache_files(const string &dir, const string &prefix) {
    DIR *dip = ::opendir(dir.c_str());
    if (!dip)
        return;

    struct dirent *dit;
    while ((dit = ::readdir(dip)) != nullptr) {
        string name = dit->d_name;
        if (name == "." || name == "..")
            continue;
        if (!prefix.empty() && name.find(prefix) != 0)
            continue;
        ::unlink((dir + "/" + name).c_str());
    }

    ::closedir(dip);
}

} // namespace

class DAPCache3Test : public TestFixture {
    CPPUNIT_TEST_SUITE(DAPCache3Test);
    CPPUNIT_TEST(test_get_cache_file_name_mangle_and_prefix);
    CPPUNIT_TEST(test_create_and_read_lock_roundtrip);
    CPPUNIT_TEST_SUITE_END();

    string d_cache_dir;
    DAPCache3 *d_cache = nullptr;

public:
    void setUp() override {
        if (d_cache_dir.empty()) {
            d_cache_dir = make_temp_dir();
            d_cache = DAPCache3::get_instance(d_cache_dir, "tst", 1024 * 1024);
        }
    }

    void tearDown() override { cleanup_cache_files(d_cache_dir, "tst#"); }

    void test_get_cache_file_name_mangle_and_prefix() {
        string expected = d_cache_dir + "/tst#a#b#c";
        CPPUNIT_ASSERT_EQUAL(expected, d_cache->get_cache_file_name("/a/b/c.nc", true));

        string expected_raw = d_cache_dir + "/tst#raw_name";
        CPPUNIT_ASSERT_EQUAL(expected_raw, d_cache->get_cache_file_name("raw_name", false));
    }

    void test_create_and_read_lock_roundtrip() {
        string path = d_cache->get_cache_file_name("/data/test.bin", true);

        int fd = -1;
        CPPUNIT_ASSERT(d_cache->create_and_lock(path, fd));
        CPPUNIT_ASSERT(fd >= 0);

        const char payload[] = "abc";
        ssize_t written = ::write(fd, payload, sizeof(payload) - 1);
        CPPUNIT_ASSERT_EQUAL((ssize_t)(sizeof(payload) - 1), written);

        d_cache->exclusive_to_shared_lock(fd);
        d_cache->unlock_and_close(fd);

        int fd2 = -1;
        CPPUNIT_ASSERT(d_cache->get_read_lock(path, fd2));
        CPPUNIT_ASSERT(fd2 >= 0);

        char buf[4] = {0, 0, 0, 0};
        ssize_t read_bytes = ::read(fd2, buf, sizeof(payload) - 1);
        CPPUNIT_ASSERT_EQUAL((ssize_t)(sizeof(payload) - 1), read_bytes);

        d_cache->unlock_and_close(fd2);

        CPPUNIT_ASSERT(file_exists(path));
        CPPUNIT_ASSERT_EQUAL(string("abc"), string(buf, sizeof(payload) - 1));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DAPCache3Test);

int main(int argc, char *argv[]) { return run_tests<DAPCache3Test>(argc, argv) ? 0 : 1; }
