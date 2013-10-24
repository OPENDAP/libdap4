# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/GNU $(XML2_CFLAGS) $(CURL_CFLAGS)
AM_CXXFLAGS =  

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

# These are not used by automake but are often useful for certain types of
# debugging. 
CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

check_PROGRAMS = das-test dds-test expr-test 

TESTS = DASTest DDSTest EXPRTest

dist_check_SCRIPTS = DASTest DDSTest EXPRTest atconfig atlocal

# Build the test drivers. The drivers all use the
# subclassed types in Test*.cc and the TestTypesFactory.

#noinst_LIBRARIES = libtest-types.a
lib_LIBRARIES = libtest-types.a

libtest_types_a_SOURCES = $(TESTSRCS) $(TEST_HDR)

testheadersdir = $(pkgincludedir)/test
testheaders_HEADERS = $(TEST_HDR)

# io_test_SOURCES = io_test.cc
# io_test_LDADD = ../libdap.la

das_test_SOURCES = das-test.cc
das_test_LDADD =  libtest-types.a ../libdapserver.la ../libdap.la 

dds_test_SOURCES = dds-test.cc
dds_test_LDADD = libtest-types.a ../libdapserver.la ../libdap.la 

expr_test_SOURCES = expr-test.cc ResponseBuilder.cc ResponseBuilder.h
expr_test_LDADD = libtest-types.a ../libdapserver.la ../libdapclient.la ../libdap.la

TESTSRCS = TestByte.cc TestInt32.cc TestFloat64.cc TestStr.cc TestUrl.cc \
	   TestArray.cc TestStructure.cc TestSequence.cc		 \
	   TestGrid.cc TestUInt32.cc TestInt16.cc TestUInt16.cc		 \
	   TestFloat32.cc TestCommon.cc TestTypeFactory.cc  TestFunction.cc

TEST_HDR = TestArray.h TestByte.h TestCommon.h TestFloat32.h TestFloat64.h    \
	TestGrid.h TestInt16.h TestInt32.h TestSequence.h TestStr.h	      \
	TestStructure.h TestTypeFactory.h TestUInt16.h TestUInt32.h TestUrl.h \
	TestFunction.h

DIRS_EXTRA = das-testsuite dds-testsuite expr-testsuite 

EXTRA_DIST = DASTest.at  $(DASTESTSUITE) DDSTest.at  $(DDSTESTSUITE) \
	EXPRTest.at $(EXPRTESTSUITE) atlocal.in $(srcdir)/package.m4 \
	$(DIRS_EXTRA)

DISTCLEANFILES = *.log DASTest.dir/* DDSTest.dir/* EXPRTest.dir/*

############## Autotest follows #####################

AUTOM4TE = autom4te

DASTESTSUITE = $(srcdir)/DASTest 
DASTESTSUITEFLAGS =
DDSTESTSUITE = $(srcdir)/DDSTest 
DDSTESTSUITEFLAGS =
EXPRTESTSUITE = $(srcdir)/EXPRTest 
EXPRTESTSUITEFLAGS =

#check-local: check-das check-dds

#check-das: atconfig atlocal $(DASTESTSUITE)
#	$(SHELL) $(DASTESTSUITE) $(DASTESTSUITEFLAGS)

#check-dds: atconfig atlocal $(DDSTESTSUITE)
#	$(SHELL) $(DDSTESTSUITE) $(DDSTESTSUITEFLAGS)

clean-local:
	test ! -f '$(DASTESTSUITE)' || $(SHELL) $(DASTESTSUITE) --clean
	test ! -f '$(DDSTESTSUITE)' || $(SHELL) $(DDSTESTSUITE) --clean

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

