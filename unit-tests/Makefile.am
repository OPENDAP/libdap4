
SUBDIRS = cache-testsuite

# Tests

AUTOMAKE_OPTIONS = foreign

# Headers in 'tests' are used by the arrayT unit tests.

AM_CPPFLAGS = -I$(top_srcdir)/GNU -I$(top_srcdir) -I$(top_srcdir)/tests
AM_LDADD =
if CPPUNIT
AM_CPPFLAGS += $(CPPUNIT_CFLAGS)
AM_LDADD += $(CPPUNIT_LIBS)
endif

# These are not used by automake but are often useful for certain types of
# debugging. Set CXXFLAGS to this in the nightly build using export ...
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

# This determines what gets built by make check
check_PROGRAMS = $(UNIT_TESTS)

# This determines what gets run by 'make check.'
TESTS = $(UNIT_TESTS)

DIRS_EXTRA = das-testsuite dds-testsuite ddx-testsuite ais_testsuite \
	rcreader-testsuite server-testsuite cgi-util-tests

EXTRA_DIST = $(DIRS_EXTRA) testFile.cc

CLEANFILES = testout .dodsrc  *.gcda *.gcno

############################################################################
# Unit Tests
#

if CPPUNIT
UNIT_TESTS = marshT arrayT attrTableT structT sequenceT ddsT RegexTest	\
	AISDatabaseParserTest AISMergeTest AISResourcesTest ArrayTest	\
	AttrTableTest ByteTest cgiUtilTest DASTest DDSTest		\
	DDXParserTest DODSFilterTest generalUtilTest HTTPCacheTest	\
	HTTPConnectTest parserUtilTest RCReaderTest SequenceTest	\
	SignalHandlerTest CEFunctionsTest GridGeoConstraintTest         \
	ArrayGeoConstraintTest
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

RegexTest_SOURCES = RegexTest.cc
RegexTest_LDADD = ../libdap.la $(AM_LDADD)

AISDatabaseParserTest_SOURCES = AISDatabaseParserTest.cc
AISDatabaseParserTest_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)
AISDatabaseParserTest_LDADD = ../libdap.la ../libdapclient.la $(AM_LDADD)

AISMergeTest_SOURCES = AISMergeTest.cc
AISMergeTest_LDADD = ../libdap.la ../libdapclient.la $(AM_LDADD)

AISResourcesTest_SOURCES = AISResourcesTest.cc
AISResourcesTest_LDADD = ../libdap.la ../libdapclient.la $(AM_LDADD)

ArrayTest_SOURCES = ArrayTest.cc
ArrayTest_LDADD = ../libdap.la $(AM_LDADD)

AttrTableTest_SOURCES = AttrTableTest.cc
AttrTableTest_LDADD = ../libdap.la $(AM_LDADD)

ByteTest_SOURCES = ByteTest.cc
ByteTest_LDADD = ../libdap.la $(AM_LDADD)

cgiUtilTest_SOURCES = cgiUtilTest.cc
cgiUtilTest_LDADD = ../libdap.la ../libdapserver.la $(AM_LDADD)

DASTest_SOURCES = DASTest.cc
DASTest_LDADD = ../libdap.la $(AM_LDADD)

DDSTest_SOURCES = DDSTest.cc
DDSTest_LDADD = ../libdap.la $(AM_LDADD)

DDXParserTest_SOURCES = DDXParserTest.cc
DDXParserTest_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)
DDXParserTest_LDADD = ../libdap.la $(AM_LDADD)

DODSFilterTest_SOURCES = DODSFilterTest.cc
DODSFilterTest_LDADD = ../libdap.la ../libdapserver.la $(AM_LDADD)

generalUtilTest_SOURCES = generalUtilTest.cc
generalUtilTest_LDADD = ../libdap.la $(AM_LDADD)

HTTPCacheTest_SOURCES = HTTPCacheTest.cc
HTTPCacheTest_LDADD = ../libdap.la ../libdapclient.la $(AM_LDADD)

HTTPConnectTest_SOURCES = HTTPConnectTest.cc
HTTPConnectTest_LDADD = ../libdap.la ../libdapclient.la $(AM_LDADD)

parserUtilTest_SOURCES = parserUtilTest.cc
parserUtilTest_LDADD = ../libdap.la $(AM_LDADD)

RCReaderTest_SOURCES = RCReaderTest.cc
RCReaderTest_LDADD = ../libdap.la ../libdapclient.la $(AM_LDADD)

SequenceTest_SOURCES = SequenceTest.cc
SequenceTest_LDADD = ../libdap.la ../tests/libtest-types.a $(AM_LDADD)

SignalHandlerTest_SOURCES = SignalHandlerTest.cc
SignalHandlerTest_LDADD = ../libdap.la $(AM_LDADD)

arrayT_SOURCES = arrayT.cc
arrayT_LDADD = ../libdap.la ../tests/libtest-types.a $(AM_LDADD)

marshT_SOURCES = marshT.cc
marshT_LDADD = ../libdap.la ../tests/libtest-types.a $(AM_LDADD)

attrTableT_SOURCES = attrTableT.cc
attrTableT_LDADD = ../libdap.la ../tests/libtest-types.a $(AM_LDADD)

structT_SOURCES = structT.cc
structT_LDADD = ../libdap.la ../tests/libtest-types.a $(AM_LDADD)

sequenceT_SOURCES = sequenceT.cc
sequenceT_LDADD = ../libdap.la ../tests/libtest-types.a $(AM_LDADD)

ddsT_SOURCES = ddsT.cc
ddsT_LDADD = ../libdap.la ../tests/libtest-types.a $(AM_LDADD)

CEFunctionsTest_SOURCES = CEFunctionsTest.cc
CEFunctionsTest_LDADD = ../libdap.la ../libdapserver.la ../tests/libtest-types.a $(AM_LDADD)

GridGeoConstraintTest_SOURCES = GridGeoConstraintTest.cc
GridGeoConstraintTest_LDADD = ../libdap.la ../libdapserver.la ../tests/libtest-types.a $(AM_LDADD)

ArrayGeoConstraintTest_SOURCES = ArrayGeoConstraintTest.cc
ArrayGeoConstraintTest_LDADD = ../libdap.la ../libdapserver.la ../tests/libtest-types.a $(AM_LDADD)

