# Makefile for libxdr

# Setup some preliminary directories
OBJDIR		= .\objects
SRCDIR		= .
INSTALLBINDIR	= ..\..\..\..\bin
INSTALLETCDIR	= ..\..\..\..\etc
INSTALLLIBDIR	= ..\..\..\..\lib
INSTALLDLLDIR	= ..\..\..\..\dll
INSTALLINCDIR	= ..\..\..\..\include

# Compiler, Linker
CC		= cl
LINK		= link

# ST = Single-Threaded Model
# MT = Multi-Threaded Model
# MD = Multi-Threaded DLL Model
# STD = Single-Threaded Model, debug version
# MTD = Multi-Threaded Model, debug version
# MDD = Multi-Thread DLL Model, debug version

# Setup compiler flags
STCFLAGS	= /ML
MTCFLAGS	= /MT
MDCFLAGS	= /MD
STDCFLAGS	= /MLd /Zi /Yd
MTDCFLAGS	= /MTd /Zi /Yd
MDDCFLAGS	= /MDd
CFLAGS		= /nologo /W0 /GR /GX /c

BASELIB		= ws2_32.lib

# Setup linker flags for libraries
STLFLAGS	= -lib /nologo
MTLFLAGS	= -lib /nologo
MDLFLAGS	= /DLL /DEF:libxdr.def /FIXED:NO /BASE:0x05000000
STDLFLAGS	= -lib /nologo
MTDLFLAGS	= -lib /nologo
MDDLFLAGS	= /DLL /DEF:libxdr.def /debug /FIXED:NO /BASE:0x05000000
LIBLIBS		= $(BASELIB)

# Setup linker flags for executables
ESTLFLAGS	= /nologo
EMTLFLAGS	= /nologo
EMDLFLAGS	= /nologo
ESTDLFLAGS	= /nologo /debug
EMTDLFLAGS	= /nologo /debug
EMDDLFLAGS	= /nologo /debug
ESTLLIBS	= libxdrST.lib $(BASELIB)
EMTLLIBS	= libxdrMT.lib $(BASELIB)
EMDLLIBS	= libxdrMD.lib $(BASELIB)
ESTDLLIBS	= libxdrSTD.lib $(BASELIB)
EMTDLLIBS	= libxdrMTD.lib $(BASELIB)
EMDDLLIBS	= libxdrMDD.lib $(BASELIB)

# Setup compiler includes and defines
INCS		= /I .
DEFS		= /D WIN32
CPPFLAGS	= $(DEFS) $(INCS)

usage:
	@echo "usage: nmake all; nmake install"
	@echo "or"
	@echo "usage: nmake model<ST|MT|MD|STD|MTD|MDD>; nmake install<ST|MT|MD|STD|MTD|MDD>"

all:	setup
	nmake modelST
	nmake modelMT
	nmake modelMD
	nmake modelSTD
	nmake modelMTD
	nmake modelMDD

modelST:
	@-md $(OBJDIR)\ST
	@nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(STLFLAGS)" LIBS="$(LIBLIBS)" OBJDIR=$(OBJDIR)\ST libxdrST.lib
	@nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST xdrtest

modelMT:
	@-md $(OBJDIR)\MT
	@nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(MTLFLAGS)" LIBS="$(LIBLIBS)" OBJDIR=$(OBJDIR)\MT libxdrMT.lib
	@nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT xdrtest

modelMD:
	@-md $(OBJDIR)\MD
	@nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(MDLFLAGS)" LIBS="$(LIBLIBS)" OBJDIR=$(OBJDIR)\MD libxdrMD.dll
	@nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD xdrtest

modelSTD:
	@-md $(OBJDIR)\STD
	@nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(STDLFLAGS)" LIBS="$(LIBLIBS)" OBJDIR=$(OBJDIR)\STD libxdrSTD.lib
	@nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD xdrtest

modelMTD:
	@-md $(OBJDIR)\MTD
	@nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(MTDLFLAGS)" LIBS="$(LIBLIBS)" OBJDIR=$(OBJDIR)\MTD libxdrMTD.lib
	@nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD xdrtest

modelMDD:
	@-md $(OBJDIR)\MDD
	@nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(MDDLFLAGS)" LIBS="$(LIBLIBS)" OBJDIR=$(OBJDIR)\MDD libxdrMDD.dll
	@nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD xdrtest

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .c

{$(SRCDIR)}.c{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ $<

LIBOBJS = $(OBJDIR)\xdr.obj \
	$(OBJDIR)\xdr_array.obj \
	$(OBJDIR)\xdr_float.obj \
	$(OBJDIR)\xdr_mem.obj \
	$(OBJDIR)\xdr_rec.obj \
	$(OBJDIR)\xdr_rec2.obj \
	$(OBJDIR)\xdr_reference.obj \
	$(OBJDIR)\xdr_stdio.obj

# Convenience dependancies
libxdrST.lib: $(OBJDIR)\libxdrST.lib
libxdrMT.lib: $(OBJDIR)\libxdrMT.lib
libxdrMD.dll: $(OBJDIR)\libxdrMD.dll
libxdrSTD.lib: $(OBJDIR)\libxdrSTD.lib
libxdrMTD.lib: $(OBJDIR)\libxdrMTD.lib
libxdrMDD.dll: $(OBJDIR)\libxdrMDD.dll

xdrtest: $(OBJDIR)\xdrtest.exe
	@$(OBJDIR)\xdrtest> $(OBJDIR)\xdrtest.log

# Link the library
$(OBJDIR)\libxdrST.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libxdrMT.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libxdrMD.dll: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libxdrSTD.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@  $(LIBOBJS) $(LIBS)

$(OBJDIR)\libxdrMTD.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libxdrMDD.dll: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\xdrtest.exe: $(OBJDIR)\xdrtest.obj
	@$(LINK) $(LFLAGS) /libpath:$(OBJDIR) /out:$@ $(OBJDIR)\xdrtest.obj $(LIBS)

$(OBJDIR)\xdr.obj : $(SRCDIR)\xdr.c
$(OBJDIR)\xdr_array.obj : $(SRCDIR)\xdr_array.c
$(OBJDIR)\xdr_float.obj : $(SRCDIR)\xdr_float.c
$(OBJDIR)\xdr_mem.obj : $(SRCDIR)\xdr_mem.c
$(OBJDIR)\xdr_rec.obj : $(SRCDIR)\xdr_rec.c
$(OBJDIR)\xdr_rec2.obj : $(SRCDIR)\xdr_rec2.c
$(OBJDIR)\xdr_reference.obj : $(SRCDIR)\xdr_reference.c
$(OBJDIR)\xdr_stdio.obj : $(SRCDIR)\xdr_stdio.c
$(OBJDIR)\xdrtest.obj: $(SRCDIR)\xdrtest.c

# Preliminary setup
setup:
	@-mkdir $(OBJDIR)
	@-mkdir $(INSTALLBINDIR)
	@-mkdir $(INSTALLETCDIR)
	@-mkdir $(INSTALLLIBDIR)
	@-mkdir $(INSTALLDLLDIR)
	@-mkdir $(INSTALLINCDIR)

# Install
install: installST installMT installMD installSTD installMTD installMDD
	@copy $(SRCDIR)\xdr.h $(INSTALLINCDIR)

installST:
	@copy $(OBJDIR)\ST\libxdrST.lib $(INSTALLLIBDIR)

installMT:
	@copy $(OBJDIR)\MT\libxdrMT.lib $(INSTALLLIBDIR)

installMD:
	@copy $(OBJDIR)\MD\libxdrMD.lib $(INSTALLLIBDIR)
	@copy $(OBJDIR)\MD\libxdrMD.dll $(INSTALLDLLDIR)

installSTD:
	@copy $(OBJDIR)\STD\libxdrSTD.lib $(INSTALLLIBDIR)

installMTD:
	@copy $(OBJDIR)\MTD\libxdrMTD.lib $(INSTALLLIBDIR)

installMDD:
	@copy $(OBJDIR)\MDD\libxdrMDD.lib $(INSTALLLIBDIR)
	@copy $(OBJDIR)\MDD\libxdrMDD.dll $(INSTALLDLLDIR)

clean:
	@-rmdir /Q /S $(OBJDIR)

!INCLUDE "dependancies"



