
# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -DDEFAULT_BASETYPE_FACTORY
AM_CXXFLAGS = -I../$(srcdir)/GNU

check_PROGRAMS = $(UNIT_TESTS) $(DEJAGNU_TESTS)

# This determines what gets run by 'make check.' The dejagnu_driver.sh 
# runs all the dejagnu test drivers by calling runtest 'by hand.'
TESTS = $(UNIT_TESTS) dejagnu_driver.sh

# Build the dejagnu test drivers as check programs; when automake fixes
# support for dejagnu so it can handle multiple drivers, add 'dejagnu to
# the automake options at the top of this file. See the section on dejagnu
# tests below. jhrg 7/6/05
check_SCRIPTS = dejagnu_driver.sh

DIRS_EXTRA = 	das-testsuite dds-testsuite expr-testsuite	\
	grid-func-testsuite cache-testsuite ais_testsuite	\
	rcreader-testsuite server-testsuite cgi-util-tests

EXTRA_DIST = $(check_SCRIPTS) $(DIRS_EXTRA) common-tests.exp 

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

das_test_SOURCES = das-test.cc
das_test_LDADD =  ../libdap.la libtest-types.a

dds_test_SOURCES = dds-test.cc
dds_test_LDADD = ../libdap.la libtest-types.a

expr_test_SOURCES = expr-test.cc
expr_test_LDADD = ../libdap.la libtest-types.a

TESTSRCS = TestByte.cc TestInt32.cc TestFloat64.cc TestStr.cc TestUrl.cc \
	   TestArray.cc TestStructure.cc TestSequence.cc		 \
	   TestGrid.cc TestUInt32.cc TestInt16.cc TestUInt16.cc		 \
	   TestFloat32.cc TestCommon.cc TestTypeFactory.cc

TEST_HDR = TestArray.h TestByte.h TestCommon.h TestFloat32.h TestFloat64.h    \
	TestGrid.h TestInt16.h TestInt32.h TestSequence.h TestStr.h	      \
	TestStructure.h TestTypeFactory.h TestUInt16.h TestUInt32.h TestUrl.h \
	config_dap.h

############################################################################
# Unit Tests
#

UNIT_TESTS = arrayT attrTableT structT sequenceT ddsT iterT	\
	AISDatabaseParserTest AISMergeTest AISResourcesTest ArrayTest	\
	AttrTableTest ByteTest cgiUtilTest DASTest DDSTest		\
	DDXParserTest DODSFilterTest generalUtilTest HTTPCacheTest	\
	HTTPConnectTest parserUtilTest RCReaderTest SequenceTest	\
	SignalHandlerTest

AISDatabaseParserTest_SOURCES = AISDatabaseParserTest.cc
AISDatabaseParserTest_LDADD = -lcppunit ../libdap.la

AISMergeTest_SOURCES = AISMergeTest.cc
AISMergeTest_LDADD = -lcppunit ../libdap.la

AISResourcesTest_SOURCES = AISResourcesTest.cc
AISResourcesTest_LDADD = -lcppunit ../libdap.la

ArrayTest_SOURCES = ArrayTest.cc
ArrayTest_LDADD = -lcppunit ../libdap.la

AttrTableTest_SOURCES = AttrTableTest.cc
AttrTableTest_LDADD = -lcppunit ../libdap.la

ByteTest_SOURCES = ByteTest.cc
ByteTest_LDADD = -lcppunit ../libdap.la

cgiUtilTest_SOURCES = cgiUtilTest.cc
cgiUtilTest_LDADD = -lcppunit ../libdap.la

DASTest_SOURCES = DASTest.cc
DASTest_LDADD = -lcppunit ../libdap.la

DDSTest_SOURCES = DDSTest.cc
DDSTest_LDADD = -lcppunit ../libdap.la

DDXParserTest_SOURCES = DDXParserTest.cc
DDXParserTest_LDADD = -lcppunit ../libdap.la

DODSFilterTest_SOURCES = DODSFilterTest.cc
DODSFilterTest_LDADD = -lcppunit ../libdap.la

generalUtilTest_SOURCES = generalUtilTest.cc
generalUtilTest_LDADD = -lcppunit ../libdap.la

HTTPCacheTest_SOURCES = HTTPCacheTest.cc
HTTPCacheTest_LDADD = -lcppunit ../libdap.la

HTTPConnectTest_SOURCES = HTTPConnectTest.cc
HTTPConnectTest_LDADD = -lcppunit ../libdap.la

parserUtilTest_SOURCES = parserUtilTest.cc
parserUtilTest_LDADD = -lcppunit ../libdap.la

RCReaderTest_SOURCES = RCReaderTest.cc
RCReaderTest_LDADD = -lcppunit ../libdap.la

SequenceTest_SOURCES = SequenceTest.cc
SequenceTest_LDADD = -lcppunit ../libdap.la

SignalHandlerTest_SOURCES = SignalHandlerTest.cc
SignalHandlerTest_LDADD = -lcppunit ../libdap.la

arrayT_SOURCES = arrayT.cc
arrayT_LDADD = -lcppunit ../libdap.la libtest-types.a

attrTableT_SOURCES = attrTableT.cc
attrTableT_LDADD = -lcppunit ../libdap.la libtest-types.a

structT_SOURCES = structT.cc
structT_LDADD = -lcppunit ../libdap.la libtest-types.a

sequenceT_SOURCES = sequenceT.cc
sequenceT_LDADD = -lcppunit ../libdap.la libtest-types.a

ddsT_SOURCES = ddsT.cc
ddsT_LDADD = -lcppunit ../libdap.la libtest-types.a

iterT_SOURCES = iterT.cc LongIterAdapter.cc
iterT_LDADD = -lcppunit ../libdap.la libtest-types.a
