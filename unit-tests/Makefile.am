
SUBDIRS = cache-testsuite

# Tests

AUTOMAKE_OPTIONS = foreign

# Headers in 'tests' are used by the arrayT unit tests.

AM_CPPFLAGS = -I$(top_srcdir)/GNU -I$(top_srcdir) -I$(top_srcdir)/tests $(CURL_CFLAGS) $(XML2_CFLAGS)
AM_LDADD =
if CPPUNIT
AM_CPPFLAGS += $(CPPUNIT_CFLAGS)
AM_LDADD += $(CPPUNIT_LIBS)
endif

# These are not used by automake but are often useful for certain types of
# debugging. Set CXXFLAGS to this in the nightly build using export ...
CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

# This determines what gets built by make check
check_PROGRAMS = $(UNIT_TESTS)

# This determines what gets run by 'make check.'
TESTS = $(UNIT_TESTS)

noinst_HEADERS = test_config.h

DIRS_EXTRA = das-testsuite dds-testsuite ddx-testsuite		\
	rcreader-testsuite server-testsuite cgi-util-tests D4-xml

EXTRA_DIST = $(DIRS_EXTRA) testFile.cc testFile.h test_config.h.in

CLEANFILES = testout .dodsrc  *.gcda *.gcno

DISTCLEANFILES = test_config.h *.strm *.file tmp.txt

test_config.h: test_config.h.in Makefile
	sed -e "s%[@]abs_srcdir[@]%${abs_srcdir}%" $< > test_config.h

############################################################################
# Unit Tests
#

if CPPUNIT
UNIT_TESTS = marshT arrayT attrTableT structT sequenceT ddsT dasT	\
	RegexTest                                                       \
	ArrayTest AttrTableTest ByteTest MIMEUtilTest ancT DASTest 	\
	DDSTest	DDXParserTest generalUtilTest 		\
	HTTPCacheTest HTTPConnectTest parserUtilTest RCReaderTest 	\
	SequenceTest SignalHandlerTest  MarshallerTest                  \
	ServerFunctionsListUnitTest 
	
# ResponseBuilderTest ResponseCacheTest Keywords2Test DODSFilterTest 
	
if DAP4_DEFINED
    UNIT_TESTS += DAP4MarshallerTest DAP4UnMarshallerTest \
    D4DimensionsTest D4EnumDefsTest
endif

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

TEST_SRC = testFile.cc testFile.h

RegexTest_SOURCES = RegexTest.cc
RegexTest_LDADD = ../libdap.la $(AM_LDADD)

ArrayTest_SOURCES = ArrayTest.cc
ArrayTest_LDADD = ../libdap.la $(AM_LDADD)

AttrTableTest_SOURCES = AttrTableTest.cc $(TEST_SRC)
AttrTableTest_LDADD = ../libdap.la $(AM_LDADD)

ByteTest_SOURCES = ByteTest.cc $(TEST_SRC)
ByteTest_LDADD = ../libdap.la $(AM_LDADD)

MIMEUtilTest_SOURCES = MIMEUtilTest.cc
MIMEUtilTest_LDADD = ../libdap.la $(AM_LDADD)

ancT_SOURCES = ancT.cc
ancT_LDADD = ../libdapserver.la ../libdap.la $(AM_LDADD)

DASTest_SOURCES = DASTest.cc
DASTest_LDADD = ../libdap.la $(AM_LDADD)

DDSTest_SOURCES = DDSTest.cc $(TEST_SRC)
DDSTest_LDADD = ../libdap.la $(AM_LDADD)

DDXParserTest_SOURCES = DDXParserTest.cc
DDXParserTest_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)
DDXParserTest_LDADD = ../libdap.la $(AM_LDADD)

# DODSFilterTest_SOURCES = DODSFilterTest.cc $(TEST_SRC)
# DODSFilterTest_LDADD = ../libdapserver.la ../libdap.la \
#	../tests/libtest-types.a $(AM_LDADD)

# ResponseBuilderTest_SOURCES = ResponseBuilderTest.cc $(TEST_SRC)
# ResponseBuilderTest_LDADD = ../libdapserver.la ../libdap.la \
#	../tests/libtest-types.a $(AM_LDADD)

# Keywords2Test_SOURCES = Keywords2Test.cc
# Keywords2Test_LDADD = ../libdap.la $(AM_LDADD)

generalUtilTest_SOURCES = generalUtilTest.cc
generalUtilTest_LDADD = ../libdap.la $(AM_LDADD)

HTTPCacheTest_SOURCES = HTTPCacheTest.cc
HTTPCacheTest_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
HTTPCacheTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

HTTPConnectTest_SOURCES = HTTPConnectTest.cc
HTTPConnectTest_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
HTTPConnectTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

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

ddsT_SOURCES = ddsT.cc $(TEST_SRC)
ddsT_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

dasT_SOURCES = dasT.cc $(TEST_SRC)
dasT_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

ServerFunctionsListUnitTest_SOURCES = ServerFunctionsListUnitTest.cc
ServerFunctionsListUnitTest_LDADD = ../libdap.la $(AM_LDADD)

# ResponseCacheTest_SOURCES = ResponseCacheTest.cc
# ResponseCacheTest_LDADD = ../tests/libtest-types.a ../libdapserver.la ../libdap.la $(AM_LDADD)

if DAP4_DEFINED

DAP4MarshallerTest_SOURCES = DAP4MarshallerTest.cc
DAP4MarshallerTest_LDADD = ../libdap.la $(AM_LDADD)

DAP4UnMarshallerTest_SOURCES = DAP4UnMarshallerTest.cc
DAP4UnMarshallerTest_LDADD = ../libdap.la $(AM_LDADD)

D4DimensionsTest_SOURCES = D4DimensionsTest.cc $(TEST_SRC)
D4DimensionsTest_LDADD = ../libdap.la $(AM_LDADD)

D4EnumDefsTest_SOURCES = D4EnumDefsTest.cc $(TEST_SRC)
D4EnumDefsTest_LDADD = ../libdap.la $(AM_LDADD)

endif
