
# Build libdap++, libtest-types++.a, getdap and deflate

AUTOMAKE_OPTIONS = foreign check-news

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -DDEFAULT_BASETYPE_FACTORY
AM_CXXFLAGS = -I$(srcdir)/GNU
AM_YFLAGS = -d -v
AM_LFLAGS = -8

# These are not used by automak but are often useful for certain types of
# debugging. 
CXXFLAGS_DEBUG = -g3 -O0 -Wall -fno-defer-pop
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

SUBDIRS = . tests

lib_LTLIBRARIES = libdap.la 

bin_PROGRAMS = getdap

sbin_PROGRAMS = deflate usage

libdap_la_SOURCES = $(DAP_SRC) $(CLIENT_SRC) $(SERVER_SRC) $(GNU_SRC)

libdap_la_LDFLAGS = -version-info $(LIBDAP_VERSION)

pkginclude_HEADERS = $(DAP_HDR) $(CLIENT_HDR) $(SERVER_HDR) $(GNU_HDR)

getdap_SOURCES = getdap.cc
getdap_LDADD = -L. -ldap

deflate_SOURCES = deflate.c
usage_SOURCES = usage.cc
usage_LDADD = -L. -ldap

# This determines what gets run by 'make check.' The dejagnu_driver.sh 
# runs all the dejagnu test drivers by calling runtest 'by hand.'
TESTS = dejagnu_driver.sh

# Build the dejagnu test drivers as noinst programs; when automake fixes
# support for dejagnu so it can handle multiple drivers, switch to its 
# targets, et cetera. See the section on dejagnu tests below. jhrg 7/1/05
check_PROGRAMS = $(DEJAGNU_TESTS)
check_SCRIPTS = dejagnu_driver.sh

LEX_YACC_EXTRA = das.lex das.y dds.lex dds.y expr.lex expr.y gse.lex gse.y \
	Error.lex Error.y

DIRS_EXTRA = win32 regex-0.12 cache-testsuite cgi-util-tests ais_testsuite \
	das-testsuite dds-testsuite expr-testsuite grid-func-testsuite	   \
	rcreader-testsuite server-testsuite

EXTRA_DIST = ChangeLog COPYING README.AIS README.dodsrc COPYRIGHT_URI	\
	GNU/README common_tests.exp dods.rc doxy.conf doxy_private.conf	\
	libdap.spec $(LEX_YACC_EXTRA) $(DIRS_EXTRA) $(check_SCRIPTS)

#############################################################################
# Library sources
# 

if USE_GNU_REGEX_FUNCS
REGEX_SRC = regex-0.12/regex.c
else
REGEX_SRC =
endif

GNU_SRC = GNU/GetOpt.cc GNU/GNURegex.cc GNU/GNUerror.cc			\
	IteratorAdapter.cc ArrayIterAdapter.cc AttrIterAdapter.cc	\
	BTIterAdapter.cc ClauseIterAdapter.cc $(REGEX_SRC)

GNU_HDR = GNU/builtin.h GNU/Pix.h GNU/GetOpt.h GNU/GNURegex.h	\
	IteratorAdapter.h ArrayIterAdapter.h AttrIterAdapter.h	\
	BTIterAdapter.h ClauseIterAdapter.h

# Roughly divide the source into DAP, Client and Server groups. In the
# future, make three different libraries. jhrg 

GRAM_SRC = lex.das.cc das.tab.cc lex.dds.cc dds.tab.cc lex.expr.cc \
	expr.tab.cc lex.Error.cc Error.tab.cc lex.gse_.cc gse.tab.cc 

DAP_SRC = $(GRAM_SRC) AttrTable.cc DAS.cc parser-util.cc escaping.cc	\
	DDS.cc DataDDS.cc DDXParser.cc Clause.cc RValue.cc Error.cc	\
	InternalErr.cc BaseType.cc Byte.cc Int32.cc Float64.cc		\
	Str.cc Url.cc Vector.cc Array.cc Structure.cc Sequence.cc	\
	Grid.cc UInt32.cc util.cc xdrutil_ppc.c ce_functions.cc		\
	GSEClause.cc Int16.cc UInt16.cc Float32.cc Constructor.cc	\
	PassiveByte.cc PassiveInt32.cc PassiveFloat64.cc PassiveStr.cc	\
	PassiveUrl.cc PassiveUInt32.cc PassiveInt16.cc PassiveUInt16.cc	\
	PassiveFloat32.cc PassiveArray.cc PassiveStructure.cc		\
	util_mit.cc BaseTypeFactory.cc

CLIENT_SRC = RCReader.cc Connect.cc HTTPConnect.cc HTTPCache.cc		\
	AISResources.cc AISDatabaseParser.cc AISMerge.cc AISConnect.cc

SERVER_SRC = SignalHandler.cc OPeNDAPFile.cc OPeNDAPDir.cc cgi_util.cc	\
	DODSFilter.cc ResponseTooBigErr.cc  

DAP_HDR = escaping.h AttrTable.h DAS.h parser.h debug.h dods-limits.h	\
	ce_functions.h Operators.h EventHandler.h trace_new.h DDS.h	\
	DataDDS.h DDXParser.h DDXExceptions.h expr.h Clause.h RValue.h	\
	Error.h InternalErr.h BaseType.h Byte.h Int32.h GSEClause.h	\
	Constructor.h Float64.h Str.h Url.h Vector.h Array.h		\
	Structure.h Sequence.h Grid.h UInt32.h util.h Int16.h UInt16.h	\
	Float32.h dods-datatypes.h util_mit.h PassiveByte.h		\
	PassiveInt32.h PassiveFloat64.h PassiveStr.h PassiveUrl.h	\
	PassiveUInt32.h PassiveInt16.h PassiveUInt16.h			\
	PassiveFloat32.h PassiveArray.h PassiveStructure.h		\
	DODSResponseObject.h BaseTypeFactory.h

CLIENT_HDR = ResponseTooBigErr.h RCReader.h Connect.h HTTPConnect.h	    \
	ObjectType.h EncodingType.h HTTPCache.h HTTPCacheDisconnectedMode.h \
	Response.h HTTPResponse.h HTTPCacheResponse.h AISConnect.h	    \
	AISMerge.h AISExceptions.h AISDatabaseParser.h AISResources.h	    \
	Resource.h HTTPCacheInterruptHandler.h

SERVER_HDR = cgi_util.h DODSFilter.h SignalHandler.h OPeNDAPFile.h	\
	OPeNDAPDir.h SignalHandlerRegisteredErr.h AlarmHandler.h

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
das_test_LDADD = -L. -ldap -ltest-types

dds_test_SOURCES = dds-test.cc
dds_test_LDADD = -L. -ldap -ltest-types

expr_test_SOURCES = expr-test.cc
expr_test_LDADD = -L. -ldap -ltest-types

TESTSRCS = TestByte.cc TestInt32.cc TestFloat64.cc TestStr.cc TestUrl.cc \
	   TestArray.cc TestStructure.cc TestSequence.cc		 \
	   TestGrid.cc TestUInt32.cc TestInt16.cc TestUInt16.cc		 \
	   TestFloat32.cc TestCommon.cc TestTypeFactory.cc

TEST_HDR = TestArray.h TestByte.h TestCommon.h TestFloat32.h TestFloat64.h    \
	TestGrid.h TestInt16.h TestInt32.h TestSequence.h TestStr.h	      \
	TestStructure.h TestTypeFactory.h TestUInt16.h TestUInt32.h TestUrl.h

############################################################################
# Special rules for the grammars. I tried to use the automake grammar support
# but these grammars are so hacked that it was taking too much time. Maybe if
# each grammar was converted one by one... jhrg 6/22/05
# 

# Build the DAS scanner and parser

lex.das.cc: das.lex das.tab.cc das.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pdas $<
	mv lex.das.c lex.das.cc

das.tab.cc das.tab.h: das.y DAS.h
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p das $<
	mv das.tab.c das.tab.cc

# DDS

lex.dds.cc: dds.lex dds.tab.cc dds.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pdds $<
	mv lex.dds.c lex.dds.cc

dds.tab.cc dds.tab.h: dds.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p dds $<
	mv dds.tab.c dds.tab.cc

# CE 

lex.expr.cc: expr.lex expr.tab.cc expr.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pexpr $<
	mv lex.expr.c lex.expr.cc

expr.tab.cc expr.tab.h: expr.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p expr $<
	mv expr.tab.c expr.tab.cc

# Build the grid selection sub_expression scanner and parser

lex.gse_.cc: gse.lex gse.tab.cc gse.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pgse_ $<
	mv lex.gse_.c lex.gse_.cc

gse.tab.cc gse.tab.h: gse.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p gse_ $<
	mv gse.tab.c gse.tab.cc

# Errors

lex.Error.cc: Error.lex Error.tab.cc Error.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -PError $<
	mv lex.Error.c lex.Error.cc

Error.tab.cc Error.tab.h: Error.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p Error $<
	mv Error.tab.c Error.tab.cc

