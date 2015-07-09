
AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/d4_ce -I$(srcdir) $(XML2_CFLAGS)
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

if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXXFLAGS_DEBUG)
endif

noinst_LTLIBRARIES = libd4_function_parser.la
pkginclude_HEADERS = D4FunctionEvaluator.h

# This line forces make to build the grammar files first, which is
# important because some of the cc files include the parser headers.
BUILT_SOURCES = lex.d4_function.cc d4_function_parser.tab.cc d4_function_parser.tab.hh \
stack.hh location.hh position.hh

libd4_function_parser_la_SOURCES = D4FunctionEvaluator.cc	\
	D4FunctionEvaluator.h D4FunctionScanner.h	\
	d4_function_parser.tab.cc d4_function_parser.tab.hh lex.d4_function.cc \
	stack.hh location.hh position.hh

libd4_ce_parser_la_CXXFLAGS=$(AM_CPPFLAGS)

EXTRA_DIST = d4_function_parser.yy d4_function_scanner.ll

DISTCLEANFILES = 

clean-local:
	-rm d4_function_parser.tab.cc d4_function_parser.tab.hh lex.d4_function.cc \
	    stack.hh position.hh location.hh

%.tab.cc %.tab.hh: %.yy
	$(YACC) $(YFLAGS) $<

#d4_function_parser.tab.cc d4_function_parser.tab.hh stack.hh location.hh position.hh: d4_function_parser.yy
#	$(YACC) $(YFLAGS) $<

# See the comment in ../d4_ce
dist-hook:
	rm $(distdir)/lex.d4_function.cc

lex.d4_function.cc: d4_function_scanner.ll d4_function_parser.tab.cc d4_function_parser.tab.hh
	$(LEX) $(LFLAGS) $<
