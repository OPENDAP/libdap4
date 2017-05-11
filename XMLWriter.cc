
// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2010 OPeNDAP, Inc.
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

/*
 * XMLWriter.cpp
 *
 *  Created on: Jul 28, 2010
 *      Author: jimg
 */

#include "config.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "XMLWriter.h"
#include "InternalErr.h"

// TODO - Bite the bullet and make the encoding UTF-8 as required by dap4. This will break a lot of tests but the baselines could be amended using  a bash script and sed.
const char *ENCODING = "ISO-8859-1";
const int XML_BUF_SIZE = 2000000;

using namespace libdap;

XMLWriter::XMLWriter(const string &pad) {
    LIBXML_TEST_VERSION;

    /* Create a new XML buffer, to which the XML document will be
     * written */
    try {
        if (!(d_doc_buf = xmlBufferCreateSize(XML_BUF_SIZE)))
            throw InternalErr(__FILE__, __LINE__, "Error allocating the xml buffer");

        xmlBufferSetAllocationScheme(d_doc_buf, XML_BUFFER_ALLOC_DOUBLEIT);

        /* Create a new XmlWriter for memory, with no compression.
         * Remark: there is no compression for this kind of xmlTextWriter */
        if (!(d_writer = xmlNewTextWriterMemory(d_doc_buf, 0)))
            throw InternalErr(__FILE__, __LINE__, "Error allocating memory for xml writer");

        if (xmlTextWriterSetIndent(d_writer, pad.length()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Error starting indentation for response document ");

        if (xmlTextWriterSetIndentString(d_writer, (const xmlChar*)pad.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Error setting indentation for response document ");

        d_started = true;
        d_ended = false;

        /* Start the document with the xml default for the version,
         * encoding ISO 8859-1 and the default for the standalone
         * declaration. MY_ENCODING defined at top of this file*/
        if (xmlTextWriterStartDocument(d_writer, NULL, ENCODING, NULL) < 0)
            throw InternalErr(__FILE__, __LINE__, "Error starting xml response document");
    }
    catch (InternalErr &e) {
        m_cleanup();
        throw;
    }

}

XMLWriter::~XMLWriter() {
    m_cleanup();
}

void XMLWriter::m_cleanup() {
    // make sure the buffer and writer are all cleaned up
    if (d_writer) {
        xmlFreeTextWriter(d_writer); // This frees both d_writer and d_doc_buf
        d_writer = 0;
        // d_doc_buf = 0;
    }

    // We could be here because of an exception and d_writer might be zero
    if (d_doc_buf) {
        xmlBufferFree(d_doc_buf);
        d_doc_buf = 0;
    }

    d_started = false;
    d_ended = false;
}

const char *XMLWriter::get_doc() {
    if (d_writer && d_started) {
        if (xmlTextWriterEndDocument(d_writer) < 0)
            throw InternalErr(__FILE__, __LINE__, "Error ending the document");

        d_ended = true;

        // must call this before getting the buffer content. Odd, but appears to be true.
        // jhrg
        xmlFreeTextWriter(d_writer);
        d_writer = 0;
    }

    if (!d_doc_buf->content)
        throw InternalErr(__FILE__, __LINE__, "Error retrieving response document as string");

    return (const char *)d_doc_buf->content;
}

unsigned int XMLWriter::get_doc_size() {
    if (d_writer && d_started) {
        if (xmlTextWriterEndDocument(d_writer) < 0)
            throw InternalErr(__FILE__, __LINE__, "Error ending the document");

        d_ended = true;

        // must call this before getting the buffer content. Odd, but appears to be true.
        // jhrg
        xmlFreeTextWriter(d_writer);
        d_writer = 0;
    }

    if (!d_doc_buf->content)
        throw InternalErr(__FILE__, __LINE__, "Error retrieving response document as string");

    // how much of the buffer is in use?
    return d_doc_buf->use;
}
