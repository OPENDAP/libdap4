# Setup directories for intermediate files and built components
OBJDIR		= .\objects
INSTALLLIBDIR	= ..\..\..\lib
INSTALLEXEDIR	= ..\..\..\bin
INCLUDEDIR	= ..\..\..\include

# Setup basic compiler flags for all .obj's
CC		= cl
#CFLAGS		= /nologo /MTd /Zi /W3 /GR /GX /c	# flags for debugger
CFLAGS		= /nologo /MT /W3 /GR /GX /c

# Setup linker flags for libraries and .exe's
LINK		= link
LIBPATH		= /libpath:$(OBJDIR) /libpath:$(INSTALLLIBDIR)
#LEXEFLAGS	= /DEBUG /subsystem:console $(LIBPATH)	# flags for debugger
#LLIBFLAGS	= -lib					# flags for debugger
LEXEFLAGS	= /nologo /subsystem:console $(LIBPATH)
LLIBFLAGS	= -lib /nologo

DODSLIBS	= libdap++.lib pthreadVCE.lib libcurl.lib librx.lib libz.lib libxdr.lib
SYSLIBS		= ws2_32.lib advapi32.lib
LINKLIBS	= $(DODSLIBS) $(SYSLIBS)
CPPUNITLIBS	= $(LINKLIBS) cppunit_dll.lib

# Setup compiler tools.  BISON_HAIRY and BISON_SIMPLE environment variables
# must be set to the _full_ path for bison.simple and bison.hairy respectively.
# These tools can (as of 10/2001) be downloaded and installed from
# http://unxutils.sourceforge.net/UnxUtils.zip.  They must also be on
# the path.
LEX	= flex
YACC	= bison
AWK	= gawk

# Setup compiler includes and defines for libraries and .exe's
INCS		= /I . /I .. /I ..\GNU /I $(INCLUDEDIR) /I $(INCLUDEDIR)\pthreads
CPPUNITINCS	= $(INCS) /I $(INCLUDEDIR)/cppunit
DEFSLIB		= /D WIN32 /D _LIB /D _WINDOWS
DEFSEXE		= /D WIN32 /D _CONSOLE
CPPLIBFLAGS	= $(DEFSLIB) $(INCS)
CPPEXEFLAGS	= $(DEFSEXE) $(INCS)
CPPUNITFLAGS	= $(DEFSEXE) $(CPPUNITINCS)

# Combine compiler flags - for objects going into .obj's, exe's
LIBOBJFLAGS	= $(CFLAGS) $(CPPLIBFLAGS)
EXEOBJFLAGS	= $(CFLAGS) $(CPPEXEFLAGS)
CPPUNITOBJFLAGS	= $(CFLAGS) $(CPPUNITFLAGS)

# Define the targets
TARGETS		= $(OBJDIR)\libdap++.lib $(OBJDIR)\geturl.exe \
		  $(OBJDIR)\usage.exe $(OBJDIR)\deflate.exe \
		  $(OBJDIR)\ThreadTest.exe

# Define the testsuite objects
TESTTARGETS	= $(OBJDIR)\das-test.exe \
		  $(OBJDIR)\dds-test.exe \
		  $(OBJDIR)\error-test.exe \
		  $(OBJDIR)\expr-test.exe

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .c .cc .exe

# Rules for library objects, both the .c and .cc's are C++
{..}.c{$(OBJDIR)}.obj:
	@$(CC) $(LIBOBJFLAGS) /Fo$@ /Tp $<

{..}.cc{$(OBJDIR)}.obj:
	@$(CC) $(LIBOBJFLAGS) /Fo$@ /Tp $<

# Rule for linking console-mode executables
{$(OBJDIR)}.obj{$(OBJDIR)}.exe:
	@$(LINK) $(LEXEFLAGS) /out:$@ $< $(LINKLIBS)

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

TYPEOBJS = $(OBJDIR)\BaseType.obj \
		$(OBJDIR)\Byte.obj \
		$(OBJDIR)\Int32.obj \
		$(OBJDIR)\Float64.obj \
		$(OBJDIR)\Str.obj \
		$(OBJDIR)\Url.obj \
		$(OBJDIR)\Vector.obj \
		$(OBJDIR)\List.obj \
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
		$(OBJDIR)\VirtualCtorList.obj \
		$(OBJDIR)\VirtualCtorStructure.obj \
		$(OBJDIR)\VirtualCtorSequence.obj \
		$(OBJDIR)\VirtualCtorGrid.obj \
		$(OBJDIR)\util_mit.obj

GNUOBJS = $(OBJDIR)\GetOpt.obj \
		$(OBJDIR)\Regex.obj \
		$(OBJDIR)\gnu_error.obj \
		$(OBJDIR)\IteratorAdapter.obj \
		$(OBJDIR)\ArrayIterAdapter.obj \
		$(OBJDIR)\AttrIterAdapter.obj \
		$(OBJDIR)\BTIterAdapter.obj \
		$(OBJDIR)\ClauseIterAdapter.obj

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

LIBOBJS = $(DASOBJS) $(DDSOBJS) $(TYPEOBJS) $(GNUOBJS) $(ERROROBJS) $(EXPROBJS) \
		$(UTILOBJS) $(CLIENTOBJS)

all: build_libxdr setup grammars $(TARGETS) check
#all: build_libxdr setup grammars $(TARGETS) check unit_check

grammars: ..\lex.das.c ..\das.tab.h ..\lex.dds.c ..\dds.tab.h ..\lex.expr.c \
		..\expr.tab.h ..\lex.Error.c ..\Error.tab.h ..\lex.gse_.c \
		..\gse.tab.h

check: $(TESTTARGETS)
	@echo Notice:
	@echo Core Test programs have been built, but they must be
	@echo executed using DejaGnu in a Cygwin environment.  If
	@echo validation of the Win32 Dods core is required, install
	@echo Cygwin w/Dejagnu and move the testsuites there in order
	@echo to run them.

build_libxdr:
	@cd libxdr
	@nmake
	@nmake install
	@cd ..

$(OBJDIR)\libdap++.lib: $(LIBOBJS)
	$(LINK) $(LLIBFLAGS) /out:$@ $(LIBOBJS)

$(OBJDIR)\geturl.exe: $(OBJDIR)\libdap++.lib $(OBJDIR)\geturl.obj

$(OBJDIR)\usage.exe: $(OBJDIR)\libdap++.lib $(OBJDIR)\usage.obj 

$(OBJDIR)\deflate.exe: $(OBJDIR)\deflate.obj 

$(OBJDIR)\ThreadTest.exe:  $(OBJDIR)\libdap++.lib $(OBJDIR)\ThreadTest.obj

install:
	copy $(OBJDIR)\libdap*.lib $(INSTALLLIBDIR)
	copy $(OBJDIR)\geturl.exe $(INSTALLEXEDIR)
	copy $(OBJDIR)\usage.exe $(INSTALLEXEDIR)
	copy $(OBJDIR)\deflate.exe $(INSTALLEXEDIR)
	copy config_dap.h $(INCLUDEDIR)
	copy dods-datatypes.h $(INCLUDEDIR)
	copy ..\debug.h $(INCLUDEDIR)
	copy ..\dods-limits.h $(INCLUDEDIR)
	copy ..\cgi_util.h $(INCLUDEDIR)
	copy ..\escaping.h $(INCLUDEDIR)
	copy ..\DODSFilter.h $(INCLUDEDIR)
	copy ..\AttrTable.h $(INCLUDEDIR)
	copy ..\DAS.h $(INCLUDEDIR)
	copy ..\parser.h $(INCLUDEDIR)
	copy ..\DDS.h $(INCLUDEDIR)
	copy ..\DataDDS.h $(INCLUDEDIR)
	copy ..\expr.h $(INCLUDEDIR)
	copy ..\Clause.h $(INCLUDEDIR)
	copy ..\RValue.h $(INCLUDEDIR)
	copy ..\Error.h $(INCLUDEDIR)
	copy ..\InternalErr.h $(INCLUDEDIR)
	copy ..\ResponseTooBigErr.h $(INCLUDEDIR)
	copy ..\Connections.h $(INCLUDEDIR)
	copy ..\Connections.cc $(INCLUDEDIR)
	copy ..\RCReader.h $(INCLUDEDIR)
	copy ..\Connect.h $(INCLUDEDIR)
	copy ..\HTTPConnect.h $(INCLUDEDIR)
	copy ..\ObjectType.h $(INCLUDEDIR)
	copy ..\EncodingType.h $(INCLUDEDIR)
	copy ..\HTTPCache.h $(INCLUDEDIR)
	copy ..\HTTPCacheDisconnectedMode.h $(INCLUDEDIR)
	copy ..\BaseType.h $(INCLUDEDIR)
	copy ..\Byte.h $(INCLUDEDIR)
	copy ..\Int32.h $(INCLUDEDIR)
	copy ..\GSEClause.h $(INCLUDEDIR)
	copy ..\Constructor.h $(INCLUDEDIR)
	copy ..\Float64.h $(INCLUDEDIR)
	copy ..\Str.h $(INCLUDEDIR)
	copy ..\Url.h $(INCLUDEDIR)
	copy ..\Vector.h $(INCLUDEDIR)
	copy ..\List.h $(INCLUDEDIR)
	copy ..\Array.h $(INCLUDEDIR)
	copy ..\Structure.h $(INCLUDEDIR)
	copy ..\Sequence.h $(INCLUDEDIR)
	copy ..\Grid.h $(INCLUDEDIR)
	copy ..\UInt32.h $(INCLUDEDIR)
	copy ..\util.h $(INCLUDEDIR)
	copy ..\Int16.h $(INCLUDEDIR)
	copy ..\UInt16.h $(INCLUDEDIR)
	copy ..\Float32.h $(INCLUDEDIR)
	copy ..\util_mit.h $(INCLUDEDIR)
	copy ..\GNU\Pix.h $(INCLUDEDIR)
	copy ..\GNU\gnu-std.h $(INCLUDEDIR)
	copy ..\GNU\GetOpt.h $(INCLUDEDIR)
	copy ..\GNU\Regex.h $(INCLUDEDIR)
	copy ..\GNU\builtin.h $(INCLUDEDIR)
	copy ..\IteratorAdapter.h $(INCLUDEDIR)
	copy ..\ArrayIterAdapter.h $(INCLUDEDIR)
	copy ..\AttrIterAdapter.h $(INCLUDEDIR)
	copy ..\BTIterAdapter.h $(INCLUDEDIR)
	copy ..\ClauseIterAdapter.h $(INCLUDEDIR)

# The DAP Proper
$(OBJDIR)\Array.obj : ..\Array.cc
$(OBJDIR)\DDS.obj : ..\DDS.cc
$(OBJDIR)\DataDDS.obj : ..\DataDDS.cc
$(OBJDIR)\BaseType.obj : ..\BaseType.cc
$(OBJDIR)\Byte.obj : ..\Byte.cc
$(OBJDIR)\Int32.obj : ..\Int32.cc
$(OBJDIR)\Float64.obj : ..\Float64.cc
$(OBJDIR)\Str.obj : ..\Str.cc
$(OBJDIR)\Url.obj : ..\Url.cc
$(OBJDIR)\Vector.obj : ..\Vector.cc
$(OBJDIR)\List.obj : ..\List.cc
$(OBJDIR)\Array.obj : ..\Array.cc
$(OBJDIR)\Structure.obj : ..\Structure.cc
$(OBJDIR)\Sequence.obj : ..\Sequence.cc
$(OBJDIR)\Grid.obj : ..\Grid.cc
$(OBJDIR)\UInt32.obj : ..\UInt32.cc
$(OBJDIR)\util.obj : ..\util.cc
$(OBJDIR)\ce_functions.obj : ..\ce_functions.cc
$(OBJDIR)\GSEClause.obj : ..\GSEClause.cc
$(OBJDIR)\Int16.obj : ..\Int16.cc
$(OBJDIR)\UInt16.obj : ..\UInt16.cc
$(OBJDIR)\Float32.obj : ..\Float32.cc
$(OBJDIR)\Constructor.obj : ..\Constructor.cc
$(OBJDIR)\VirtualCtorByte.obj : ..\VirtualCtorByte.cc
$(OBJDIR)\VirtualCtorInt16.obj : ..\VirtualCtorInt16.cc
$(OBJDIR)\VirtualCtorUInt16.obj : ..\VirtualCtorUInt16.cc
$(OBJDIR)\VirtualCtorInt32.obj : ..\VirtualCtorInt32.cc
$(OBJDIR)\VirtualCtorUInt32.obj : ..\VirtualCtorUInt32.cc
$(OBJDIR)\VirtualCtorFloat32.obj : ..\VirtualCtorFloat32.cc
$(OBJDIR)\VirtualCtorFloat64.obj : ..\VirtualCtorFloat64.cc
$(OBJDIR)\VirtualCtorStr.obj : ..\VirtualCtorStr.cc
$(OBJDIR)\VirtualCtorUrl.obj : ..\VirtualCtorUrl.cc
$(OBJDIR)\VirtualCtorArray.obj : ..\VirtualCtorArray.cc
$(OBJDIR)\VirtualCtorList.obj : ..\VirtualCtorList.cc
$(OBJDIR)\VirtualCtorStructure.obj : ..\VirtualCtorStructure.cc
$(OBJDIR)\VirtualCtorSequence.obj : ..\VirtualCtorSequence.cc
$(OBJDIR)\VirtualCtorGrid.obj : ..\VirtualCtorGrid.cc
$(OBJDIR)\util_mit.obj : ..\util_mit.cc
$(OBJDIR)\IteratorAdapter.obj : ..\IteratorAdapter.cc
$(OBJDIR)\ArrayIterAdapter.obj : ..\ArrayIterAdapter.cc
$(OBJDIR)\AttrIterAdapter.obj : ..\AttrIterAdapter.cc
$(OBJDIR)\BTIterAdapter.obj : ..\BTIterAdapter.cc
$(OBJDIR)\ClauseIterAdapter.obj : ..\ClauseIterAdapter.cc
$(OBJDIR)\Error.obj : ..\Error.cc
$(OBJDIR)\InternalErr.obj : ..\InternalErr.cc
$(OBJDIR)\ResponseTooBigErr.obj : ..\ResponseTooBigErr.cc
$(OBJDIR)\Clause.obj : ..\Clause.cc
$(OBJDIR)\RValue.obj : ..\RValue.cc
$(OBJDIR)\cgi_util.obj : ..\cgi_util.cc
$(OBJDIR)\escaping.obj : ..\escaping.cc
$(OBJDIR)\DODSFilter.obj : ..\DODSFilter.cc
$(OBJDIR)\Connections.obj : ..\Connections.cc
$(OBJDIR)\RCReader.obj : ..\RCReader.cc
$(OBJDIR)\Connect.obj : ..\Connect.cc
$(OBJDIR)\HTTPConnect.obj : ..\HTTPConnect.cc
$(OBJDIR)\HTTPCache.obj : ..\HTTPCache.cc

# Test objects
$(OBJDIR)\TestArray.obj : ..\TestArray.cc
$(OBJDIR)\TestByte.obj : ..\TestByte.cc
$(OBJDIR)\TestFloat32.obj : ..\TestFloat32.cc
$(OBJDIR)\TestFloat64.obj : ..\TestFloat64.cc
$(OBJDIR)\TestGrid.obj : ..\TestGrid.cc
$(OBJDIR)\TestInt16.obj : ..\TestInt16.cc
$(OBJDIR)\TestInt32.obj : ..\TestInt32.cc
$(OBJDIR)\TestList.obj : ..\TestList.cc
$(OBJDIR)\TestSequence.obj : ..\TestSequence.cc
$(OBJDIR)\TestStr.obj : ..\TestStr.cc
$(OBJDIR)\TestStructure.obj : ..\TestStructure.cc
$(OBJDIR)\TestUInt16.obj : ..\TestUInt16.cc
$(OBJDIR)\TestUInt32.obj : ..\TestUInt32.cc
$(OBJDIR)\TestUrl.obj : ..\TestUrl.cc

# Objects from Bison/Lex generated source
$(OBJDIR)\das.tab.obj   : ..\das.tab.c
$(OBJDIR)\dds.tab.obj   : ..\dds.tab.c
$(OBJDIR)\expr.tab.obj  : ..\expr.tab.c
$(OBJDIR)\Error.tab.obj : ..\Error.tab.c
$(OBJDIR)\gse.tab.obj   : ..\gse.tab.c
$(OBJDIR)\lex.das.obj   : ..\lex.das.c
$(OBJDIR)\lex.dds.obj   : ..\lex.dds.c
$(OBJDIR)\lex.Error.obj : ..\lex.Error.c
$(OBJDIR)\lex.expr.obj  : ..\lex.expr.c
$(OBJDIR)\lex.gse_.obj  : ..\lex.gse_.c

# Source generated by compiler tools
..\lex.das.c:	..\das.lex ..\das.tab.c ..\das.tab.h
	$(LEX) -Pdas ..\das.lex
	copy lex.das.c ..\lex.das.c
	del lex.das.c

..\das.tab.c ..\das.tab.h: ..\das.y ..\DAS.h
	$(YACC) -d -v -p das ..\das.y

..\lex.dds.c:	..\dds.lex ..\dds.tab.c ..\dds.tab.h
	$(LEX) -Pdds ..\dds.lex
	copy lex.dds.c ..\lex.dds.c
	del lex.dds.c

..\dds.tab.c ..\dds.tab.h: ..\dds.y
	$(YACC) -d -v -p dds ..\dds.y

..\lex.expr.c:	..\expr.lex ..\expr.tab.c ..\expr.tab.h
	$(LEX) -Pexpr ..\expr.lex
	copy lex.expr.c ..\lex.expr.c
	del lex.expr.c

..\expr.tab.c ..\expr.tab.h: ..\expr.y
	$(YACC) -d -v -p expr ..\expr.y

..\lex.Error.c:	..\Error.lex ..\Error.tab.c ..\Error.tab.h
	$(LEX) -PError ..\Error.lex
	copy lex.Error.c ..\lex.Error.c
	del lex.Error.c

..\Error.tab.c ..\Error.tab.h: ..\Error.y
	$(YACC) -d -v -p Error ..\Error.y

..\lex.gse_.c:	..\gse.lex ..\gse.tab.c ..\gse.tab.h
	$(LEX) -Pgse_ ..\gse.lex
	copy lex.gse_.c ..\lex.gse_.c
	del lex.gse_.c

..\gse.tab.c ..\gse.tab.h: ..\gse.y
	$(YACC) -d -v -p gse_ ..\gse.y


# GNU Subdirectory.  There exists an object "Error.obj" that
# (accounting for lack of case sensitivity in win32) conflicts with
# "error.obj" and makes the dependancies rules somewhat more
# complicated as a result.  This conflict, along with a similar
# conflict between "Error.cc" and "error.cc" is what makes the
# following explicit dependancies necessary.
$(OBJDIR)\GetOpt.obj : ..\GNU\GetOpt.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\GNU\GetOpt.cc
$(OBJDIR)\Regex.obj : ..\GNU\Regex.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\GNU\Regex.cc
$(OBJDIR)\gnu_error.obj : ..\GNU\error.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\GNU\error.cc

# Command-Line Client and select utils in console mode
$(OBJDIR)\geturl.obj    : ..\geturl.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\geturl.cc

$(OBJDIR)\usage.obj    : ..\usage.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\usage.cc

$(OBJDIR)\deflate.obj    : ..\deflate.c
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tc ..\deflate.c

# Test Suite Objects.
$(OBJDIR)\das-test.obj   : ..\das-test.cc $(OBJDIR)\libdap++.lib
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\das-test.cc
$(OBJDIR)\dds-test.obj   : ..\dds-test.cc $(OBJDIR)\libdap++.lib
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\dds-test.cc
$(OBJDIR)\error-test.obj : ..\error-test.cc $(OBJDIR)\libdap++.lib
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\error-test.cc
$(OBJDIR)\expr-test.obj  : ..\expr-test.cc $(OBJDIR)\libdap++.lib
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\expr-test.cc

$(OBJDIR)\das-test.exe		: $(OBJDIR)\das-test.obj $(TESTOBJS)
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\das-test.obj $(TESTOBJS) $(LINKLIBS)

$(OBJDIR)\dds-test.exe		: $(OBJDIR)\dds-test.obj $(TESTOBJS)
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\dds-test.obj $(TESTOBJS) $(LINKLIBS)

$(OBJDIR)\error-test.exe	: $(OBJDIR)\error-test.obj $(TESTOBJS)
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\error-test.obj $(TESTOBJS) $(LINKLIBS)

$(OBJDIR)\expr-test.exe		: $(OBJDIR)\expr-test.obj $(TESTOBJS)
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\expr-test.obj $(TESTOBJS) $(LINKLIBS)

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
	$(OBJDIR)\cgiUtilTest
#	$(OBJDIR)\HTTPConnectTest
	$(OBJDIR)\HTTPCacheTest
	$(OBJDIR)\SequenceTest
	$(OBJDIR)\AttrTableTest
	$(OBJDIR)\DDSTest
	$(OBJDIR)\ByteTest
	$(OBJDIR)\DASTest
	$(OBJDIR)\generalUtilTest
	$(OBJDIR)\parserUtilTest


$(OBJDIR)\parserUtilTest.exe	: $(OBJDIR)\parserUtilTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\parserUtilTest.obj $(CPPUNITLIBS)
$(OBJDIR)\generalUtilTest.exe	: $(OBJDIR)\generalUtilTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\generalUtilTest.obj $(CPPUNITLIBS)
$(OBJDIR)\DASTest.exe		: $(OBJDIR)\DASTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\DASTest.obj $(CPPUNITLIBS)
$(OBJDIR)\ByteTest.exe		: $(OBJDIR)\ByteTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\ByteTest.obj $(CPPUNITLIBS)
$(OBJDIR)\SequenceTest.exe	: $(OBJDIR)\SequenceTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\SequenceTest.obj $(CPPUNITLIBS)
$(OBJDIR)\AttrTableTest.exe	: $(OBJDIR)\AttrTableTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\AttrTableTest.obj $(CPPUNITLIBS)
$(OBJDIR)\DDSTest.exe		: $(OBJDIR)\DDSTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\DDSTest.obj $(CPPUNITLIBS)
$(OBJDIR)\cgiUtilTest.exe	: $(OBJDIR)\cgiUtilTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\cgiUtilTest.obj $(CPPUNITLIBS)
$(OBJDIR)\HTTPCacheTest.exe	: $(OBJDIR)\HTTPCacheTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\HTTPCacheTest.obj $(CPPUNITLIBS)
$(OBJDIR)\HTTPConnectTest.exe	: $(OBJDIR)\HTTPConnectTest.obj
	$(LINK) $(LEXEFLAGS) /out:$@ $(OBJDIR)\HTTPConnectTest.obj $(CPPUNITLIBS)

$(OBJDIR)\parserUtilTest.obj	: ..\parserUtilTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\parserUtilTest.cc
$(OBJDIR)\generalUtilTest.obj	: ..\generalUtilTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\generalUtilTest.cc
$(OBJDIR)\DASTest.obj		: ..\DASTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\DASTest.cc
$(OBJDIR)\ByteTest.obj		: ..\ByteTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\ByteTest.cc
$(OBJDIR)\SequenceTest.obj	: ..\SequenceTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\SequenceTest.cc
$(OBJDIR)\AttrTableTest.obj	: ..\AttrTableTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\AttrTableTest.cc
$(OBJDIR)\DDSTest.obj		: ..\DDSTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\DDSTest.cc
$(OBJDIR)\cgiUtilTest.obj	: ..\cgiUtilTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\cgiUtilTest.cc
$(OBJDIR)\HTTPCacheTest.obj	: ..\HTTPCacheTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\HTTPCacheTest.cc
$(OBJDIR)\HTTPConnectTest.obj	: ..\HTTPConnectTest.cc
	$(CC) $(CPPUNITOBJFLAGS) /Fo$@ /Tp ..\HTTPConnectTest.cc

# Create the install and intermediate file directories if they
# don't exist.  We also have to create a temporary unistd.h because
# bison and/or flex create source that includes that - even though
# there is no such header under win32 and it is unnecessary there
# (that is, even though these utils can run under win32 - by definition
# they generate output for the UNIX world).  This stupid pet trick
# also properly defines isatty() for the generated source under win32.

# We once stuck with using very simple ("non-extended") DOS commands
# w/o all the fancy flags.  That was in support of building on
# Win95/98.  We no longer support 95/98 so we can go ahead and use
# the more robust DOS commands.  The below can be made less repititive
# as a result - when time permits.
setup:
	@-mkdir $(OBJDIR)
	@-mkdir $(INSTALLLIBDIR)
	@-mkdir $(INSTALLEXEDIR)
	@-mkdir $(INCLUDEDIR)
	@-echo /*  Dummy patch header to get win32 flex/bison generated  */ > unistd.h
	@-echo /*  source code to compile under win32.  If you see this  */ >> unistd.h
	@-echo /*  temp file, feel free to scratch it.                   */ >> unistd.h
	@echo #include "io.h" >> unistd.h

clean:
	@cd libxdr
	@-nmake clean
	@cd .. 
	@-del $(OBJDIR)\*.obj
	@-del $(OBJDIR)\*.lib
	@-del $(OBJDIR)\*.exe
	@-rmdir $(OBJDIR)
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



