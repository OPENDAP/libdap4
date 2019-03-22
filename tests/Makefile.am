# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/GNU -I$(top_srcdir)/d4_ce \
-I$(top_srcdir)/d4_function $(XML2_CFLAGS) $(CURL_CFLAGS) $(TIRPC_CFLAGS)
AM_CXXFLAGS =  

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align

# If valgrind is present, add valgrind targets.
# @VALGRIND_CHECK_RULES@

if USE_ASAN
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
endif

if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXX11_FLAG) $(CXXFLAGS_DEBUG) $(ASAN_FLAGS)
else
AM_CXXFLAGS += -g -O2 $(CXX11_FLAG)
endif

check_PROGRAMS = das-test dds-test expr-test 

if DAP4_DEFINED
check_PROGRAMS += dmr-test
endif

# This will use automake's test envokation code. It's better to use
# our own so we can pass in flags for autotest, esp --jobs=N to get
# the autotest tests to run in parallel. See check-local: below.

# TESTS = DASTest DDSTest EXPRTest DMRTest getdapTest

check_SCRIPTS = $(DASTESTSUITE) $(DDSTESTSUITE) $(EXPRTESTSUITE) \
	$(DMRTESTSUITE) $(GETDAPTESTSUITE)

# Build the test drivers. The drivers all use the
# subclassed types in Test*.cc and the TestTypesFactory.

lib_LIBRARIES = libtest-types.a

libtest_types_a_SOURCES = $(TESTSRCS) $(TEST_HDR)
libtest_types_a_CXXFLAGS = $(AM_CXXFLAGS) -fPIC

testheadersdir = $(pkgincludedir)/test
testheaders_HEADERS = $(TEST_HDR)

das_test_SOURCES = das-test.cc
das_test_LDADD =  libtest-types.a ../libdapserver.la ../libdap.la 

dds_test_SOURCES = dds-test.cc
dds_test_LDADD = libtest-types.a ../libdapserver.la ../libdap.la 

expr_test_SOURCES = expr-test.cc ResponseBuilder.cc ResponseBuilder.h
expr_test_LDADD = libtest-types.a ../libdapserver.la ../libdapclient.la ../libdap.la

if DAP4_DEFINED
dmr_test_SOURCES = dmr-test.cc D4ResponseBuilder.cc D4ResponseBuilder.h
dmr_test_LDADD = libtest-types.a ../libdapserver.la ../libdap.la
endif

TESTSRCS = TestByte.cc TestInt32.cc TestFloat64.cc TestStr.cc TestUrl.cc \
	   TestArray.cc TestStructure.cc TestSequence.cc		 \
	   TestGrid.cc TestUInt32.cc TestInt16.cc TestUInt16.cc		 \
	   TestFloat32.cc TestTypeFactory.cc D4TestTypeFactory.cc \
	   TestD4Group.cc TestInt8.cc TestInt64.cc TestUInt64.cc TestD4Sequence.cc \
	   TestD4Enum.cc TestD4Opaque.cc TestFunction.cc D4TestFunction.cc

# TestCommon.cc Removed jhrg 3/12/15

TEST_HDR = TestArray.h TestByte.h TestCommon.h TestFloat32.h TestFloat64.h    \
	TestGrid.h TestInt16.h TestInt32.h TestSequence.h TestStr.h	      \
	TestStructure.h TestTypeFactory.h TestUInt16.h TestUInt32.h TestUrl.h \
	D4TestTypeFactory.h TestD4Group.h TestInt8.h TestInt64.h TestUInt64.h \
	TestD4Sequence.h TestD4Enum.h TestD4Opaque.h TestFunction.h D4TestFunction.h

DIRS_EXTRA = das-testsuite dds-testsuite expr-testsuite dmr-testsuite \
	getdap-testsuite

EXTRA_DIST = DASTest.at $(DASTESTSUITE) DDSTest.at $(DDSTESTSUITE) \
	EXPRTest.at $(EXPRTESTSUITE) DMRTest.at $(DMRTESTSUITE) \
	getdapTest.at $(GETDAPTESTSUITE) getdap-testsuite atlocal.in \
	$(srcdir)/package.m4 $(DIRS_EXTRA)

CLEANFILES = dmr-testsuite.tar.gz

DISTCLEANFILES = *.log

# DASTest.dir/* DDSTest.dir/* EXPRTest.dir/* DMRTest.dir/* getdapTest.dir/*

dmr-testsuite.tar.gz: dmr-testsuite/*.xml
	tar -czf $@ $^

############## Autotest follows #####################

AUTOM4TE = autom4te

TESTSUITEFLAGS =

DASTESTSUITE = $(srcdir)/DASTest

DDSTESTSUITE = $(srcdir)/DDSTest

EXPRTESTSUITE = $(srcdir)/EXPRTest

DMRTESTSUITE = $(srcdir)/DMRTest

GETDAPTESTSUITE = $(srcdir)/getdapTest

check-local: atconfig atlocal $(DASTESTSUITE) $(DDSTESTSUITE) \
$(EXPRTESTSUITE) $(DMRTESTSUITE) $(GETDAPTESTSUITE) 
	$(SHELL) '$(DASTESTSUITE)' $(TESTSUITEFLAGS)
	$(SHELL) '$(DDSTESTSUITE)' $(TESTSUITEFLAGS)
	$(SHELL) '$(EXPRTESTSUITE)' $(TESTSUITEFLAGS)
	$(SHELL) '$(DMRTESTSUITE)' $(TESTSUITEFLAGS)
	$(SHELL) '$(GETDAPTESTSUITE)' $(TESTSUITEFLAGS)

# Make sure there are no spaces after the DASTESTSUITE names. jhrg 3/16/15
# Delete the scripts here and not using CLEANFILES because this target is
# run after the 'regular' clean: and we need the test scripts to run their
# --clean option. Once that's done, then remove them. 
clean-local:
	test ! -f '$(DASTESTSUITE)' || $(SHELL) $(DASTESTSUITE) --clean
	test ! -f '$(DDSTESTSUITE)' || $(SHELL) $(DDSTESTSUITE) --clean
	test ! -f '$(EXPRTESTSUITE)' || $(SHELL) $(EXPRTESTSUITE) --clean
	test ! -f '$(DMRTESTSUITE)' || $(SHELL) $(DMRTESTSUITE) --clean
	test ! -f '$(GETDAPTESTSUITE)' || $(SHELL) $(GETDAPTESTSUITE) --clean

distclean-local:
	-rm atconfig

AUTOTEST = $(AUTOM4TE) --language=autotest

$(DASTESTSUITE): $(srcdir)/DASTest.at $(srcdir)/package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

$(DDSTESTSUITE): $(srcdir)/DDSTest.at $(srcdir)/package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

$(EXPRTESTSUITE): $(srcdir)/EXPRTest.at $(srcdir)/package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

$(DMRTESTSUITE): $(srcdir)/DMRTest.at $(srcdir)/package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

$(GETDAPTESTSUITE): $(srcdir)/getdapTest.at $(srcdir)/package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

# The `:;' works around a Bash 3.2 bug when the output is not writable.
$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	:;{ \
	echo '# Signature of the current package.' && \
	echo 'm4_define([AT_PACKAGE_NAME],      [@PACKAGE_NAME@])' && \
	echo 'm4_define([AT_PACKAGE_TARNAME],   [@PACKAGE_TARNAME@])' && \
	echo 'm4_define([AT_PACKAGE_VERSION],   [@PACKAGE_VERSION@])' && \
	echo 'm4_define([AT_PACKAGE_STRING],    [@PACKAGE_STRING@])' && \
	echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	} >'$(srcdir)/package.m4'

