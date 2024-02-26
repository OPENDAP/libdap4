
# Tests

AUTOMAKE_OPTIONS = foreign

# Headers in 'tests' are used by the arrayT unit tests.

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/d4_ce $(CURL_CFLAGS) $(XML2_CFLAGS) $(TIRPC_CFLAGS)
AM_LDADD = $(XML2_LIBS)
AM_CXXFLAGS = $(CXX_STD_FLAG)

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align -Wno-self-assign-overloaded
endif

if CPPUNIT
AM_CPPFLAGS += $(CPPUNIT_CFLAGS)
AM_LDADD += $(CPPUNIT_LIBS)
endif

CXXFLAGS_DEBUG = -g3 -O0

if USE_ASAN
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
endif

if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXXFLAGS_DEBUG) $(ASAN_FLAGS)
else
AM_CXXFLAGS += -g -O2
endif

AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

# This determines what gets built by make check
check_PROGRAMS = $(UNIT_TESTS)

# This determines what gets run by 'make check.'
TESTS = $(UNIT_TESTS)

noinst_HEADERS = test_config.h

DIRS_EXTRA =

EXTRA_DIST = $(DIRS_EXTRA) test_config.h.in

CLEANFILES = testout .dodsrc  *.gcda *.gcno *.gcov *.trs *.log *.file D4-xml.tar.gz *.output

DISTCLEANFILES = test_config.h *.strm *.file tmp.txt

test_config.h: test_config.h.in Makefile
	sed -e "s%[@]abs_srcdir[@]%${abs_srcdir}%" $< > test_config.h

############################################################################
# Unit Tests
#

if CPPUNIT
UNIT_TESTS = D4ConstraintEvaluatorTest
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

TEST_SRC = testFile.cc testFile.h
LDADD = ../../libdap.la $(AM_LDADD)
AM_LDFLAGS += -static

D4ConstraintEvaluatorTest_SOURCES = D4ConstraintEvaluatorTest.cc

# util_mitTest_LDADD = ../libdap.la ../libdapclient.la $(AM_LDADD)
