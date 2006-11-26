# Makefile for libdap (under VC++ on win32)

# Setup some preliminary directories
OBJDIR		= .\objects
SRCDIR		= ..
ROOTINSTALLDIR	= ..\..\prepkg
INSTALLLIBDIR	= $(ROOTINSTALLDIR)\lib
INSTALLDLLDIR	= $(ROOTINSTALLDIR)\dll
INSTALLBINDIR	= $(ROOTINSTALLDIR)\bin
INSTALLETCDIR	= $(ROOTINSTALLDIR)\etc
INSTALLINCDIR	= $(ROOTINSTALLDIR)\include

INCLUDEDIR	= $(ROOTINSTALLDIR)\include

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
DAP_MDLFLAGS	= -libpath:$(OBJDIR)\MD /dll /def:libdap.def /map /fixed:NO /base:0x06000000 /NODEFAULTLIB:LIBCMT.lib

# Setup linker flags for libdapclient
CLIENT_MTLFLAGS	= -lib /nologo -libpath:$(OBJDIR)\MT
CLIENT_MDLFLAGS	= -libpath:$(OBJDIR)\MD /dll /def:libdapclient.def /map /fixed:NO /base:0x07000000 /NODEFAULTLIB:LIBCMT.lib

# Setup linker flags for libdapserver
SERVER_MTLFLAGS	= -lib /nologo -libpath:$(OBJDIR)\MT
SERVER_MDLFLAGS	= -libpath:$(OBJDIR)\MD /dll /def:libdapserver.def /map /fixed:NO /base:0x08000000 /NODEFAULTLIB:LIBCMT.lib

# Setup link libraries for libdap
DAP_MTLIBS		=
DAP_MDLIBS		= libdirent.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup link libraries for libdapclient
CLIENT_MTLIBS		=
CLIENT_MDLIBS		= libdapMD.lib libdirent.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup link libraries for libdapserver
SERVER_MTLIBS		=
SERVER_MDLIBS		= libdapMD.lib libdirent.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup linker flags for executables
EMTLFLAGS	= /nologo /libpath:$(OBJDIR)\MT /NODEFAULTLIB:MSVCRT.lib
EMDLFLAGS	= /nologo /libpath:$(OBJDIR)\MD

# Setup link libraries for executables
EMTLLIBS	= libdapclientMT.lib libdapserverMT.lib libdapMT.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib
EMDLLIBS	= libdapclientMD.lib libdapserverMD.lib libdapMD.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup compiler includes and defines
INCS		= /I . /I .. /I ..\GNU /I ..\gl /I $(INCLUDEDIR)
DEFS		= /D WIN32 /D WIN32_LEAN_AND_MEAN /D DVR=\"DODS/3.4.8\"
CPPFLAGS	= $(DEFS) $(INCS)

default:
	@echo "usage: nmake all; nmake install"
	@echo "or"
	@echo "usage: nmake model<MT|MD>; nmake install<MT|MD>"

all:	setup modelMT modelMD

modelMT: setup
	@-md $(OBJDIR)\MT
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(DAP_MTLFLAGS)" LIBS="$(DAP_MTLIBS)" OBJDIR=$(OBJDIR)\MT libdapMT.lib
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(CLIENT_MTLFLAGS)" LIBS="$(CLIENT_MTLIBS)" OBJDIR=$(OBJDIR)\MT libdapclientMT.lib
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(SERVER_MTLFLAGS)" LIBS="$(SERVER_MTLIBS)" OBJDIR=$(OBJDIR)\MT libdapserverMT.lib
	@nmake /NOLOGO installMT

	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT getdap
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT deflate

modelMD: setup
	@-md $(OBJDIR)\MD
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(DAP_MDLFLAGS)" LIBS="$(DAP_MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapMD.dll	
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(CLIENT_MDLFLAGS)" LIBS="$(CLIENT_MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapclientMD.dll
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(SERVER_MDLFLAGS)" LIBS="$(SERVER_MDLIBS)" OBJDIR=$(OBJDIR)\MD libdapserverMD.dll
	@nmake installMD

	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD getdap
	@nmake /NOLOGO CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD deflate

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

GNU_OBJS = $(OBJDIR)\GetOpt.obj \
		$(OBJDIR)\GNURegex.obj \
		$(OBJDIR)\regex.obj \
		$(OBJDIR)\strcasecmp.obj \
		$(OBJDIR)\strncasecmp.obj \
		$(OBJDIR)\strnlen1.obj

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
		$(OBJDIR)\BaseTypeFactory.obj \
		$(OBJDIR)\SignalHandler.obj \
		$(OBJDIR)\InternalErr.obj \
		$(OBJDIR)\util.obj \
		$(OBJDIR)\xdrutil_ppc.obj \
		$(OBJDIR)\parser-util.obj \
		$(OBJDIR)\escaping.obj \
		$(OBJDIR)\Clause.obj \
		$(OBJDIR)\RValue.obj \
		$(OBJDIR)\gse.tab.obj \
		$(OBJDIR)\ce_functions.obj \
		$(OBJDIR)\GSEClause.obj \
		$(OBJDIR)\lex.gse_.obj \
		$(OBJDIR)\ArrayGeoConstraint.obj

CLIENT_OBJS = $(OBJDIR)\RCReader.obj \
		$(OBJDIR)\Connect.obj \
		$(OBJDIR)\HTTPConnect.obj \
		$(OBJDIR)\HTTPCache.obj \
		$(OBJDIR)\AISResources.obj \
		$(OBJDIR)\AISDatabaseParser.obj \
		$(OBJDIR)\AISMerge.obj \
		$(OBJDIR)\AISConnect.obj \
		$(OBJDIR)\util_mit.obj \
		$(OBJDIR)\ResponseTooBigErr.obj

SERVER_OBJS = $(OBJDIR)\DODSFilter.obj \
		$(OBJDIR)\OPeNDAPFile.obj \
		$(OBJDIR)\OPeNDAPDir.obj \
		$(OBJDIR)\cgi_util.obj

# Convenience dependancies
libdapMT.lib:		$(OBJDIR)\libdapMT.lib
libdapMD.dll:		$(OBJDIR)\libdapMD.dll
libdapclientMT.lib:	$(OBJDIR)\libdapclientMT.lib
libdapclientMD.dll:	$(OBJDIR)\libdapclientMD.dll
libdapserverMT.lib:	$(OBJDIR)\libdapserverMT.lib
libdapserverMD.dll:	$(OBJDIR)\libdapserverMD.dll

getdap:		$(OBJDIR)\getdap.exe
deflate:	$(OBJDIR)\deflate.exe

# Link the libraries
$(OBJDIR)\libdapMT.lib: $(DAP_OBJS) $(GNU_OBJS)
	$(LINK) $(LFLAGS) /out:$@ $(DAP_OBJS) $(GNU_OBJS) $(LIBS)

$(OBJDIR)\libdapMD.dll: $(DAP_OBJS) $(GNU_OBJS) libdap.def
	$(LINK) $(LFLAGS) /out:$@ $(DAP_OBJS) $(GNU_OBJS) $(LIBS)

$(OBJDIR)\libdapclientMT.lib: $(CLIENT_OBJS)
	$(LINK) $(LFLAGS) /out:$@ $(CLIENT_OBJS) $(LIBS)

$(OBJDIR)\libdapclientMD.dll: $(CLIENT_OBJS) libdapclient.def
	$(LINK) $(LFLAGS) /out:$@ $(CLIENT_OBJS) $(LIBS)

$(OBJDIR)\libdapserverMT.lib: $(SERVER_OBJS)
	$(LINK) $(LFLAGS) /out:$@ $(SERVER_OBJS) $(LIBS)

$(OBJDIR)\libdapserverMD.dll: $(SERVER_OBJS) libdapserver.def
	$(LINK) $(LFLAGS) /out:$@ $(SERVER_OBJS) $(LIBS)
	
# Here's another way to do that mess below, but it also has
# the dependancy on MinGW's libtool, so is less desirable
# as we are looking to automate this completely in the future
# and the sole remaining dependancy on 'sed' fostered by
# replacing the use of 'dlltool' with MS utils is more desirable. 
#	@dlltool -z $@ --export-all-symbols $(DAP_OBJS) $(GNU_OBJS)
#	@rename $@ tmp.def
#	@sed --nocr -e '/^.[\?][\?]_.*/ d' \
#		-e '/^._CTA.*/ d' \
#		-e '/^._TI.*/ d' \
#		-e '/^._real.*/ d' tmp.def > $@
#	@del tmp.def
#

# This isn't exposed in the usage statement as it isn't intended
# for general users.  Rebuilding the .def's depends upon the installation
# of MinGW (or at least an acceptable win32 'sed').
exports: setup libdap.def libdapclient.def libdapserver.def

# Explainations for patterns to sed
# -e arg1   # Find the methods we intend to expose
# -e arg2   # Remove 'deleting destructors'
# -e arg3   # Move spaces to a tab
# -e arg4-7 # Remove things just _referenced_ from 3rd-party tools
# -e arg8   # Get rid of leading spaces
libdap.def: $(DAP_OBJS) $(GNU_OBJS)
	@lib /NOLOGO /out:$@.tmp $(DAP_OBJS) $(GNU_OBJS)
	@dumpbin /out:tmp.def /linkermember:2 $@.tmp
	@echo EXPORTS > $@
	@sed --nocr -n -e '/[0-9a-zA-Z] [_\?]/ !d' \
		-e '/^.[\?][\?]_.*/ d' \
		-e '/^_libdap_/ d' \
		-e '/^_re_/ d' \
		-e '/^_reg/ d' \
		-e '/^_str/ d' \
		-e '/^_xdr_/ d' \
		-e 's/..........// p' < tmp.def >> $@
	@del $@.tmp tmp.def

libdapclient.def: $(CLIENT_OBJS)
	@lib /NOLOGO /out:$@.tmp $(CLIENT_OBJS)
	@dumpbin /out:tmp.def /linkermember:2 $@.tmp
	@echo EXPORTS > $@
	@sed --nocr -n -e '/[0-9a-zA-Z] [_\?]/ !d' \
		-e '/^.[\?][\?]_.*/ d' \
		-e '/^_libdap_/ d' \
		-e '/^_re_/ d' \
		-e '/^_reg/ d' \
		-e '/^_str/ d' \
		-e '/^_xdr_/ d' \
		-e 's/..........// p' < tmp.def >> $@
	@del $@.tmp tmp.def

libdapserver.def: $(SERVER_OBJS)
	@lib /NOLOGO /out:$@.tmp $(SERVER_OBJS)
	@dumpbin /out:tmp.def /linkermember:2 $@.tmp
	@echo EXPORTS > $@
	@sed --nocr -n -e '/[0-9a-zA-Z] [_\?]/ !d' \
		-e '/^.[\?][\?]_.*/ d' \
		-e '/^_libdap_/ d' \
		-e '/^_re_/ d' \
		-e '/^_reg/ d' \
		-e '/^_str/ d' \
		-e '/^_xdr_/ d' \
		-e 's/..........// p' < tmp.def >> $@
	@del $@.tmp tmp.def

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
$(OBJDIR)\PassiveByte.obj				: $(SRCDIR)\PassiveByte.cc
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
$(OBJDIR)\AISResources.obj				: $(SRCDIR)\AISResources.cc
$(OBJDIR)\AISDatabaseParser.obj			: $(SRCDIR)\AISDatabaseParser.cc
$(OBJDIR)\AISMerge.obj					: $(SRCDIR)\AISMerge.cc
$(OBJDIR)\AISConnect.obj				: $(SRCDIR)\AISConnect.cc
$(OBJDIR)\util_mit.obj					: $(SRCDIR)\util_mit.cc
$(OBJDIR)\ResponseTooBigErr.obj			: $(SRCDIR)\ResponseTooBigErr.cc

# libdap server
$(OBJDIR)\DODSFilter.obj				: $(SRCDIR)\DODSFilter.cc
$(OBJDIR)\OPeNDAPFile.obj				: $(SRCDIR)\OPeNDAPFile.cc
$(OBJDIR)\OPeNDAPDir.obj				: $(SRCDIR)\OPeNDAPDir.cc
$(OBJDIR)\cgi_util.obj					: $(SRCDIR)\cgi_util.cc

# Regular expression code
$(OBJDIR)\GetOpt.obj		: $(SRCDIR)\GNU\GetOpt.cc
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\GNU\GetOpt.cc
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
	@$(LEX) -Pdas $(SRCDIR)\das.lex
	@copy lex.das.c $(SRCDIR)\lex.das.cc
	@del lex.das.c

$(SRCDIR)\das.tab.cc $(SRCDIR)\das.tab.h: $(SRCDIR)\das.y $(SRCDIR)\DAS.h
	@$(YACC) $(YACCFLAGS) -p das $(SRCDIR)\das.y
	@copy $(SRCDIR)\das.tab.c $(SRCDIR)\das.tab.cc
	@del $(SRCDIR)\das.tab.c

$(SRCDIR)\lex.dds.cc:	$(SRCDIR)\dds.lex $(SRCDIR)\dds.tab.cc $(SRCDIR)\dds.tab.h
	@$(LEX) -Pdds $(SRCDIR)\dds.lex
	@copy lex.dds.c $(SRCDIR)\lex.dds.cc
	@del lex.dds.c

$(SRCDIR)\dds.tab.cc $(SRCDIR)\dds.tab.h: $(SRCDIR)\dds.y
	@$(YACC) $(YACCFLAGS) -p dds $(SRCDIR)\dds.y
	@copy $(SRCDIR)\dds.tab.c $(SRCDIR)\dds.tab.cc
	@del $(SRCDIR)\dds.tab.c

$(SRCDIR)\lex.ce_expr.cc: $(SRCDIR)\ce_expr.lex $(SRCDIR)\ce_expr.tab.cc $(SRCDIR)\ce_expr.tab.h
	@$(LEX) -Pce_expr $(SRCDIR)\ce_expr.lex
	@copy lex.ce_expr.c $(SRCDIR)\lex.ce_expr.cc
	@del lex.ce_expr.c

$(SRCDIR)\ce_expr.tab.cc $(SRCDIR)\ce_expr.tab.h: $(SRCDIR)\ce_expr.y
	@$(YACC) $(YACCFLAGS) -p ce_expr $(SRCDIR)\ce_expr.y
	@copy $(SRCDIR)\ce_expr.tab.c $(SRCDIR)\ce_expr.tab.cc
	@del $(SRCDIR)\ce_expr.tab.c

$(SRCDIR)\lex.Error.cc:	$(SRCDIR)\Error.lex $(SRCDIR)\Error.tab.cc $(SRCDIR)\Error.tab.h
	@$(LEX) -PError $(SRCDIR)\Error.lex
	@copy lex.Error.c $(SRCDIR)\lex.Error.cc
	@del lex.Error.c

$(SRCDIR)\Error.tab.cc $(SRCDIR)\Error.tab.h: $(SRCDIR)\Error.y
	@$(YACC) $(YACCFLAGS) -p Error $(SRCDIR)\Error.y
	@copy $(SRCDIR)\Error.tab.c $(SRCDIR)\Error.tab.cc
	@del $(SRCDIR)\Error.tab.c

$(SRCDIR)\lex.gse_.cc:	$(SRCDIR)\gse.lex $(SRCDIR)\gse.tab.cc $(SRCDIR)\gse.tab.h
	@$(LEX) -Pgse_ $(SRCDIR)\gse.lex
	@copy lex.gse_.c $(SRCDIR)\lex.gse_.cc
	@del lex.gse_.c

$(SRCDIR)\gse.tab.cc $(SRCDIR)\gse.tab.h: $(SRCDIR)\gse.y
	@$(YACC) $(YACCFLAGS) -p gse_ $(SRCDIR)\gse.y
	@copy $(SRCDIR)\gse.tab.c $(SRCDIR)\gse.tab.cc
	@del $(SRCDIR)\gse.tab.c

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
	@-md $(OBJDIR)
	@-md $(INSTALLLIBDIR)
	@-md $(INSTALLBINDIR)
	@-md $(INSTALLETCDIR)
	@-md $(INSTALLDLLDIR)
	@-md $(INSTALLINCDIR)
	@-md $(INSTALLINCDIR)\GNU
	@-echo /*  Dummy patch header to get win32 flex/bison generated  */ > unistd.h
	@-echo /*  source code to compile under win32.  If you see this  */ >> unistd.h
	@-echo /*  temp file, feel free to scratch it.                   */ >> unistd.h
	@-echo #include "io.h" >> unistd.h

# Win32 libnc-dap requires the gl\regex.h.  Under unix builds, it is simply
# assumed to be on the machine already.
install: installMT installMD
	copy $(OBJDIR)\MT\getdap.exe $(INSTALLBINDIR)
	copy $(OBJDIR)\MT\deflate.exe $(INSTALLETCDIR)
	copy dods-datatypes.h $(INSTALLINCDIR)
	copy $(SRCDIR)\*.h $(INSTALLINCDIR)
	copy $(SRCDIR)\GNU\*.h $(INSTALLINCDIR)\GNU
	copy $(SRCDIR)\gl\regex.h $(INSTALLINCDIR)

installMT:
	@-copy $(OBJDIR)\MT\libdapMT.lib $(INSTALLLIBDIR)
	@-copy $(OBJDIR)\MT\libdapclientMT.lib $(INSTALLLIBDIR)
	@-copy $(OBJDIR)\MT\libdapserverMT.lib $(INSTALLLIBDIR)
	
installMD:
	@-copy $(OBJDIR)\MD\libdapMD.lib $(INSTALLLIBDIR)
	@-copy $(OBJDIR)\MD\libdapMD.dll $(INSTALLDLLDIR)
	@-copy $(OBJDIR)\MD\libdapclientMD.lib $(INSTALLLIBDIR)
	@-copy $(OBJDIR)\MD\libdapclientMD.dll $(INSTALLDLLDIR)
	@-copy $(OBJDIR)\MD\libdapserverMD.lib $(INSTALLLIBDIR)
	@-copy $(OBJDIR)\MD\libdapserverMD.dll $(INSTALLDLLDIR)

clean:
	@-rd /Q /S $(OBJDIR)
	@-del unistd.h

grammarclean: clean
	@-del ..\*.tab.cc
	@-del ..\*.tab.h
	@-del ..\lex.*.cc

!INCLUDE "dependancies"



