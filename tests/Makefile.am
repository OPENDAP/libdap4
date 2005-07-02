
# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -DDEFAULT_BASETYPE_FACTORY
AM_CXXFLAGS = -I../$(srcdir)/GNU

check_PROGRAMS = $(UNIT_TESTS)
TESTS = $(UNIT_TESTS)

############################################################################
# Unit Tests
#

UNIT_TESTS = arrayT attrTableT structT sequenceT ddsT ddsPT iterT	\
	AISDatabaseParserTest AISMergeTest AISResourcesTest ArrayTest	\
	AttrTableTest ByteTest cgiUtilTest DASTest DDSTest		\
	DDXParserTest DODSFilterTest generalUtilTest HTTPCacheTest	\
	HTTPConnectTest parserUtilTest RCReaderTest SequenceTest	\
	SignalHandlerTest

AISDatabaseParserTest_SOURCES = AISDatabaseParserTest.cc
AISDatabaseParserTest_LDADD = -lcppunit -L../ -ldap

AISMergeTest_SOURCES = AISMergeTest.cc
AISMergeTest_LDADD = -lcppunit -L../ -ldap

AISResourcesTest_SOURCES = AISResourcesTest.cc
AISResourcesTest_LDADD = -lcppunit -L../ -ldap

ArrayTest_SOURCES = ArrayTest.cc
ArrayTest_LDADD = -lcppunit -L../ -ldap

AttrTableTest_SOURCES = AttrTableTest.cc
AttrTableTest_LDADD = -lcppunit -L../ -ldap

ByteTest_SOURCES = ByteTest.cc
ByteTest_LDADD = -lcppunit -L../ -ldap

cgiUtilTest_SOURCES = cgiUtilTest.cc
cgiUtilTest_LDADD = -lcppunit -L../ -ldap

DASTest_SOURCES = DASTest.cc
DASTest_LDADD = -lcppunit -L../ -ldap

DDSTest_SOURCES = DDSTest.cc
DDSTest_LDADD = -lcppunit -L../ -ldap

DDXParserTest_SOURCES = DDXParserTest.cc
DDXParserTest_LDADD = -lcppunit -L../ -ldap

DODSFilterTest_SOURCES = DODSFilterTest.cc
DODSFilterTest_LDADD = -lcppunit -L../ -ldap

generalUtilTest_SOURCES = generalUtilTest.cc
generalUtilTest_LDADD = -lcppunit -L../ -ldap

HTTPCacheTest_SOURCES = HTTPCacheTest.cc
HTTPCacheTest_LDADD = -lcppunit -L../ -ldap

HTTPConnectTest_SOURCES = HTTPConnectTest.cc
HTTPConnectTest_LDADD = -lcppunit -L../ -ldap

parserUtilTest_SOURCES = parserUtilTest.cc
parserUtilTest_LDADD = -lcppunit -L../ -ldap

RCReaderTest_SOURCES = RCReaderTest.cc
RCReaderTest_LDADD = -lcppunit -L../ -ldap

SequenceTest_SOURCES = SequenceTest.cc
SequenceTest_LDADD = -lcppunit -L../ -ldap

SignalHandlerTest_SOURCES = SignalHandlerTest.cc
SignalHandlerTest_LDADD = -lcppunit -L../ -ldap

arrayT_SOURCES = arrayT.cc
arrayT_LDADD = -lcppunit -L../ -ldap -ltest-types

attrTableT_SOURCES = attrTableT.cc
attrTableT_LDADD = -lcppunit -L../ -ldap -ltest-types

structT_SOURCES = structT.cc
structT_LDADD = -lcppunit -L../ -ldap -ltest-types

sequenceT_SOURCES = sequenceT.cc
sequenceT_LDADD = -lcppunit -L../ -ldap -ltest-types

ddsT_SOURCES = ddsT.cc
ddsT_LDADD = -lcppunit -L../ -ldap -ltest-types

ddsPT_SOURCES = ddsPT.cc
ddsPT_LDADD = -lcppunit -L../ -ldap -ltest-types

iterT_SOURCES = iterT.cc LongIterAdapter.cc
iterT_LDADD = -lcppunit -L../ -ldap -ltest-types
