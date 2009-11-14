
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

noinst_HEADERS = test_config.h
# noinst_SCRIPTS = ais_testsuite/ais_database.xml

DIRS_EXTRA = das-testsuite dds-testsuite ddx-testsuite		\
	rcreader-testsuite server-testsuite cgi-util-tests	\
	ce-functions-testsuite

# Defining this instead of just adding the directory because now there are
# generated files in ais_testsuite. If just list the directory then it
# copies the generated file as well, which we don't want.

#### Remove

AIS_EXTRA = ais_testsuite/123.das ais_testsuite/ais_database.xml.in	\
	ais_testsuite/ais_error_1.xml ais_testsuite/ais_error_2.xml	\
	ais_testsuite/ais_error_3.xml ais_testsuite/ais_error_4.xml	\
	ais_testsuite/ais_error_5.xml ais_testsuite/ais_error_6.xml	\
	ais_testsuite/bears_1.nc.das ais_testsuite/coads.das		\
	ais_testsuite/fnoc1.nc.das ais_testsuite/fnoc2_replace.das	\
	ais_testsuite/fnoc3_fallback.das ais_testsuite/starts_with_number.das

####

EXTRA_DIST = $(DIRS_EXTRA) testFile.cc test_config.h.in

# $(AIS_EXTRA)

CLEANFILES = testout .dodsrc  *.gcda *.gcno

DISTCLEANFILES = test_config.h

# ais_testsuite/ais_database.xml

test_config.h: test_config.h.in Makefile
	sed -e "s%[@]abs_srcdir[@]%${abs_srcdir}%" $< > test_config.h

#### Remove

ais_testsuite/ais_database.xml: ais_testsuite/ais_database.xml.in ../config.status
	mkdir -p ais_testsuite
	chmod +w ais_testsuite
	sed -e "s%[@]abs_srcdir[@]%${abs_srcdir}%" $< > ais_testsuite/ais_database.xml

####

############################################################################
# Unit Tests
#

if CPPUNIT
#UNIT_TESTS = AttrTableTest
UNIT_TESTS = marshT arrayT attrTableT structT sequenceT ddsT dasT	\
	RegexTest                                                       \
	ArrayTest AttrTableTest ByteTest MIMEUtilTest ancT DASTest 	\
	DDSTest	DDXParserTest DODSFilterTest generalUtilTest 		\
	HTTPCacheTest HTTPConnectTest parserUtilTest RCReaderTest 	\
	SequenceTest SignalHandlerTest CEFunctionsTest 			\
	GridGeoConstraintTest ArrayGeoConstraintTest MarshallerTest     \
	fdiostreamTest \
	Dap4DatasetTest

#  AISDatabaseParserTest AISMergeTest AISResourcesTest	

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

# AISDatabaseParserTest_SOURCES = AISDatabaseParserTest.cc
# AISDatabaseParserTest_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)
# AISDatabaseParserTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

# AISMergeTest_SOURCES = AISMergeTest.cc
# AISMergeTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

# AISResourcesTest_SOURCES = AISResourcesTest.cc
# AISResourcesTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

ArrayTest_SOURCES = ArrayTest.cc
ArrayTest_LDADD = ../libdap.la $(AM_LDADD)

AttrTableTest_SOURCES = AttrTableTest.cc
AttrTableTest_LDADD = ../libdap.la $(AM_LDADD)

ByteTest_SOURCES = ByteTest.cc
ByteTest_LDADD = ../libdap.la $(AM_LDADD)

MIMEUtilTest_SOURCES = MIMEUtilTest.cc
MIMEUtilTest_LDADD = ../libdap.la $(AM_LDADD)
#  ../libdapserver.la

ancT_SOURCES = ancT.cc
ancT_LDADD = ../libdapserver.la ../libdap.la $(AM_LDADD)

DASTest_SOURCES = DASTest.cc
DASTest_LDADD = ../libdap.la $(AM_LDADD)

DDSTest_SOURCES = DDSTest.cc
DDSTest_LDADD = ../libdap.la $(AM_LDADD)

Dap4DatasetTest_SOURCES = Dap4DatasetTest.cc
Dap4DatasetTest_LDADD = ../libdap.la $(AM_LDADD)

DDXParserTest_SOURCES = DDXParserTest.cc
DDXParserTest_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)
DDXParserTest_LDADD = ../libdap.la $(AM_LDADD)
#  ../libdapserver.la 

DODSFilterTest_SOURCES = DODSFilterTest.cc
DODSFilterTest_LDADD = ../libdapserver.la ../libdap.la \
	../tests/libtest-types.a $(AM_LDADD)

generalUtilTest_SOURCES = generalUtilTest.cc
generalUtilTest_LDADD = ../libdap.la $(AM_LDADD)
# ../libdapserver.la 

HTTPCacheTest_SOURCES = HTTPCacheTest.cc
HTTPCacheTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)
#  ../libdapserver.la

HTTPConnectTest_SOURCES = HTTPConnectTest.cc
HTTPConnectTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)
#  ../libdapserver.la

parserUtilTest_SOURCES = parserUtilTest.cc
parserUtilTest_LDADD = ../libdap.la $(AM_LDADD)

RCReaderTest_SOURCES = RCReaderTest.cc
RCReaderTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

SequenceTest_SOURCES = SequenceTest.cc
SequenceTest_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

SignalHandlerTest_SOURCES = SignalHandlerTest.cc
SignalHandlerTest_LDADD = ../libdap.la $(AM_LDADD)

arrayT_SOURCES = arrayT.cc
arrayT_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

MarshallerTest_SOURCES = MarshallerTest.cc
MarshallerTest_LDADD = ../tests/libtest-types.a ../libdapclient.la ../libdap.la $(AM_LDADD)

marshT_SOURCES = marshT.cc
marshT_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

attrTableT_SOURCES = attrTableT.cc
attrTableT_LDADD = ../tests/libtest-types.a  ../libdap.la $(AM_LDADD)

structT_SOURCES = structT.cc
structT_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

sequenceT_SOURCES = sequenceT.cc
sequenceT_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

ddsT_SOURCES = ddsT.cc
ddsT_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

dasT_SOURCES = dasT.cc
dasT_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

CEFunctionsTest_SOURCES = CEFunctionsTest.cc
CEFunctionsTest_LDADD = ../tests/libtest-types.a  ../libdap.la $(AM_LDADD)
# ../libdapserver.la

GridGeoConstraintTest_SOURCES = GridGeoConstraintTest.cc
GridGeoConstraintTest_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)
# ../libdapserver.la 

ArrayGeoConstraintTest_SOURCES = ArrayGeoConstraintTest.cc
ArrayGeoConstraintTest_LDADD =  ../tests/libtest-types.a ../libdap.la $(AM_LDADD)
# ../libdapserver.la 

fdiostreamTest_SOURCES = fdiostreamTest.cc
fdiostreamTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)
