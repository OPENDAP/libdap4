
# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -DDEFAULT_BASETYPE_FACTORY
AM_CXXFLAGS = -I../$(srcdir)/GNU

UNIT_TESTS = arrayT attrTableT structT sequenceT ddsT ddsPT iterT

check_PROGRAMS = $(UNIT_TESTS)
TESTS = $(UNIT_TESTS)

arrayT_SOURCES = arrayT.cc
arrayT_LDADD = -lcppunit -L../ -ldap -ltest-types

attrTableT_SOURCES = attrTableT.cc
attrTableT_LDADD = -lcppunit -L../ -ldap -ltest-types

structT_SOURCES = structT.cc
structT_LDADD = -lcppunit -L../ -ldap -ltest-types

sequenceT_SOURCES = sequenceT.cc
sequenceT_LDADD = -lcppunit -L../ -ldap -ltest-types

ddsT_SOURCES = ddsT.cc
ddsT_LDADD = -lcppunit -L../ -ldap -ltest-types

ddsPT_SOURCES = ddsPT.cc
ddsPT_LDADD = -lcppunit -L../ -ldap -ltest-types

iterT_SOURCES = iterT.cc LongIterAdapter.cc
iterT_LDADD = -lcppunit -L../ -ldap -ltest-types
