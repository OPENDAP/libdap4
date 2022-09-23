
AUTOMAKE_OPTIONS = foreign

# We need this to make sure that parallel jobs don't fail for 
# lack of stack.hh, ... because those are built by bison in one
# shot.
.NOTPARALLEL:

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/d4_ce -I$(srcdir)  $(XML2_CFLAGS) $(TIRPC_CFLAGS)
AM_CXXFLAGS = -Wall -W -Wcast-align -Wno-inconsistent-missing-override

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

if USE_ASAN
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
endif

# See note in d4_function about CXX11_FLAG. jhrg 4/5/19
if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXX11_FLAG) $(CXXFLAGS_DEBUG) $(ASAN_FLAGS)
else
AM_CXXFLAGS += -g -O2 $(CXX11_FLAG)
endif

AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

noinst_LTLIBRARIES = libd4_ce_parser.la
pkginclude_HEADERS = D4ConstraintEvaluator.h

# This line forces make to build the grammar files first, which is
# important because some of the cc files include the parser headers.
BUILT_SOURCES = lex.d4_ce.cc d4_ce_parser.tab.hh d4_ce_parser.tab.cc \
	stack.hh location.hh position.hh

libd4_ce_parser_la_SOURCES = lex.d4_ce.cc d4_ce_parser.tab.cc	\
	d4_ce_parser.tab.hh stack.hh location.hh position.hh \
	D4ConstraintEvaluator.cc  D4ConstraintEvaluator.h D4CEScanner.h   

libd4_ce_parser_la_CXXFLAGS = $(AM_CPPFLAGS) $(AM_CXXFLAGS)

EXTRA_DIST = d4_ce_parser.yy d4_ce_scanner.ll

CLEANFILES = *.gcda *.gcno *.gcov

DISTCLEANFILES = 

clean-local:
	-rm d4_ce_parser.tab.cc	d4_ce_parser.tab.hh lex.d4_ce.cc stack.hh \
		location.hh position.hh

%.tab.cc %.tab.hh: %.yy
	$(YACC) $(YFLAGS) $<

d4_ce_parser.tab.cc d4_ce_parser.tab.hh stack.hh location.hh position.hh: d4_ce_parser.yy
	$(YACC) $(YFLAGS) $<

# For now there are two min versions of flex's FlexLexer.h; one that
# defines the LexerInput() method returning int and one returning size_t.
# Including the gnerated lex.*.cc code causes a compilation error when
# the version on the packaging machine differs from the version on the
# building machine. For now I just remove the generated file. However, 
# if a fix can be found, then we can remove the flex requirement from
# a simple (tar.gz) source build. jhrg 7.9.15
dist-hook:
	rm $(distdir)/lex.d4_ce.cc

lex.d4_ce.cc: d4_ce_scanner.ll d4_ce_parser.tab.cc d4_ce_parser.tab.hh
	$(LEX) $(LFLAGS) $<

