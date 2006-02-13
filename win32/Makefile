# Makefile for libdap (under VC++ on win32)

# Setup some preliminary directories
OBJDIR		= .\objects
SRCDIR		= ..
TESTSRCDIR	= ..\tests
INSTALLLIBDIR	= ..\..\prepkg\lib
INSTALLDLLDIR	= ..\..\prepkg\dll
INSTALLBINDIR	= ..\..\prepkg\bin
INSTALLETCDIR	= ..\..\prepkg\etc
INSTALLINCDIR	= ..\..\prepkg\include

INCLUDEDIR	= ..\..\prepkg\include

# Compiler, Linker
CC		= cl
LINK		= link

# Compiler tools
LEX	= flex
YACC	= bison

# We suppress #line directives under win32 to prevent warnings resulting
# when a "\" followed by some char is interpreted as an escape sequence.
YACCFLAGS	= --no-lines -d -v

# MT = Multi-Threaded Model
# MD = Multi-Threaded DLL Model

# Setup compiler flags
MTCFLAGS	= /MT /D HAVE_PTHREAD_H
MDCFLAGS	= /MD /D HAVE_PTHREAD_H
CFLAGS		= /nologo /W0 /GR /EHsc /c

# Setup linker flags for libdap
MTLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MDLFLAGS	= -libpath:$(INSTALLLIBDIR) /dll /def:libdap.def /map /fixed:NO /base:0x06000000

# Setup link libraries for libdap
MTLIBS		=
MDLIBS		= libxml2.lib libcurl.lib zlib.lib libxdrMD.lib pthreadVC.lib

# Setup linker flags for executables
EMTLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR) /NODEFAULTLIB:MSVCRT.lib
EMDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR) /NODEFAULTLIB:MSVCRT.lib

# Setup link libraries for executables
EMTLLIBS	= libdapMT.lib libxml2.lib libcurl.lib zlib.lib libxdrMT.lib pthreadVC.lib
EMDLLIBS	= libdapMD.lib libxml2.lib libcurl.lib zlib.lib libxdrMD.lib pthreadVC.lib
CPPUNITLIBS	= cppunit_dll.lib

# Setup compiler includes and defines
INCS		= /I . /I .. /I ..\GNU /I ..\gl /I $(INCLUDEDIR) /I $(INCLUDEDIR)\pthreads /I $(INCLUDEDIR)\libxml2
CPPUNITINCS	= $(INCS) /I $(INCLUDEDIR)/cppunit
DEFS		= /D WIN32 /D WIN32_LEAN_AND_MEAN /D DVR=\"DODS/3.4.8\"
CPPFLAGS	= $(DEFS) $(INCS)

default:
	@echo "usage: nmake all; nmake install"
	@echo "or"
	@echo "usage: nmake model<MT|MD>; nmake install<MT|MD>"

all:	setup
	nmake modelMT
	nmake modelMD

modelMT: setup
	@-md $(OBJDIR)\MT
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(MTLFLAGS)" LIBS="$(MTLIBS)" OBJDIR=$(OBJDIR)\MT libdapMT.lib
	nmake installMT
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT getdap
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT deflate
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT check
#	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT unit_check

modelMD: setup
	@-md $(OBJDIR)\MD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(MDLFLAGS)" LIBS="$(MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapMD.dll
	nmake installMD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD getdap
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD deflate
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD check
#	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD unit_check

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .c .cc .exe

# Rules for library objects, both the .c and .cc's are C++
{$(SRCDIR)}.c{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $<

{$(SRCDIR)}.cc{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $<

{$(TESTSRCDIR)}.cc{$(OBJDIR)}.obj:
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

CLIENTOBJS = $(OBJDIR)\RCReader.obj \
		$(OBJDIR)\Connect.obj \
		$(OBJDIR)\HTTPConnect.obj \
		$(OBJDIR)\HTTPCache.obj

TYPEOBJS = $(OBJDIR)\BaseType.obj \
		$(OBJDIR)\BaseTypeFactory.obj \
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
		$(GSE_GRAM_OBJS) \
		$(OBJDIR)\Int16.obj \
		$(OBJDIR)\UInt16.obj \
		$(OBJDIR)\Float32.obj \
		$(OBJDIR)\Constructor.obj \
		$(OBJDIR)\util_mit.obj

TESTOBJS = $(OBJDIR)\TestByte.obj \
		$(OBJDIR)\TestInt32.obj \
# Issue with trunc()
#		$(OBJDIR)\TestFloat64.obj \
		$(OBJDIR)\TestStr.obj \
		$(OBJDIR)\TestUrl.obj \
		$(OBJDIR)\TestArray.obj \
		$(OBJDIR)\TestStructure.obj \
		$(OBJDIR)\TestSequence.obj \
		$(OBJDIR)\TestGrid.obj \
		$(OBJDIR)\TestUInt32.obj \
		$(OBJDIR)\TestInt16.obj \
		$(OBJDIR)\TestUInt16.obj \
# Issue with trunc()
#		$(OBJDIR)\TestFloat32.obj \
		$(OBJDIR)\TestCommon.obj \
		$(OBJDIR)\TestTypeFactory.obj

GNUOBJS = $(OBJDIR)\GetOpt.obj \
		$(OBJDIR)\GNURegex.obj \
		$(OBJDIR)\GNUerror.obj \
		$(OBJDIR)\regex.obj \
		$(OBJDIR)\strcasecmp.obj \
		$(OBJDIR)\strncasecmp.obj \
		$(OBJDIR)\strnlen1.obj

AISOBJS = $(OBJDIR)\AISResources.obj \
		$(OBJDIR)\AISDatabaseParser.obj \
		$(OBJDIR)\AISMerge.obj \
		$(OBJDIR)\AISConnect.obj

LIBOBJS = $(DASOBJS) $(DDSOBJS) $(TYPEOBJS) $(GNUOBJS) $(ERROROBJS) $(EXPROBJS) \
		$(UTILOBJS) $(CLIENTOBJS) $(AISOBJS)

# Convenience dependancies
libdapMT.lib:	$(OBJDIR)\libdapMT.lib
libdapMD.dll:	$(OBJDIR)\libdapMD.dll

getdap:		$(OBJDIR)\getdap.exe
deflate:	$(OBJDIR)\deflate.exe
# Removed expr-test temporarily as workaround for trunc() issue
#check: 		$(OBJDIR)\das-test.exe $(OBJDIR)\dds-test.exe $(OBJDIR)\error-test.exe $(OBJDIR)\expr-test.exe
check: 		$(OBJDIR)\das-test.exe $(OBJDIR)\dds-test.exe $(OBJDIR)\error-test.exe
		@echo Notice:
		@echo Core Test programs have been built, but they must be
		@echo executed using DejaGnu in a Cygwin environment.  If
		@echo validation of the Win32 Dods core is required, install
		@echo Cygwin w/Dejagnu and move the testsuites there in order
		@echo to run them.  Optimally, the testsuite should be ran once
		@echo for each of the 4 library models.

# Link the library
$(OBJDIR)\libdapMT.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libdapMD.dll: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

grammars: $(SRCDIR)\lex.das.c $(SRCDIR)\das.tab.h $(SRCDIR)\lex.dds.c $(SRCDIR)\dds.tab.h \
		$(SRCDIR)\lex.expr.c $(SRCDIR)\expr.tab.h $(SRCDIR)\lex.Error.c $(SRCDIR)\Error.tab.h \
		$(SRCDIR)\lex.gse_.c $(SRCDIR)\gse.tab.h

# The DAP Proper
$(OBJDIR)\AISResources.obj	: $(SRCDIR)\AISResources.cc
$(OBJDIR)\AISDatabaseParser.obj : $(SRCDIR)\AISDatabaseParser.cc
$(OBJDIR)\AISMerge.obj		: $(SRCDIR)\AISMerge.cc
$(OBJDIR)\AISConnect.obj	: $(SRCDIR)\AISConnect.cc
$(OBJDIR)\Array.obj		: $(SRCDIR)\Array.cc
$(OBJDIR)\DDS.obj		: $(SRCDIR)\DDS.cc
$(OBJDIR)\DataDDS.obj		: $(SRCDIR)\DataDDS.cc
$(OBJDIR)\BaseType.obj		: $(SRCDIR)\BaseType.cc
$(OBJDIR)\BaseTypeFactory.obj	: $(SRCDIR)\BaseTypeFactory.cc
$(OBJDIR)\Byte.obj		: $(SRCDIR)\Byte.cc
$(OBJDIR)\Int32.obj		: $(SRCDIR)\Int32.cc
$(OBJDIR)\Float64.obj		: $(SRCDIR)\Float64.cc
$(OBJDIR)\Str.obj		: $(SRCDIR)\Str.cc
$(OBJDIR)\Url.obj		: $(SRCDIR)\Url.cc
$(OBJDIR)\Vector.obj		: $(SRCDIR)\Vector.cc
$(OBJDIR)\List.obj		: $(SRCDIR)\List.cc
$(OBJDIR)\Array.obj		: $(SRCDIR)\Array.cc
$(OBJDIR)\Structure.obj		: $(SRCDIR)\Structure.cc
$(OBJDIR)\Sequence.obj		: $(SRCDIR)\Sequence.cc
$(OBJDIR)\Grid.obj		: $(SRCDIR)\Grid.cc
$(OBJDIR)\UInt32.obj		: $(SRCDIR)\UInt32.cc
$(OBJDIR)\util.obj		: $(SRCDIR)\util.cc
$(OBJDIR)\ce_functions.obj	: $(SRCDIR)\ce_functions.cc
$(OBJDIR)\GSEClause.obj		: $(SRCDIR)\GSEClause.cc
$(OBJDIR)\Int16.obj		: $(SRCDIR)\Int16.cc
$(OBJDIR)\UInt16.obj		: $(SRCDIR)\UInt16.cc
$(OBJDIR)\Float32.obj		: $(SRCDIR)\Float32.cc
$(OBJDIR)\Constructor.obj	: $(SRCDIR)\Constructor.cc
$(OBJDIR)\util_mit.obj		: $(SRCDIR)\util_mit.cc
$(OBJDIR)\Error.obj		: $(SRCDIR)\Error.cc
$(OBJDIR)\InternalErr.obj	: $(SRCDIR)\InternalErr.cc
$(OBJDIR)\ResponseTooBigErr.obj : $(SRCDIR)\ResponseTooBigErr.cc
$(OBJDIR)\Clause.obj		: $(SRCDIR)\Clause.cc
$(OBJDIR)\RValue.obj		: $(SRCDIR)\RValue.cc
$(OBJDIR)\cgi_util.obj		: $(SRCDIR)\cgi_util.cc
$(OBJDIR)\escaping.obj		: $(SRCDIR)\escaping.cc
$(OBJDIR)\DODSFilter.obj	: $(SRCDIR)\DODSFilter.cc
$(OBJDIR)\RCReader.obj		: $(SRCDIR)\RCReader.cc
$(OBJDIR)\Connect.obj		: $(SRCDIR)\Connect.cc
$(OBJDIR)\HTTPConnect.obj	: $(SRCDIR)\HTTPConnect.cc
$(OBJDIR)\HTTPCache.obj		: $(SRCDIR)\HTTPCache.cc

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

# Regular expression code
$(OBJDIR)\GetOpt.obj		: $(SRCDIR)\GNU\GetOpt.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\GetOpt.cc
$(OBJDIR)\GNUerror.obj		: $(SRCDIR)\GNU\GNUerror.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\GNUerror.cc
$(OBJDIR)\GNURegex.obj		: $(SRCDIR)\GNU\GNURegex.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\GNURegex.cc
$(OBJDIR)\regex.obj		: $(SRCDIR)\gl\regex.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\gl\regex.c
$(OBJDIR)\strcasecmp.obj	: $(SRCDIR)\gl\strcasecmp.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\gl\strcasecmp.c
$(OBJDIR)\strncasecmp.obj	: $(SRCDIR)\gl\strncasecmp.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\gl\strncasecmp.c
$(OBJDIR)\strnlen1.obj	: $(SRCDIR)\gl\strnlen1.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\gl\strnlen1.c

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

$(OBJDIR)\getdap.exe: $(OBJDIR)\getdap.obj

$(OBJDIR)\deflate.exe: $(OBJDIR)\deflate.obj 

# Command-Line Client and select utils in console mode
$(OBJDIR)\getdap.obj    : $(SRCDIR)\getdap.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\getdap.cc

$(OBJDIR)\deflate.obj    : $(SRCDIR)\deflate.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\deflate.c

# Test Suite Objects
$(OBJDIR)\das-test.obj   	: $(TESTSRCDIR)\das-test.cc
$(OBJDIR)\dds-test.obj   	: $(TESTSRCDIR)\dds-test.cc
$(OBJDIR)\expr-test.obj  	: $(TESTSRCDIR)\expr-test.cc
$(OBJDIR)\error-test.obj	: $(SRCDIR)\error-test.cc
$(OBJDIR)\TestByte.obj		: $(TESTSRCDIR)\TestByte.cc
$(OBJDIR)\TestInt32.obj		: $(TESTSRCDIR)\TestInt32.cc
# Issue with trunc()
#$(OBJDIR)\TestFloat64.obj	: $(TESTSRCDIR)\TestFloat64.cc
$(OBJDIR)\TestStr.obj		: $(TESTSRCDIR)\TestStr.cc
$(OBJDIR)\TestUrl.obj		: $(TESTSRCDIR)\TestUrl.cc
$(OBJDIR)\TestArray.obj		: $(TESTSRCDIR)\TestArray.cc
$(OBJDIR)\TestStructure.obj	: $(TESTSRCDIR)\TestStructure.cc
$(OBJDIR)\TestSequence.obj	: $(TESTSRCDIR)\TestSequence.cc
$(OBJDIR)\TestGrid.obj		: $(TESTSRCDIR)\TestGrid.cc
$(OBJDIR)\TestUInt32.obj	: $(TESTSRCDIR)\TestUInt32.cc
$(OBJDIR)\TestInt16.obj		: $(TESTSRCDIR)\TestInt16.cc
$(OBJDIR)\TestUInt16.obj	: $(TESTSRCDIR)\TestUInt16.cc
# Issue with trunc()
#$(OBJDIR)\TestFloat32.obj	: $(TESTSRCDIR)\TestFloat32.cc
$(OBJDIR)\TestCommon.obj	: $(TESTSRCDIR)\TestCommon.cc
$(OBJDIR)\TestTypeFactory.obj	: $(TESTSRCDIR)\TestTypeFactory.cc

$(OBJDIR)\das-test.exe		: $(OBJDIR)\das-test.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\das-test.obj $(LIBS)

$(OBJDIR)\dds-test.exe		: $(OBJDIR)\dds-test.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\dds-test.obj $(LIBS)

$(OBJDIR)\error-test.exe	: $(OBJDIR)\error-test.obj
	@$(LINK) $(LFLAGS) /out:$@ $(OBJDIR)\error-test.obj $(LIBS)

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

$(OBJDIR)\parserUtilTest.obj	: ..\unit-tests\parserUtilTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\parserUtilTest.cc
$(OBJDIR)\generalUtilTest.obj	: ..\unit-tests\generalUtilTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\generalUtilTest.cc
$(OBJDIR)\DASTest.obj		: ..\unit-tests\DASTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\DASTest.cc
$(OBJDIR)\ByteTest.obj		: ..\unit-tests\ByteTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\ByteTest.cc
$(OBJDIR)\SequenceTest.obj	: ..\unit-tests\SequenceTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\SequenceTest.cc
$(OBJDIR)\AttrTableTest.obj	: ..\unit-tests\AttrTableTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\AttrTableTest.cc
$(OBJDIR)\DDSTest.obj		: ..\unit-tests\DDSTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\DDSTest.cc
$(OBJDIR)\cgiUtilTest.obj	: ..\unit-tests\cgiUtilTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\cgiUtilTest.cc
$(OBJDIR)\HTTPCacheTest.obj	: ..\unit-tests\HTTPCacheTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\HTTPCacheTest.cc
$(OBJDIR)\HTTPConnectTest.obj	: ..\unit-tests\HTTPConnectTest.cc
	@$(CC) $(CFLAGS) $(CPPUNITINCS) $(CPPFLAGS) /Fo$@ /Tp ..\unit-tests\HTTPConnectTest.cc

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
	@-echo /*  Dummy patch header to get win32 flex/bison generated  */ > unistd.h
	@-echo /*  source code to compile under win32.  If you see this  */ >> unistd.h
	@-echo /*  temp file, feel free to scratch it.                   */ >> unistd.h
	@-echo #include "io.h" >> unistd.h

install: installMT installMD
	copy $(OBJDIR)\MT\getdap.exe $(INSTALLBINDIR)

	copy $(OBJDIR)\MT\usage.exe $(INSTALLETCDIR)
	copy $(OBJDIR)\MT\deflate.exe $(INSTALLETCDIR)

#	copy config.h $(INSTALLINCDIR)
	copy dods-datatypes.h $(INSTALLINCDIR)
	copy $(SRCDIR)\*.h $(INSTALLINCDIR)

	copy $(SRCDIR)\GNU\*.h $(INSTALLINCDIR)\GNU

installMT:
	@-copy $(OBJDIR)\MT\libdapMT.lib $(INSTALLLIBDIR)

installMD:
	@-copy $(OBJDIR)\MD\libdapMD.lib $(INSTALLLIBDIR)
	@-copy $(OBJDIR)\MD\libdapMD.dll $(INSTALLDLLDIR)

clean:
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



