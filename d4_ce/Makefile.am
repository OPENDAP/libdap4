# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(srcdir) $(XML2_CFLAGS)
AM_CXXFLAGS =  

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

# These are not used by automake but are often useful for certain types of
# debugging. 
CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

noinst_LTLIBRARIES = libd4_ce_parser.la

libd4_ce_parser_la_SOURCES = d4_ce_parser.tab.cc d4_ce_parser.tab.hh lex.d4_ce.cc \
	FlexLexer.h  location.hh position.hh stack.hh D4CEDriver.cc D4CEDriver.h \
	D4CEScanner.h 

EXTRA_DIST = 

DISTCLEANFILES = 

clean-local:
	-rm location.hh position.hh stack.hh d4_ce_parser.tab.cc d4_ce_parser.tab.hh lex.d4_ce.cc

lex.d4_ce.cc: d4_ce_scanner.ll d4_ce_parser.tab.cc d4_ce_parser.tab.hh
	$(LEX) $(LFLAGS) $<

d4_ce_parser.tab.cc d4_ce_parser.tab.hh stack.hh location.hh position.hh: d4_ce_parser.yy
	$(YACC) $(YFLAGS) $<
