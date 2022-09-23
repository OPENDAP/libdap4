# Build libdap, libtest-types.a, getdap, getdap4

AUTOMAKE_OPTIONS = foreign check-news subdir-objects
ACLOCAL_AMFLAGS = -I conf -I gl/m4

aclocaldir=$(datadir)/aclocal
pkgconfigdir=$(libdir)/pkgconfig

AM_CPPFLAGS = -I$(top_builddir)/gl -I$(top_srcdir)/gl -I$(top_srcdir)/GNU $(XML2_CFLAGS) $(TIRPC_CFLAGS)
AM_CXXFLAGS = -Wall -W -Wcast-align -Wno-deprecated-register -Wno-inconsistent-missing-override

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align -Wno-deprecated-register
endif

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
# ./configure --disable-shared CXXFLAGS=...
# or ./configure --enable-developer --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align 

# Build using the Address sanitizer (gcc 4.8+ and clang 3.3+).
# See: https://github.com/google/sanitizers/wiki/AddressSanitizer
# jhrg 2/6/18
if USE_ASAN
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
endif

if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXX11_FLAG) $(CXXFLAGS_DEBUG) $(ASAN_FLAGS)
else
AM_CXXFLAGS += -g -O2 $(CXX11_FLAG)
endif

AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

SUBDIRS = gl d4_ce d4_function . unit-tests tests
DIST_SUBDIRS = gl d4_ce d4_function unit-tests tests

noinst_LTLIBRARIES = libparsers.la

lib_LTLIBRARIES = libdap.la libdapclient.la libdapserver.la

bin_SCRIPTS = dap-config dap-config-pkgconfig

bin_PROGRAMS = getdap getdap4

# Including the generated grammar sources in BUILT_SOURCES ensures
# they are built first. jhrg 6/18/15
BUILT_SOURCES = $(BISON_SRC) $(FLEX_SRC) dods-datatypes.h xdr-datatypes.h \
	getdap.1 dap-config.1 getdap4.1

man_MANS = getdap.1 dap-config.1 getdap4.1

%.1: %.man1
	groff -t -e -mandoc -Tascii $< | col -bx > $@

libparsers_la_SOURCES = $(FLEX_SRC) $(BISON_SRC)  
libparsers_la_CPPFLAGS =  $(XML2_CFLAGS) $(TIRPC_CFLAGS)
libparsers_la_LIBADD = $(XML2_LIBS)

libdap_la_SOURCES = $(DAP_SRC) $(GNU_SRC)  $(DAP4_ONLY_SRC)

libdap_la_LDFLAGS = -version-info $(LIBDAP_VERSION)
libdap_la_CPPFLAGS = $(AM_CPPFLAGS)
libdap_la_LIBADD = $(XML2_LIBS) $(PTHREAD_LIBS) gl/libgnu.la d4_ce/libd4_ce_parser.la \
d4_function/libd4_function_parser.la libparsers.la $(CRYPTO_LIBS)

libdapclient_la_SOURCES = $(CLIENT_SRC) $(DAP4_CLIENT_HDR) $(DAP4_CLIENT_SRC)
libdapclient_la_LDFLAGS = -version-info $(CLIENTLIB_VERSION)
libdapclient_la_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)
libdapclient_la_LIBADD = $(CURL_LIBS) libdap.la $(PTHREAD_LIBS)

libdapserver_la_SOURCES = $(SERVER_SRC)
libdapserver_la_LDFLAGS = -version-info $(SERVERLIB_VERSION)
libdapserver_la_LIBADD = libdap.la $(UUID_LIBS)

pkginclude_HEADERS = $(DAP_HDR) $(GNU_HDR) $(CLIENT_HDR) $(SERVER_HDR) $(DAP4_ONLY_HDR) $(DAP4_CLIENT_HDR)

noinst_HEADERS = config_dap.h

getdap_SOURCES = getdap.cc
getdap_LDADD = libdapclient.la libdap.la
getdap_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)

getdap4_SOURCES = getdap4.cc
getdap4_LDADD = libdapclient.la libdap.la
getdap4_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)

LEX_YACC_EXTRA = das.lex das.yy dds.lex dds.yy ce_expr.lex ce_expr.yy \
Error.lex Error.yy

EXTRA_DIST = ChangeLog COPYING README.md README.dodsrc COPYRIGHT_URI	\
	COPYRIGHT_W3C GNU/README doxy.conf.in libdap.spec.in libdap.spec VCPP	\
	dods-datatypes-static.h xdr-datatypes-static.h		\
	dap-config-pkgconfig $(LEX_YACC_EXTRA) OSX_Resources	\
	getdap.man1 dap-config.man1 getdap4.man1

dist_aclocal_DATA = conf/libdap.m4
pkgconfig_DATA = libdap.pc libdapclient.pc libdapserver.pc

CLEANFILES = *.log *.output *.gcda *.gcno *.gcov $(BISON_SRC) $(FLEX_SRC) \
	dods-datatypes.h xdr-datatypes.h getdap.1 dap-config.1 getdap4.1

DISTCLEAN = libdap.spec

HTML_DOCS=html
DOXYGEN_CONF=doxy.conf

# This target only builds the documents, it does not move them to
# github. Use the 'gh-docs' target below for that.
.PHONY: docs
docs: ${DOXYGEN_CONF}
	doxygen ${DOXYGEN_CONF}

# GitHub.io docs 
.PHONY: gh-docs 
gh-docs:
	./build-gh-docs.sh

# Build a compile_commands.json.tmp file for use with CLion. This will emable
# reasonable use for all the source that's compiled using either 'all' or
# 'check' but other targets will have to be added by hand. See the compiledb
# github site (https://github.com/nickdiego/compiledb) for more info.
# jhrg 9/25/19
.PHONY: compiledb
compiledb:
	make -nwk all check | compiledb

# cccc computes metrics like Lines of code and McCabe. It's available
# on the web...
.PHONY: cccc
cccc:
	-mkdir cccc
	cccc --outdir=cccc $(DAP_SRC) $(SERVER_SRC) $(CLIENT_SRC) \
	$(DAP_HDR) $(SERVER_HDR) $(CLIENT_HDR)

###########################################################################

# dist-hook:
#	if test -n "$$VERSION"; then cat $$VERSION NEWS > NEWS.tmp; mv NEWS.tmp NEWS; fi


# Build linux RPMs. Use the environment variable 'RPM_OPTIONS' to pass in
# extra options like --nodeps and --macros

srpm: dist
	rpmbuild -ts --clean $(RPM_OPTIONS) @PACKAGE@-@VERSION@.tar.gz

rpm: dist
	rpmbuild -tb --clean $(RPM_OPTIONS) @PACKAGE@-@VERSION@.tar.gz

# Build the debain package.
deb: dist
	( mv @PACKAGE@-@PACKAGE_VERSION@.tar.gz ../ )
	( mv ../@PACKAGE@-@PACKAGE_VERSION@.tar.gz ../@PACKAGE@_@PACKAGE_VERSION@.orig.tar.gz )
	( cd ../ && tar xzf @PACKAGE@_@PACKAGE_VERSION@.orig.tar.gz )
	( cp -r debian/ ../@PACKAGE@-@PACKAGE_VERSION@ )
	( cd ../@PACKAGE@-@PACKAGE_VERSION@ && debuild -us -uc )

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
	./OSX_Resources/update_mac_package_contents.pl README.md
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
	cp README.md README.* NEWS @PACKAGE@-@PACKAGE_VERSION@
	dropdmg -i --sanitize-for-servers --format zlib @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@

pkg: pkg-main 

#############################################################################
# Library sources
# 

GNU_SRC = GNU/GetOpt.cc GNU/GNURegex.cc

GNU_HDR = GNU/GetOpt.h GNU/GNURegex.h

BISON_SRC = das.tab.cc das.tab.hh dds.tab.cc	dds.tab.hh \
	ce_expr.tab.cc ce_expr.tab.hh Error.tab.cc Error.tab.hh 

FLEX_SRC = lex.das.cc lex.dds.cc lex.ce_expr.cc lex.Error.cc

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
	ServerFunctionsList.cc ServerFunction.cc DapXmlNamespaces.cc \
	MarshallerThread.cc

DAP4_ONLY_SRC = D4StreamMarshaller.cc D4StreamUnMarshaller.cc Int64.cc \
        UInt64.cc Int8.cc D4ParserSax2.cc D4BaseTypeFactory.cc \
        D4Dimensions.cc  D4EnumDefs.cc D4Group.cc DMR.cc \
        D4Attributes.cc D4Enum.cc chunked_ostream.cc chunked_istream.cc \
        D4Sequence.cc D4Maps.cc D4Opaque.cc D4AsyncUtil.cc D4RValue.cc \
        D4FilterClause.cc

Operators.h: ce_expr.tab.hh

# Operators.h is included in with the source to prevent it from bing installed
# with the other headers. It includes one of the built grammar file headers.

CLIENT_SRC = RCReader.cc Connect.cc HTTPConnect.cc HTTPCache.cc	\
	util_mit.cc ResponseTooBigErr.cc HTTPCacheTable.cc

DAP4_CLIENT_SRC = D4Connect.cc

SERVER_SRC = DODSFilter.cc Ancillary.cc
# ResponseBuilder.cc ResponseCache.cc

DAP_HDR = AttrTable.h DAS.h DDS.h DataDDS.h DDXParserSAX2.h		\
	DDXExceptions.h BaseType.h Byte.h Int32.h Float64.h Str.h	\
	Url.h Vector.h Array.h Constructor.h Structure.h Sequence.h	\
	Grid.h UInt32.h Int16.h UInt16.h Float32.h BaseTypeFactory.h	\
	ObjectType.h EncodingType.h SignalHandler.h Error.h		\
	InternalErr.h util.h escaping.h parser.h debug.h dods-limits.h	\
	dods-datatypes.h Type.h		\
	util_mit.h expr.h Clause.h RValue.h ConstraintEvaluator.h	\
	ce_parser.h DapIndent.h DapObj.h XDRFileMarshaller.h		\
	Marshaller.h UnMarshaller.h XDRFileUnMarshaller.h		\
	XDRStreamMarshaller.h XDRUtils.h xdr-datatypes.h mime_util.h	\
	cgi_util.h XDRStreamUnMarshaller.h Keywords2.h XMLWriter.h \
	ServerFunctionsList.h ServerFunction.h media_types.h \
	DapXmlNamespaces.h parser-util.h MarshallerThread.h

DAP4_ONLY_HDR = D4StreamMarshaller.h D4StreamUnMarshaller.h Int64.h \
        UInt64.h Int8.h D4ParserSax2.h D4BaseTypeFactory.h \
        D4Maps.h D4Dimensions.h D4EnumDefs.h D4Group.h DMR.h D4Attributes.h \
        D4AttributeType.h D4Enum.h chunked_stream.h chunked_ostream.h \
        chunked_istream.h D4Sequence.h crc.h D4Opaque.h D4AsyncUtil.h \
        D4Function.h D4RValue.h D4FilterClause.h

if USE_C99_TYPES
dods-datatypes.h: dods-datatypes-static.h
	cp -p $< dods-datatypes.h
else
dods-datatypes.h: dods-datatypes-config.h
	cp -p $< dods-datatypes.h
endif

xdr-datatypes.h: xdr-datatypes-config.h
	cp -p $< xdr-datatypes.h

CLIENT_HDR = RCReader.h Connect.h HTTPConnect.h HTTPCache.h		\
	HTTPCacheDisconnectedMode.h HTTPCacheInterruptHandler.h		\
	Response.h HTTPResponse.h HTTPCacheResponse.h PipeResponse.h	\
	StdinResponse.h SignalHandlerRegisteredErr.h			\
	ResponseTooBigErr.h Resource.h HTTPCacheTable.h HTTPCacheMacros.h

DAP4_CLIENT_HDR = D4Connect.h

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

# Using this pattern rule is a way to get gnu make to run bison only once.
# The targets that explicitly name the two sources to be built will run it
# twice with a parallel build. jhrg 6/18/15

%.tab.cc %.tab.hh: %.yy
	$(YACC) $(YFLAGS) $<

lex.das.cc: das.lex das.tab.cc das.tab.hh
	$(LEX) $(LFLAGS) $<

#das.tab.cc das.tab.hh: das.yy DAS.h
#	$(YACC) $(YFLAGS) $<

# DDS

lex.dds.cc: dds.lex dds.tab.cc dds.tab.hh
	$(LEX) $(LFLAGS) $<

#dds.tab.cc dds.tab.hh: dds.yy
#	$(YACC) $(YFLAGS) $<

# CE

lex.ce_expr.cc: ce_expr.lex ce_expr.tab.cc ce_expr.tab.hh
	$(LEX) $(LFLAGS) $<

#ce_expr.tab.cc ce_expr.tab.hh: ce_expr.yy
#	$(YACC) $(YFLAGS) $<

# Errors

lex.Error.cc: Error.lex Error.tab.cc Error.tab.hh
	$(LEX) $(LFLAGS) $<

#Error.tab.cc Error.tab.hh: Error.yy
#	$(YACC) $(YFLAGS) $<
