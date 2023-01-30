
SUBDIRS = cache-testsuite

# Tests

AUTOMAKE_OPTIONS = foreign

# Headers in 'tests' are used by the arrayT unit tests.

AM_CPPFLAGS = -I$(top_srcdir)/GNU -I$(top_srcdir) -I$(top_srcdir)/tests \
    $(CURL_CFLAGS) $(XML2_CFLAGS) $(TIRPC_CFLAGS)
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
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
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

noinst_HEADERS = test_config.h

DIRS_EXTRA = das-testsuite dds-testsuite ddx-testsuite \
rcreader-testsuite server-testsuite cgi-util-tests D4-xml \
chunked-io D4-marshaller D4-type-testsuite dmr-testsuite \
dmr-to-dap2-testsuite dmr-rt-testsuite

EXTRA_DIST = $(DIRS_EXTRA) testFile.cc testFile.h test_config.h.in \
run_tests_cppunit.h valgrind_suppressions.txt

CLEANFILES = testout .dodsrc  *.gcda *.gcno *.gcov *.trs *.log *.file D4-xml.tar.gz *.output

DISTCLEANFILES = test_config.h *.strm *.file tmp.txt

test_config.h: test_config.h.in Makefile
	sed -e "s%[@]abs_srcdir[@]%${abs_srcdir}%" $< > test_config.h

D4-xml.tar.gz: D4-xml/DMR_*[0-9].xml
	tar -czf $@ $^

# Override the default
# check: HTTPConnectTest
#	./HTTPConnectTest -d

############################################################################
# Unit Tests
#

if CPPUNIT
UNIT_TESTS = marshT arrayT attrTableT structT sequenceT ddsT dasT \
	RegexTest ArrayTest GridTest AttrTableTest ByteTest MIMEUtilTest ancT DASTest \
	DDSTest	DDXParserTest  generalUtilTest HTTPConnectTest parserUtilTest \
	RCReaderTest SequenceTest SignalHandlerTest  MarshallerTest \
	HTTPCacheTest ServerFunctionsListUnitTest Int8Test Int16Test UInt16Test \
	Int32Test UInt32Test Int64Test UInt64Test Float32Test Float64Test \
	D4BaseTypeFactoryTest BaseTypeFactoryTest util_mitTest

# Unit tests for DAP4-only code. jhrg 2/4/22
UNIT_TESTS += D4MarshallerTest D4UnMarshallerTest D4DimensionsTest \
	D4EnumDefsTest D4GroupTest D4ParserSax2Test D4AttributesTest D4EnumTest \
	chunked_iostream_test D4AsyncDocTest DMRTest D4FilterClauseTest \
	D4SequenceTest DmrRoundTripTest DmrToDap2Test IsDap4ProjectedTest

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

# If cppunit exists and big array test option are chosen, turn on BigArrayTest.
if CPPUNIT
if USE_BA
UNIT_TESTS +=BigArrayTest
endif
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
LDADD = ../libdap.la $(AM_LDADD)
AM_LDFLAGS += -static

RegexTest_SOURCES = RegexTest.cc

util_mitTest_SOURCES = util_mitTest.cc
util_mitTest_LDADD = ../libdap.la ../libdapclient.la $(AM_LDADD)

ArrayTest_SOURCES = ArrayTest.cc

GridTest_SOURCES = GridTest.cc

AttrTableTest_SOURCES = AttrTableTest.cc $(TEST_SRC)

ByteTest_SOURCES = ByteTest.cc $(TEST_SRC)

MIMEUtilTest_SOURCES = MIMEUtilTest.cc

ancT_SOURCES = ancT.cc
ancT_LDADD = ../libdap.la ../libdapserver.la $(AM_LDADD)

DASTest_SOURCES = DASTest.cc

DDSTest_SOURCES = DDSTest.cc $(TEST_SRC)

Int8Test_SOURCES = Int8Test.cc $(TEST_SRC)

Int16Test_SOURCES = Int16Test.cc $(TEST_SRC)

UInt16Test_SOURCES = UInt16Test.cc $(TEST_SRC)

Int32Test_SOURCES = Int32Test.cc $(TEST_SRC)

UInt32Test_SOURCES = UInt32Test.cc $(TEST_SRC)

Int64Test_SOURCES = Int64Test.cc $(TEST_SRC)

UInt64Test_SOURCES = UInt64Test.cc $(TEST_SRC)

Float32Test_SOURCES = Float32Test.cc $(TEST_SRC)

Float64Test_SOURCES = Float64Test.cc $(TEST_SRC)

D4BaseTypeFactoryTest_SOURCES = D4BaseTypeFactoryTest.cc $(TEST_SRC)

BaseTypeFactoryTest_SOURCES = BaseTypeFactoryTest.cc $(TEST_SRC)

DDXParserTest_SOURCES = DDXParserTest.cc
DDXParserTest_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)

generalUtilTest_SOURCES = generalUtilTest.cc

HTTPCacheTest_SOURCES = HTTPCacheTest.cc
HTTPCacheTest_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
HTTPCacheTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

HTTPConnectTest_SOURCES = HTTPConnectTest.cc
HTTPConnectTest_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
HTTPConnectTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

parserUtilTest_SOURCES = parserUtilTest.cc

RCReaderTest_SOURCES = RCReaderTest.cc
RCReaderTest_LDADD = ../libdapclient.la ../libdap.la $(AM_LDADD)

SequenceTest_SOURCES = SequenceTest.cc
SequenceTest_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

SignalHandlerTest_SOURCES = SignalHandlerTest.cc

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

D4MarshallerTest_SOURCES = D4MarshallerTest.cc

D4UnMarshallerTest_SOURCES = D4UnMarshallerTest.cc

D4DimensionsTest_SOURCES = D4DimensionsTest.cc $(TEST_SRC)

D4EnumDefsTest_SOURCES = D4EnumDefsTest.cc $(TEST_SRC)

D4GroupTest_SOURCES = D4GroupTest.cc $(TEST_SRC)

D4ParserSax2Test_SOURCES = D4ParserSax2Test.cc $(TEST_SRC)

D4AttributesTest_SOURCES = D4AttributesTest.cc $(TEST_SRC)

D4EnumTest_SOURCES = D4EnumTest.cc $(TEST_SRC)

chunked_iostream_test_SOURCES = chunked_iostream_test.cc $(TEST_SRC)

D4AsyncDocTest_SOURCES = D4AsyncDocTest.cc $(TEST_SRC)

DMRTest_SOURCES = DMRTest.cc $(TEST_SRC)

DmrRoundTripTest_SOURCES = DmrRoundTripTest.cc $(TEST_SRC)

DmrToDap2Test_SOURCES = DmrToDap2Test.cc $(TEST_SRC)

D4FilterClauseTest_SOURCES = D4FilterClauseTest.cc

D4SequenceTest_SOURCES = D4SequenceTest.cc $(TEST_SRC)

BigArrayTest_SOURCES = BigArrayTest.cc $(TEST_SRC)

D4SequenceTest_LDADD = ../tests/libtest-types.a ../libdap.la $(AM_LDADD)

IsDap4ProjectedTest_SOURCES = IsDap4ProjectedTest.cc
