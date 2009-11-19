/*
 * Dap4SharedDimension.cc
 *
 *  Created on: Nov 10, 2009
 *      Author: jimg
 */

#include "config.h"

#include <ostream>
#include <sstream>

#include "DapIndent.h"
#include "Dap4SharedDimension.h"
#include "InternalErr.h"
#include "escaping.h"

#define XML_CHAR(x) reinterpret_cast<const xmlChar*>(x)

using namespace std;

namespace libdap {

/** @brief Write the Shared Dimension
 *
 * This is the old-style print_xml() method that writes XML using strings.
 *
 * @deprecated
 * @param out Write to this stream
 * @param space Indentation; default is four spaces
 * @param constrained Should the current projection be used? Default is false.
 */
void Dap4SharedDimension::print_xml(ostream &out, string space, bool) {
    out << space << "<Dimension name=\"" << id2xml(d_name) << "\" size=\""
	    << d_size << "\"/>" << endl;
}

/** @brief Write the Share Dimension
 *
 * New style output method. This writes to the xml Text Writer pointer. The
 * libxml2 library handles formatting. The constrained parameter operates as
 * with print_xml(ostream &, string, bool).
 *
 * @param writer Write to this libxml2 buffer
 * @param constrained Should the current projection be used? Default is false.
 */
void Dap4SharedDimension::print_xml(xmlTextWriterPtr writer, bool) {
#if 0
    xmlTextWriterPtr writer;
    xmlBufferPtr buf;

    /* Create a new XML buffer, to which the XML document will be
     * written */
    buf = xmlBufferCreate();
    if (!buf)
        throw InternalErr(__FILE__, __LINE__, "Error creating the xml buffer");

    /* Create a new XmlWriter for memory, with no compression.
     * Remark: there is no compression for this kind of xmlTextWriter */
    writer = xmlNewTextWriterMemory(buf, 0);
    if (!writer)
        throw InternalErr(__FILE__, __LINE__, "Error creating the xml writer");

    /* Start the document with the xml default for the version,
     * encoding ISO 8859-1 and the default for the standalone
     * declaration. */
    if (xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0)
        throw InternalErr(__FILE__, __LINE__, "Error starting xml document");

   #endif


    if (xmlTextWriterStartElement(writer, XML_CHAR("Dimension")) < 0)
        throw InternalErr(__FILE__, __LINE__, "Error writing Dimension");
    if (xmlTextWriterWriteAttribute(writer, XML_CHAR("name"), XML_CHAR(d_name.c_str())) < 0)
        throw InternalErr(__FILE__, __LINE__, "Error writing Dimension name");
    ostringstream size; size << d_size;
    if (xmlTextWriterWriteAttribute(writer, XML_CHAR("size"), XML_CHAR(size.str().c_str())) < 0)
        throw InternalErr(__FILE__, __LINE__, "Error writing Dimension size");
    if (xmlTextWriterEndElement(writer) < 0)
        throw InternalErr(__FILE__, __LINE__, "Error closing Dimension");
}

void Dap4SharedDimension::dump(ostream &strm) const {
    strm << DapIndent::LMarg << "Dap4SharedDimension::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    strm << "name: " << d_name << endl;
    strm << "size: " << d_size << endl;
    DapIndent::UnIndent() ;
}

}
