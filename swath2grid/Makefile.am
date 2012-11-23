
# Build libswath2grid, part of libdap.

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

noinst_LTLIBRARIES = libswath2grid.la

libswath2grid_la_SOURCES = $(SRCS) $(HDRS)
libswath2grid_la_CPPFLAGS = $(GDAL_CFLAGS)
libswath2grid_la_LDFLAGS = 
libswath2grid_la_LIBADD = $(GDAL_LIBS)

SRCS = AbstractDataset.cpp NC_GOES_Dataset.cpp wcs_error.cpp    \
BoundingBox.cpp wcsUtil.cpp

HDRS = AbstractDataset.h NC_GOES_Dataset.h wcs_error.h    \
BoundingBox.h wcsUtil.h
