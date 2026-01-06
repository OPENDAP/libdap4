//
// Created by James Gallagher on 1/5/26.
//

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <fstream>
#include <string>

#include <unistd.h>     // access, unlink
#include <sys/stat.h>   // stat

#include "TempFile.h"

// --- If TempFile is in the same translation unit for testing, remove this include and paste class above. ---

namespace {
bool file_exists(const std::string& path) {
    return !path.empty() && (::access(path.c_str(), F_OK) == 0);
}

std::string read_all(const std::string& path) {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    CPPUNIT_ASSERT_MESSAGE("Failed to open file for reading: " + path, in.is_open());

    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    return contents;
}
} // namespace

class TempFileTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TempFileTest);
    CPPUNIT_TEST(testCreatesAndPathExists);
    CPPUNIT_TEST(testWriteAndReadBack);
    CPPUNIT_TEST(testDestructorUnlinksFile);
    CPPUNIT_TEST(testReleaseKeepsFile);
    CPPUNIT_TEST(testMoveConstructorTransfersOwnership);
    CPPUNIT_TEST(testMoveAssignmentTransfersOwnershipAndCleansOld);
    CPPUNIT_TEST_SUITE_END();

public:
    void testCreatesAndPathExists() {
        TempFile tf("/tmp/tempfiletest-XXXXXX");
        CPPUNIT_ASSERT(!tf.path().empty());
        CPPUNIT_ASSERT(tf.is_open());
        CPPUNIT_ASSERT_MESSAGE("Temp file should exist on disk", file_exists(tf.path()));
    }

    void testWriteAndReadBack() {
        std::string path;
        {
            TempFile tf("/tmp/tempfiletest-XXXXXX", std::ios::out | std::ios::binary | std::ios::trunc);
            path = tf.path();

            tf.stream() << "hello";
            tf.stream() << "\n";
            tf.stream() << "world";
            tf.stream().flush();
            CPPUNIT_ASSERT(tf.stream().good());

            // Close early to ensure data is fully written before we read it.
            tf.close_stream();
            CPPUNIT_ASSERT_MESSAGE("File should still exist before destructor", file_exists(path));

            const std::string got = read_all(path);
            CPPUNIT_ASSERT_EQUAL(std::string("hello\nworld"), got);

            // TempFile destructor will unlink.
        }
        CPPUNIT_ASSERT_MESSAGE("Temp file should be unlinked after scope exit", !file_exists(path));
    }

    void testDestructorUnlinksFile() {
        std::string path;
        {
            TempFile tf("/tmp/tempfiletest-XXXXXX");
            path = tf.path();
            CPPUNIT_ASSERT(file_exists(path));
        }
        CPPUNIT_ASSERT_MESSAGE("Temp file should be unlinked after destruction", !file_exists(path));
    }

    void testReleaseKeepsFile() {
        std::string path;
        {
            TempFile tf("/tmp/tempfiletest-XXXXXX");
            path = tf.path();
            CPPUNIT_ASSERT(file_exists(path));

            tf.stream() << "persist";
            tf.stream().flush();

            // Prevent unlink-on-destroy.
            std::string released = tf.release();
            CPPUNIT_ASSERT_EQUAL(path, released);
        }

        CPPUNIT_ASSERT_MESSAGE("File should still exist after release()", file_exists(path));
        CPPUNIT_ASSERT_EQUAL(std::string("persist"), read_all(path));

        // Clean up to avoid leaving junk behind.
        ::unlink(path.c_str());
        CPPUNIT_ASSERT(!file_exists(path));
    }

    void testMoveConstructorTransfersOwnership() {
        std::string path;
        {
            TempFile a("/tmp/tempfiletest-XXXXXX");
            path = a.path();
            CPPUNIT_ASSERT(file_exists(path));

            a.stream() << "moved";
            a.stream().flush();

            TempFile b(std::move(a));
            CPPUNIT_ASSERT_EQUAL(path, b.path());
            CPPUNIT_ASSERT(b.is_open());

            // moved-from object should no longer own the path
            CPPUNIT_ASSERT(a.path().empty());

            // On scope exit: b should unlink, a should do nothing.
        }
        CPPUNIT_ASSERT_MESSAGE("File should be unlinked after moved-to object destruction", !file_exists(path));
    }

    void testMoveAssignmentTransfersOwnershipAndCleansOld() {
        std::string path1, path2;

        {
            TempFile a("/tmp/tempfiletest-XXXXXX");
            a.stream() << "first";
            a.stream().flush();
            path1 = a.path();
            CPPUNIT_ASSERT(file_exists(path1));

            TempFile b("/tmp/tempfiletest-XXXXXX");
            b.stream() << "second";
            b.stream().flush();
            path2 = b.path();
            CPPUNIT_ASSERT(file_exists(path2));

            // Move-assign b into a:
            // - a should clean up its current file (unlink path1)
            // - a should take ownership of b's file (path2)
            // - b should become empty/non-owning
            a = std::move(b);

            CPPUNIT_ASSERT_MESSAGE("Old file owned by a should be cleaned up during move assignment",
                                   !file_exists(path1));

            CPPUNIT_ASSERT_EQUAL(path2, a.path());
            CPPUNIT_ASSERT(b.path().empty());
            CPPUNIT_ASSERT(file_exists(path2));
        }

        // After leaving scope, a should unlink path2.
        CPPUNIT_ASSERT_MESSAGE("Final owned file should be unlinked at scope exit", !file_exists(path2));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TempFileTest);
