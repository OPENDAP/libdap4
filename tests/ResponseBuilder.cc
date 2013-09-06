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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <uuid/uuid.h>  // used to build CID header value for data ddx
#ifndef WIN32
#include <sys/wait.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <cstring>
#include <ctime>

//#define DODS_DEBUG

#include "DAS.h"
#include "DDS.h"

#include "ConstraintEvaluator.h"
#include "DDXParserSAX2.h"
#include "Ancillary.h"
#include "XDRStreamMarshaller.h"
#include "XDRFileUnMarshaller.h"

#include "DMR.h"
#include "XMLWriter.h"
#include "D4StreamMarshaller.h"

#include "debug.h"
#include "mime_util.h"	// for last_modified_time() and rfc_822_date()
#include "escaping.h"
#include "util.h"

#ifndef WIN32
#include "SignalHandler.h"
#include "EventHandler.h"
#include "AlarmHandler.h"
#endif

#include "ResponseBuilder.h"

#define CRLF "\r\n"             // Change here, expr-test.cc
using namespace std;
using namespace libdap;

/** Called when initializing a ResponseBuilder that's not going to be passed
 command line arguments. */
void ResponseBuilder::initialize()
{
	// Set default values. Don't use the C++ constructor initialization so
	// that a subclass can have more control over this process.
	d_dataset = "";
	d_ce = "";
	d_btp_func_ce = "";
	d_timeout = 0;

	d_default_protocol = DAP_PROTOCOL_VERSION;
}

ResponseBuilder::~ResponseBuilder()
{
	// If an alarm was registered, delete it. The register code in SignalHandler
	// always deletes the old alarm handler object, so only the one returned by
	// remove_handler needs to be deleted at this point.
	delete dynamic_cast<AlarmHandler*>(SignalHandler::instance()->remove_handler(SIGALRM));
}

/** Set the constraint expression. This will filter the CE text removing
 * any 'WWW' escape characters except space. Spaces are left in the CE
 * because the CE parser uses whitespace to delimit tokens while some
 * datasets have identifiers that contain spaces. It's possible to use
 * double quotes around identifiers too, but most client software doesn't
 * know about that.
 *
 * @@brief Set the CE
 * @param _ce The constraint expression
 */
void ResponseBuilder::set_ce(string _ce)
{
	d_ce = www2id(_ce, "%", "%20");
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
void ResponseBuilder::set_dataset_name(const string ds)
{
	d_dataset = www2id(ds, "%", "%20");
}

/**
 * Build/return the BLOB part of the DAP2 data response.
 */
void ResponseBuilder::dataset_constraint(ostream &out, DDS &dds, ConstraintEvaluator & eval, bool ce_eval)
{
	// send constrained DDS
	DBG(cerr << "Inside dataset_constraint" << endl);

	dds.print_constrained(out);
	out << "Data:\n";
	out << flush;

	XDRStreamMarshaller m(out);

	try {
		// Send all variables in the current projection (send_p())
		for (DDS::Vars_iter i = dds.var_begin(); i != dds.var_end(); i++) {
			if ((*i)->send_p()) {
				(*i)->serialize(eval, dds, m, ce_eval);
			}
		}
	}
	catch (Error & e) {
		throw;
	}
}

/** Send the data in the DDS object back to the client program. The data is
 encoded using a Marshaller, and enclosed in a MIME document which is all sent
 to \c data_stream.

 @note This is the DAP2 data response.

 @brief Transmit data.
 @param dds A DDS object containing the data to be sent.
 @param eval A reference to the ConstraintEvaluator to use.
 @param data_stream Write the response to this stream.
 @param anc_location A directory to search for ancillary files (in
 addition to the CWD).  This is used in a call to
 get_data_last_modified_time().
 @param with_mime_headers If true, include the MIME headers in the response.
 Defaults to true.
 @return void */
void ResponseBuilder::send_data(ostream &data_stream, DDS &dds, ConstraintEvaluator &eval, bool with_mime_headers)
{
	DBG(cerr << "Simple constraint" << endl);

	eval.parse_constraint(d_ce, dds); // Throws Error if the ce doesn't parse.

	dds.tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

	if (dds.get_response_limit() != 0 && dds.get_request_size(true) > dds.get_response_limit()) {
		string msg = "The Request for " + long_to_string(dds.get_request_size(true) / 1024)
				+ "KB is too large; requests for this user are limited to "
				+ long_to_string(dds.get_response_limit() / 1024) + "KB.";
		throw Error(msg);
	}

	if (with_mime_headers)
		set_mime_binary(data_stream, dods_data, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

	dataset_constraint(data_stream, dds, eval);
	data_stream << flush;
}

//// DAP4 methods follow

/** Use values of this instance to establish a timeout alarm for the server.
 If the timeout value is zero, do nothing.
*/
void ResponseBuilder::establish_timeout(ostream &stream) const
{
    if (d_timeout > 0) {
        SignalHandler *sh = SignalHandler::instance();
        EventHandler *old_eh = sh->register_handler(SIGALRM, new AlarmHandler(stream));
        delete old_eh;
        alarm(d_timeout);
    }
}

void ResponseBuilder::remove_timeout() const
{
	alarm(0);
}

/**
 * Write the on-the-wire DMR response.
 *
 * @note There is no timeout set on this reponse.
 *
 * @param out Write to this stream object
 * @param dmr This is the DMR to write
 * @param eval Use this ConstraintEvaluator
 * @param with_mime_headers If true, include the MIME headers in the response
 */
void ResponseBuilder::send_dmr(ostream &out, DMR &dmr, ConstraintEvaluator &eval, bool with_mime_headers)
{
#if 0
	// TODO Add CE Parser; and see below
	if (!d_ce.empty()) eval.parse_constraint(d_ce, dmr); // Throws Error if the ce doesn't parse.
#endif
	if (with_mime_headers) set_mime_text(out, dap4_ddx, x_plain, last_modified_time(d_dataset), dmr.dap_version());

	XMLWriter xml;
	dmr.print_dap4(xml, !d_ce.empty() /* true == constrained */);
	out << xml.get_doc() << flush;
}

/**
 *
 * @param out
 * @param dmr
 * @param eval
 * @param start
 * @param boundary
 * @param filter true if there are filters to apply to variables
 */
void ResponseBuilder::dataset_constraint_dmr(ostream &out, DMR &dmr, ConstraintEvaluator &eval,
        const string &start, const string &boundary, bool filter)
{
    // Write the MPM headers for the DDX (text/xml) part of the response
    libdap::set_mime_ddx_boundary(out, boundary, start, dap4_ddx, x_plain);

    // Write the DMR
    XMLWriter xml;
    dmr.print_dap4(xml, filter);
    out << xml.get_doc() << flush;

    // Make cid
    uuid_t uu;
    uuid_generate(uu);
    char uuid[37];
    uuid_unparse(uu, &uuid[0]);
    char domain[256];
    if (getdomainname(domain, 255) != 0 || strlen(domain) == 0)
        strncpy(domain, "opendap.org", 255);

    string cid = string(&uuid[0]) + "@" + string(&domain[0]);

    // write the data part mime headers here
    set_mime_data_boundary(out, boundary, cid, dap4_data, x_plain);

    D4StreamMarshaller m(out);
#if 1
    // Send all variables in the current projection (send_p()). In DAP4,
    // all of the top-level variables are serialized with their checksums.
    // Internal variables are not.

    // pas in m, dmr, eval, filter
    dmr.root()->serialize(m, dmr, eval, filter);
#endif
}

void ResponseBuilder::send_data_dmr(ostream &out, DMR &dmr, ConstraintEvaluator &eval,
		const string &start, const string &boundary, bool with_mime_headers)
{
	// Set up the alarm.
	establish_timeout(out);

	try {
#if 0
		bool filter = eval.parse_constraint(d_ce, dmr); // Throws Error if the ce doesn't parse.
#endif
		if (dmr.response_limit() != 0 && dmr.request_size(true) > dmr.response_limit()) {
			string msg = "The Request for " + long_to_string(dmr.request_size(true) / 1024)
					+ "MB is too large; requests for this user are limited to "
					+ long_to_string(dmr.response_limit() / 1024) + "MB.";
			throw Error(msg);
		}

		if (with_mime_headers)
			set_mime_multipart(out, boundary, start, dap4_data_ddx, x_plain, last_modified_time(d_dataset));

		dataset_constraint_dmr(out, dmr, eval, boundary, start, true /*filter*/);

		if (with_mime_headers) out << CRLF << "--" << boundary << "--" << CRLF;

		out << flush;

		remove_timeout();
	}
	catch (...) {
		remove_timeout();
		throw;
	}
}
