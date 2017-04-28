
SUBDIRS = cache-testsuite

# Tests

AUTOMAKE_OPTIONS = foreign

# Headers in 'tests' are used by the arrayT unit tests.

AM_CPPFLAGS = -I$(top_srcdir)/GNU -I$(top_srcdir) -I$(top_srcdir)/tests $(CURL_CFLAGS) $(XML2_CFLAGS)
AM_LDADD = $(XML2_LIBS)
AM_CXXFLAGS = 
# CXXFLAGS =

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

if CPPUNIT
AM_CPPFLAGS += $(CPPUNIT_CFLAGS)
AM_LDADD += $(CPPUNIT_LIBS)
endif

CXXFLAGS_DEBUG = -g3 -O0 -Wall -Wcast-align

if USE_VALGRIND
TESTS_ENVIRONMENT=valgrind --quiet --trace-children=yes --error-exitcode=1 \
--leak-check=yes --dsymutil=yes --suppressions=valgrind_suppressions.txt

# skip using --log-file="valgrind-%p.log" because it always generates files
# Might also drop --dsymutil=yes because it can take a long time to run.
endif

if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXXFLAGS_DEBUG)
endif

# This determines what gets built by make check
check_PROGRAMS = $(UNIT_TESTS)

# This determines what gets run by 'make check.'
TESTS = $(UNIT_TESTS)

noinst_HEADERS = test_config.h

DIRS_EXTRA = das-testsuite dds-testsuite ddx-testsuite \
rcreader-testsuite server-testsuite cgi-util-tests D4-xml \
chunked-io D4-marshaller D4-type-testsuite

EXTRA_DIST = $(DIRS_EXTRA) testFile.cc testFile.h test_config.h.in \
valgrind_suppressions.txt

CLEANFILES = testout .dodsrc  *.gcda *.gcno *.gcov *.trs *.log *.file D4-xml.tar.gz

DISTCLEANFILES = test_config.h *.strm *.file tmp.txt

test_config.h: test_config.h.in Makefile
	sed -e "s%[@]abs_srcdir[@]%${abs_srcdir}%" $< > test_config.h

D4-xml.tar.gz: D4-xml/DMR_*[0-9].xml
	tar -czf $@ $^

############################################################################
# Unit Tests
#

if CPPUNIT
UNIT_TESTS = marshT arrayT attrTableT structT sequenceT ddsT dasT \
	RegexTest ArrayTest AttrTableTest ByteTest MIMEUtilTest ancT DASTest \
	DDSTest	DDXParserTest  generalUtilTest HTTPConnectTest parserUtilTest \
	RCReaderTest SequenceTest SignalHandlerTest  MarshallerTest \
	HTTPCacheTest ServerFunctionsListUnitTest

if DAP4_DEFINED
UNIT_TESTS += D4MarshallerTest D4UnMarshallerTest D4DimensionsTest \
	D4EnumDefsTest D4GroupTest D4ParserSax2Test D4AttributesTest D4EnumTest \
	chunked_iostream_test D4AsyncDocTest DMRTest D4FilterClauseTest \
	D4SequenceTest DmrRoundTripTest DmrToDap2Test
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

D4MarshallerTest_SOURCES = D4MarshallerTest.cc
D4MarshallerTest_LDADD = ../libdap.la $(AM_LDADD)

D4UnMarshallerTest_SOURCES = D4UnMarshallerTest.cc
D4UnMarshallerTest_LDADD = ../libdap.la $(AM_LDADD)

D4DimensionsTest_SOURCES = D4DimensionsTest.cc $(TEST_SRC)
D4DimensionsTest_LDADD = ../libdap.la $(AM_LDADD)

D4EnumDefsTest_SOURCES = D4EnumDefsTest.cc $(TEST_SRC)
D4EnumDefsTest_LDADD = ../libdap.la $(AM_LDADD)

D4GroupTest_SOURCES = D4GroupTest.cc $(TEST_SRC)
D4GroupTest_LDADD = ../libdap.la $(AM_LDADD)

D4ParserSax2Test_SOURCES = D4ParserSax2Test.cc $(TEST_SRC)
D4ParserSax2Test_LDADD = ../libdap.la $(AM_LDADD)

D4AttributesTest_SOURCES = D4AttributesTest.cc $(TEST_SRC)
D4AttributesTest_LDADD = ../libdap.la $(AM_LDADD)

D4EnumTest_SOURCES = D4EnumTest.cc $(TEST_SRC)
D4EnumTest_LDADD = ../libdap.la $(AM_LDADD)

chunked_iostream_test_SOURCES = chunked_iostream_test.cc $(TEST_SRC)
chunked_iostream_test_LDADD = ../libdap.la $(AM_LDADD)

D4AsyncDocTest_SOURCES = D4AsyncDocTest.cc $(TEST_SRC)
D4AsyncDocTest_LDADD = ../libdap.la $(AM_LDADD)

DMRTest_SOURCES = DMRTest.cc $(TEST_SRC)
DMRTest_LDADD = ../libdap.la $(AM_LDADD)

DmrRoundTripTest_SOURCES = DmrRoundTripTest.cc $(TEST_SRC)
DmrRoundTripTest_LDADD = ../libdap.la $(AM_LDADD)

DmrToDap2Test_SOURCES = DmrToDap2Test.cc $(TEST_SRC)
DmrToDap2Test_LDADD = ../libdap.la $(AM_LDADD)

D4FilterClauseTest_SOURCES = D4FilterClauseTest.cc
D4FilterClauseTest_LDADD = ../libdap.la $(AM_LDADD)

D4SequenceTest_SOURCES = D4SequenceTest.cc $(TEST_SRC)
D4SequenceTest_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

endif
