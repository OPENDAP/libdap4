
SUBDIRS = cache-testsuite

# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/tests
AM_CXXFLAGS = -I$(top_srcdir)/GNU

# These are not used by automake but are often useful for certain types of
# debugging. Set CXXFLAGS to this in the nightly build using export ...
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

check_PROGRAMS = $(UNIT_TESTS)

# This determines what gets run by 'make check.' The dejagnu_driver.sh 
# runs all the dejagnu test drivers by calling runtest 'by hand.'
TESTS = $(UNIT_TESTS)

DIRS_EXTRA = das-testsuite dds-testsuite ddx-testsuite ais_testsuite \
	rcreader-testsuite server-testsuite cgi-util-tests

EXTRA_DIST = $(DIRS_EXTRA) 

CLEANFILES = testout .dodsrc

############################################################################
# Unit Tests
#

UNIT_TESTS = arrayT attrTableT structT sequenceT ddsT RegexTest	\
	AISDatabaseParserTest AISMergeTest AISResourcesTest ArrayTest	\
	AttrTableTest ByteTest cgiUtilTest DASTest DDSTest		\
	DDXParserTest DODSFilterTest generalUtilTest HTTPCacheTest	\
	HTTPConnectTest parserUtilTest RCReaderTest SequenceTest	\
	SignalHandlerTest CEFunctionsTest


RegexTest_SOURCES = RegexTest.cc
RegexTest_LDADD = -lcppunit ../libdap.la

AISDatabaseParserTest_SOURCES = AISDatabaseParserTest.cc
AISDatabaseParserTest_LDADD = -lcppunit ../libdap.la ../libdapclient.la

AISMergeTest_SOURCES = AISMergeTest.cc
AISMergeTest_LDADD = -lcppunit ../libdap.la ../libdapclient.la

AISResourcesTest_SOURCES = AISResourcesTest.cc
AISResourcesTest_LDADD = -lcppunit ../libdap.la ../libdapclient.la

ArrayTest_SOURCES = ArrayTest.cc
ArrayTest_LDADD = -lcppunit ../libdap.la

AttrTableTest_SOURCES = AttrTableTest.cc
AttrTableTest_LDADD = -lcppunit ../libdap.la

ByteTest_SOURCES = ByteTest.cc
ByteTest_LDADD = -lcppunit ../libdap.la

cgiUtilTest_SOURCES = cgiUtilTest.cc
cgiUtilTest_LDADD = -lcppunit ../libdap.la ../libdapserver.la

DASTest_SOURCES = DASTest.cc
DASTest_LDADD = -lcppunit ../libdap.la

DDSTest_SOURCES = DDSTest.cc
DDSTest_LDADD = -lcppunit ../libdap.la

DDXParserTest_SOURCES = DDXParserTest.cc
DDXParserTest_LDADD = -lcppunit ../libdap.la

DODSFilterTest_SOURCES = DODSFilterTest.cc
DODSFilterTest_LDADD = -lcppunit ../libdap.la ../libdapserver.la

generalUtilTest_SOURCES = generalUtilTest.cc
generalUtilTest_LDADD = -lcppunit ../libdap.la

HTTPCacheTest_SOURCES = HTTPCacheTest.cc
HTTPCacheTest_LDADD = -lcppunit ../libdap.la ../libdapclient.la

HTTPConnectTest_SOURCES = HTTPConnectTest.cc
HTTPConnectTest_LDADD = -lcppunit ../libdap.la ../libdapclient.la

parserUtilTest_SOURCES = parserUtilTest.cc
parserUtilTest_LDADD = -lcppunit ../libdap.la

RCReaderTest_SOURCES = RCReaderTest.cc
RCReaderTest_LDADD = -lcppunit ../libdap.la ../libdapclient.la

SequenceTest_SOURCES = SequenceTest.cc
SequenceTest_LDADD = -lcppunit ../libdap.la ../tests/libtest-types.a

SignalHandlerTest_SOURCES = SignalHandlerTest.cc
SignalHandlerTest_LDADD = -lcppunit ../libdap.la

arrayT_SOURCES = arrayT.cc
arrayT_LDADD = -lcppunit ../libdap.la ../tests/libtest-types.a

attrTableT_SOURCES = attrTableT.cc
attrTableT_LDADD = -lcppunit ../libdap.la ../tests/libtest-types.a

structT_SOURCES = structT.cc
structT_LDADD = -lcppunit ../libdap.la ../tests/libtest-types.a

sequenceT_SOURCES = sequenceT.cc
sequenceT_LDADD = -lcppunit ../libdap.la ../tests/libtest-types.a

ddsT_SOURCES = ddsT.cc
ddsT_LDADD = -lcppunit ../libdap.la ../tests/libtest-types.a

CEFunctionsTest_SOURCES = CEFunctionsTest.cc
CEFunctionsTest_LDADD = -lcppunit ../libdap.la ../libdapserver.la

