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

# MT = Static Link Multi-Threaded Model
# MD = Dynamic Link Multi-Threaded DLL Model

# Setup compiler flags
MTCFLAGS	= /MT /D HAVE_PTHREAD_H
MDCFLAGS	= /MD /D HAVE_PTHREAD_H
CFLAGS		= /nologo /W0 /GR /EHsc /c

# Setup linker flags for libdap
MTLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MDLFLAGS	= -libpath:$(INSTALLLIBDIR) /dll /def:libdap.def /map /fixed:NO /base:0x06000000 /NODEFAULTLIB:LIBCMT.lib

# Setup link libraries for libdap
MTLIBS		=
MDLIBS		= libdirent.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup linker flags for executables
EMTLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR) /NODEFAULTLIB:MSVCRT.lib
EMDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)

# Setup link libraries for executables
EMTLLIBS	= libdapMT.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib
EMDLLIBS	= libdapMD.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib
CPPUNITLIBS	= cppunit_dll.lib

# Setup compiler includes and defines
INCS		= /I . /I .. /I ..\GNU /I ..\gl /I $(INCLUDEDIR)
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

modelMD: setup
	@-md $(OBJDIR)\MD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(MDLFLAGS)" LIBS="$(MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapMD.dll
	nmake installMD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD getdap
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD deflate

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

GRAM_OBJS = $(OBJDIR)\lex.das.obj \
		$(OBJDIR)\das.tab.obj \
		$(OBJDIR)\lex.dds.obj \
		$(OBJDIR)\dds.tab.obj \
		$(OBJDIR)\lex.expr.obj \
		$(OBJDIR)\expr.tab.obj \
		$(OBJDIR)\lex.Error.obj \
		$(OBJDIR)\Error.tab.obj \
		$(OBJDIR)\lex.gse_.obj \
		$(OBJDIR)\gse.tab.obj

DAP_OBJS = $(GRAM_OBJS) \
		$(OBJDIR)\AttrTable.obj \
		$(OBJDIR)\DAS.obj \
		$(OBJDIR)\parser-util.obj \
		$(OBJDIR)\escaping.obj \
		$(OBJDIR)\DDS.obj \
		$(OBJDIR)\DataDDS.obj \
		$(OBJDIR)\DDXParser.obj \
		$(OBJDIR)\Clause.obj \
		$(OBJDIR)\RValue.obj \
		$(OBJDIR)\Error.obj \
		$(OBJDIR)\InternalErr.obj \
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
		$(OBJDIR)\xdrutil_ppc.obj \
		$(OBJDIR)\ce_functions.obj \
		$(OBJDIR)\GSEClause.obj \
		$(OBJDIR)\Int16.obj \
		$(OBJDIR)\UInt16.obj \
		$(OBJDIR)\Float32.obj \
		$(OBJDIR)\Constructor.obj \
		$(OBJDIR)\PassiveByte.obj \
		$(OBJDIR)\PassiveInt32.obj \
		$(OBJDIR)\PassiveFloat64.obj \
		$(OBJDIR)\PassiveStr.obj \
		$(OBJDIR)\PassiveUrl.obj \
		$(OBJDIR)\PassiveUInt32.obj \
		$(OBJDIR)\PassiveInt16.obj \
		$(OBJDIR)\PassiveUInt16.obj \
		$(OBJDIR)\PassiveFloat32.obj \
		$(OBJDIR)\PassiveArray.obj \
		$(OBJDIR)\PassiveStructure.obj \
		$(OBJDIR)\util_mit.obj \
		$(OBJDIR)\BaseTypeFactory.obj 

CLIENT_OBJS = $(OBJDIR)\RCReader.obj \
		$(OBJDIR)\Connect.obj \
		$(OBJDIR)\HTTPConnect.obj \
		$(OBJDIR)\HTTPCache.obj \
		$(OBJDIR)\AISResources.obj \
		$(OBJDIR)\AISDatabaseParser.obj \
		$(OBJDIR)\AISMerge.obj \
		$(OBJDIR)\AISConnect.obj

SERVER_OBJS = $(OBJDIR)\SignalHandler.obj \
		$(OBJDIR)\OPeNDAPFile.obj \
		$(OBJDIR)\OPeNDAPDir.obj \
		$(OBJDIR)\cgi_util.obj \
		$(OBJDIR)\DODSFilter.obj \
		$(OBJDIR)\ResponseTooBigErr.obj  

GNU_OBJS = $(OBJDIR)\GetOpt.obj \
		$(OBJDIR)\GNURegex.obj \
		$(OBJDIR)\GNUerror.obj \
		$(OBJDIR)\regex.obj \
		$(OBJDIR)\strcasecmp.obj \
		$(OBJDIR)\strncasecmp.obj \
		$(OBJDIR)\strnlen1.obj

LIBOBJS = $(DAP_OBJS) $(CLIENT_OBJS) $(SERVER_OBJS) $(GNU_OBJS)

# Convenience dependancies
libdapMT.lib:	$(OBJDIR)\libdapMT.lib
libdapMD.dll:	$(OBJDIR)\libdapMD.dll

getdap:		$(OBJDIR)\getdap.exe
deflate:	$(OBJDIR)\deflate.exe

# Link the library
$(OBJDIR)\libdapMT.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libdapMD.dll: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

# The DAP Proper
$(OBJDIR)\AttrTable.obj   	: $(SRCDIR)\AttrTable.cc
$(OBJDIR)\DAS.obj   		: $(SRCDIR)\DAS.cc
$(OBJDIR)\parser-util.obj   	: $(SRCDIR)\parser-util.cc
$(OBJDIR)\escaping.obj   	: $(SRCDIR)\escaping.cc
$(OBJDIR)\DDS.obj   		: $(SRCDIR)\DDS.cc
$(OBJDIR)\DataDDS.obj   	: $(SRCDIR)\DataDDS.cc
$(OBJDIR)\DDXParser.obj   	: $(SRCDIR)\DDXParser.cc
$(OBJDIR)\Clause.obj   		: $(SRCDIR)\Clause.cc
$(OBJDIR)\RValue.obj   		: $(SRCDIR)\RValue.cc
$(OBJDIR)\Error.obj   		: $(SRCDIR)\Error.cc
$(OBJDIR)\InternalErr.obj   	: $(SRCDIR)\InternalErr.cc
$(OBJDIR)\BaseType.obj   	: $(SRCDIR)\BaseType.cc
$(OBJDIR)\Byte.obj   		: $(SRCDIR)\Byte.cc
$(OBJDIR)\Int32.obj   		: $(SRCDIR)\Int32.cc
$(OBJDIR)\Float64.obj   	: $(SRCDIR)\Float64.cc
$(OBJDIR)\Str.obj   		: $(SRCDIR)\Str.cc
$(OBJDIR)\Url.obj   		: $(SRCDIR)\Url.cc
$(OBJDIR)\Vector.obj   		: $(SRCDIR)\Vector.cc
$(OBJDIR)\Array.obj   		: $(SRCDIR)\Array.cc
$(OBJDIR)\Structure.obj   	: $(SRCDIR)\Structure.cc
$(OBJDIR)\Sequence.obj   	: $(SRCDIR)\Sequence.cc
$(OBJDIR)\Grid.obj   		: $(SRCDIR)\Grid.cc
$(OBJDIR)\UInt32.obj   		: $(SRCDIR)\UInt32.cc
$(OBJDIR)\util.obj   		: $(SRCDIR)\util.cc
$(OBJDIR)\xdrutil_ppc.obj   	: $(SRCDIR)\xdrutil_ppc.c
$(OBJDIR)\ce_functions.obj   	: $(SRCDIR)\ce_functions.cc
$(OBJDIR)\GSEClause.obj   	: $(SRCDIR)\GSEClause.cc
$(OBJDIR)\Int16.obj   		: $(SRCDIR)\Int16.cc
$(OBJDIR)\UInt16.obj   		: $(SRCDIR)\UInt16.cc
$(OBJDIR)\Float32.obj   	: $(SRCDIR)\Float32.cc
$(OBJDIR)\Constructor.obj   	: $(SRCDIR)\Constructor.cc
$(OBJDIR)\PassiveByte.obj   	: $(SRCDIR)\PassiveByte.cc
$(OBJDIR)\PassiveInt32.obj   	: $(SRCDIR)\PassiveInt32.cc
$(OBJDIR)\PassiveFloat64.obj	: $(SRCDIR)\PassiveFloat64.cc
$(OBJDIR)\PassiveStr.obj	: $(SRCDIR)\PassiveStr.cc
$(OBJDIR)\PassiveUrl.obj	: $(SRCDIR)\PassiveUrl.cc
$(OBJDIR)\PassiveUInt32.obj	: $(SRCDIR)\PassiveUInt32.cc
$(OBJDIR)\PassiveInt16.obj	: $(SRCDIR)\PassiveInt16.cc
$(OBJDIR)\PassiveUInt16.obj	: $(SRCDIR)\PassiveUInt16.cc
$(OBJDIR)\PassiveFloat32.obj	: $(SRCDIR)\PassiveFloat32.cc
$(OBJDIR)\PassiveArray.obj	: $(SRCDIR)\PassiveArray.cc
$(OBJDIR)\PassiveStructure.obj	: $(SRCDIR)\PassiveStructure.cc
$(OBJDIR)\util_mit.obj		: $(SRCDIR)\util_mit.cc
$(OBJDIR)\BaseTypeFactory.obj	: $(SRCDIR)\BaseTypeFactory.cc

# DAP Client Side
$(OBJDIR)\RCReader.obj		: $(SRCDIR)\RCReader.cc
$(OBJDIR)\Connect.obj		: $(SRCDIR)\Connect.cc
$(OBJDIR)\HTTPConnect.obj	: $(SRCDIR)\HTTPConnect.cc
$(OBJDIR)\HTTPCache.obj		: $(SRCDIR)\HTTPCache.cc
$(OBJDIR)\AISResources.obj	: $(SRCDIR)\AISResources.cc
$(OBJDIR)\AISDatabaseParser.obj	: $(SRCDIR)\AISDatabaseParser.cc
$(OBJDIR)\AISMerge.obj		: $(SRCDIR)\AISMerge.cc
$(OBJDIR)\AISConnect.obj	: $(SRCDIR)\AISConnect.cc

# Dap Server Side
$(OBJDIR)\SignalHandler.obj	: $(SRCDIR)\SignalHandler.cc
$(OBJDIR)\OPeNDAPFile.obj 	: $(SRCDIR)\OPeNDAPFile.cc
$(OBJDIR)\OPeNDAPDir.obj	: $(SRCDIR)\OPeNDAPDir.cc
$(OBJDIR)\cgi_util.obj		: $(SRCDIR)\cgi_util.cc
$(OBJDIR)\DODSFilter.obj	: $(SRCDIR)\DODSFilter.cc
$(OBJDIR)\ResponseTooBigErr.obj	: $(SRCDIR)\ResponseTooBigErr.cc

# Objects from Bison/Lex generated source
$(OBJDIR)\das.tab.obj   : $(SRCDIR)\das.tab.cc
$(OBJDIR)\dds.tab.obj   : $(SRCDIR)\dds.tab.cc
$(OBJDIR)\expr.tab.obj  : $(SRCDIR)\expr.tab.cc
$(OBJDIR)\Error.tab.obj : $(SRCDIR)\Error.tab.cc
$(OBJDIR)\gse.tab.obj   : $(SRCDIR)\gse.tab.cc
$(OBJDIR)\lex.das.obj   : $(SRCDIR)\lex.das.cc
$(OBJDIR)\lex.dds.obj   : $(SRCDIR)\lex.dds.cc
$(OBJDIR)\lex.Error.obj : $(SRCDIR)\lex.Error.cc
$(OBJDIR)\lex.expr.obj  : $(SRCDIR)\lex.expr.cc
$(OBJDIR)\lex.gse_.obj  : $(SRCDIR)\lex.gse_.cc

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
$(SRCDIR)\lex.das.cc:	$(SRCDIR)\das.lex $(SRCDIR)\das.tab.cc $(SRCDIR)\das.tab.h
	$(LEX) -Pdas $(SRCDIR)\das.lex
	copy lex.das.c $(SRCDIR)\lex.das.cc
	del lex.das.c

$(SRCDIR)\das.tab.cc $(SRCDIR)\das.tab.h: $(SRCDIR)\das.y $(SRCDIR)\DAS.h
	$(YACC) $(YACCFLAGS) -p das $(SRCDIR)\das.y

$(SRCDIR)\lex.dds.cc:	$(SRCDIR)\dds.lex $(SRCDIR)\dds.tab.cc $(SRCDIR)\dds.tab.h
	$(LEX) -Pdds $(SRCDIR)\dds.lex
	copy lex.dds.c $(SRCDIR)\lex.dds.cc
	del lex.dds.c

$(SRCDIR)\dds.tab.cc $(SRCDIR)\dds.tab.h: $(SRCDIR)\dds.y
	$(YACC) $(YACCFLAGS) -p dds $(SRCDIR)\dds.y

$(SRCDIR)\lex.expr.cc:	$(SRCDIR)\expr.lex $(SRCDIR)\expr.tab.cc $(SRCDIR)\expr.tab.h
	$(LEX) -Pexpr $(SRCDIR)\expr.lex
	copy lex.expr.c $(SRCDIR)\lex.expr.cc
	del lex.expr.c

$(SRCDIR)\expr.tab.cc $(SRCDIR)\expr.tab.h: $(SRCDIR)\expr.y
	$(YACC) $(YACCFLAGS) -p expr $(SRCDIR)\expr.y

$(SRCDIR)\lex.Error.cc:	$(SRCDIR)\Error.lex $(SRCDIR)\Error.tab.cc $(SRCDIR)\Error.tab.h
	$(LEX) -PError $(SRCDIR)\Error.lex
	copy lex.Error.c $(SRCDIR)\lex.Error.cc
	del lex.Error.c

$(SRCDIR)\Error.tab.cc $(SRCDIR)\Error.tab.h: $(SRCDIR)\Error.y
	$(YACC) $(YACCFLAGS) -p Error $(SRCDIR)\Error.y

$(SRCDIR)\lex.gse_.cc:	$(SRCDIR)\gse.lex $(SRCDIR)\gse.tab.cc $(SRCDIR)\gse.tab.h
	$(LEX) -Pgse_ $(SRCDIR)\gse.lex
	copy lex.gse_.c $(SRCDIR)\lex.gse_.cc
	del lex.gse_.c

$(SRCDIR)\gse.tab.cc $(SRCDIR)\gse.tab.h: $(SRCDIR)\gse.y
	$(YACC) $(YACCFLAGS) -p gse_ $(SRCDIR)\gse.y

$(OBJDIR)\getdap.exe: $(OBJDIR)\getdap.obj

$(OBJDIR)\deflate.exe: $(OBJDIR)\deflate.obj 

# Command-Line Client and select utils in console mode
$(OBJDIR)\getdap.obj    : $(SRCDIR)\getdap.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\getdap.cc

$(OBJDIR)\deflate.obj    : $(SRCDIR)\deflate.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $(SRCDIR)\deflate.c

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
	copy $(OBJDIR)\MT\deflate.exe $(INSTALLETCDIR)
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

grammarclean: clean
	@-del ..\*.tab.cc
	@-del ..\*.tab.h
	@-del ..\lex.*.cc

!INCLUDE "dependancies"



