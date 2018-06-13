
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
 * XMLWriter.h
 *
 *  Created on: Jul 28, 2010
 *      Author: jimg
 */

#ifndef XMLWRITER_H_
#define XMLWRITER_H_

#include <libxml/xmlwriter.h>

#include <string>

namespace libdap {

class XMLWriter {
private:
    // Various xml writer stuff
    xmlTextWriterPtr d_writer;
    xmlBufferPtr d_doc_buf;
    bool d_started;
    bool d_ended;

    std::string d_doc;

    void m_cleanup() ;

public:
    XMLWriter(const std::string &pad = "    ");
    virtual ~XMLWriter();

    xmlTextWriterPtr get_writer() const { return d_writer; }
    const char *get_doc();
    unsigned int get_doc_size();
};

} // namespace libdap

#endif /* XMLWRITER_H_ */
