
AUTOMAKE_OPTIONS = foreign

SUBDIRS = . unit-tests

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(srcdir) -I$(top_srcdir) -I$(top_srcdir)/GNU $(XML2_CFLAGS)
AM_CXXFLAGS =

if COMPILER_IS_GCC
AM_CXXFLAGS += -Wall -W -Wcast-align
endif

CXXFLAGS_DEBUG = -g3 -O0

if USE_ASAN
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
endif

# See note in d4_function about CXX_STD_FLAG. jhrg 4/5/19
if BUILD_DEVELOPER
AM_CXXFLAGS += $(CXX_STD_FLAG) $(CXXFLAGS_DEBUG) $(ASAN_FLAGS)
else
AM_CXXFLAGS += -g -O2 $(CXX_STD_FLAG)
endif

AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

noinst_LTLIBRARIES = libhttp_dap.la
pkginclude_HEADERS = HTTPConnect.h

BUILT_SOURCES =

libhttp_dap_la_SOURCES = HTTPCache.cc \
                         HTTPCache.h \
                         HTTPCacheDisconnectedMode.h \
                         HTTPCacheInterruptHandler.h \
                         HTTPCacheMacros.h \
                         HTTPCacheResponse.h \
                         HTTPCacheTable.cc \
                         HTTPCacheTable.h \
                         HTTPConnect.cc \
                         HTTPConnect.h \
                         HTTPResponse.h \
                         ResponseTooBigErr.h \
                         ResponseTooBigErr.cc

libhttp_dap_la_CXXFLAGS = $(AM_CPPFLAGS) $(AM_CXXFLAGS)

EXTRA_DIST =

CLEANFILES = *.gcda *.gcno *.gcov

DISTCLEANFILES = 

