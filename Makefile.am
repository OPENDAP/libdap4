# Build libdap, libtest-types.a, getdap

AUTOMAKE_OPTIONS = foreign check-news
ACLOCAL_AMFLAGS = -I conf -I gl/m4

aclocaldir=$(datadir)/aclocal
pkgconfigdir=$(libdir)/pkgconfig

AM_CPPFLAGS = -I$(top_srcdir)/gl -I$(top_srcdir)/GNU $(XML2_CFLAGS)
AM_CXXFLAGS = 

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

# if USING_GRIDFIELDS
# AM_CPPFLAGS += $(GF_CFLAGS)
# GRIDFIELDS = ugridFunctions
# endif

AM_YFLAGS = -d -v -t
AM_LFLAGS = -8

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
#     export CXXFLAGS='...'; ./configure --disable-shared
# or ./configure --enable-developer --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align 
# -Werror There are some warnings in this code that are here to stay...
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXXFLAGS_DEBUG)
endif

SUBDIRS = gl . tests unit-tests
DIST_SUBDIRS = gl tests unit-tests

noinst_LTLIBRARIES = libparsers.la

lib_LTLIBRARIES = libdap.la libdapclient.la libdapserver.la

bin_SCRIPTS = dap-config dap-config-pkgconfig

bin_PROGRAMS = getdap

BUILT_SOURCES = $(GRAM_SRC) dods-datatypes.h xdr-datatypes.h getdap.1 dap-config.1

man_MANS = getdap.1 dap-config.1

%.1: %.man1
	groff -t -e -mandoc -Tascii $< | col -bx > $@

libparsers_la_SOURCES = $(GRAM_SRC) 
libparsers_la_CPPFLAGS = $(AM_CPPFLAGS)
libparsers_la_LIBADD = $(XML2_LIBS)

libdap_la_SOURCES = $(DAP_SRC) $(GNU_SRC)
if DAP4_DEFINED
    libdap_la_SOURCES += $(DAP4_ONLY_SRC)
endif

libdap_la_LDFLAGS = -version-info $(LIBDAP_VERSION)
libdap_la_CPPFLAGS = $(AM_CPPFLAGS)
libdap_la_LIBADD = $(XML2_LIBS) $(PTHREAD_LIBS) gl/libgnu.la libparsers.la
if DAP4_DEFINED
    libdap_la_LIBADD += -lcrypto 
endif

libdapclient_la_SOURCES = $(CLIENT_SRC) 
libdapclient_la_LDFLAGS = -version-info $(CLIENTLIB_VERSION)
libdapclient_la_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
libdapclient_la_LIBADD = $(CURL_LIBS) libdap.la $(PTHREAD_LIBS)

libdapserver_la_SOURCES = $(SERVER_SRC)
libdapserver_la_LDFLAGS = -version-info $(SERVERLIB_VERSION)
libdapserver_la_LIBADD = libdap.la $(UUID_LIBS)

pkginclude_HEADERS = $(DAP_HDR) $(GNU_HDR) $(CLIENT_HDR) $(SERVER_HDR) 
# TODO Until the compile-time issue with headers like BaseType.h depending on
# configure params is fixed, always install these headers.
if DAP4_DEFINED
    pkginclude_HEADERS += $(DAP4_ONLY_HDR)
endif

noinst_HEADERS = config_dap.h

getdap_SOURCES = getdap.cc
getdap_LDADD = libdapclient.la libdap.la
getdap_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)

LEX_YACC_EXTRA = das.lex das.yy dds.lex dds.yy ce_expr.lex ce_expr.yy \
Error.lex Error.yy

EXTRA_DIST = ChangeLog COPYING README.dodsrc COPYRIGHT_URI		\
	COPYRIGHT_W3C GNU/README doxy.conf doxy_private.conf		\
	libdap.spec VCPP dods-datatypes-static.h			\
	xdr-datatypes-static.h dap-config-pkgconfig $(LEX_YACC_EXTRA)	\
	$(GRAM_SRC) OSX_Resources getdap.man1 dap-config.man1

dist_aclocal_DATA = conf/libdap.m4
pkgconfig_DATA = libdap.pc libdapclient.pc libdapserver.pc

CLEANFILES = *.log *.output *.gcda *.gcno $(GRAM_SRC) dods-datatypes.h \
    xdr-datatypes.h getdap.1 dap-config.1

# Not nearly as clean as it could be, but this removes .svn directories
# in subdirs.
dist-hook:
	rm -rf `find $(distdir) -name .svn`

# Copy the generated grammar files to the 'grammarfiles' directory.
.PHONY: grammarfiles
grammarfiles: $(GRAM_SRC)
	for f in $(GRAM_SRC); do cp $$f grammarfiles; done

.PHONY: docs
docs:
	doxygen $(srcdir)/doxy.conf
	(cd docs && tar -czf html.tar.gz html)

# cccc computes metrics like Lines of code and McCabe. It'a available
# on the web...
.PHONY: cccc
cccc:
	-mkdir cccc
	cccc --outdir=cccc $(DAP_SRC) $(SERVER_SRC) $(CLIENT_SRC) \
	$(DAP_HDR) $(SERVER_HDR) $(CLIENT_HDR)

###########################################################################

# Fortify targets.
.PHONY: fortify
fortify: $(BUILT_SOURCES)
	(cd gl && sourceanalyzer -b @PACKAGE@-gl $(MAKE) $(MFLAGS) )
	sourceanalyzer -b @PACKAGE@-parsers $(MAKE) $(MFLAGS) libparsers.la
	sourceanalyzer -b @PACKAGE@ $(MAKE) $(MFLAGS) libdap.la libdapserver.la libdapclient.la
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr

# run fortify on only our code, not the gnulib code or the flex/bison code.
# This does skip testing some of the functions we wrote in the *.lex/*.yy
# files but should eliminate the false positives from flex and bison, too.
.PHONY: fortify-lite
fortify-lite: dods-datatypes.h xdr-datatypes.h
	( cd gl && $(MAKE) )
	$(MAKE) libparsers.la
	sourceanalyzer -b @PACKAGE@ $(MAKE) libdap.la libdapserver.la libdapclient.la
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr

# No longer used? jhrg 1/29/13
#grammar_objects = lex.das.o das.tab.o lex.dds.o dds.tab.o lex.ce_expr.o \
#ce_expr.tab.o lex.gse_.o gse.tab.o lex.Error.o Error.tab.o

# Use this to clean the fortify project.
.PHONY: fortifyclean
fortifyclean:
	(cd gl && sourceanalyzer -b @PACKAGE@-gl -clean )
	sourceanalyzer -b @PACKAGE@ -clean

###########################################################################

# Build linux RPMs. Use the environment variable 'RPM_OPTIONS' to pass in
# extra options like --nodeps and --macros

srpm: dist
	rpmbuild -ts --clean $(RPM_OPTIONS) @PACKAGE@-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb --clean $(RPM_OPTIONS) @PACKAGE@-@PACKAGE_VERSION@.tar.gz

###########################################################################

# Build OS/X Packages. The strange operations with configure and make
# are there so that the values built into dap-config will match the mac
# osx install dirs and not the temp directory used to build the packages

PACKAGEMAKER=/Developer/usr/bin/packagemaker
PKG_CONF_FLAGS=

clean-pkg:
	-rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg

pkg-build: clean-pkg
	./configure --prefix=$(prefix) --disable-dependency-tracking $(PKG_CONF_FLAGS)
	make clean all
	DESTDIR=`pwd`/mac_osx make install

pkg-main: pkg-build
	./OSX_Resources/update_mac_package_contents.pl README
	cat ./OSX_Resources/Info.plist.proto | \
	   sed -e "s^_PREFIX_^/usr/local/servers/hyrax-1.8.8^g" \
	       -e "s^_FULL_VERSION_^@PACKAGE_VERSION@^g" \
	       -e "s^_MAJOR_VERSION_^@PACKAGE_MAJOR_VERSION@^g" \
	       -e "s^_MINOR_VERSION_^@PACKAGE_MINOR_VERSION@^g" \
	       > foo
	mv foo ./OSX_Resources/Info.plist
	${PACKAGEMAKER} --root mac_osx --id org.opendap.@PACKAGE@ \
	    --title "@PACKAGE@ @PACKAGE_VERSION@" --version @PACKAGE_VERSION@ \
	    --out @PACKAGE@-@PACKAGE_VERSION@.pkg --resources OSX_Resources

# 	   sed -e "s^_PREFIX_^$(prefix)^g" 

pkg-dmg: pkg-main
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@.dmg
	mkdir @PACKAGE@-@PACKAGE_VERSION@
	cp -r @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@
	cp README README.* NEWS @PACKAGE@-@PACKAGE_VERSION@
	dropdmg -i --sanitize-for-servers --format zlib @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@

pkg: pkg-main 

# Note that the gcov options -f and -b are useful but sometimes make looking
# at the results of coverage analysis a little taxing. -b reports on all
# branched and -f reports on all functions. The -l -o options summarize on a
# per-file basis. 3/27/98 jhrg
collect-coverage-data:
	(cd test-coverage; \
         cov_dat="coverage-data-`date +%m.%d.%y`"; \
         touch $$cov_dat; \
         for f in $(ALLSRCS); do \
            echo "\n*** Coverage data for $$f ***\n" >> $$cov_dat; \
            gcov -l -o ../ $$f >> $$cov_dat; \
         done)

#############################################################################
# Library sources
# 

GNU_SRC = GNU/GetOpt.cc GNU/GNURegex.cc

GNU_HDR = GNU/GetOpt.h GNU/GNURegex.h

GRAM_SRC = lex.das.cc das.tab.cc das.tab.hh lex.dds.cc dds.tab.cc	\
	dds.tab.hh lex.ce_expr.cc ce_expr.tab.cc ce_expr.tab.hh		\
	lex.Error.cc Error.tab.cc Error.tab.hh 

DAP_SRC = AttrTable.cc DAS.cc DDS.cc DataDDS.cc DDXParserSAX2.cc	\
	BaseType.cc Byte.cc Int32.cc Float64.cc Str.cc Url.cc		\
	Vector.cc Array.cc Structure.cc Sequence.cc Grid.cc UInt32.cc	\
	Int16.cc UInt16.cc Float32.cc Constructor.cc			\
	BaseTypeFactory.cc SignalHandler.cc Error.cc InternalErr.cc	\
	util.cc xdrutil_ppc.c parser-util.cc escaping.cc		\
	Clause.cc RValue.cc			\
	ConstraintEvaluator.cc DapIndent.cc	\
	Operators.h XDRUtils.cc XDRFileMarshaller.cc			\
	XDRStreamMarshaller.cc XDRFileUnMarshaller.cc			\
	XDRStreamUnMarshaller.cc mime_util.cc Keywords2.cc XMLWriter.cc \
	ServerFunctionsList.cc ServerFunction.cc

# DAPCache3.cc 

DAP4_ONLY_SRC = DAP4StreamMarshaller.cc DAP4StreamUnMarshaller.cc Int64.cc \
        UInt64.cc Int8.cc D4ParserSax2.cc D4BaseTypeFactory.cc \
        D4Maps.cc D4Dimensions.cc  D4EnumDef.cc D4Group.cc

Operators.h: ce_expr.tab.hh

# Operators.h is included in with the source to prevent it from bing installed
# with the other headers. It includes one of the built grammar file headers.

CLIENT_SRC = RCReader.cc Connect.cc HTTPConnect.cc HTTPCache.cc	\
	util_mit.cc ResponseTooBigErr.cc HTTPCacheTable.cc

SERVER_SRC = DODSFilter.cc Ancillary.cc
# ResponseBuilder.cc ResponseCache.cc

DAP_HDR = AttrTable.h DAS.h DDS.h DataDDS.h DDXParserSAX2.h		\
	DDXExceptions.h BaseType.h Byte.h Int32.h Float64.h Str.h	\
	Url.h Vector.h Array.h Constructor.h Structure.h Sequence.h	\
	Grid.h UInt32.h Int16.h UInt16.h Float32.h BaseTypeFactory.h	\
	ObjectType.h EncodingType.h SignalHandler.h Error.h		\
	InternalErr.h util.h escaping.h parser.h debug.h dods-limits.h	\
	dods-datatypes.h 		\
	util_mit.h expr.h Clause.h RValue.h ConstraintEvaluator.h	\
	ce_parser.h DapIndent.h DapObj.h XDRFileMarshaller.h		\
	Marshaller.h UnMarshaller.h XDRFileUnMarshaller.h		\
	XDRStreamMarshaller.h XDRUtils.h xdr-datatypes.h mime_util.h	\
	cgi_util.h XDRStreamUnMarshaller.h Keywords2.h XMLWriter.h \
	ServerFunctionsList.h ServerFunction.h

# DAPCache.h

DAP4_ONLY_HDR = DAP4StreamMarshaller.h DAP4StreamUnMarshaller.h Int64.h \
        UInt64.h Int8.h D4ParserSax2.h D4BaseTypeFactory.h D4ParseError.h \
        D4Maps.h D4Dimensions.h D4EnumDef.h D4Group.h

if USE_C99_TYPES
dods-datatypes.h: dods-datatypes-static.h
else
dods-datatypes.h: dods-datatypes-config.h
endif
	cp -p $< dods-datatypes.h

xdr-datatypes.h: xdr-datatypes-config.h
	cp -p $< xdr-datatypes.h

CLIENT_HDR = RCReader.h Connect.h HTTPConnect.h HTTPCache.h		\
	HTTPCacheDisconnectedMode.h HTTPCacheInterruptHandler.h		\
	Response.h HTTPResponse.h HTTPCacheResponse.h PipeResponse.h	\
	StdinResponse.h SignalHandlerRegisteredErr.h			\
	ResponseTooBigErr.h Resource.h HTTPCacheTable.h HTTPCacheMacros.h

SERVER_HDR = DODSFilter.h AlarmHandler.h EventHandler.h Ancillary.h
#	ResponseBuilder.h ResponseCache.h

############################################################################
# Special rules for the grammars. I tried to use the automake grammar support
# but these grammars are so hacked that it was taking too much time. Maybe if
# each grammar was converted one by one... jhrg 6/22/05
# 
# I switched to using flex options instead of sed and mv. jhrg 02/28/08
#
# Build the DAS scanner and parser

lex.das.cc: das.lex das.tab.cc das.tab.hh
	$(LEX) $(LFLAGS) $<

das.tab.cc das.tab.hh: das.yy DAS.h
	$(YACC) $(YFLAGS) $<

# DDS

lex.dds.cc: dds.lex dds.tab.cc dds.tab.hh
	$(LEX) $(LFLAGS) $<

dds.tab.cc dds.tab.hh: dds.yy
	$(YACC) $(YFLAGS) $<

# CE

lex.ce_expr.cc: ce_expr.lex ce_expr.tab.cc ce_expr.tab.hh
	$(LEX) $(LFLAGS) $<

ce_expr.tab.cc ce_expr.tab.hh: ce_expr.yy
	$(YACC) $(YFLAGS) $<

# Errors

lex.Error.cc: Error.lex Error.tab.cc Error.tab.hh
	$(LEX) $(LFLAGS) $<

Error.tab.cc Error.tab.hh: Error.yy
	$(YACC) $(YFLAGS) $<
