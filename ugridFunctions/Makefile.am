
# Build libugridFunctions, part of libdap.

AUTOMAKE_OPTIONS = foreign

# AM_CPPFLAGS = 

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
#     export CXXFLAGS='...'; ./configure --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

# SUBDIRS = 
# DIST_SUBDIRS = 

noinst_LTLIBRARIES = libugridFunctions.la

libugridFunctions_la_SOURCES = $(SRCS) $(HDRS)
libugridFunctions_la_CPPFLAGS = $(GF_CFLAGS) $(XML2_CFLAGS)
libugridFunctions_la_LDFLAGS = 
libugridFunctions_la_LIBADD = $(GF_LIBS) 

SRCS = gf2.cc ugrid_utils.cc MeshDataVariable.cc TwoDMeshTopology.cc  gf3.cc

# NC_GOES_Dataset.cpp NC_GOES_Dataset.h

HDRS = gf2.h ugrid_utils.h MeshDataVariable.h  TwoDMeshTopology.h gf3.h
