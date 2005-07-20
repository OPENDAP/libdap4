
# Build libdap++, libtest-types++.a, getdap and deflate

AUTOMAKE_OPTIONS = foreign check-news
ACLOCAL_AMFLAGS = -I gl/m4

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir)/gl
AM_CXXFLAGS = -I$(top_srcdir)/GNU
AM_YFLAGS = -d -v
AM_LFLAGS = -8

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
#     export CXXFLAGS='...'; ./configure --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0 -Wall -fno-defer-pop
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

SUBDIRS = gl . tests 
DIST_SUBDIRS = unit-tests gl tests

lib_LTLIBRARIES = libdap.la 

bin_SCRIPTS = dap-config

bin_PROGRAMS = getdap

sbin_PROGRAMS = deflate usage

libdap_la_SOURCES = $(DAP_SRC) $(CLIENT_SRC) $(SERVER_SRC) $(GNU_SRC)

libdap_la_LDFLAGS = -version-info $(LIBDAP_VERSION)
libdap_la_LIBADD = gl/libgnu.la

pkginclude_HEADERS = $(DAP_HDR) $(CLIENT_HDR) $(SERVER_HDR) $(GNU_HDR)
noinst_HEADERS = config_dap.h

getdap_SOURCES = getdap.cc
getdap_LDADD = libdap.la

deflate_SOURCES = deflate.c
usage_SOURCES = usage.cc
usage_LDADD = libdap.la

LEX_YACC_EXTRA = das.lex das.y dds.lex dds.y expr.lex expr.y gse.lex gse.y \
	Error.lex Error.y

LEX_YACC_FILES = das.tab.h das.tab.cc lex.das.cc dds.tab.h dds.tab.cc \
	lex.dds.cc expr.tab.h expr.tab.cc lex.expr.cc Error.tab.h Error.tab.cc \
	lex.Error.cc gse.tab.h gse.tab.cc lex.gse_.cc

EXTRA_DIST = ChangeLog COPYING README.AIS README.dodsrc COPYRIGHT_URI	 \
	COPYRIGHT_W3C GNU/README doxy.conf doxy_private.conf libdap.spec \
	win32 $(LEX_YACC_EXTRA) $(LEX_YACC_FILES) gl/m4

CLEANFILES = *.log das.output dds.output expr.output Error.output gse.output

# MAINTAINERCLEANFILES = $(LEX_YACC_FILES)

#############################################################################
# Library sources
# 

GNU_SRC = GNU/GetOpt.cc GNU/GNURegex.cc GNU/GNUerror.cc			\
	IteratorAdapter.cc ArrayIterAdapter.cc AttrIterAdapter.cc	\
	BTIterAdapter.cc ClauseIterAdapter.cc 

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

