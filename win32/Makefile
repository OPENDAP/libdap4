
# Setup directories for intermediate files and built components
OBJDIR		= .\objects
INSTALLLIBDIR	= ..\..\..\lib
INSTALLEXEDIR	= ..\..\..\bin
INCLUDEDIR	= ..\..\..\include

# Directory containing 3rd-party packages used by Dods
PKGDIR		= ..\..\..\packages

# Setup basic compiler flags for all .obj's
CC		= cl
CFLAGS		= /nologo /ML /W0 /GR /GX /c
#CFLAGS		= /nologo /MLd /Zi /W3 /GR /GX /c	# flags for debugger

# Setup linker flags for libraries and .exe's
LINK		= link
LIBPATH		= /libpath:$(PKGDIR)\lib /libpath:$(OBJDIR) /libpath:$(INSTALLLIBDIR)
#LEXEFLAGS	= /DEBUG /subsystem:console $(LIBPATH)	# flags for debugger
#LLIBFLAGS	= -lib					# flags for debugger
LEXEFLAGS	= /nologo /subsystem:console $(LIBPATH)
LLIBFLAGS	= -lib /nologo

DODSLIBS	= libwww.lib librx.lib libz.lib libdap++.lib libxdr.lib
SYSLIBS		= ws2_32.lib advapi32.lib
LINKLIBS	= $(DODSLIBS) $(SYSLIBS)

# Setup compiler tools.  BISON_HAIRY and BISON_SIMPLE environment variables
# must be set to the _full_ path for bison.simple and bison.hairy respectively.
# These tools can (as of 5/2000) be downloaded and installed from
# ftp://ftp.uni-koeln.de/pc/win32/misc/unxutils.zip.  They must also be on
# the path.
LEX	= flex
YACC	= bison
AWK	= gawk

# Setup compiler includes and defines for libraries and .exe's
INCS		= /I . /I .. /I ..\GNU /I ..\..\..\packages\include \
		  /I ..\..\..\packages\include\w3c-libwww \
		  /I ..\..\..\include
DEFSLIB		= /D WIN32 /D _LIB /D _WINDOWS
DEFSEXE		= /D WIN32 /D _CONSOLE
CPPLIBFLAGS	= $(DEFSLIB) $(INCS)
CPPEXEFLAGS	= $(DEFSEXE) $(INCS)

# Combine compiler flags - for objects going into .obj's and for objects going into .exe's
LIBOBJFLAGS	= $(CFLAGS) $(CPPLIBFLAGS)
EXEOBJFLAGS	= $(CFLAGS) $(CPPEXEFLAGS)

# Define the targets
TARGETS		= $(OBJDIR)\libdap++.lib $(OBJDIR)\geturl.exe \
		  $(OBJDIR)\usage.exe $(OBJDIR)\deflate.exe

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
	$(LINK) $(LEXEFLAGS) /out:$@ $< $(TESTOBJS) $(LINKLIBS)

DAPOBJS = $(OBJDIR)\Array.obj \
		$(OBJDIR)\AttrTable.obj \
		$(OBJDIR)\BaseType.obj \
		$(OBJDIR)\Byte.obj \
		$(OBJDIR)\ce_functions.obj \
		$(OBJDIR)\cgi_util.obj \
		$(OBJDIR)\Clause.obj \
		$(OBJDIR)\Connect.obj \
		$(OBJDIR)\Connections.obj \
		$(OBJDIR)\DAS.obj \
		$(OBJDIR)\DataDDS.obj \
		$(OBJDIR)\DDS.obj \
		$(OBJDIR)\DODSFilter.obj \
		$(OBJDIR)\Error.obj \
		$(OBJDIR)\escaping.obj \
		$(OBJDIR)\Float32.obj \
		$(OBJDIR)\Float64.obj \
		$(OBJDIR)\Grid.obj \
		$(OBJDIR)\GSEClause.obj \
		$(OBJDIR)\Int16.obj \
		$(OBJDIR)\Int32.obj \
		$(OBJDIR)\InternalErr.obj \
		$(OBJDIR)\List.obj \
		$(OBJDIR)\parser-util.obj \
		$(OBJDIR)\RValue.obj \
		$(OBJDIR)\Sequence.obj \
		$(OBJDIR)\Str.obj \
		$(OBJDIR)\Structure.obj \
		$(OBJDIR)\UInt16.obj \
		$(OBJDIR)\UInt32.obj \
		$(OBJDIR)\Url.obj \
		$(OBJDIR)\util.obj \
		$(OBJDIR)\Vector.obj

TESTOBJS = $(OBJDIR)\TestArray.obj \
		$(OBJDIR)\TestByte.obj \
		$(OBJDIR)\TestFloat32.obj \
		$(OBJDIR)\TestFloat64.obj \
		$(OBJDIR)\TestGrid.obj \
		$(OBJDIR)\TestInt16.obj \
		$(OBJDIR)\TestInt32.obj \
		$(OBJDIR)\TestList.obj \
		$(OBJDIR)\TestSequence.obj \
		$(OBJDIR)\TestStr.obj \
		$(OBJDIR)\TestStructure.obj \
		$(OBJDIR)\TestUInt16.obj \
		$(OBJDIR)\TestUInt32.obj \
		$(OBJDIR)\TestUrl.obj

LEXOBJS = $(OBJDIR)\das.tab.obj \
		$(OBJDIR)\dds.tab.obj \
		$(OBJDIR)\expr.tab.obj \
		$(OBJDIR)\Error.tab.obj \
		$(OBJDIR)\gse.tab.obj \
		$(OBJDIR)\lex.das.obj \
		$(OBJDIR)\lex.dds.obj \
		$(OBJDIR)\lex.Error.obj \
		$(OBJDIR)\lex.expr.obj \
		$(OBJDIR)\lex.gse_.obj \

GNUOBJS = $(OBJDIR)\DLList.obj \
		$(OBJDIR)\GetOpt.obj \
		$(OBJDIR)\Regex.obj \
		$(OBJDIR)\SLList.obj \
		$(OBJDIR)\gnu_error.obj \
		$(OBJDIR)\hash.obj

LIBOBJS = $(LEXOBJS) $(GNUOBJS) $(DAPOBJS)

all: build_libxdr setup $(TARGETS) check

check: $(TESTTARGETS)
	@echo Notice:
	@echo Test programs have been built, but they currently can
	@echo only be run under DejaGnu in a Cygwin environment.
	@echo To validate the Win32 Dods core, install cygwin and move
	@echo the executables into a dods distribution to run them there.

build_libxdr:
	@cd libxdr
	@nmake
	@nmake install
	@cd ..

$(OBJDIR)\libdap++.lib: $(LIBOBJS)
	$(LINK) $(LLIBFLAGS) /out:$@ $(LIBOBJS)

$(OBJDIR)\geturl.exe: $(OBJDIR)\geturl.obj $(TESTOBJS) $(OBJDIR)\libdap++.lib

$(OBJDIR)\usage.exe: $(OBJDIR)\usage.obj $(TESTOBJS) $(OBJDIR)\libdap++.lib

$(OBJDIR)\deflate.exe: $(OBJDIR)\deflate.obj $(OBJDIR)\libdap++.lib

install:
	copy $(OBJDIR)\libdap*.lib $(INSTALLLIBDIR)
	copy $(OBJDIR)\geturl.exe $(INSTALLEXEDIR)
	copy $(OBJDIR)\usage.exe $(INSTALLEXEDIR)
	copy $(OBJDIR)\deflate.exe $(INSTALLEXEDIR)
	copy config_dap.h $(INCLUDEDIR)
	copy ..\debug.h $(INCLUDEDIR)
	copy ..\dods-limits.h $(INCLUDEDIR)
	copy ..\Error.h $(INCLUDEDIR)
	copy ..\InternalErr.h $(INCLUDEDIR)
	copy ..\expr.h $(INCLUDEDIR)
	copy ..\Clause.h $(INCLUDEDIR)
	copy ..\RValue.h $(INCLUDEDIR)
	copy ..\BaseType.h $(INCLUDEDIR)
	copy ..\Byte.h $(INCLUDEDIR)
	copy ..\Int32.h $(INCLUDEDIR)
	copy ..\GSEClause.h $(INCLUDEDIR)
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
	copy ..\DDS.h $(INCLUDEDIR)
	copy ..\DataDDS.h $(INCLUDEDIR)
	copy ..\AttrTable.h $(INCLUDEDIR)
	copy ..\DAS.h $(INCLUDEDIR)
	copy ..\parser.h $(INCLUDEDIR)
	copy ..\GNU\Pix.h $(INCLUDEDIR)
	copy ..\GNU\SLList.h $(INCLUDEDIR)
	copy ..\GNU\DLList.h $(INCLUDEDIR)
	copy ..\GNU\gnu-std.h $(INCLUDEDIR)
	copy ..\GNU\GetOpt.h $(INCLUDEDIR)
	copy ..\GNU\Regex.h $(INCLUDEDIR)
	copy ..\GNU\builtin.h $(INCLUDEDIR)
	copy ..\Connect.h $(INCLUDEDIR)
	copy ..\Connections.h $(INCLUDEDIR)
	copy ..\cgi_util.h $(INCLUDEDIR)
	copy ..\DODSFilter.h $(INCLUDEDIR)
	copy ..\escaping.h $(INCLUDEDIR)

# The DAP Proper
$(OBJDIR)\Array.obj : ..\Array.cc
$(OBJDIR)\AttrTable.obj : ..\AttrTable.cc
$(OBJDIR)\BaseType.obj : ..\BaseType.cc
$(OBJDIR)\Byte.obj : ..\Byte.cc
$(OBJDIR)\ce_functions.obj : ..\ce_functions.cc
$(OBJDIR)\cgi_util.obj : ..\cgi_util.cc
$(OBJDIR)\Clause.obj : ..\Clause.cc
$(OBJDIR)\Connect.obj : ..\Connect.cc
$(OBJDIR)\Connections.obj : ..\Connections.cc
$(OBJDIR)\DAS.obj : ..\DAS.cc
$(OBJDIR)\DataDDS.obj : ..\DataDDS.cc
$(OBJDIR)\DDS.obj : ..\DDS.cc
$(OBJDIR)\DODSFilter.obj : ..\DODSFilter.cc
$(OBJDIR)\Error.obj : ..\Error.cc
$(OBJDIR)\escaping.obj : ..\escaping.cc
$(OBJDIR)\Float32.obj : ..\Float32.cc
$(OBJDIR)\Float64.obj : ..\Float64.cc
$(OBJDIR)\Grid.obj : ..\Grid.cc
$(OBJDIR)\GSEClause.obj : ..\GSEClause.cc
$(OBJDIR)\Int16.obj : ..\Int16.cc
$(OBJDIR)\Int32.obj : ..\Int32.cc
$(OBJDIR)\InternalErr.obj : ..\InternalErr.cc
$(OBJDIR)\List.obj : ..\List.cc
$(OBJDIR)\parser-util.obj : ..\parser-util.cc
$(OBJDIR)\RValue.obj : ..\RValue.cc
$(OBJDIR)\Sequence.obj : ..\Sequence.cc
$(OBJDIR)\Str.obj : ..\Str.cc
$(OBJDIR)\Structure.obj : ..\Structure.cc
$(OBJDIR)\UInt16.obj : ..\UInt16.cc
$(OBJDIR)\UInt32.obj : ..\UInt32.cc
$(OBJDIR)\Url.obj : ..\Url.cc
$(OBJDIR)\util.obj : ..\Util.cc
$(OBJDIR)\Vector.obj : ..\Vector.cc

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
..\das.tab.c		: ..\das.lex ..\das.y
	$(LEX) -i -Pdas ..\das.lex
	$(YACC) -d -v -p das ..\das.y
	$(AWK) -f ../munge-bison.awk ../das.tab.c > tmp.tab.c
	copy tmp.tab.c ..\das.tab.c
	copy lex.das.c ..\lex.das.c
	del tmp.tab.c
	del lex.das.c

..\dds.tab.c		: ..\dds.lex ..\dds.y
	$(LEX) -i -Pdds ..\dds.lex
	$(YACC) -d -v -p dds ..\dds.y
	$(AWK) -f ../munge-bison.awk ../dds.tab.c > tmp.tab.c
	copy tmp.tab.c ..\dds.tab.c
	copy lex.dds.c ..\lex.dds.c
	del tmp.tab.c
	del lex.dds.c

..\expr.tab.c		: ..\expr.lex ..\expr.y
	$(LEX) -i -Pexpr ..\expr.lex
	$(YACC) -d -v -p expr ..\expr.y
	$(AWK) -f ../munge-bison.awk ../expr.tab.c > tmp.tab.c
	copy tmp.tab.c ..\expr.tab.c
	copy lex.expr.c ..\lex.expr.c
	del tmp.tab.c
	del lex.expr.c

..\Error.tab.c		: ..\Error.lex ..\Error.y
	$(LEX) -i -PError ..\Error.lex
	$(YACC) -d -v -p Error ..\Error.y
	$(AWK) -f ../munge-bison.awk ../Error.tab.c > tmp.tab.c
	copy tmp.tab.c ..\Error.tab.c
	copy lex.Error.c ..\lex.Error.c
	del tmp.tab.c
	del lex.Error.c

..\gse.tab.c		: ..\gse.lex ..\gse.y
	$(LEX) -8 -Pgse_ ..\gse.lex
	$(YACC) -d -v -p gse_ ..\gse.y
	copy lex.gse_.c ..\lex.gse_.c
	del lex.gse_.c

# GNU Subdirectory.  There exists an object "Error.obj" that
# (accounting for lack of case sensitivity in win32) conflicts with
# "error.obj" and makes the dependancies rules somewhat more
# complicated as a result.  This conflict, along with a similar
# conflict between "Error.cc" and "error.cc" is what makes the
# following explicit dependancies necessary.
$(OBJDIR)\DLList.obj : ..\GNU\DLList.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\GNU\DLList.cc
$(OBJDIR)\GetOpt.obj : ..\GNU\GetOpt.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\GNU\GetOpt.cc
$(OBJDIR)\Regex.obj : ..\GNU\Regex.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\GNU\Regex.cc
$(OBJDIR)\SLList.obj : ..\GNU\SLList.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\GNU\SLList.cc
$(OBJDIR)\hash.obj : ..\GNU\hash.cc
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\GNU\hash.cc
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
# There is no DejaGnu for win32 so these tests can only be run
# under cygwin or via some (as yet non-existant) script that invokes
# them with the appropriate args and compares their output to their
# expected output.  These are all console-mode applications.
$(OBJDIR)\das-test.obj   : ..\das-test.cc $(OBJDIR)\libdap++.lib
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\das-test.cc
$(OBJDIR)\dds-test.obj   : ..\dds-test.cc $(OBJDIR)\libdap++.lib
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\dds-test.cc
$(OBJDIR)\error-test.obj : ..\error-test.cc $(OBJDIR)\libdap++.lib
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\error-test.cc
$(OBJDIR)\expr-test.obj  : ..\expr-test.cc $(OBJDIR)\libdap++.lib
	@$(CC) $(EXEOBJFLAGS) /Fo$@ /Tp ..\expr-test.cc

# Create the install and intermediate file directories if they
# don't exist.  We also have to create a temporary unistd.h because
# bison and/or flex create source that includes that - even though
# there is no such header under win32 and it is unnecessary there
# (that is, even though these utils can run under win32 - by definition
# they generate output for the UNIX world).  This stupid pet trick
# also properly defines isatty() for the generated source under win32.
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
	@-rmdir /S /Q $(OBJDIR)
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



