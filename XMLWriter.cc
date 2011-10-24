/*
 * XMLWriter.cpp
 *
 *  Created on: Jul 28, 2010
 *      Author: jimg
 */

#include "XMLWriter.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <InternalErr.h>

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
        d_doc_buf = 0;
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
    }

    if (!d_doc_buf->content)
        throw InternalErr(__FILE__, __LINE__, "Error retrieving response document as string");

    return (const char *)d_doc_buf->content;

#if 0
    if (*(const char *)d_doc_buf->content == '\n')
        return (const char *)d_doc_buf + 1;
    else
        return (const char *)d_doc_buf->content;
#endif
}
