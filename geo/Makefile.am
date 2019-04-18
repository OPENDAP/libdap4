
AUTOMAKE_OPTIONS = foreign

# We need this to make sure that parallel jobs don't fail for 
# lack of stack.hh, ... because those are built by bison in one
# shot.
.NOTPARALLEL:

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/geo -I$(srcdir) $(XML2_CFLAGS) $(TIRPC_CFLAGS)
AM_CXXFLAGS =  

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

# autoconf/automake includes support for yacc and lex so that the input
# files for those compilers can be listed in a _SOURCES variable and the
# build will just work. I had a fair amount of hassle getting that going
# and then realized that cmake might undo all that effort. Also, the
# changes are not local to this dir only since I'd have to edit the DAP2
# grammars as well. I've left the edits in a comments although I'm not sure
# it ever worked correctly. jhrg 10/21/14
#
# AM_YFLAGS= -d
# AM_LFLAGS = -d

CXXFLAGS_DEBUG = -g3 -O0 -Wall -W -Wcast-align
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

if USE_ASAN
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
endif

if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXXFLAGS_DEBUG) $(ASAN_FLAGS)
else
AM_CXXFLAGS += -g -O2
endif

AM_LDFLAGS =
if ENABLE_COVERAGE
AM_CXXFLAGS += -fprofile-arcs -ftest-coverage -pg
AM_LDFLAGS += -lgcov --coverage -pg
endif

noinst_LTLIBRARIES = libgeodap.la
pkginclude_HEADERS = GSEClause.h grid_utils.h gse_parser.h

# This line forces make to build the grammar files first, which is
# important because some of the cc files include the parser headers.
BUILT_SOURCES = lex.gse.cc gse.tab.hh gse.tab.cc

libgeodap_la_SOURCES = lex.gse.cc gse.tab.hh gse.tab.cc GSEClause.cc \
	GSEClause.h grid_utils.cc grid_utils.h

libgeodap_la_CXXFLAGS = $(AM_CPPFLAGS) $(AM_CXXFLAGS)

EXTRA_DIST = gse.yy gse.ll

DISTCLEANFILES = 

clean-local:
	-rm lex.gse.cc gse.tab.hh gse.tab.cc

%.tab.cc %.tab.hh: %.yy
	$(YACC) $(YFLAGS) $<

# For now there are two min versions of flex's FlexLexer.h; one that
# defines the LexerInput() method returning int and one returning size_t.
# Including the gnerated lex.*.cc code causes a compilation error when
# the version on the packaging machine differs from the version on the
# building machine. For now I just remove the generated file. However, 
# if a fix can be found, then we can remove the flex requirement from
# a simple (tar.gz) source build. jhrg 7.9.15
dist-hook:
	rm $(distdir)/lex.gse.cc

lex.gse.cc: gse.ll gse.tab.cc gse.tab.hh
	$(LEX) $(LFLAGS) $<

