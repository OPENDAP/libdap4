
# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/GNU
AM_CXXFLAGS =  

# These are not used by automake but are often useful for certain types of
# debugging. 
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

check_PROGRAMS = $(DEJAGNU_TESTS)
# noinst_PROGRAMS = io_test

# This determines what gets run by 'make check.' The dejagnu_driver.sh 
# runs all the dejagnu test drivers by calling runtest 'by hand.'
TESTS = dejagnu_driver.sh

# Build the dejagnu test drivers as check programs; when automake fixes
# support for dejagnu so it can handle multiple drivers, add 'dejagnu' to
# the automake options at the top of this file. See the section on dejagnu
# tests below. jhrg 7/6/05
dist_check_SCRIPTS = dejagnu_driver.sh

DIRS_EXTRA = das-testsuite dds-testsuite expr-testsuite ddx-testsuite	     \
	grid-func-testsuite cache-testsuite ais_testsuite rcreader-testsuite \
	server-testsuite cgi-util-tests

EXTRA_DIST = $(DIRS_EXTRA) common_tests.exp

CLEANFILES = *.log *.sum

############################################################################
# These tests use dejagnu. The drivers (passed to --tool) all use the
# subclassed types in Test*.cc and the TestTypesFactory.

noinst_LIBRARIES = libtest-types.a

libtest_types_a_SOURCES = $(TESTSRCS) $(TEST_HDR)

# noinst_PROGRAMS above uses this variable.
DEJAGNU_TESTS = das-test dds-test expr-test 

# Use this when automake adds support for multiple dejagnu test drivers. jhrg
# 6/30/05. 
# DEJATOOL = $(DEJAGNU_TESTS)
# RUNTESTDEFAULTFLAGS = --tool $$tool --srcdir $${tool}suite

# io_test_SOURCES = io_test.cc
# io_test_LDADD = ../libdap.la

das_test_SOURCES = das-test.cc
das_test_LDADD =  libtest-types.a ../libdap.la 

dds_test_SOURCES = dds-test.cc
dds_test_LDADD = libtest-types.a ../libdap.la 

expr_test_SOURCES = expr-test.cc
expr_test_LDADD = libtest-types.a ../libdapserver.la ../libdap.la

TESTSRCS = TestByte.cc TestInt32.cc TestFloat64.cc TestStr.cc TestUrl.cc \
	   TestArray.cc TestStructure.cc TestSequence.cc		 \
	   TestGrid.cc TestUInt32.cc TestInt16.cc TestUInt16.cc		 \
	   TestFloat32.cc TestCommon.cc TestTypeFactory.cc

TEST_HDR = TestArray.h TestByte.h TestCommon.h TestFloat32.h TestFloat64.h    \
	TestGrid.h TestInt16.h TestInt32.h TestSequence.h TestStr.h	      \
	TestStructure.h TestTypeFactory.h TestUInt16.h TestUInt32.h TestUrl.h
