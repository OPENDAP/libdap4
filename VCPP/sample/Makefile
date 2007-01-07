# Sample Makefile to illustrate how to compile and link with
# libdap (under VC++ 8.x)

OBJDIR		= .\objects
SRCDIR		= .

# Compiler, Linker
CC		= cl
LINK		= link

# Setup compiler flags
MTCFLAGS	= /MT
MDCFLAGS	= /MD
CFLAGS		= /nologo /W0 /GR /EHsc /c

# Setup linker flags
EMTLFLAGS	= /nologo /libpath:..\..\lib /NODEFAULTLIB:MSVCRT.lib
EMDLFLAGS	= /nologo /libpath:..\..\lib

# Setup link libraries
EMTLLIBS	= libdapMT.lib libdapclientMT.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib
EMDLLIBS	= libdapMD.lib libdapclientMD.lib libxml2.lib libcurl.lib zlib.lib libxdr.lib pthreadVC.lib

# Setup compiler includes and defines
INCS		= /I . /I ..\..\include /I ..\..\include\GNU /I ..\..\include\gl /I ..\..\include\xdr /I ..\..\include\curl /I ..\..\include\pthreads
CPPUNITINCS	= $(INCS) /I $(INCLUDEDIR)/cppunit
DEFS		= /D WIN32 /D WIN32_LEAN_AND_MEAN
CPPFLAGS	= $(DEFS) $(INCS)

default:
	@echo "usage: nmake model<MT|MD>; nmake all"

all:
	nmake modelMT
	nmake modelMD

modelMT: setup
	-md $(OBJDIR)\MT
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT getdap

modelMD: setup
	-md $(OBJDIR)\MD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD getdap

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .cc .exe

{$(SRCDIR)}.cc{$(OBJDIR)}.obj:
	$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $<

# Rule for linking executables.  See Microsoft's "mt" command for
# embedding external .manifests as necessary.
{$(OBJDIR)}.obj{$(OBJDIR)}.exe:
	$(LINK) $(LFLAGS) /out:$@ $< $(LIBS)

getdap:		$(OBJDIR)\getdap.exe

$(OBJDIR)\getdap.exe: $(OBJDIR)\getdap.obj

# Command-Line Client and select utils in console mode
$(OBJDIR)\getdap.obj    : $(SRCDIR)\getdap.cc
	$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $(SRCDIR)\getdap.cc

setup:
	-mkdir $(OBJDIR)

clean:
	-rmdir /Q /S $(OBJDIR)





