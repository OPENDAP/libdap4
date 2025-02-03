
# Tests

.NOTPARALLEL:

SUBDIRS = cache-testsuite

AUTOMAKE_OPTIONS = foreign

# Headers in 'tests' are used by the arrayT unit tests.

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/GNU -I$(top_srcdir)/http_dap \
    $(CURL_CFLAGS) $(XML2_CFLAGS) $(TIRPC_CFLAGS)
AM_CXXFLAGS = $(CXX_STD_FLAG)

AM_LDADD = $(top_builddir)/libdapclient.la $(top_builddir)/libdap.la

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align -Wno-self-assign-overloaded
endif

if CPPUNIT
AM_CPPFLAGS += $(CPPUNIT_CFLAGS)
AM_LDADD += $(CPPUNIT_LIBS)
endif

CXXFLAGS_DEBUG = -g3 -O0

if USE_ASAN
# old ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
endif

if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXXFLAGS_DEBUG) $(ASAN_FLAGS)
else
AM_CXXFLAGS += -g -O2
endif

# This will run the tests using the OSX 'leaks' tool. However, running the tests
# may require linking all of the tests with -static. Using LOG_COMPILER requires
# automake 1.12+ For some reason, OSX's leaks tool does not work reliably with
# parallel make runs. jhrg 2/17/22

if USE_LEAKS
LOG_COMPILER = $(LEAKS)
# valgrind
AM_LOG_FLAGS = $(LEAKS_FLAGS)
# --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all
endif

AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

# This determines what gets built by make check
check_PROGRAMS = $(UNIT_TESTS)

# This determines what gets run by 'make check.'
TESTS = $(UNIT_TESTS)

noinst_HEADERS = test_config.h run_tests_cppunit.h remove_directory.h

DIRS_EXTRA =

EXTRA_DIST = $(DIRS_EXTRA) test_config.h.in

CLEANFILES = testout .dodsrc  *.gcda *.gcno *.gcov *.trs *.log *.file *.output

DISTCLEANFILES = test_config.h *.strm *.file tmp.txt

test_config.h: test_config.h.in Makefile
	sed -e "s%[@]abs_srcdir[@]%${abs_srcdir}%" \
	    -e "s%[@]abs_builddir[@]%${abs_builddir}%" $< > test_config.h

############################################################################
# Unit Tests
#

if CPPUNIT
UNIT_TESTS = HTTPThreadsConnectTest HTTPConnectTest HTTPCacheTest

else
UNIT_TESTS =

check-local:
	@echo ""
	@echo "**********************************************************"
	@echo "You must have cppunit 1.12.x or greater installed to run *"
	@echo "check target in unit-tests directory                     *"
	@echo "**********************************************************"
	@echo ""
endif

# making these link statically makes them easier to debug and to use
# with OSX's leaks tool. It also seems to force the linker to use the
# locally built copy of the library and not the installed copy.
# leaks example: leaks --atExit -- ./D4AttributesTest
# jhrg 2/17/22
#
# Started reworking this file to reduce the number of program-specific configurations.
# added -static for OSX 'leaks.' jhrg 2/18/22

TEST_SRC = remove_directory.cc remove_directory.h
AM_LDFLAGS += -static

HTTPCacheTest_SOURCES = HTTPCacheTest.cc $(TEST_SRC)
HTTPCacheTest_CPPFLAGS = $(AM_CPPFLAGS)
HTTPCacheTest_LDADD = $(AM_LDADD)

HTTPConnectTest_SOURCES = HTTPConnectTest.cc $(TEST_SRC)
HTTPConnectTest_CPPFLAGS = $(AM_CPPFLAGS)
HTTPConnectTest_LDADD = $(AM_LDADD)

HTTPThreadsConnectTest_SOURCES = HTTPThreadsConnectTest.cc $(TEST_SRC)
HTTPThreadsConnectTest_CPPFLAGS = $(AM_CPPFLAGS)
HTTPThreadsConnectTest_LDADD = $(AM_LDADD)
