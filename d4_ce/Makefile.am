# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/d4_ce -I$(srcdir) $(XML2_CFLAGS)
AM_CXXFLAGS =  

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

# These are not used by automake but are often useful for certain types of
# debugging. 
CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

noinst_LTLIBRARIES = libd4_ce_parser.la
pkginclude_HEADERS = D4CEDriver.h

libd4_ce_parser_la_SOURCES = d4_ce_parser.tab.cc d4_ce_parser.tab.hh lex.d4_ce.cc \
	location.hh position.hh stack.hh D4CEDriver.cc D4CEDriver.h D4CEScanner.h
	 
libd4_ce_parser_la_CXXFLAGS=$(AM_CPPFLAGS)

EXTRA_DIST = d4_ce_parser.yy d4_ce_scanner.ll

DISTCLEANFILES = 

clean-local:
	-rm location.hh position.hh stack.hh d4_ce_parser.tab.cc d4_ce_parser.tab.hh lex.d4_ce.cc

lex.d4_ce.cc: d4_ce_scanner.ll d4_ce_parser.tab.cc d4_ce_parser.tab.hh
	$(LEX) $(LFLAGS) $<

d4_ce_parser.tab.cc d4_ce_parser.tab.hh stack.hh location.hh position.hh: d4_ce_parser.yy
	$(YACC) $(YFLAGS) $<
