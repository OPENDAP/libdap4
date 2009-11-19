// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sstream>

#include <libxml/xmlwriter.h>

#include "config.h"

// #define DODS_DEBUG2
#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "Dap4Group.h"
#include "AttrTable.h"

#include "GNURegex.h"
#include "util.h"
#include "debug.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

namespace libdap {

class Dap4GroupTest: public TestFixture {
private:
    Dap4Group *g0, *g1, *g2;
    AttrTable a;

public:
    Dap4GroupTest() :
	g0(0), g1(0), g2(0)
    {
	a.append_attr("long_name", "String", "groups");
	a.append_attr("coefs", "Float64", "0.05");
	a.append_attr("coefs", "Float64", "0.25");
    }
    ~Dap4GroupTest()
    {
	delete g0; g0 = 0;
	delete g1; g1 = 0;
	delete g2; g2 = 0;
    }

    void setUp()
    {
	g0 = new Dap4Group("g0");

	g1 = new Dap4Group("g1");
	g1->add_var(new Byte("b"));
	g1->add_var(new Str("s"));
	Array a("a", new Byte("a"));
	a.append_dim(1024, "lat");
	a.append_dim(1024, "lon");
	g1->add_var(&a);

	g2 = new Dap4Group("g2");
	g2->add_var(new Byte("b"));
	g2->add_var(new Str("s"));
	g2->add_dimension("lon", 1024);
	g2->add_dimension("lat", 1024);
	g2->add_dimension("alt", 10);
    }

    void tearDown()
    {
	delete g0; g0 = 0;
	delete g1; g1 = 0;
	delete g2; g2 = 0;
    }

    bool re_match(Regex &r, const string &s)
    {
	int match = r.match(s.c_str(), s.length());
	DBG(cerr << "Match: " << match << " should be: " << s.length()
		<< endl);
	return match == static_cast<int> (s.length());
    }

    CPPUNIT_TEST_SUITE( Dap4GroupTest );

    CPPUNIT_TEST(print_empty_xml);
    CPPUNIT_TEST(print_xml);
    CPPUNIT_TEST(copy_constructor);
    CPPUNIT_TEST(assignment_operator);
    CPPUNIT_TEST(with_dimensions);
    CPPUNIT_TEST(with_dimensions_and_attributes);
    CPPUNIT_TEST(xml_writer_output);

    CPPUNIT_TEST_SUITE_END();

    void print_empty_xml()
    {
	ostringstream oss;
	g0->print_xml(oss);
	DBG2(cerr << "g0 output: '" << oss.str() << "'" << endl);
	Regex r("    <Group name=\"g0\"/>\n");
	CPPUNIT_ASSERT(re_match(r, oss.str()));
    }

    void print_xml()
    {
	ostringstream oss;
	g1->print_xml(oss);
	DBG2(cerr << "g1 output: '" << oss.str() << "'" << endl);
	Regex r("\
    <Group name=\"g1\">\n\
        <Byte name=\"b\"/>\n\
        <String name=\"s\"/>\n\
        <Array name=\"a\">\n\
            <Byte/>\n\
            <dimension name=\"lat\" size=\"1024\"/>\n\
            <dimension name=\"lon\" size=\"1024\"/>\n\
        </Array>\n\
    </Group>\n");
	CPPUNIT_ASSERT(re_match(r, oss.str()));
    }

    void copy_constructor()
    {
	Dap4Group *g = new Dap4Group(*g1);

	ostringstream oss;
	g->print_xml(oss);
	DBG2(cerr << "g1 output: '" << oss.str() << "'" << endl);
	Regex r("\
    <Group name=\"g1\">\n\
        <Byte name=\"b\"/>\n\
        <String name=\"s\"/>\n\
        <Array name=\"a\">\n\
            <Byte/>\n\
            <dimension name=\"lat\" size=\"1024\"/>\n\
            <dimension name=\"lon\" size=\"1024\"/>\n\
        </Array>\n\
    </Group>\n");
	CPPUNIT_ASSERT(re_match(r, oss.str()));
    }

    void assignment_operator()
    {
	Dap4Group g = *g1;

	ostringstream oss;
	g.print_xml(oss);
	DBG2(cerr << "g1 output: '" << oss.str() << "'" << endl);
	Regex r("\
    <Group name=\"g1\">\n\
        <Byte name=\"b\"/>\n\
        <String name=\"s\"/>\n\
        <Array name=\"a\">\n\
            <Byte/>\n\
            <dimension name=\"lat\" size=\"1024\"/>\n\
            <dimension name=\"lon\" size=\"1024\"/>\n\
        </Array>\n\
    </Group>\n");
	CPPUNIT_ASSERT(re_match(r, oss.str()));
    }

    void with_dimensions() {
	ostringstream oss;
	g2->print_xml(oss);
	DBG(cerr << "g2 output: '" << oss.str() << "'" << endl);
	Regex r("\
    <Group name=\"g2\">\n\
        <Dimension name=\"lon\" size=\"1024\"/>\n\
        <Dimension name=\"lat\" size=\"1024\"/>\n\
        <Dimension name=\"alt\" size=\"10\"/>\n\
        <Byte name=\"b\"/>\n\
        <String name=\"s\"/>\n\
    </Group>\n");
	CPPUNIT_ASSERT(re_match(r, oss.str()));
    }

    void with_dimensions_and_attributes() {
	g2->set_attr_table(a);
	ostringstream oss;
	g2->print_xml(oss);
	DBG(cerr << "g2 output: '" << oss.str() << "'" << endl);
	Regex r("\
    <Group name=\"g2\">\n\
        <Dimension name=\"lon\" size=\"1024\"/>\n\
        <Dimension name=\"lat\" size=\"1024\"/>\n\
        <Dimension name=\"alt\" size=\"10\"/>\n\
        <Attribute name=\"long_name\" type=\"String\">\n\
            <value>groups</value>\n\
        </Attribute>\n\
        <Attribute name=\"coefs\" type=\"Float64\">\n\
            <value>0.05</value>\n\
            <value>0.25</value>\n\
        </Attribute>\n\
        <Byte name=\"b\"/>\n\
        <String name=\"s\"/>\n\
    </Group>\n");
	CPPUNIT_ASSERT(re_match(r, oss.str()));
    }

    void xml_writer_output() {
	xmlTextWriterPtr writer;
	xmlBufferPtr buf;

	/* Create a new XML buffer, to which the XML document will be
	 * written */
	buf = xmlBufferCreate();
	if (!buf)
	    throw InternalErr(__FILE__, __LINE__,
		    "Error creating the xml buffer");

	/* Create a new XmlWriter for memory, with no compression.
	 * Remark: there is no compression for this kind of xmlTextWriter */
	writer = xmlNewTextWriterMemory(buf, 0);
	if (!writer)
	    throw InternalErr(__FILE__, __LINE__,
		    "Error creating the xml writer");

	/* Start the document with the xml default for the version,
	 * encoding UTF-8 and the default for the standalone
	 * declaration. */
	if (xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0)
	    throw InternalErr(__FILE__, __LINE__, "Error starting xml document");

	if (xmlTextWriterSetIndent(writer, 1) < 0
		|| xmlTextWriterSetIndentString(writer, (xmlChar*)"    ") < 0)
	    throw InternalErr(__FILE__, __LINE__, "Error setting indentation for XML output");

	g2->set_attr_table(a);
	g2->print_xml(writer);

	/* Here we could close the elements ORDER and EXAMPLE using the
	 * function xmlTextWriterEndElement, but since we do not want to
	 * write any other elements, we simply call xmlTextWriterEndDocument,
	 * which will do all the work. */
	if (xmlTextWriterEndDocument(writer) < 0)
	    throw InternalErr(__FILE__, __LINE__, "Error at xmlTextWriterEndDocument");

	xmlFreeTextWriter(writer);
	cerr << (const char *) buf->content << endl;

	xmlBufferFree(buf);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Dap4GroupTest);

}

int main(int, char**)
{
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = runner.run("", false);

    return wasSuccessful ? 0 : 1;
}

