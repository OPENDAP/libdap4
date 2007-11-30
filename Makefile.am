
# Build libdap, libtest-types.a, getdap and deflate

AUTOMAKE_OPTIONS = foreign check-news
ACLOCAL_AMFLAGS = -I conf -I gl/m4

aclocaldir=$(datadir)/aclocal
pkgconfigdir=$(libdir)/pkgconfig

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir)/gl -I$(top_srcdir)/GNU
AM_CXXFLAGS = -Wall -W -Wcast-align
AM_YFLAGS = -d -v
AM_LFLAGS = -8

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
#     export CXXFLAGS='...'; ./configure --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

SUBDIRS = gl . tests 
DIST_SUBDIRS = unit-tests gl tests

lib_LTLIBRARIES = libdap.la libdapclient.la libdapserver.la

bin_SCRIPTS = dap-config

bin_PROGRAMS = getdap

sbin_PROGRAMS = deflate

BUILT_SOURCES = $(GRAM_SRC) dods-datatypes.h xdr-datatypes.h

libdap_la_SOURCES = $(DAP_SRC) $(GNU_SRC)
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

pkginclude_HEADERS = $(DAP_HDR) $(GNU_HDR) $(CLIENT_HDR) $(SERVER_HDR) 
noinst_HEADERS = config_dap.h

getdap_SOURCES = getdap.cc
getdap_LDADD = libdapclient.la libdap.la 

deflate_SOURCES = deflate.c
deflate_LDADD = $(ZLIB_LIBS)
deflate_CPPFLAGS = $(ZLIB_CFLAGS)
deflate_LDFLAGS = $(ZLIB_LDFLAGS)

LEX_YACC_EXTRA = das.lex das.y dds.lex dds.y ce_expr.lex ce_expr.y gse.lex \
	gse.y Error.lex Error.y

MACOSX_RESOURCES_EXTRA =  INSTALL_MacOSX.rtf README_MacOSX.rtf OSX_Resources

EXTRA_DIST = ChangeLog COPYING README.AIS README.dodsrc COPYRIGHT_URI	 \
	COPYRIGHT_W3C GNU/README doxy.conf doxy_private.conf libdap.spec \
	VCPP dods-datatypes-static.h xdr-datatypes-static.h \
	$(LEX_YACC_EXTRA) $(GRAM_SRC) $(MACOSX_RESOURCES_EXTRA)

dist_aclocal_DATA = conf/libdap.m4
pkgconfig_DATA = libdap.pc libdapclient.pc libdapserver.pc

CLEANFILES = *.log *.output *.gcda *.gcno $(GRAM_SRC)

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

# Build OS/X Packages. The strange operations with configure and dap-config
# are there so that the values built into dap-config will match the mac
# osx install dirs and not the temp directory used to build the packages

PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker*

pkg:
	rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg
	./configure --prefix=/usr $(PKG_CONFIGURE_FLAGS)
	make clean all
	DESTDIR=${PWD}/mac_osx make install
	${PACKAGEMAKER} -build -p @PACKAGE@-@PACKAGE_VERSION@.pkg -f mac_osx/usr -ds -r OSX_Resources -i OSX_Resources/Info.plist -d OSX_Resources/Description.plist

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

GRAM_SRC = lex.das.cc das.tab.cc das.tab.h \
	lex.dds.cc dds.tab.cc dds.tab.h \
	lex.ce_expr.cc ce_expr.tab.cc ce_expr.tab.h \
	lex.Error.cc Error.tab.cc Error.tab.h gse.tab.h gse.tab.cc \
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

Operators.h: ce_expr.tab.h

# Operators.h is included in with the source to prevent it from bing installed
# with the other headers. It includes one of the built grammar file headers.

CLIENT_SRC = RCReader.cc Connect.cc HTTPConnect.cc HTTPCache.cc		\
	AISResources.cc AISDatabaseParser.cc AISMerge.cc AISConnect.cc  \
	util_mit.cc ResponseTooBigErr.cc

SERVER_SRC = DODSFilter.cc cgi_util.cc

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

if USE_C99_TYPES
xdr-datatypes.h: xdr-datatypes-static.h
else
xdr-datatypes.h: xdr-datatypes-config.h
endif
	cp -p $< xdr-datatypes.h

CLIENT_HDR = RCReader.h Connect.h HTTPConnect.h HTTPCache.h		 \
	HTTPCacheDisconnectedMode.h HTTPCacheInterruptHandler.h		 \
	Response.h HTTPResponse.h HTTPCacheResponse.h  PipeResponse.h	 \
	StdinResponse.h SignalHandlerRegisteredErr.h ResponseTooBigErr.h \
	AISConnect.h AISMerge.h AISExceptions.h AISDatabaseParser.h	 \
	AISResources.h Resource.h

SERVER_HDR = DODSFilter.h cgi_util.h AlarmHandler.h EventHandler.h

############################################################################
# Special rules for the grammars. I tried to use the automake grammar support
# but these grammars are so hacked that it was taking too much time. Maybe if
# each grammar was converted one by one... jhrg 6/22/05
# 

# Build the DAS scanner and parser

lex.das.cc: das.lex das.tab.cc das.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pdas $<
	mv lex.das.c lex.das.cc
	sed 's/lex\.das\.c/lex\.das\.cc/g' < lex.das.cc > tmp
	mv tmp lex.das.cc

das.tab.cc das.tab.h: das.y DAS.h
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p das $<
	mv das.tab.c das.tab.cc

# DDS

lex.dds.cc: dds.lex dds.tab.cc dds.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pdds $<
	mv lex.dds.c lex.dds.cc
	sed 's/lex\.dds\.c/lex\.dds\.cc/g' < lex.dds.cc > tmp
	mv tmp lex.dds.cc


dds.tab.cc dds.tab.h: dds.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p dds $<
	mv dds.tab.c dds.tab.cc

# CE

lex.ce_expr.cc: ce_expr.lex ce_expr.tab.cc ce_expr.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pce_expr $<
	mv lex.ce_expr.c lex.ce_expr.cc
	sed 's/lex\.ce_expr\.c/lex\.ce_expr\.cc/g' < lex.ce_expr.cc > tmp
	mv tmp lex.ce_expr.cc

ce_expr.tab.cc ce_expr.tab.h: ce_expr.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p ce_expr $<
	mv ce_expr.tab.c ce_expr.tab.cc

# Build the grid selection sub_expression scanner and parser

lex.gse_.cc: gse.lex gse.tab.cc gse.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pgse_ $<
	mv lex.gse_.c lex.gse_.cc
	sed 's/lex\.gse_\.c/lex\.gse_\.cc/g' < lex.gse_.cc > tmp
	mv tmp lex.gse_.cc

gse.tab.cc gse.tab.h: gse.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p gse_ $<
	mv gse.tab.c gse.tab.cc

# Errors

lex.Error.cc: Error.lex Error.tab.cc Error.tab.h
	$(LEX) $(LFLAGS) $(AM_LFLAGS) -PError $<
	mv lex.Error.c lex.Error.cc
	sed 's/lex\.Error\.c/lex\.Error\.cc/g' < lex.Error.cc > tmp
	mv tmp lex.Error.cc

Error.tab.cc Error.tab.h: Error.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p Error $<
	mv Error.tab.c Error.tab.cc

