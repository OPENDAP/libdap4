# Makefile for libdap

# Setup some preliminary directories
OBJDIR		= .\objects
SRCDIR		= ..
INSTALLLIBDIR	= ..\..\..\lib
INSTALLDLLDIR	= ..\..\..\dll
INSTALLBINDIR	= ..\..\..\bin
INSTALLETCDIR	= ..\..\..\etc
INSTALLINCDIR	= ..\..\..\include

INCLUDEDIR	= ..\..\..\include

# Compiler, Linker
CC		= cl
LINK		= link

# Setup compiler tools.  BISON_HAIRY and BISON_SIMPLE environment variables
# must be set to the _full_ path for bison.simple and bison.hairy respectively.
# These tools can (as of 12/2003) be downloaded and installed from
# http://unxutils.sourceforge.net/UnxUtils.zip.  They must also be on
# the path.
LEX	= flex
YACC	= bison

# We suppress #line directives under win32 to prevent warnings resulting
# when a "\" followed by some char is interpreted as an escape sequence.
YACCFLAGS	= --no-lines -d -v

# ST = Single-Threaded Model
# MT = Multi-Threaded Model
# MD = Multi-Threaded DLL Model
# STD = Single-Threaded Model, debug version
# MTD = Multi-Threaded Model, debug version
# MDD = Multi-Thread DLL Model, debug version


# Setup compiler flags
STCFLAGS	= /ML
MTCFLAGS	= /MT /D HAVE_PTHREAD_H
MDCFLAGS	= /MD /D HAVE_PTHREAD_H
STDCFLAGS	= /MLd /Zi /Yd
MTDCFLAGS	= /MTd /D HAVE_PTHREAD_H /Zi /Yd
MDDCFLAGS	= /MDd /D HAVE_PTHREAD_H /Zi /Yd
CFLAGS		= /nologo /W0 /GR /GX /c

# Setup linker flags for libdap
STLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MTLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MDLFLAGS	= -libpath:$(INSTALLLIBDIR) /dll /def:libdap.def /map /fixed:NO /base:0x06000000
STDLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MTDLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MDDLFLAGS	= -libpath:$(INSTALLLIBDIR) /dll /def:libdap.def /map /debug /fixed:NO /base:0x06000000

# Setup link libraries for libdap
STLIBS		= 
MTLIBS		=
MDLIBS		= libxml2MD.lib libcurlMD.lib libzMD.lib libxdrMD.lib pthreadVC.lib
STDLIBS		= 
MTDLIBS		=
MDDLIBS		= libxml2MDD.lib libcurlMDD.lib libzMDD.lib libxdrMDD.lib pthreadVC.lib

# Setup linker flags for executables
ESTLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)
EMTLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)
EMDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)
ESTDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR) /debug
EMTDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR) /debug
EMDDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR) /debug

# Setup link libraries for executables
ESTLLIBS	= libdapST.lib libxml2ST.lib libcurlST.lib libzST.lib libxdrST.lib
EMTLLIBS	= libdapMT.lib libxml2MT.lib libcurlMT.lib libzMT.lib libxdrMT.lib pthreadVC.lib
EMDLLIBS	= libdapMD.lib libxml2MD.lib libcurlMD.lib libzMD.lib libxdrMD.lib pthreadVC.lib
ESTDLLIBS	= libdapSTD.lib libxml2STD.lib libcurlSTD.lib libzSTD.lib libxdrSTD.lib
EMTDLLIBS	= libdapMTD.lib libxml2MTD.lib libcurlMTD.lib libzMTD.lib libxdrMTD.lib pthreadVC.lib
EMDDLLIBS	= libdapMDD.lib libxml2MDD.lib libcurlMDD.lib libzMDD.lib libxdrMDD.lib pthreadVC.lib
CPPUNITLIBS	= cppunit_dll.lib

# Setup compiler includes and defines
INCS		= /I . /I .. /I ..\GNU /I ..\regex-0.12 /I $(INCLUDEDIR) /I $(INCLUDEDIR)\pthreads /I $(INCLUDEDIR)\libxml2
CPPUNITINCS	= $(INCS) /I $(INCLUDEDIR)/cppunit
DEFS		= /D WIN32 /D WIN32_LEAN_AND_MEAN /D DVR=\"DODS/3.4.8\"
CPPFLAGS	= $(DEFS) $(INCS)

default:
	@echo "usage: nmake all; nmake install"
	@echo "or"
	@echo "usage: nmake model<ST|MT|MD|STD|MTD|MDD>; nmake install<ST|MT|MD|STD|MTD|MDD>"

all:	grammars build_libxdr setup
	nmake modelST
	nmake modelMT
	nmake modelMD
	nmake modelSTD
	nmake modelMTD
	nmake modelMDD

modelST: setup
	@-md $(OBJDIR)\ST
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(STLFLAGS)" LIBS="$(STLIBS)" OBJDIR=$(OBJDIR)\ST libdapST.lib
	nmake installST
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST geturl
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST usage
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST deflate
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST check
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST unit_check

modelMT: setup
	@-md $(OBJDIR)\MT
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(MTLFLAGS)" LIBS="$(MTLIBS)" OBJDIR=$(OBJDIR)\MT libdapMT.lib
	nmake installMT
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT geturl
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT usage
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT deflate
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT ThreadTest
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT check
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT unit_check

modelMD: setup
	@-md $(OBJDIR)\MD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(MDLFLAGS)" LIBS="$(MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapMD.dll
	nmake installMD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD geturl
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD usage
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD deflate
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD ThreadTest
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD check
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD unit_check

modelSTD: setup
	@-md $(OBJDIR)\STD
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(STDLFLAGS)" LIBS="$(STDLIBS)" OBJDIR=$(OBJDIR)\STD libdapSTD.lib
	nmake installSTD
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD geturl
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD usage
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD deflate
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD check
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD unit_check

modelMTD: setup
	@-md $(OBJDIR)\MTD
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(MTDLFLAGS)" LIBS="$(MTDLIBS)" OBJDIR=$(OBJDIR)\MTD libdapMTD.lib
	nmake installMTD
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD geturl
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD usage
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD deflate
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD ThreadTest
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD check
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD unit_check

modelMDD: setup
	@-md $(OBJDIR)\MDD
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(MDDLFLAGS)" LIBS="$(MDDLIBS)" OBJDIR=$(OBJDIR)\MDD libdapMDD.dll
	nmake installMDD
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD geturl
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD usage
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD deflate
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD ThreadTest
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD check
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD unit_check

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .c .cc .exe

# Rules for library objects, both the .c and .cc's are C++
{$(SRCDIR)}.c{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $<

{$(SRCDIR)}.cc{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $<

# Rule for linking executables
{$(OBJDIR)}.obj{$(OBJDIR)}.exe:
	@$(LINK) $(LFLAGS) /out:$@ $< $(LIBS)

UTILOBJS = $(OBJDIR)\cgi_util.obj \
		$(OBJDIR)\escaping.obj \
		$(OBJDIR)\DODSFilter.obj

DAS_GRAM_OBJS = $(OBJDIR)\lex.das.obj \
		$(OBJDIR)\das.tab.obj \
		$(OBJDIR)\parser-util.obj
DDS_GRAM_OBJS = $(OBJDIR)\lex.dds.obj \
		$(OBJDIR)\dds.tab.obj \
		$(OBJDIR)\parser-util.obj
EXPR_GRAM_OBJS = $(OBJDIR)\lex.expr.obj \
		$(OBJDIR)\expr.tab.obj \
		$(OBJDIR)\RValue.obj
ERROR_GRAM_OBJS = $(OBJDIR)\lex.Error.obj \
		$(OBJDIR)\Error.tab.obj
GSE_GRAM_OBJS = $(OBJDIR)\lex.gse_.obj \
		$(OBJDIR)\gse.tab.obj

DASOBJS = $(OBJDIR)\AttrTable.obj \
		$(OBJDIR)\DAS.obj \
		$(DAS_GRAM_OBJS)

DDSOBJS = $(OBJDIR)\DDS.obj \
		$(OBJDIR)\DataDDS.obj \
		$(DDS_GRAM_OBJS)

EXPROBJS = $(OBJDIR)\Clause.obj \
		$(OBJDIR)\RValue.obj \
		$(EXPR_GRAM_OBJS)

ERROROBJS = $(OBJDIR)\Error.obj \
		$(OBJDIR)\InternalErr.obj \
		$(OBJDIR)\ResponseTooBigErr.obj \
		$(ERROR_GRAM_OBJS)

CLIENTOBJS = $(OBJDIR)\Connections.obj \
		$(OBJDIR)\RCReader.obj \
		$(OBJDIR)\Connect.obj \
		$(OBJDIR)\HTTPConnect.obj \
		$(OBJDIR)\HTTPCache.obj

TYPEOBJS = $(GSE_GRAM_OBJS) \
		$(OBJDIR)\BaseType.obj \
		$(OBJDIR)\Byte.obj \
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
		$(OBJDIR)\util.obj \
		$(OBJDIR)\ce_functions.obj \
		$(OBJDIR)\GSEClause.obj \
		$(OBJDIR)\Int16.obj \
		$(OBJDIR)\UInt16.obj \
		$(OBJDIR)\Float32.obj \
		$(OBJDIR)\Constructor.obj \
		$(OBJDIR)\VirtualCtorByte.obj \
		$(OBJDIR)\VirtualCtorInt16.obj \
		$(OBJDIR)\VirtualCtorUInt16.obj \
		$(OBJDIR)\VirtualCtorInt32.obj \
		$(OBJDIR)\VirtualCtorUInt32.obj \
		$(OBJDIR)\VirtualCtorFloat32.obj \
		$(OBJDIR)\VirtualCtorFloat64.obj \
		$(OBJDIR)\VirtualCtorStr.obj \
		$(OBJDIR)\VirtualCtorUrl.obj \
		$(OBJDIR)\VirtualCtorArray.obj \
		$(OBJDIR)\VirtualCtorStructure.obj \
		$(OBJDIR)\VirtualCtorSequence.obj \
		$(OBJDIR)\VirtualCtorGrid.obj \
		$(OBJDIR)\util_mit.obj

# As of 8/03, these are only used to link expr-test.  expr-test
# will link without them, but most of those tests will fail when
# linked without.
TESTOBJS = $(OBJDIR)\TestByte.obj \
		$(OBJDIR)\TestInt32.obj \
		$(OBJDIR)\TestFloat64.obj \
		$(OBJDIR)\TestStr.obj \
		$(OBJDIR)\TestUrl.obj \
		$(OBJDIR)\TestArray.obj \
		$(OBJDIR)\TestStructure.obj \
		$(OBJDIR)\TestList.obj \
		$(OBJDIR)\TestSequence.obj \
		$(OBJDIR)\TestGrid.obj \
		$(OBJDIR)\TestUInt32.obj \
		$(OBJDIR)\TestInt16.obj \
		$(OBJDIR)\TestUInt16.obj \
		$(OBJDIR)\TestFloat32.obj

GNUOBJS = $(OBJDIR)\GetOpt.obj \
		$(OBJDIR)\Regex.obj \
		$(OBJDIR)\gnu_regex.obj \
		$(OBJDIR)\gnu_error.obj \
		$(OBJDIR)\IteratorAdapter.obj \
		$(OBJDIR)\ArrayIterAdapter.obj \
		$(OBJDIR)\AttrIterAdapter.obj \
		$(OBJDIR)\BTIterAdapter.obj \
		$(OBJDIR)\ClauseIterAdapter.obj

AISOBJS = $(OBJDIR)\AISResources.obj \
		$(OBJDIR)\AISDatabaseParser.obj \
		$(OBJDIR)\AISMerge.obj \
		$(OBJDIR)\AISConnect.obj

LIBOBJS = $(DASOBJS) $(DDSOBJS) $(TYPEOBJS) $(GNUOBJS) $(ERROROBJS) $(EXPROBJS) \
		$(UTILOBJS) $(CLIENTOBJS) $(AISOBJS)

# Convenience dependancies
libdapST.lib:	$(OBJDIR)\libdapST.lib
libdapMT.lib:	$(OBJDIR)\libdapMT.lib
libdapMD.dll:	$(OBJDIR)\libdapMD.dll
libdapSTD.lib:	$(OBJDIR)\libdapSTD.lib
libdapMTD.lib:	$(OBJDIR)\libdapMTD.lib
libdapMDD.dll:	$(OBJDIR)\libdapMDD.dll

geturl:		$(OBJDIR)\geturl.exe
usage:		$(OBJDIR)\usage.exe
deflate:	$(OBJDIR)\deflate.exe
ThreadTest:	$(OBJDIR)\ThreadTest.exe
check: 		$(OBJDIR)\das-test.exe $(OBJDIR)\dds-test.exe $(OBJDIR)\error-test.exe $(OBJDIR)\expr-test.exe
		@echo Notice:
		@echo Core Test programs have been built, but they must be
		@echo executed using DejaGnu in a Cygwin environment.  If
		@echo validation of the Win32 Dods core is required, install
		@echo Cygwin w/Dejagnu and move the testsuites there in order
		@echo to run them.  Optimally, the testsuite should be ran once
		@echo for each of the 6 library models.

# Link the library
$(OBJDIR)\libdapST.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libdapMT.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libdapMD.dll: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libdapSTD.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@  $(LIBOBJS) $(LIBS)

$(OBJDIR)\libdapMTD.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libdapMDD.dll: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

grammars: $(SRCDIR)\lex.das.c $(SRCDIR)\das.tab.h $(SRCDIR)\lex.dds.c $(SRCDIR)\dds.tab.h \
		$(SRCDIR)\lex.expr.c $(SRCDIR)\expr.tab.h $(SRCDIR)\lex.Error.c $(SRCDIR)\Error.tab.h \
		$(SRCDIR)\lex.gse_.c $(SRCDIR)\gse.tab.h

build_libxdr:
	@cd libxdr
	@nmake all
	@nmake install
	@cd ..

# The DAP Proper
$(OBJDIR)\AISResources.obj : $(SRCDIR)\AISResources.cc
$(OBJDIR)\AISDatabaseParser.obj : $(SRCDIR)\AISDatabaseParser.cc
$(OBJDIR)\AISMerge.obj : $(SRCDIR)\AISMerge.cc
$(OBJDIR)\AISConnect.obj : $(SRCDIR)\AISConnect.cc
$(OBJDIR)\Array.obj : $(SRCDIR)\Array.cc
$(OBJDIR)\DDS.obj : $(SRCDIR)\DDS.cc
$(OBJDIR)\DataDDS.obj : $(SRCDIR)\DataDDS.cc
$(OBJDIR)\BaseType.obj : $(SRCDIR)\BaseType.cc
$(OBJDIR)\Byte.obj : $(SRCDIR)\Byte.cc
$(OBJDIR)\Int32.obj : $(SRCDIR)\Int32.cc
$(OBJDIR)\Float64.obj : $(SRCDIR)\Float64.cc
$(OBJDIR)\Str.obj : $(SRCDIR)\Str.cc
$(OBJDIR)\Url.obj : $(SRCDIR)\Url.cc
$(OBJDIR)\Vector.obj : $(SRCDIR)\Vector.cc
$(OBJDIR)\Array.obj : $(SRCDIR)\Array.cc
$(OBJDIR)\Structure.obj : $(SRCDIR)\Structure.cc
$(OBJDIR)\Sequence.obj : $(SRCDIR)\Sequence.cc
$(OBJDIR)\Grid.obj : $(SRCDIR)\Grid.cc
$(OBJDIR)\UInt32.obj : $(SRCDIR)\UInt32.cc
$(OBJDIR)\util.obj : $(SRCDIR)\util.cc
$(OBJDIR)\ce_functions.obj : $(SRCDIR)\ce_functions.cc
$(OBJDIR)\GSEClause.obj : $(SRCDIR)\GSEClause.cc
$(OBJDIR)\Int16.obj : $(SRCDIR)\Int16.cc
$(OBJDIR)\UInt16.obj : $(SRCDIR)\UInt16.cc
$(OBJDIR)\Float32.obj : $(SRCDIR)\Float32.cc
$(OBJDIR)\Constructor.obj : $(SRCDIR)\Constructor.cc
$(OBJDIR)\VirtualCtorByte.obj : $(SRCDIR)\VirtualCtorByte.cc
$(OBJDIR)\VirtualCtorInt16.obj : $(SRCDIR)\VirtualCtorInt16.cc
$(OBJDIR)\VirtualCtorUInt16.obj : $(SRCDIR)\VirtualCtorUInt16.cc
$(OBJDIR)\VirtualCtorInt32.obj : $(SRCDIR)\VirtualCtorInt32.cc
$(OBJDIR)\VirtualCtorUInt32.obj : $(SRCDIR)\VirtualCtorUInt32.cc
$(OBJDIR)\VirtualCtorFloat32.obj : $(SRCDIR)\VirtualCtorFloat32.cc
$(OBJDIR)\VirtualCtorFloat64.obj : $(SRCDIR)\VirtualCtorFloat64.cc
$(OBJDIR)\VirtualCtorStr.obj : $(SRCDIR)\VirtualCtorStr.cc
$(OBJDIR)\VirtualCtorUrl.obj : $(SRCDIR)\VirtualCtorUrl.cc
$(OBJDIR)\VirtualCtorArray.obj : $(SRCDIR)\VirtualCtorArray.cc
$(OBJDIR)\VirtualCtorStructure.obj : $(SRCDIR)\VirtualCtorStructure.cc
$(OBJDIR)\VirtualCtorSequence.obj : $(SRCDIR)\VirtualCtorSequence.cc
$(OBJDIR)\VirtualCtorGrid.obj : $(SRCDIR)\VirtualCtorGrid.cc
$(OBJDIR)\util_mit.obj : $(SRCDIR)\util_mit.cc
$(OBJDIR)\IteratorAdapter.obj : $(SRCDIR)\IteratorAdapter.cc
$(OBJDIR)\ArrayIterAdapter.obj : $(SRCDIR)\ArrayIterAdapter.cc
$(OBJDIR)\AttrIterAdapter.obj : $(SRCDIR)\AttrIterAdapter.cc
$(OBJDIR)\BTIterAdapter.obj : $(SRCDIR)\BTIterAdapter.cc
$(OBJDIR)\ClauseIterAdapter.obj : $(SRCDIR)\ClauseIterAdapter.cc
$(OBJDIR)\Error.obj : $(SRCDIR)\Error.cc
$(OBJDIR)\InternalErr.obj : $(SRCDIR)\InternalErr.cc
$(OBJDIR)\ResponseTooBigErr.obj : $(SRCDIR)\ResponseTooBigErr.cc
$(OBJDIR)\Clause.obj : $(SRCDIR)\Clause.cc
$(OBJDIR)\RValue.obj : $(SRCDIR)\RValue.cc
$(OBJDIR)\cgi_util.obj : $(SRCDIR)\cgi_util.cc
$(OBJDIR)\escaping.obj : $(SRCDIR)\escaping.cc
$(OBJDIR)\DODSFilter.obj : $(SRCDIR)\DODSFilter.cc
$(OBJDIR)\Connections.obj : $(SRCDIR)\Connections.cc
$(OBJDIR)\RCReader.obj : $(SRCDIR)\RCReader.cc
$(OBJDIR)\Connect.obj : $(SRCDIR)\Connect.cc
$(OBJDIR)\HTTPConnect.obj : $(SRCDIR)\HTTPConnect.cc
$(OBJDIR)\HTTPCache.obj : $(SRCDIR)\HTTPCache.cc

# Objects from Bison/Lex generated source
$(OBJDIR)\das.tab.obj   : $(SRCDIR)\das.tab.c
$(OBJDIR)\dds.tab.obj   : $(SRCDIR)\dds.tab.c
$(OBJDIR)\expr.tab.obj  : $(SRCDIR)\expr.tab.c
$(OBJDIR)\Error.tab.obj : $(SRCDIR)\Error.tab.c
$(OBJDIR)\gse.tab.obj   : $(SRCDIR)\gse.tab.c
$(OBJDIR)\lex.das.obj   : $(SRCDIR)\lex.das.c
$(OBJDIR)\lex.dds.obj   : $(SRCDIR)\lex.dds.c
$(OBJDIR)\lex.Error.obj : $(SRCDIR)\lex.Error.c
$(OBJDIR)\lex.expr.obj  : $(SRCDIR)\lex.expr.c
$(OBJDIR)\lex.gse_.obj  : $(SRCDIR)\lex.gse_.c

# Source generated by compiler tools
$(SRCDIR)\lex.das.c:	$(SRCDIR)\das.lex $(SRCDIR)\das.tab.c $(SRCDIR)\das.tab.h
	$(LEX) -Pdas $(SRCDIR)\das.lex
	copy lex.das.c $(SRCDIR)\lex.das.c
	del lex.das.c

$(SRCDIR)\das.tab.c $(SRCDIR)\das.tab.h: $(SRCDIR)\das.y $(SRCDIR)\DAS.h
	$(YACC) $(YACCFLAGS) -p das $(SRCDIR)\das.y

$(SRCDIR)\lex.dds.c:	$(SRCDIR)\dds.lex $(SRCDIR)\dds.tab.c $(SRCDIR)\dds.tab.h
	$(LEX) -Pdds $(SRCDIR)\dds.lex
	copy lex.dds.c $(SRCDIR)\lex.dds.c
	del lex.dds.c

$(SRCDIR)\dds.tab.c $(SRCDIR)\dds.tab.h: $(SRCDIR)\dds.y
	$(YACC) $(YACCFLAGS) -p dds $(SRCDIR)\dds.y

$(SRCDIR)\lex.expr.c:	$(SRCDIR)\expr.lex $(SRCDIR)\expr.tab.c $(SRCDIR)\expr.tab.h
	$(LEX) -Pexpr $(SRCDIR)\expr.lex
	copy lex.expr.c $(SRCDIR)\lex.expr.c
	del lex.expr.c

$(SRCDIR)\expr.tab.c $(SRCDIR)\expr.tab.h: $(SRCDIR)\expr.y
	$(YACC) $(YACCFLAGS) -p expr $(SRCDIR)\expr.y

$(SRCDIR)\lex.Error.c:	$(SRCDIR)\Error.lex $(SRCDIR)\Error.tab.c $(SRCDIR)\Error.tab.h
	$(LEX) -PError $(SRCDIR)\Error.lex
	copy lex.Error.c $(SRCDIR)\lex.Error.c
	del lex.Error.c

$(SRCDIR)\Error.tab.c $(SRCDIR)\Error.tab.h: $(SRCDIR)\Error.y
	$(YACC) $(YACCFLAGS) -p Error $(SRCDIR)\Error.y

$(SRCDIR)\lex.gse_.c:	$(SRCDIR)\gse.lex $(SRCDIR)\gse.tab.c $(SRCDIR)\gse.tab.h
	$(LEX) -Pgse_ $(SRCDIR)\gse.lex
	copy lex.gse_.c $(SRCDIR)\lex.gse_.c
	del lex.gse_.c

$(SRCDIR)\gse.tab.c $(SRCDIR)\gse.tab.h: $(SRCDIR)\gse.y
	$(YACC) $(YACCFLAGS) -p gse_ $(SRCDIR)\gse.y

# GNU Subdirectory.  There exists an object "Error.obj" that
# (accounting for lack of case sensitivity in win32) conflicts with
# "error.obj" and makes the dependancies rules somewhat more
# complicated as a result.  This conflict, along with a similar
# conflict between "Error.cc" and "error.cc" is what makes the
# following explicit dependancies necessary.
$(OBJDIR)\GetOpt.obj : $(SRCDIR)\GNU\GetOpt.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\GetOpt.cc
$(OBJDIR)\gnu_error.obj : $(SRCDIR)\GNU\error.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\error.cc
$(OBJDIR)\gnu_regex.obj : $(SRCDIR)\GNU\Regex.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\Regex.cc
$(OBJDIR)\Regex.obj : $(SRCDIR)\regex-0.12\regex.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\regex-0.12\regex.c

$(OBJDIR)\geturl.exe: $(OBJDIR)\geturl.obj

$(OBJDIR)\usage.exe: $(OBJDIR)\usage.obj 

$(OBJDIR)\deflate.exe: $(OBJDIR)\deflate.obj 

$(OBJDIR)\ThreadTest.exe: $(OBJDIR)\ThreadTest.obj

# Command-Line Client and select utils in console mode
$(OBJDIR)\geturl.obj    : $(SRCDIR)\geturl.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\geturl.cc

$(OBJDIR)\usage.obj    : $(SRCDIR)\usage.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\usage.cc

$(OBJDIR)\deflate.obj    : $(SRCDIR)\deflate.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\deflate.c

# Test Suite Objects.
$(OBJDIR)\das-test.obj   : $(SRCDIR)\das-test.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\das-test.cc
$(OBJDIR)\dds-test.obj   : $(SRCDIR)\dds-test.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\dds-test.cc
$(OBJDIR)\error-test.obj : $(SRCDIR)\error-test.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\error-test.cc
$(OBJDIR)\expr-test.obj  : $(SRCDIR)\expr-test.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\expr-test.cc

$(OBJDIR)\das-test.exe		: $(OBJDIR)\das-test.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\das-test.obj $(LIBS)

$(OBJDIR)\dds-test.exe		: $(OBJDIR)\dds-test.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\dds-test.obj $(LIBS)

$(OBJDIR)\error-test.exe	: $(OBJDIR)\error-test.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\error-test.obj $(LIBS)

# expr-test will link without the TESTOBJS.  However, a side-effect
# of failing to link with those is that expr-test will fail during
# running of the core testsuite.
# ROM - 12/03.
$(OBJDIR)\expr-test.exe		: $(OBJDIR)\expr-test.obj $(TESTOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\expr-test.obj $(TESTOBJS) $(LIBS)

# cppUnit-based Unit test objects
UNIT_TEST_PROGS = $(OBJDIR)\parserUtilTest.exe \
		$(OBJDIR)\generalUtilTest.exe \
		$(OBJDIR)\DASTest.exe \
		$(OBJDIR)\DDSTest.exe \
		$(OBJDIR)\ByteTest.exe \
		$(OBJDIR)\SequenceTest.exe \
		$(OBJDIR)\AttrTableTest.exe \
		$(OBJDIR)\cgiUtilTest.exe \
		$(OBJDIR)\HTTPCacheTest.exe \
		$(OBJDIR)\HTTPConnectTest.exe

unit_check: $(UNIT_TEST_PROGS)
#	$(OBJDIR)\cgiUtilTest
##	$(OBJDIR)\HTTPConnectTest
#	$(OBJDIR)\HTTPCacheTest
#	$(OBJDIR)\SequenceTest
#	$(OBJDIR)\AttrTableTest
#	$(OBJDIR)\DDSTest
#	$(OBJDIR)\ByteTest
#	$(OBJDIR)\DASTest
#	$(OBJDIR)\generalUtilTest
#	$(OBJDIR)\parserUtilTest

$(OBJDIR)\parserUtilTest.exe	: $(OBJDIR)\parserUtilTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\parserUtilTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\generalUtilTest.exe	: $(OBJDIR)\generalUtilTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\generalUtilTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\DASTest.exe		: $(OBJDIR)\DASTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\DASTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\ByteTest.exe		: $(OBJDIR)\ByteTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\ByteTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\SequenceTest.exe	: $(OBJDIR)\SequenceTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\SequenceTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\AttrTableTest.exe	: $(OBJDIR)\AttrTableTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\AttrTableTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\DDSTest.exe		: $(OBJDIR)\DDSTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\DDSTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\cgiUtilTest.exe	: $(OBJDIR)\cgiUtilTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\cgiUtilTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\HTTPCacheTest.exe	: $(OBJDIR)\HTTPCacheTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\HTTPCacheTest.obj $(CPPUNITLIBS) $(LIBS)
$(OBJDIR)\HTTPConnectTest.exe	: $(OBJDIR)\HTTPConnectTest.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\HTTPConnectTest.obj $(CPPUNITLIBS) $(LIBS)

$(OBJDIR)\parserUtilTest.obj	: ..\parserUtilTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\parserUtilTest.cc
$(OBJDIR)\generalUtilTest.obj	: ..\generalUtilTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\generalUtilTest.cc
$(OBJDIR)\DASTest.obj		: ..\DASTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\DASTest.cc
$(OBJDIR)\ByteTest.obj		: ..\ByteTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\ByteTest.cc
$(OBJDIR)\SequenceTest.obj	: ..\SequenceTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\SequenceTest.cc
$(OBJDIR)\AttrTableTest.obj	: ..\AttrTableTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\AttrTableTest.cc
$(OBJDIR)\DDSTest.obj		: ..\DDSTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\DDSTest.cc
$(OBJDIR)\cgiUtilTest.obj	: ..\cgiUtilTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\cgiUtilTest.cc
$(OBJDIR)\HTTPCacheTest.obj	: ..\HTTPCacheTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\HTTPCacheTest.cc
$(OBJDIR)\HTTPConnectTest.obj	: ..\HTTPConnectTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\HTTPConnectTest.cc

$(OBJDIR)\xdr.obj : $(SRCDIR)\xdr.c
$(OBJDIR)\xdr_array.obj : $(SRCDIR)\xdr_array.c
$(OBJDIR)\xdr_float.obj : $(SRCDIR)\xdr_float.c
$(OBJDIR)\xdr_mem.obj : $(SRCDIR)\xdr_mem.c
$(OBJDIR)\xdr_rec.obj : $(SRCDIR)\xdr_rec.c
$(OBJDIR)\xdr_rec2.obj : $(SRCDIR)\xdr_rec2.c
$(OBJDIR)\xdr_reference.obj : $(SRCDIR)\xdr_reference.c
$(OBJDIR)\xdr_stdio.obj : $(SRCDIR)\xdr_stdio.c
$(OBJDIR)\xdrtest.obj: $(SRCDIR)\xdrtest.c

# Preliminary setup.  Create the install and intermediate file directories
# if they don't exist.  We also have to create a temporary unistd.h because
# bison and/or flex create source that includes that - even though
# there is no such header under win32 and it is unnecessary there
# (that is, even though these utils can run under win32 - by definition
# they generate output for the UNIX world).  This stupid pet trick
# also properly defines isatty() for the generated source under win32.
setup:
	@-mkdir $(OBJDIR)
	@-mkdir $(INSTALLLIBDIR)
	@-mkdir $(INSTALLBINDIR)
	@-mkdir $(INSTALLETCDIR)
	@-mkdir $(INSTALLDLLDIR)
	@-mkdir $(INSTALLINCDIR)
	@-mkdir $(INSTALLINCDIR)\GNU
	@-mkdir $(INSTALLINCDIR)\regex-0.12
	@-echo /*  Dummy patch header to get win32 flex/bison generated  */ > unistd.h
	@-echo /*  source code to compile under win32.  If you see this  */ >> unistd.h
	@-echo /*  temp file, feel free to scratch it.                   */ >> unistd.h
	@-echo #include "io.h" >> unistd.h

install: installST installMT installMD installSTD installMTD installMDD
# We use the Singly-threaded static versions by default for the
# executables.
	copy $(OBJDIR)\ST\geturl.exe $(INSTALLBINDIR)

	copy $(OBJDIR)\ST\usage.exe $(INSTALLETCDIR)
	copy $(OBJDIR)\ST\deflate.exe $(INSTALLETCDIR)

	copy config_dap.h $(INSTALLINCDIR)
	copy dods-datatypes.h $(INSTALLINCDIR)
	copy $(SRCDIR)\*.h $(INSTALLINCDIR)
	copy $(SRCDIR)\Connections.cc $(INSTALLINCDIR)

	copy $(SRCDIR)\GNU\*.h $(INSTALLINCDIR)\GNU
	copy $(SRCDIR)\regex-0.12\*.h $(INSTALLINCDIR)\regex-0.12

installST:
	@-copy $(OBJDIR)\ST\libdapST.lib $(INSTALLLIBDIR)

installMT:
	@-copy $(OBJDIR)\MT\libdapMT.lib $(INSTALLLIBDIR)

installMD:
	@-copy $(OBJDIR)\MD\libdapMD.lib $(INSTALLLIBDIR)
	@-copy $(OBJDIR)\MD\libdapMD.dll $(INSTALLDLLDIR)

installSTD:
	@-copy $(OBJDIR)\STD\libdapSTD.lib $(INSTALLLIBDIR)

installMTD:
	@-copy $(OBJDIR)\MTD\libdapMTD.lib $(INSTALLLIBDIR)

installMDD:
	@-copy $(OBJDIR)\MDD\libdapMDD.lib $(INSTALLLIBDIR)
	@-copy $(OBJDIR)\MDD\libdapMDD.dll $(INSTALLDLLDIR)

clean:
	@cd libxdr
	@-nmake clean
	@-cd ..
	@-rmdir /Q /S $(OBJDIR)
	@-del *.pdb
	@-del unistd.h

grammarclean:
	@-del ..\lex.das.c 
	@-del ..\das.tab.c
	@-del ..\das.tab.h
	@-del ..\lex.dds.c
	@-del ..\dds.tab.c
	@-del ..\dds.tab.h
	@-del ..\lex.expr.c
	@-del ..\expr.tab.c
	@-del ..\expr.tab.h
	@-del ..\lex.Error.c
	@-del ..\Error.tab.c
	@-del ..\Error.tab.h
	@-del ..\lex.gse_.c
	@-del ..\gse.tab.c
	@-del ..\gse.tab.h

!INCLUDE "dependancies"



