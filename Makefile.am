# Build libdap, libtest-types.a, getdap and deflate

AUTOMAKE_OPTIONS = foreign check-news
ACLOCAL_AMFLAGS = -I conf -I gl/m4

aclocaldir=$(datadir)/aclocal
pkgconfigdir=$(libdir)/pkgconfig

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir)/gl -I$(top_srcdir)/GNU -I$(top_srcdir)/xstream
AM_CXXFLAGS = 
if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

AM_YFLAGS = -d -v
AM_LFLAGS = -8

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
#     export CXXFLAGS='...'; ./configure --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

SUBDIRS = gl . tests unit-tests
DIST_SUBDIRS = unit-tests gl tests

lib_LTLIBRARIES = libdap.la libdapclient.la libdapserver.la

bin_SCRIPTS = dap-config dap-config-pkgconfig

bin_PROGRAMS = getdap outbuf_test

sbin_PROGRAMS = deflate

BUILT_SOURCES = $(GRAM_SRC) dods-datatypes.h xdr-datatypes.h

libdap_la_SOURCES = $(DAP_SRC) $(GNU_SRC) $(XSTREAM_SRC)
libdapclient_la_SOURCES = $(CLIENT_SRC) 
libdapserver_la_SOURCES = $(SERVER_SRC)

libdap_la_LDFLAGS = -version-info $(LIBDAP_VERSION)
libdap_la_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)
libdap_la_LIBADD = $(XML2_LIBS) $(PTHREAD_LIBS) gl/libgnu.la

libdapclient_la_LDFLAGS = -version-info $(CLIENTLIB_VERSION)
libdapclient_la_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS) $(XML2_CFLAGS)
libdapclient_la_LIBADD = $(CURL_LIBS) libdap.la $(PTHREAD_LIBS)

libdapserver_la_LDFLAGS = -version-info $(SERVERLIB_VERSION)
libdapserver_la_LIBADD = libdap.la

pkginclude_HEADERS = $(DAP_HDR) $(GNU_HDR) $(XSTREAM_HDR) $(CLIENT_HDR) $(SERVER_HDR) 
noinst_HEADERS = config_dap.h

outbuf_test_SOURCES = GNU/outbuf.cc

getdap_SOURCES = getdap.cc
getdap_LDADD = libdapclient.la libdap.la 
getdap_CPPFLAGS = $(AM_CPPFLAGS) $(CURL_CFLAGS)

deflate_SOURCES = deflate.c
deflate_LDADD = $(ZLIB_LIBS)
deflate_CPPFLAGS = $(ZLIB_CFLAGS)
deflate_LDFLAGS = $(ZLIB_LDFLAGS)

LEX_YACC_EXTRA = das.lex das.y dds.lex dds.y ce_expr.lex ce_expr.y gse.lex \
	gse.y Error.lex Error.y

EXTRA_DIST = ChangeLog COPYING README.AIS README.dodsrc COPYRIGHT_URI	 \
	COPYRIGHT_W3C GNU/README doxy.conf doxy_private.conf libdap.spec \
	VCPP dods-datatypes-static.h xdr-datatypes-static.h xstream/README \
	dap-config-pkgconfig \
	$(LEX_YACC_EXTRA) $(GRAM_SRC) OSX_Resources

dist_aclocal_DATA = conf/libdap.m4
pkgconfig_DATA = libdap.pc libdapclient.pc libdapserver.pc

CLEANFILES = *.log *.output *.gcda *.gcno $(GRAM_SRC) dods-datatypes.h xdr-datatypes.h

# MAINTAINERCLEANFILES = $(GRAM_SRC)

# Copy the generated grammar files to the 'grammarfiles' directory.
.PHONY: grammarfiles
grammarfiles: $(GRAM_SRC)
	for f in $(GRAM_SRC); do cp $$f grammarfiles; done

.PHONY: docs
docs:
	doxygen $(srcdir)/doxy.conf
	(cd docs && tar -czf html.tar.gz html)

###########################################################################

# Fortify targets.

.PHONY: fortify
fortify: dods-datatypes.h
	(cd gl && sourceanalyzer -b @PACKAGE@ $(MAKE) )
	sourceanalyzer -b @PACKAGE@ $(MAKE) libdap.la libdapserver.la
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr

# run fortify on only our code, not the gnulib code or the flex/bison code.
# This does skip testing some of the functions we wrote in the *.lex/*.y
# files but should eliminate the false positives from flex and bison, too.
.PHONY: fortify-lite
fortify-lite: dods-datatypes.h $(grammar_objects)
	( cd gl && $(MAKE) )
	sourceanalyzer -b @PACKAGE@ $(MAKE) libdap.la libdapserver.la
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr

grammar_objects = lex.das.o das.tab.o lex.dds.o dds.tab.o lex.ce_expr.o \
ce_expr.tab.o lex.gse_.o gse.tab.o lex.Error.o Error.tab.o

# Use this to clean the fortify project.
.PHONY: fortifyclean
fortifyclean:
	(cd gl && sourceanalyzer -b @PACKAGE@ -clean )
	sourceanalyzer -b @PACKAGE@ -clean

###########################################################################

# Build linux RPMs

srpm: dist
	rpmbuild -ts @PACKAGE@-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb @PACKAGE@-@PACKAGE_VERSION@.tar.gz

###########################################################################

# Build OS/X Packages. The strange operations with configure and make
# are there so that the values built into dap-config will match the mac
# osx install dirs and not the temp directory used to build the packages

PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker*

UNIV_ARCH=-arch ppc -arch i386
MAC_SYSROOT=Developer/SDKs/MacOSX10.4u.sdk

PKG_CONF_FLAGS=
#CXXFLAGS="$(CXXFLAGS) $(UNIV_ARCH) -isysroot /$(MAC_SYSROOT)" \
#CFLAGS="$(CFLAGS) $(UNIV_ARCH) -isysroot /$(MAC_SYSROOT)" \
#LDFLAGS="$(LDFLAGS) $(UNIV_ARCH) -Wl,-syslibroot,/$(MAC_SYSROOT)"

# LDFLAGS="$(LDFLAGS) $(UNIV_ARCH) -Wl,-syslibroot,$(MAC_SYSROOT)"

clean-pkg:
	-rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg

pkg-build:
	./configure --prefix=/usr --disable-dependency-tracking $(PKG_CONF_FLAGS)
	make clean all
	DESTDIR=${PWD}/mac_osx make install

pkg-main:
	./OSX_Resources/update_mac_package_contents.pl README "configure.ac" "mac_osx/usr"
	${PACKAGEMAKER} -build -p @PACKAGE@-@PACKAGE_VERSION@.pkg \
		-f mac_osx/usr \
		-ds -r OSX_Resources \
		-i OSX_Resources/Info.plist \
		-d OSX_Resources/Description.plist
	-rm -rf mac_osx

pkg-dmg:
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@.dmg
	mkdir @PACKAGE@-@PACKAGE_VERSION@
	mv @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@
	cp README README.* NEWS @PACKAGE@-@PACKAGE_VERSION@
	dropdmg -i --sanitize-for-servers --format zlib @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@

pkg: clean-pkg pkg-build pkg-main pkg-dmg

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

XSTREAM_SRC = xstream/posix.cpp xstream/fd.cpp xstream/common.cpp

XSTREAM_HDR = xstream/posix.h xstream/fd.h xstream/common.h 

GRAM_SRC = lex.das.cc das.tab.cc das.tab.hh \
	lex.dds.cc dds.tab.cc dds.tab.hh \
	lex.ce_expr.cc ce_expr.tab.cc ce_expr.tab.hh \
	lex.Error.cc Error.tab.cc Error.tab.hh gse.tab.hh gse.tab.cc \
	lex.gse_.cc

DAP_SRC = AttrTable.cc DAS.cc DDS.cc DataDDS.cc DDXParser.cc BaseType.cc \
	Byte.cc Int32.cc Float64.cc Str.cc Url.cc Vector.cc Array.cc	 \
	Structure.cc Sequence.cc Grid.cc UInt32.cc Int16.cc UInt16.cc	 \
	Float32.cc Constructor.cc					 \
	BaseTypeFactory.cc SignalHandler.cc Error.cc InternalErr.cc	 \
	util.cc xdrutil_ppc.c parser-util.cc escaping.cc		 \
	$(GRAM_SRC) ce_functions.cc GSEClause.cc \
	GeoConstraint.cc GridGeoConstraint.cc \
	Clause.cc RValue.cc ConstraintEvaluator.cc ArrayGeoConstraint.cc \
	DapIndent.cc Operators.h					 \
	XDRUtils.cc XDRFileMarshaller.cc XDRFileUnMarshaller.cc		 \
	XDRStreamMarshaller.cc

Operators.h: ce_expr.tab.hh

# Operators.h is included in with the source to prevent it from bing installed
# with the other headers. It includes one of the built grammar file headers.

CLIENT_SRC = RCReader.cc Connect.cc HTTPConnect.cc HTTPCache.cc		\
	AISResources.cc AISDatabaseParser.cc AISMerge.cc AISConnect.cc  \
	util_mit.cc ResponseTooBigErr.cc HTTPCacheTable.cc

SERVER_SRC = DODSFilter.cc cgi_util.cc Ancillary.cc

DAP_HDR = AttrTable.h DAS.h DDS.h DataDDS.h DDXParser.h DDXExceptions.h	   \
	BaseType.h Byte.h Int32.h Float64.h Str.h Url.h Vector.h Array.h   \
	Constructor.h Structure.h Sequence.h Grid.h UInt32.h Int16.h	   \
	UInt16.h Float32.h						   \
	BaseTypeFactory.h ObjectType.h EncodingType.h \
	SignalHandler.h Error.h InternalErr.h		   \
	util.h escaping.h parser.h debug.h dods-limits.h dods-datatypes.h \
	GeoConstraint.h GridGeoConstraint.h ArrayGeoConstraint.h	  \
	ce_functions.h  gse_parser.h GSEClause.h util_mit.h		  \
	expr.h Clause.h RValue.h ConstraintEvaluator.h ce_parser.h	  \
	DapIndent.h DapObj.h XDRFileMarshaller.h Marshaller.h		  \
	XDRFileUnMarshaller.h UnMarshaller.h XDRStreamMarshaller.h	  \
	XDRUtils.h xdr-datatypes.h

if USE_C99_TYPES
dods-datatypes.h: dods-datatypes-static.h
else
dods-datatypes.h: dods-datatypes-config.h
endif
	cp -p $< dods-datatypes.h

# I think that we should not be using a static header for the xdr
# types - this is breain gon many platforms because although the c99
# headers are present other type names are used for the xdr
# types. jhrg 6/25/08

# if USE_C99_TYPES
# xdr-datatypes.h: xdr-datatypes-static.h
#else
#endif

xdr-datatypes.h: xdr-datatypes-config.h
	cp -p $< xdr-datatypes.h

CLIENT_HDR = RCReader.h Connect.h HTTPConnect.h HTTPCache.h		 \
	HTTPCacheDisconnectedMode.h HTTPCacheInterruptHandler.h		 \
	Response.h HTTPResponse.h HTTPCacheResponse.h  PipeResponse.h	 \
	StdinResponse.h SignalHandlerRegisteredErr.h ResponseTooBigErr.h \
	AISConnect.h AISMerge.h AISExceptions.h AISDatabaseParser.h	 \
	AISResources.h Resource.h HTTPCacheTable.h

SERVER_HDR = DODSFilter.h cgi_util.h AlarmHandler.h EventHandler.h Ancillary.h

############################################################################
# Special rules for the grammars. I tried to use the automake grammar support
# but these grammars are so hacked that it was taking too much time. Maybe if
# each grammar was converted one by one... jhrg 6/22/05
# 
# I switched to using flex options instead of sed and mv. jhrg 02/28/08
#
# Build the DAS scanner and parser

lex.das.cc: das.lex das.tab.cc das.tab.hh
	$(LEX) $(LFLAGS) $(AM_LFLAGS) $<
#	mv lex.das.c lex.das.cc
#	sed 's/lex\.das\.c/lex\.das\.cc/g' < lex.das.cc > tmp
#	mv tmp lex.das.cc

# The funny if statment accommodates older versions of bison that treats
# -o differently than the 2.x versions. jhrg 3/2/08
das.tab.cc das.tab.hh: das.y DAS.h
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p das -o das.tab.cc $<
	if test -e das.tab.cc.h; then mv das.tab.cc.h das.tab.hh; fi
#	mv das.tab.c das.tab.cc

# DDS

lex.dds.cc: dds.lex dds.tab.cc dds.tab.hh
	$(LEX) $(LFLAGS) $(AM_LFLAGS) $<
#	mv lex.dds.c lex.dds.cc
#	sed 's/lex\.dds\.c/lex\.dds\.cc/g' < lex.dds.cc > tmp
#	mv tmp lex.dds.cc


dds.tab.cc dds.tab.hh: dds.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p dds -o dds.tab.cc $<
	if test -e dds.tab.cc.h; then mv dds.tab.cc.h dds.tab.hh; fi
#	mv dds.tab.c dds.tab.cc

# CE

lex.ce_expr.cc: ce_expr.lex ce_expr.tab.cc ce_expr.tab.hh
	$(LEX) $(LFLAGS) $(AM_LFLAGS) $<
#	mv lex.ce_expr.c lex.ce_expr.cc
#	sed 's/lex\.ce_expr\.c/lex\.ce_expr\.cc/g' < lex.ce_expr.cc > tmp
#	mv tmp lex.ce_expr.cc

ce_expr.tab.cc ce_expr.tab.hh: ce_expr.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p ce_expr -o ce_expr.tab.cc $<
	if test -e ce_expr.tab.cc.h; then mv ce_expr.tab.cc.h ce_expr.tab.hh; fi
#	mv ce_expr.tab.c ce_expr.tab.cc

# Build the grid selection sub_expression scanner and parser

lex.gse_.cc: gse.lex gse.tab.cc gse.tab.hh
	$(LEX) $(LFLAGS) $(AM_LFLAGS) $<
#	mv lex.gse_.c lex.gse_.cc
#	sed 's/lex\.gse_\.c/lex\.gse_\.cc/g' < lex.gse_.cc > tmp
#	mv tmp lex.gse_.cc

gse.tab.cc gse.tab.hh: gse.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p gse_ -o gse.tab.cc $<
	if test -e gse.tab.cc.h; then mv gse.tab.cc.h gse.tab.hh; fi
#	mv gse.tab.c gse.tab.cc

# Errors

lex.Error.cc: Error.lex Error.tab.cc Error.tab.hh
	$(LEX) $(LFLAGS) $(AM_LFLAGS) $<
#	mv lex.Error.c lex.Error.cc
#	sed 's/lex\.Error\.c/lex\.Error\.cc/g' < lex.Error.cc > tmp
#	mv tmp lex.Error.cc

Error.tab.cc Error.tab.hh: Error.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p Error -o Error.tab.cc $<
	if test -e Error.tab.cc.h; then mv Error.tab.cc.h Error.tab.hh; fi
#	mv Error.tab.c Error.tab.cc

