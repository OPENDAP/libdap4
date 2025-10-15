// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2011 OPeNDAP, Inc.
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

#include "config.h"

#include <signal.h>
#include <unistd.h>

#include "D4Group.h"
#include "D4StreamMarshaller.h"
#include "DMR.h"
#include "XMLWriter.h"
#include "chunked_ostream.h"

#include "D4ConstraintEvaluator.h"
// #include "D4FunctionEvaluator.h"

#include "escaping.h"
#include "mime_util.h" // for last_modified_time() and rfc_822_date()

#ifndef WIN32
#include "AlarmHandler.h"
#include "EventHandler.h"
#include "SignalHandler.h"
#endif

#include "D4ResponseBuilder.h"

const std::string CRLF = "\r\n"; // Change here, expr-test.cc/dmr-test.cc

using namespace std;
using namespace libdap;

void D4ResponseBuilder::initialize() {
    // Set default values. Don't use the C++ constructor initialization so
    // that a subclass can have more control over this process.
    d_dataset = "";
    d_timeout = 0;

    d_default_protocol = "4.0"; // DAP_PROTOCOL_VERSION;
}

D4ResponseBuilder::~D4ResponseBuilder() {
    // If an alarm was registered, delete it. The register code in SignalHandler
    // always deletes the old alarm handler object, so only the one returned by
    // remove_handler needs to be deleted at this point.
    delete dynamic_cast<AlarmHandler *>(libdap::SignalHandler::remove_handler(SIGALRM));
}

/** Set the dataset name, which is a string used to access the dataset
 * on the machine running the server. That is, this is typically a pathname
 * to a data file, although it doesn't have to be. This is not
 * echoed in error messages (because that would reveal server
 * storage patterns that data providers might want to hide). All WWW-style
 * escapes are replaced except for spaces.
 *
 * @brief Set the dataset pathname.
 * @param ds The pathname (or equivalent) to the dataset.
 */
void D4ResponseBuilder::set_dataset_name(const string ds) { d_dataset = www2id(ds, "%", "%20"); }

//// DAP4 methods follow

/** Use values of this instance to establish a timeout alarm for the server.
 If the timeout value is zero, do nothing.
*/
void D4ResponseBuilder::establish_timeout(ostream &stream) const {
    if (d_timeout > 0) {
#if 0
          SignalHandler *sh = SignalHandler::instance();
#endif

        EventHandler *old_eh = libdap::SignalHandler::register_handler(SIGALRM, new AlarmHandler(stream));
        delete old_eh;
        alarm(d_timeout);
    }
}

void D4ResponseBuilder::remove_timeout() const { alarm(0); }

/**
 * Write the on-the-wire DMR response.
 *
 * @note There is no timeout set on this reponse.
 *
 * @param out Write to this stream object
 * @param dmr This is the DMR to write

 * @param with_mime_headers If true, include the MIME headers in the response
 */
void D4ResponseBuilder::send_dmr(ostream &out, DMR &dmr, bool with_mime_headers, bool constrained) {
    if (with_mime_headers)
        set_mime_text(out, dap4_dmr, x_plain, last_modified_time(d_dataset), dmr.dap_version());

    XMLWriter xml;
    dmr.print_dap4(xml, constrained /* true == constrained */);
    out << xml.get_doc() << flush;
}

void D4ResponseBuilder::send_dap(ostream &out, DMR &dmr, bool with_mime_headers, bool constrained) {
    try {
        // Set up the alarm.
        establish_timeout(out);

        if (dmr.response_limit() != 0 && dmr.request_size(true) > dmr.response_limit()) {
            string msg = "The Request for " + long_to_string(dmr.request_size(true) / 1024) +
                         "MB is too large; requests for this user are limited to " +
                         long_to_string(dmr.response_limit() / 1024) + "MB.";
            throw Error(msg);
        }

        if (with_mime_headers)
            set_mime_binary(out, dap4_data, x_plain, last_modified_time(d_dataset), dmr.dap_version());

        // Write the DMR
        XMLWriter xml;
        dmr.print_dap4(xml, constrained);

        // now make the chunked output stream; set the size to be at least chunk_size
        // but make sure that the whole of the xml plus the CRLF can fit in the first
        // chunk. (+2 for the CRLF bytes).
        chunked_ostream cos(out, max((unsigned int)CHUNK_SIZE, xml.get_doc_size() + 2));

        // using flush means that the DMR and CRLF are in the first chunk.
        cos << xml.get_doc() << CRLF << flush;

        // Write the data, chunked with checksums
        D4StreamMarshaller m(cos, true, dmr.use_checksums());
        dmr.root()->serialize(m, dmr, constrained);

        out << flush;

        remove_timeout();
    } catch (...) {
        remove_timeout();
        throw;
    }
}
