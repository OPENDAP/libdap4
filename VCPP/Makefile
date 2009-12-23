# Makefile for libdap (under VC++ on win32)

# Source code, samples, docs and target build areas
OBJDIR			= .\objects
SRCDIR			= ..
DOCDIR			= .\doc
SAMPLEDIR		= .\sample

# This is the prepackaging area.  Contents should never be on the build path.
# Fill in the VERSION (e.g., "= 3.7.3") to generate a final, version stamped
# release of the product.
PRODUCT			= libdap
VERSION			=
PKGDIR			= ..\..\prepkg\opendap\$(PRODUCT)$(VERSION)
PKGLIBDIR		= $(PKGDIR)\lib
PKGDLLDIR		= $(PKGDIR)\dll
PKGBINDIR		= $(PKGDIR)\bin
PKGETCDIR		= $(PKGDIR)\etc
PKGINCDIR		= $(PKGDIR)\include
PKGSAMPLEDIR	= $(PKGDIR)\sample

# This is the installation area.  It should never be on the build path either
# (except when building other, non-core components).
INSTALLDIR		= ..\..
INSTALLLIBDIR	= $(INSTALLDIR)\lib
INSTALLDLLDIR	= $(INSTALLDIR)\dll
INSTALLBINDIR	= $(INSTALLDIR)\bin
INSTALLETCDIR	= $(INSTALLDIR)\etc
INSTALLINCDIR	= $(INSTALLDIR)\include

# Headers only come from the source distribution itself.  Neither the
# Prepackaging or installation areas should be on any path for building
# the OPeNDAP Core.
INCLUDEDIR		= $(SRCDIR)

# Compiler, Linker
CC		= cl
LINK	= link

# Compiler tools
LEX	= flex
YACC	= bison

# We suppress #line directives under win32 to prevent warnings resulting
# when a "\" followed by some char is interpreted as an escape sequence.
YACCFLAGS	= --no-lines -d -v

# MT = Static Link Multi-Threaded Model
# MD = Dynamic Link Multi-Threaded DLL Model

# Setup compiler flags
MTCFLAGS	= /MT /D HAVE_PTHREAD_H
MDCFLAGS	= /MD /D HAVE_PTHREAD_H
CFLAGS		= /nologo /W0 /GR /EHsc /c

# Setup linker flags for libdap
DAP_MTLFLAGS	= -lib /nologo -libpath:$(OBJDIR)\MT
DAP_MDLFLAGS	= /nologo -libpath:$(OBJDIR)\MD /dll /def:libdap.def /map /fixed:NO /base:0x06000000 /NODEFAULTLIB:LIBCMT.lib

# Setup linker flags for libdapclient
CLIENT_MTLFLAGS	= -lib /nologo -libpath:$(OBJDIR)\MT
CLIENT_MDLFLAGS	= /nologo -libpath:$(OBJDIR)\MD /dll /def:libdapclient.def /map /fixed:NO /base:0x07000000 /NODEFAULTLIB:LIBCMT.lib

# Setup linker flags for libdapserver
SERVER_MTLFLAGS	= -lib /nologo -libpath:$(OBJDIR)\MT
SERVER_MDLFLAGS	= /nologo -libpath:$(OBJDIR)\MD /dll /def:libdapserver.def /map /fixed:NO /base:0x08000000 /NODEFAULTLIB:LIBCMT.lib

# Setup link libraries for libdap
DAP_MTLIBS		=
DAP_MDLIBS		= libdirent.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup link libraries for libdapclient
CLIENT_MTLIBS	=
CLIENT_MDLIBS	= libdapMD.lib libdirent.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup link libraries for libdapserver
SERVER_MTLIBS	=
SERVER_MDLIBS	= libdapMD.lib libdirent.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup linker flags for executables
EMTLFLAGS		= /nologo /libpath:$(OBJDIR)\MT /NODEFAULTLIB:MSVCRT.lib
EMDLFLAGS		= /nologo /libpath:$(OBJDIR)\MD /NODEFAULTLIB:LIBCMT.lib

# Setup link libraries for executables
EMTLLIBS		= libdapclientMT.lib libdapserverMT.lib libdapMT.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib
EMDLLIBS		= libdapclientMD.lib libdapserverMD.lib libdapMD.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup compiler includes and defines
INCS		= /I . /I .. /I ..\GNU /I ..\win32\gl /I $(INCLUDEDIR)
DEFS		= /D WIN32 /D WIN32_LEAN_AND_MEAN /D HAVE_CONFIG_H
CPPFLAGS	= $(DEFS) $(INCS)

default:
	@echo "usage: nmake all; nmake install; nmake package"

all:	setup modelMT modelMD

modelMT:
	@-md $(OBJDIR)\MT
	@echo "Building MT model libraries for the Core"
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(DAP_MTLFLAGS)" LIBS="$(DAP_MTLIBS)" OBJDIR=$(OBJDIR)\MT libdapMT.lib
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(CLIENT_MTLFLAGS)" LIBS="$(CLIENT_MTLIBS)" OBJDIR=$(OBJDIR)\MT libdapclientMT.lib
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(SERVER_MTLFLAGS)" LIBS="$(SERVER_MTLIBS)" OBJDIR=$(OBJDIR)\MT libdapserverMT.lib

	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT getdap
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT deflate
	@echo "MT model libraries for the Core are complete"

modelMD:
	@-md $(OBJDIR)\MD
	@echo "Building MD model libraries for the Core"
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(DAP_MDLFLAGS)" LIBS="$(DAP_MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapMD.dll	
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(CLIENT_MDLFLAGS)" LIBS="$(CLIENT_MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapclientMD.dll
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(SERVER_MDLFLAGS)" LIBS="$(SERVER_MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapserverMD.dll

	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD getdap
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD deflate
	@echo "MD model libraries for the Core are complete"

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .c .cc .exe

# Rules for library objects, both the .c and .cc's are C++
{$(SRCDIR)}.c{$(OBJDIR)}.obj:
	$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $<

{$(SRCDIR)}.cc{$(OBJDIR)}.obj:
	$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $<

# Rule for linking executables
{$(OBJDIR)}.obj{$(OBJDIR)}.exe:
	$(LINK) $(LFLAGS) /out:$@ $< $(LIBS)

GNU_OBJS = $(OBJDIR)\GetOpt.obj \
		$(OBJDIR)\GNURegex.obj \
		$(OBJDIR)\regex.obj \
		$(OBJDIR)\strcasecmp.obj \
		$(OBJDIR)\strncasecmp.obj \
		$(OBJDIR)\strnlen1.obj \
		$(OBJDIR)\localcharset.obj

GRAM_OBJS = $(OBJDIR)\lex.das.obj \
		$(OBJDIR)\das.tab.obj \
		$(OBJDIR)\lex.dds.obj \
		$(OBJDIR)\dds.tab.obj \
		$(OBJDIR)\lex.ce_expr.obj \
		$(OBJDIR)\ce_expr.tab.obj \
		$(OBJDIR)\lex.Error.obj \
		$(OBJDIR)\Error.tab.obj

DAP_OBJS = $(GRAM_OBJS) \
		$(OBJDIR)\AttrTable.obj \
		$(OBJDIR)\ConstraintEvaluator.obj \
		$(OBJDIR)\DAS.obj \
		$(OBJDIR)\DDS.obj \
		$(OBJDIR)\DataDDS.obj \
		$(OBJDIR)\DDXParser.obj \
		$(OBJDIR)\BaseType.obj \
		$(OBJDIR)\Byte.obj \
		$(OBJDIR)\Error.obj \
		$(OBJDIR)\Int32.obj \
		$(OBJDIR)\Float64.obj \
		$(OBJDIR)\Str.obj \
		$(OBJDIR)\Url.obj \
		$(OBJDIR)\Vector.obj \
		$(OBJDIR)\Array.obj \
		$(OBJDIR)\Structure.obj \
		$(OBJDIR)\Sequence.obj \
		$(OBJDIR)\Grid.obj \
		$(OBJDIR)\UInt32.obj \
		$(OBJDIR)\Int16.obj \
		$(OBJDIR)\UInt16.obj \
		$(OBJDIR)\Float32.obj \
		$(OBJDIR)\GeoConstraint.obj \
		$(OBJDIR)\GridGeoConstraint.obj \
		$(OBJDIR)\Constructor.obj \
		$(OBJDIR)\BaseTypeFactory.obj \
		$(OBJDIR)\SignalHandler.obj \
		$(OBJDIR)\InternalErr.obj \
		$(OBJDIR)\util.obj \
		$(OBJDIR)\xdrutil_ppc.obj \
		$(OBJDIR)\XDRUtils.obj	 \
		$(OBJDIR)\XDRFileMarshaller.obj	\
		$(OBJDIR)\XDRFileUnMarshaller.obj	\
		$(OBJDIR)\XDRStreamMarshaller.obj	 \
		$(OBJDIR)\parser-util.obj \
		$(OBJDIR)\escaping.obj \
		$(OBJDIR)\Clause.obj \
		$(OBJDIR)\RValue.obj \
		$(OBJDIR)\gse.tab.obj \
		$(OBJDIR)\ce_functions.obj \
		$(OBJDIR)\GSEClause.obj \
		$(OBJDIR)\lex.gse_.obj \
		$(OBJDIR)\ArrayGeoConstraint.obj \
		$(OBJDIR)\DapIndent.obj

CLIENT_OBJS = $(OBJDIR)\RCReader.obj \
		$(OBJDIR)\Connect.obj \
		$(OBJDIR)\HTTPConnect.obj \
		$(OBJDIR)\HTTPCache.obj \
		$(OBJDIR)\HTTPCacheTable.obj \
		$(OBJDIR)\AISResources.obj \
		$(OBJDIR)\AISDatabaseParser.obj \
		$(OBJDIR)\AISMerge.obj \
		$(OBJDIR)\AISConnect.obj \
		$(OBJDIR)\util_mit.obj \
		$(OBJDIR)\ResponseTooBigErr.obj

SERVER_OBJS = $(OBJDIR)\DODSFilter.obj \
		$(OBJDIR)\Ancillary.obj \
		$(OBJDIR)\cgi_util.obj

# Convenience dependancies
libdapMT.lib:		$(OBJDIR)\libdapMT.lib
libdapclientMT.lib:	$(OBJDIR)\libdapclientMT.lib
libdapserverMT.lib:	$(OBJDIR)\libdapserverMT.lib
libdapMD.dll:		$(OBJDIR)\libdapMD.dll
libdapclientMD.dll:	$(OBJDIR)\libdapclientMD.dll
libdapserverMD.dll:	$(OBJDIR)\libdapserverMD.dll

getdap:		$(OBJDIR)\getdap.exe
deflate:	$(OBJDIR)\deflate.exe

# Build the static-link core libraries
$(OBJDIR)\libdapMT.lib: $(DAP_OBJS) $(GNU_OBJS)
	$(LINK) $(LFLAGS) /out:$@ $** $(LIBS)
	
$(OBJDIR)\libdapserverMT.lib: $(SERVER_OBJS)
	$(LINK) $(LFLAGS) /out:$@ $** $(LIBS)

$(OBJDIR)\libdapclientMT.lib: $(CLIENT_OBJS)
	$(LINK) $(LFLAGS) /out:$@ $** $(LIBS)
	
############################################################################
# The song and dances before the link lines below are to get a legitimate
# module export definition (.def) for the dll's.  We use lib, dumpbin and
# a 3rd-party awk to do this automatically in a rather round-about way.
#
# The $$ reduces to just $ as make evaluates that.  The \? to awk tells it
# to be looking for a literal "?".  A textual description of the awk command
# is as follows: "Find all second fields (2nd whitespace delimited groups of
# characters) in the text lines that begin with ? and do not begin with ??_
# (i.e., all C++ code originating methods minus the destructors which should
# not be exposed do to how dll's work) and output those in the module export
# definition file."  These are the methods that will be exposed to programs
# that use the dll in question.  C code references begins with just "_" and
# we need not expose those.
############################################################################
	
# Build the dynamic-link core libraries
$(OBJDIR)\libdapMD.dll: $(DAP_OBJS) $(GNU_OBJS)
	@lib /NOLOGO /out:$(OBJDIR)\tmp.lib $** > $(OBJDIR)\devnull
	@dumpbin /OUT:$(OBJDIR)\tmp.def /linkermember:2 $(OBJDIR)\tmp.lib > $(OBJDIR)\devnull
	@echo EXPORTS > libdap.def
	@gawk "{ if ($$2 ~ /^[\?].*/ && $$2 !~ /^\?\?_.*/) print $$2 }" < $(OBJDIR)\tmp.def >> libdap.def
	$(LINK) $(LFLAGS) /out:$@ $** $(LIBS)
	@del libdap.def $(OBJDIR)\tmp.lib $(OBJDIR)\tmp.def $(OBJDIR)\devnull

$(OBJDIR)\libdapclientMD.dll: $(CLIENT_OBJS)
	@lib /NOLOGO /out:$(OBJDIR)\tmp.lib $** > $(OBJDIR)\devnull
	@dumpbin /OUT:$(OBJDIR)\tmp.def /linkermember:2 $(OBJDIR)\tmp.lib > $(OBJDIR)\devnull
	@echo EXPORTS > libdapclient.def
	@gawk "{ if ($$2 ~ /^[\?].*/ && $$2 !~ /^\?\?_.*/) print $$2 }" < $(OBJDIR)\tmp.def >> libdapclient.def
	$(LINK) $(LFLAGS) /out:$@ $** $(LIBS)
	@del libdapclient.def $(OBJDIR)\tmp.lib $(OBJDIR)\tmp.def $(OBJDIR)\devnull

$(OBJDIR)\libdapserverMD.dll: $(SERVER_OBJS)
	@lib /NOLOGO /out:$(OBJDIR)\tmp.lib $** > $(OBJDIR)\devnull
	@dumpbin /OUT:$(OBJDIR)\tmp.def /linkermember:2 $(OBJDIR)\tmp.lib > $(OBJDIR)\devnull
	@echo EXPORTS > libdapserver.def
	@gawk "{ if ($$2 ~ /^[\?].*/ && $$2 !~ /^\?\?_.*/) print $$2 }" < $(OBJDIR)\tmp.def >> libdapserver.def
	$(LINK) $(LFLAGS) /out:$@ $** $(LIBS)
	@del libdapserver.def $(OBJDIR)\tmp.lib $(OBJDIR)\tmp.def $(OBJDIR)\devnull

# Libdap with the gram objs
$(OBJDIR)\lex.das.obj					: $(SRCDIR)\lex.das.cc
$(OBJDIR)\das.tab.obj					: $(SRCDIR)\das.tab.cc
$(OBJDIR)\lex.dds.obj					: $(SRCDIR)\lex.dds.cc
$(OBJDIR)\dds.tab.obj					: $(SRCDIR)\dds.tab.cc
$(OBJDIR)\lex.ce_expr.obj				: $(SRCDIR)\lex.ce_expr.cc
$(OBJDIR)\ce_expr.tab.obj				: $(SRCDIR)\ce_expr.tab.cc
$(OBJDIR)\lex.Error.obj					: $(SRCDIR)\lex.Error.cc
$(OBJDIR)\Error.obj						: $(SRCDIR)\Error.cc
$(OBJDIR)\Error.tab.obj					: $(SRCDIR)\Error.tab.cc
$(OBJDIR)\AttrTable.obj					: $(SRCDIR)\AttrTable.cc
$(OBJDIR)\ConstraintEvaluator.obj 		: $(SRCDIR)\ConstraintEvaluator.cc
$(OBJDIR)\DapIndent.obj					: $(SRCDIR)\DapIndent.cc
$(OBJDIR)\DAS.obj						: $(SRCDIR)\DAS.cc
$(OBJDIR)\DDS.obj						: $(SRCDIR)\DDS.cc
$(OBJDIR)\DataDDS.obj					: $(SRCDIR)\DataDDS.cc
$(OBJDIR)\DDXParser.obj					: $(SRCDIR)\DDXParser.cc
$(OBJDIR)\BaseType.obj					: $(SRCDIR)\BaseType.cc
$(OBJDIR)\Byte.obj						: $(SRCDIR)\Byte.cc
$(OBJDIR)\Int32.obj						: $(SRCDIR)\Int32.cc
$(OBJDIR)\Float64.obj					: $(SRCDIR)\Float64.cc
$(OBJDIR)\GeoConstraint.obj				: $(SRCDIR)\GeoConstraint.cc
$(OBJDIR)\GridGeoConstraint.obj			: $(SRCDIR)\GridGeoConstraint.cc
$(OBJDIR)\Str.obj						: $(SRCDIR)\Str.cc
$(OBJDIR)\Url.obj						: $(SRCDIR)\Url.cc
$(OBJDIR)\Vector.obj					: $(SRCDIR)\Vector.cc
$(OBJDIR)\Array.obj						: $(SRCDIR)\Array.cc
$(OBJDIR)\Structure.obj					: $(SRCDIR)\Structure.cc
$(OBJDIR)\Sequence.obj					: $(SRCDIR)\Sequence.cc
$(OBJDIR)\Grid.obj						: $(SRCDIR)\Grid.cc
$(OBJDIR)\UInt32.obj					: $(SRCDIR)\UInt32.cc
$(OBJDIR)\Int16.obj						: $(SRCDIR)\Int16.cc
$(OBJDIR)\UInt16.obj					: $(SRCDIR)\UInt16.cc
$(OBJDIR)\Float32.obj					: $(SRCDIR)\Float32.cc
$(OBJDIR)\Constructor.obj				: $(SRCDIR)\Constructor.cc
$(OBJDIR)\PassiveInt32.obj				: $(SRCDIR)\PassiveInt32.cc
$(OBJDIR)\PassiveFloat64.obj			: $(SRCDIR)\PassiveFloat64.cc
$(OBJDIR)\PassiveStr.obj				: $(SRCDIR)\PassiveStr.cc
$(OBJDIR)\PassiveUrl.obj				: $(SRCDIR)\PassiveUrl.cc
$(OBJDIR)\PassiveUInt32.obj				: $(SRCDIR)\PassiveUInt32.cc
$(OBJDIR)\PassiveInt16.obj				: $(SRCDIR)\PassiveInt16.cc
$(OBJDIR)\PassiveUInt16.obj				: $(SRCDIR)\PassiveUInt16.cc
$(OBJDIR)\PassiveFloat32.obj			: $(SRCDIR)\PassiveFloat32.cc
$(OBJDIR)\PassiveArray.obj				: $(SRCDIR)\PassiveArray.cc
$(OBJDIR)\PassiveStructure.obj			: $(SRCDIR)\PassiveStructure.cc
$(OBJDIR)\BaseTypeFactory.obj			: $(SRCDIR)\BaseTypeFactory.cc
$(OBJDIR)\SignalHandler.obj				: $(SRCDIR)\SignalHandler.cc
$(OBJDIR)\InternalErr.obj				: $(SRCDIR)\InternalErr.cc
$(OBJDIR)\util.obj						: $(SRCDIR)\util.cc
$(OBJDIR)\xdrutil_ppc.obj				: $(SRCDIR)\Xdrutil_ppc.c
$(OBJDIR)\XDRUtils.obj					: $(SRCDIR)\XDRUtils.cc
$(OBJDIR)\XDRFileMarshaller.obj			: $(SRCDIR)\XDRFileMarshaller.cc
$(OBJDIR)\XDRFileUnMarshaller.obj		: $(SRCDIR)\XDRFileUnMarshaller.cc	
$(OBJDIR)\XDRStreamMarshaller.obj		: $(SRCDIR)\XDRStreamMarshaller.cc
$(OBJDIR)\parser-util.obj				: $(SRCDIR)\parser-util.cc
$(OBJDIR)\escaping.obj					: $(SRCDIR)\escaping.cc
$(OBJDIR)\Clause.obj					: $(SRCDIR)\Clause.cc
$(OBJDIR)\RValue.obj					: $(SRCDIR)\RValue.cc
$(OBJDIR)\gse.tab.obj					: $(SRCDIR)\gse.tab.cc
$(OBJDIR)\ce_functions.obj				: $(SRCDIR)\ce_functions.cc
$(OBJDIR)\GSEClause.obj					: $(SRCDIR)\GSEClause.cc
$(OBJDIR)\lex.gse_.obj					: $(SRCDIR)\lex.gse_.cc
$(OBJDIR)\ArrayGeoConstraint.obj		: $(SRCDIR)\ArrayGeoConstraint.cc

# Libdap client
$(OBJDIR)\RCReader.obj					: $(SRCDIR)\RCReader.cc
$(OBJDIR)\Connect.obj					: $(SRCDIR)\Connect.cc
$(OBJDIR)\HTTPConnect.obj				: $(SRCDIR)\HTTPConnect.cc
$(OBJDIR)\HTTPCache.obj					: $(SRCDIR)\HTTPCache.cc
$(OBJDIR)\HTTPCacheTable.obj			: $(SRCDIR)\HTTPCacheTable.cc
$(OBJDIR)\AISResources.obj				: $(SRCDIR)\AISResources.cc
$(OBJDIR)\AISDatabaseParser.obj			: $(SRCDIR)\AISDatabaseParser.cc
$(OBJDIR)\AISMerge.obj					: $(SRCDIR)\AISMerge.cc
$(OBJDIR)\AISConnect.obj				: $(SRCDIR)\AISConnect.cc
$(OBJDIR)\util_mit.obj					: $(SRCDIR)\util_mit.cc
$(OBJDIR)\ResponseTooBigErr.obj			: $(SRCDIR)\ResponseTooBigErr.cc

# libdap server
$(OBJDIR)\DODSFilter.obj				: $(SRCDIR)\DODSFilter.cc
$(OBJDIR)\Ancillary.obj					: $(SRCDIR)\Ancillary.cc
$(OBJDIR)\cgi_util.obj					: $(SRCDIR)\cgi_util.cc

# Regular expression code
$(OBJDIR)\GetOpt.obj		: $(SRCDIR)\GNU\GetOpt.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\GetOpt.cc
$(OBJDIR)\GNURegex.obj		: $(SRCDIR)\GNU\GNURegex.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\GNURegex.cc
$(OBJDIR)\regex.obj		: $(SRCDIR)\win32\gl\regex.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\win32\gl\regex.c
$(OBJDIR)\strcasecmp.obj	: $(SRCDIR)\win32\gl\strcasecmp.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\win32\gl\strcasecmp.c
$(OBJDIR)\strncasecmp.obj	: $(SRCDIR)\win32\gl\strncasecmp.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\win32\gl\strncasecmp.c
$(OBJDIR)\strnlen1.obj	: $(SRCDIR)\win32\gl\strnlen1.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\win32\gl\strnlen1.c
$(OBJDIR)\localcharset.obj	: $(SRCDIR)\win32\gl\localcharset.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\win32\gl\localcharset.c

# Source generated by compiler tools
$(SRCDIR)\lex.das.cc:	$(SRCDIR)\das.lex $(SRCDIR)\das.tab.cc $(SRCDIR)\das.tab.hh
	$(LEX) -8 $(SRCDIR)\das.lex
	@move lex.das.cc $(SRCDIR)\lex.das.cc
#	@del lex.das.c

$(SRCDIR)\das.tab.cc $(SRCDIR)\das.tab.hh: $(SRCDIR)\das.y $(SRCDIR)\DAS.h
	$(YACC) $(YACCFLAGS) -p das -o $(SRCDIR)\das.tab.cc $(SRCDIR)\das.y
#	@move $(SRCDIR)\das.tab.cc.h $(SRCDIR)\das.tab.hh
#	@del $(SRCDIR)\das.tab.cc.h

$(SRCDIR)\lex.dds.cc:	$(SRCDIR)\dds.lex $(SRCDIR)\dds.tab.cc $(SRCDIR)\dds.tab.hh
	$(LEX) -8 $(SRCDIR)\dds.lex
	@move lex.dds.cc $(SRCDIR)\lex.dds.cc
#	@del lex.dds.c

$(SRCDIR)\dds.tab.cc $(SRCDIR)\dds.tab.hh: $(SRCDIR)\dds.y
	$(YACC) $(YACCFLAGS) -p dds -o $(SRCDIR)\dds.tab.cc $(SRCDIR)\dds.y
	@move $(SRCDIR)\dds.tab.cc.h $(SRCDIR)\dds.tab.hh
#	@del $(SRCDIR)\dds.tab.c

$(SRCDIR)\lex.ce_expr.cc: $(SRCDIR)\ce_expr.lex $(SRCDIR)\ce_expr.tab.cc $(SRCDIR)\ce_expr.tab.hh
	$(LEX) -8 $(SRCDIR)\ce_expr.lex
	@move lex.ce_expr.cc $(SRCDIR)\lex.ce_expr.cc
#	@del lex.ce_expr.c

$(SRCDIR)\ce_expr.tab.cc $(SRCDIR)\ce_expr.tab.hh: $(SRCDIR)\ce_expr.y
	$(YACC) $(YACCFLAGS) -p ce_expr -o $(SRCDIR)\ce_expr.tab.cc $(SRCDIR)\ce_expr.y
	@move $(SRCDIR)\ce_expr.tab.cc.h $(SRCDIR)\ce_expr.tab.hh
#	@del $(SRCDIR)\ce_expr.tab.c

$(SRCDIR)\lex.Error.cc:	$(SRCDIR)\Error.lex $(SRCDIR)\Error.tab.cc $(SRCDIR)\Error.tab.hh
	$(LEX) -8 $(SRCDIR)\Error.lex
	@move lex.Error.cc $(SRCDIR)\lex.Error.cc
#	@del lex.Error.c

$(SRCDIR)\Error.tab.cc $(SRCDIR)\Error.tab.hh: $(SRCDIR)\Error.y
	$(YACC) $(YACCFLAGS) -p Error -o $(SRCDIR)\Error.tab.cc $(SRCDIR)\Error.y
	@move $(SRCDIR)\Error.tab.cc.h $(SRCDIR)\Error.tab.hh
#	@del $(SRCDIR)\Error.tab.c

$(SRCDIR)\lex.gse_.cc:	$(SRCDIR)\gse.lex $(SRCDIR)\gse.tab.cc $(SRCDIR)\gse.tab.hh
	$(LEX) -8 $(SRCDIR)\gse.lex
	@move lex.gse_.cc $(SRCDIR)\lex.gse_.cc
#	@del lex.gse_.c

$(SRCDIR)\gse.tab.cc $(SRCDIR)\gse.tab.hh: $(SRCDIR)\gse.y
	$(YACC) $(YACCFLAGS) -p gse_ -o $(SRCDIR)\gse.tab.cc $(SRCDIR)\gse.y
	@move $(SRCDIR)\gse.tab.cc.h $(SRCDIR)\gse.tab.hh
#	@del $(SRCDIR)\gse.tab.c

$(OBJDIR)\getdap.exe: $(OBJDIR)\getdap.obj

$(OBJDIR)\deflate.exe: $(OBJDIR)\deflate.obj 

# Command-Line Client and select utils in console mode
$(OBJDIR)\getdap.obj    : $(SRCDIR)\getdap.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\getdap.cc

$(OBJDIR)\deflate.obj    : $(SRCDIR)\deflate.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\deflate.c

# We have to create a temporary unistd.h because bison
# and/or flex create source that includes that - even though
# there is no such header under win32 and it is unnecessary there
# (that is, even though these utils can run under win32 - by definition
# they generate output for the UNIX world).  This stupid pet trick
# also properly defines isatty() for the generated source under win32.
setup:
	@-echo /*  Dummy patch header to get win32 flex/bison generated  */ > unistd.h
	@-echo /*  source code to compile under win32.  If you see this  */ >> unistd.h
	@-echo /*  temp file, feel free to scratch it.                   */ >> unistd.h
	@-echo #include "io.h" >> unistd.h

# Win32 libnc-dap requires the win32\gl\regex.h.  Under unix builds, it is simply
# assumed to be on the machine already.
install: all mkinstdirs
	copy $(OBJDIR)\MT\libdapMT.lib $(INSTALLLIBDIR)
	copy $(OBJDIR)\MT\libdapclientMT.lib $(INSTALLLIBDIR)
	copy $(OBJDIR)\MT\libdapserverMT.lib $(INSTALLLIBDIR)

	copy $(OBJDIR)\MD\libdapMD.dll* $(INSTALLDLLDIR)
	mt -nologo -manifest $(INSTALLDLLDIR)\libdapMD.dll.manifest -outputresource:$(INSTALLDLLDIR)\libdapMD.dll;2
	del $(INSTALLDLLDIR)\libdapMD.dll.manifest
	copy $(OBJDIR)\MD\libdapMD.lib $(INSTALLLIBDIR)
	copy $(OBJDIR)\MD\libdapclientMD.dll* $(INSTALLDLLDIR)
	mt -nologo -manifest $(INSTALLDLLDIR)\libdapclientMD.dll.manifest -outputresource:$(INSTALLDLLDIR)\libdapclientMD.dll;2
	del $(INSTALLDLLDIR)\libdapclientMD.dll.manifest
	copy $(OBJDIR)\MD\libdapclientMD.lib $(INSTALLLIBDIR)
	copy $(OBJDIR)\MD\libdapserverMD.dll* $(INSTALLDLLDIR)
	mt -nologo -manifest $(INSTALLDLLDIR)\libdapserverMD.dll.manifest -outputresource:$(INSTALLDLLDIR)\libdapserverMD.dll;2
	del $(INSTALLDLLDIR)\libdapserverMD.dll.manifest
	copy $(OBJDIR)\MD\libdapserverMD.lib $(INSTALLLIBDIR)

	copy $(OBJDIR)\MD\getdap.exe* $(INSTALLBINDIR)
	mt -nologo -manifest $(INSTALLBINDIR)\getdap.exe.manifest -outputresource:$(INSTALLBINDIR)\getdap.exe;1
	del $(INSTALLBINDIR)\getdap.exe.manifest
	copy $(OBJDIR)\MD\deflate.exe* $(INSTALLETCDIR)
	mt -nologo -manifest $(INSTALLETCDIR)\deflate.exe.manifest -outputresource:$(INSTALLETCDIR)\deflate.exe;1
	del $(INSTALLETCDIR)\deflate.exe.manifest
	copy dods-datatypes.h $(INSTALLINCDIR)
	copy xdr-datatypes.h $(INSTALLINCDIR)
	copy $(SRCDIR)\*.h $(INSTALLINCDIR)
	copy $(SRCDIR)\GNU\*.h $(INSTALLINCDIR)\GNU
	copy $(SRCDIR)\win32\gl\regex.h $(INSTALLINCDIR)
	
package: all mkpkgdirs document
	copy $(OBJDIR)\MT\libdapMT.lib $(PKGLIBDIR)
	copy $(OBJDIR)\MT\libdapclientMT.lib $(PKGLIBDIR)
	copy $(OBJDIR)\MT\libdapserverMT.lib $(PKGLIBDIR)

	copy $(OBJDIR)\MD\libdapMD.dll* $(PKGDLLDIR)
	mt -nologo -manifest $(PKGDLLDIR)\libdapMD.dll.manifest -outputresource:$(PKGDLLDIR)\libdapMD.dll;2
	del $(PKGDLLDIR)\libdapMD.dll.manifest
	copy $(OBJDIR)\MD\libdapMD.lib $(PKGLIBDIR)
	copy $(OBJDIR)\MD\libdapclientMD.dll* $(PKGDLLDIR)
	mt -nologo -manifest $(PKGDLLDIR)\libdapclientMD.dll.manifest -outputresource:$(PKGDLLDIR)\libdapclientMD.dll;2
	del $(PKGDLLDIR)\libdapclientMD.dll.manifest
	copy $(OBJDIR)\MD\libdapclientMD.lib $(PKGLIBDIR)
	copy $(OBJDIR)\MD\libdapserverMD.dll* $(PKGDLLDIR)
	mt -nologo -manifest $(PKGDLLDIR)\libdapserverMD.dll.manifest -outputresource:$(PKGDLLDIR)\libdapserverMD.dll;2
	del $(PKGDLLDIR)\libdapserverMD.dll.manifest
	copy $(OBJDIR)\MD\libdapserverMD.lib $(PKGLIBDIR)

	copy $(OBJDIR)\MD\getdap.exe* $(PKGBINDIR)
	mt -nologo -manifest $(PKGBINDIR)\getdap.exe.manifest -outputresource:$(PKGBINDIR)\getdap.exe;1
	del $(PKGBINDIR)\getdap.exe.manifest
	copy $(OBJDIR)\MD\deflate.exe* $(PKGETCDIR)
	mt -nologo -manifest $(PKGETCDIR)\deflate.exe.manifest -outputresource:$(PKGETCDIR)\deflate.exe;1
	del $(PKGETCDIR)\deflate.exe.manifest
	copy dods-datatypes.h $(PKGINCDIR)
	copy xdr-datatypes.h $(PKGINCDIR)
	copy $(SRCDIR)\*.h $(PKGINCDIR)
	copy $(SRCDIR)\GNU\*.h $(PKGINCDIR)\GNU
	copy $(SRCDIR)\win32\gl\regex.h $(PKGINCDIR)
	
mkinstdirs:	
	@-md $(INSTALLLIBDIR)
	@-md $(INSTALLBINDIR)
	@-md $(INSTALLETCDIR)
	@-md $(INSTALLDLLDIR)
	@-md $(INSTALLINCDIR)
	@-md $(INSTALLINCDIR)\GNU
	
mkpkgdirs:	
	@-md $(PKGLIBDIR)
	@-md $(PKGBINDIR)
	@-md $(PKGETCDIR)
	@-md $(PKGDLLDIR)
	@-md $(PKGINCDIR)
	@-md $(PKGINCDIR)\GNU
	
document:
	@-md $(PKGSAMPLEDIR)
	copy $(SAMPLEDIR)\* $(PKGSAMPLEDIR)
	copy $(DOCDIR)\readme.txt $(PKGDIR)

clean:
	@-rd /Q /S $(OBJDIR) $(PKGDIR)
	@-del unistd.h

grammarclean: clean
	@-del $(SRCDIR)\*.tab.cc
	@-del $(SRCDIR)\*.tab.hh
	@-del $(SRCDIR)\lex.*.cc

!INCLUDE "dependancies"



