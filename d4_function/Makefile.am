
AUTOMAKE_OPTIONS = foreign

# .NOTPARALLEL:

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/d4_ce -I$(srcdir) $(XML2_CFLAGS)
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
# old ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
endif

# Removed  $(CXX_STD_FLAG) since c++0x, gcc 4.4.x and bison -gneerated code
# do not work together. jhrg 4//5/19
if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXX_STD_FLAG) $(CXXFLAGS_DEBUG) $(ASAN_FLAGS)
else
AM_CXXFLAGS += -g -O2 $(CXX_STD_FLAG)
endif

AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

noinst_LTLIBRARIES = libd4_function_parser.la
pkginclude_HEADERS = D4FunctionEvaluator.h

# This line forces make to build the grammar files first, which is
# important because some of the cc files include the parser headers.
BUILT_SOURCES = lex.d4_function.cc d4_function_parser.tab.cc d4_function_parser.tab.hh \
	location.hh stack.hh position.hh
# stack.hh position.hh; removed for bison 3.3 jhrg 6/17/25, added back for 3.0 6/17/25

libd4_function_parser_la_SOURCES = D4FunctionEvaluator.cc	\
	D4FunctionEvaluator.h D4FunctionScanner.h	\
	d4_function_parser.tab.cc d4_function_parser.tab.hh lex.d4_function.cc \
	location.hh stack.hh position.hh
# stack.hh position.hh

libd4_ce_parser_la_CXXFLAGS = $(AM_CPPFLAGS) $(AM_CXXFLAGS)

EXTRA_DIST = d4_function_parser.yy d4_function_scanner.ll

CLEANFILES = *.gcda *.gcno *.gcov

DISTCLEANFILES = 

clean-local:
	-rm d4_function_parser.tab.cc d4_function_parser.tab.hh lex.d4_function.cc \
	    position.hh stack.hh position.hh
# stack.hh position.hh

%.tab.cc %.tab.hh: %.yy
	$(YACC) $(YFLAGS) $<

location.hh stack.hh position.hh: d4_function_parser.tab.cc d4_function_parser.tab.hh
# stack.hh position.hh

# See the comment in ../d4_ce
dist-hook:
	rm $(distdir)/lex.d4_function.cc

lex.d4_function.cc: d4_function_scanner.ll d4_function_parser.tab.cc d4_function_parser.tab.hh
	$(LEX) $(LFLAGS) $<
