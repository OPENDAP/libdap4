
# Setup directories
OBJDIR		= .\objects
INSTALLDIR	= ..\..\..\..\lib
INCLUDEDIR	= ..\..\..\..\include

# Setup compiler and linker flags
CC		= cl
CFLAGS		= /nologo /ML /W0 /GR /GX /c
#CFLAGS		= /MLd /Zi /W3 /GR /GX /c	# debug
LINK		= link
LFLAGS		= -lib /nologo
LIBS		= ws2_32.lib

# Setup compiler includes and defines
INCS		= /I .
DEFS		= /D WIN32 /D _LIB
CPPFLAGS	= $(DEFS) $(INCS)

# Define the targets
TARGET		= $(OBJDIR)\libxdr.lib
TESTTARG	= $(OBJDIR)\xdrtest.exe

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .c

{.}.c{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ $<

LIBOBJS = $(OBJDIR)\xdr.obj \
		$(OBJDIR)\xdr_array.obj \
		$(OBJDIR)\xdr_float.obj \
		$(OBJDIR)\xdr_mem.obj \
		$(OBJDIR)\xdr_rec.obj \
		$(OBJDIR)\xdr_rec2.obj \
		$(OBJDIR)\xdr_reference.obj \
		$(OBJDIR)\xdr_stdio.obj

TESTOBJS = $(OBJDIR)\xdrtest.obj

all: setup $(TARGET) $(TESTTARG)

$(OBJDIR)\libxdr.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS)

$(OBJDIR)\xdrtest.exe: $(TESTOBJS)
	@$(LINK) /nologo /subsystem:console /out:$@ $(TESTOBJS) $(TARGET) $(LIBS)

install:
	copy $(TARGET) $(INSTALLDIR)
	copy .\xdr.h $(INCLUDEDIR)	

$(OBJDIR)\xdr.obj : .\xdr.c
$(OBJDIR)\xdr_array.obj : .\xdr_array.c
$(OBJDIR)\xdr_float.obj : .\xdr_float.c
$(OBJDIR)\xdr_mem.obj : .\xdr_mem.c
$(OBJDIR)\xdr_rec.obj : .\xdr_rec.c
$(OBJDIR)\xdr_rec2.obj : .\xdr_rec2.c
$(OBJDIR)\xdr_reference.obj : .\xdr_reference.c
$(OBJDIR)\xdr_stdio.obj : .\xdr_stdio.c
$(OBJDIR)\xdrtest.obj: .\xdrtest.c

setup:
	@-mkdir $(OBJDIR)
	@-mkdir $(INSTALLDIR)
	@-mkdir $(INCLUDEDIR)

clean:
	@-rmdir /S /Q $(OBJDIR)

!INCLUDE "dependancies"



