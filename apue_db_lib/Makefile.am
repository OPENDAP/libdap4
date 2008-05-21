
AUTOMAKE_OPTIONS = foreign 

AM_CPPFLAGS = -Wall

check_PROGRAMS = t4
noinst_LIBRARIES = libapue_db.a

libapue_db_a_SOURCES = error.c lockreg.c db.c

t4_SOURCES = t4.c
t4_LDADD = -L. -lapue_db

TESTS = t4
