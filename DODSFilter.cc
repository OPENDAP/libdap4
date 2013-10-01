
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation of the DODSFilter class. This class is used to build dods
// filter programs which, along with a CGI program, comprise OPeNDAP servers.
// jhrg 8/26/97


#include "config.h"

#include <signal.h>

#ifndef WIN32
#include <unistd.h>   // for getopt
#include <sys/wait.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>

#include <uuid/uuid.h>	// used to build CID header value for data ddx

#include <GetOpt.h>

#include "DAS.h"
#include "DDS.h"
#include "debug.h"
#include "mime_util.h"
#include "Ancillary.h"
#include "util.h"
#include "escaping.h"
#include "DODSFilter.h"
#include "XDRStreamMarshaller.h"
#include "InternalErr.h"

#ifndef WIN32
#include "SignalHandler.h"
#include "EventHandler.h"
#include "AlarmHandler.h"
#endif

#define CRLF "\r\n"             // Change here, expr-test.cc and DODSFilter.cc

using namespace std;

namespace libdap {

const string usage =
    "Usage: <handler name> -o <response> -u <url> [options ...] [data set]\n\
    \n\
    options: -o <response>: DAS, DDS, DataDDS, DDX, BLOB or Version (Required)\n\
    -u <url>: The complete URL minus the CE (required for DDX)\n\
    -c: Compress the response using the deflate algorithm.\n\
    -e <expr>: When returning a DataDDS, use <expr> as the constraint.\n\
    -v <version>: Use <version> as the version number\n\
    -d <dir>: Look for ancillary file in <dir> (deprecated).\n\
    -f <file>: Look for ancillary data in <file> (deprecated).\n\
    -r <dir>: Use <dir> as a cache directory\n\
    -l <time>: Conditional request; if data source is unchanged since\n\
    <time>, return an HTTP 304 response.\n\
    -t <seconds>: Timeout the handler after <seconds>.\n\
    -h: This message.";

/** Create an instance of DODSFilter using the command line
arguments passed by the CGI (or other) program.  The default
constructor is private; this and the copy constructor (which is
just the default copy constructor) are the only way to create an
instance of DODSFilter.

These are the valid options:

<dl>
<dt><i>filename</i><dd>
The name of the file on which the filter is to operate.  Usually
this would be the file whose data has been requested. In fact, this class
can be specialized and <i>any meaning</i> can be associated to this
string. It could be the name of a database, for example.

<dt><tt>-o</tt> <i>response</i><dd>

Specifies the type of response desired. The \e response is a string
and must be one of \c DAS, \c DDS, \c DataDDS or \c Version. Note
that \c Version returns version information in the body of the response
and is useful for debugging, et cetera. Each response returns version
information in an HTTP header for internal use by a client.

<dt><tt>-c</tt><dd>
Send compressed data. Data are compressed using the deflate program.

<dt><tt>-e</tt> <i>expression</i><dd>
This option specifies a non-blank constraint expression used to
subsample a dataset.

<dt><tt>-v</tt> <i>cgi-version</i><dd> Set the CGI/Server version to
<tt>cgi-version</tt>. This is a way for the caller to set version
information passed back to the client either as the response to a
version request of in the response headers.

<dt><tt>-d</tt> <i>ancdir</i><dd>
Specifies that ancillary data be sought in the <i>ancdir</i>
directory. <i>ancdir</i> must end in '/'.

<dt><tt>-f</tt> <i>ancfile</i><dd>
Specifies that ancillary data may be found in a file called
<i>ancfile</i>.

<dt><tt>-r</tt> <i>cache directory</i><dd>
Specify a directory to use if/when files are to be cached. Not all
handlers support caching and each uses its own rules tailored to a
specific file or data type.

<dt><tt>-t</tt> <i>timeout</i><dd> Specifies a a timeout value in
seconds. If the server runs longer than \e timeout seconds, an Error is
returned to the client explaining that the request has timed out.

<dt><tt>-l</tt> <i>time</i><dd> Indicates that the request is a
conditional request; send a complete response if and only if the data has
changed since <i>time</i>. If it has not changed since <i>time</i>, then
send a 304 (Not Modified) response. The <i>time</i> parameter is the
<tt>Last-Modified</tt> time from an If-Modified-Since condition GET
request. It is given in seconds since the start of the Unix epoch
(Midnight, 1 Jan 1970).

</dl>

@brief DODSFilter constructor. */

DODSFilter::DODSFilter(int argc, char *argv[]) throw(Error)
{
    initialize(argc, argv);

    DBG(cerr << "d_comp: " << d_comp << endl);
    DBG(cerr << "d_ce: " << d_ce << endl);
    DBG(cerr << "d_cgi_ver: " << d_cgi_ver << endl);
    DBG(cerr << "d_response: " << d_response << endl);
    DBG(cerr << "d_anc_dir: " << d_anc_dir << endl);
    DBG(cerr << "d_anc_file: " << d_anc_file << endl);
    DBG(cerr << "d_cache_dir: " << d_cache_dir << endl);
    DBG(cerr << "d_conditional_request: " << d_conditional_request << endl);
    DBG(cerr << "d_if_modified_since: " << d_if_modified_since << endl);
    DBG(cerr << "d_url: " << d_url << endl);
    DBG(cerr << "d_timeout: " << d_timeout << endl);
}

DODSFilter::~DODSFilter()
{
}

/** Called when initializing a DODSFilter that's not going to be passed a
command line arguments. */
void
DODSFilter::initialize()
{
    // Set default values. Don't use the C++ constructor initialization so
    // that a subclass can have more control over this process.
    d_comp = false;
    d_bad_options = false;
    d_conditional_request = false;
    d_dataset = "";
    d_ce = "";
    d_cgi_ver = "";
    d_anc_dir = "";
    d_anc_file = "";
    d_cache_dir = "";
    d_response = Unknown_Response;;
    d_anc_das_lmt = 0;
    d_anc_dds_lmt = 0;
    d_if_modified_since = -1;
    d_url = "";
    d_program_name = "Unknown";
    d_timeout = 0;

#ifdef WIN32
    //  We want serving from win32 to behave in a manner
    //  similar to the UNIX way - no CR->NL terminated lines
    //  in files. Hence stdout goes to binary mode.
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

/** Initialize. Specializations can call this once an empty DODSFilter has
been created using the default constructor. Using a method such as this
provides a way to specialize the process_options() method and then have
that specialization called by the subclass' constructor.

This class and any class that specializes it should call this method in
its constructor. Note that when this method is called, the object is \e
not fully constructed.

@param argc The argument count
@param argv The vector of char * argument strings. */
void
DODSFilter::initialize(int argc, char *argv[])
{
    initialize();

    d_program_name = argv[0];

    // This should be specialized by a subclass. This may throw Error.
    int next_arg = process_options(argc, argv);

    // Look at what's left after processing the command line options. Either
    // there MUST be a dataset name OR the caller is asking for version
    // information. If neither is true, then the options are bad.
    if (next_arg < argc) {
        d_dataset = argv[next_arg];
        d_dataset = www2id(d_dataset, "%", "%20");
    }
    else if (get_response() != Version_Response)
        print_usage();   // Throws Error
}

/** Processing the command line options passed to the filter is handled by
this method so that specializations can change the options easily.

@param argc The argument count
@param argv The vector of char * argument strings.
@return The index of the next, unprocessed, argument. This must be the
identifier passed to the filter program that identifies the data source.
It's often a file name. */
int
DODSFilter::process_options(int argc, char *argv[])
{
    DBG(cerr << "Entering process_options... ");

    int option_char;
    GetOpt getopt (argc, argv, "ce: v: d: f: r: l: o: u: t: ");

    while ((option_char = getopt()) != EOF) {
        switch (option_char) {
        case 'c': d_comp = true; break;
        case 'e': set_ce(getopt.optarg); break;
        case 'v': set_cgi_version(getopt.optarg); break;
        case 'd': d_anc_dir = getopt.optarg; break;
        case 'f': d_anc_file = getopt.optarg; break;
        case 'r': d_cache_dir = getopt.optarg; break;
        case 'o': set_response(getopt.optarg); break;
        case 'u': set_URL(getopt.optarg); break;
        case 't': d_timeout = atoi(getopt.optarg); break;
        case 'l':
            d_conditional_request = true;
            d_if_modified_since
            = static_cast<time_t>(strtol(getopt.optarg, NULL, 10));
            break;
        case 'h': print_usage();
            break;
                                 // exit(1);
                                 // Removed 12/29/2011; exit should
                                 // not be called by a library. NB:
                                 // print_usage() throws Error.
        default: print_usage();  // Throws Error
            break;
        }
    }

    DBGN(cerr << "exiting." << endl);

    return getopt.optind; // return the index of the next argument
}

/** @brief Is this request conditional?

@return True if the request is conditional.
@see get_request_if_modified_since(). */
bool
DODSFilter::is_conditional() const
{
    return d_conditional_request;
}

/** Set the CGI/Server version number. Servers use this when answering
requests for version information. The version `number' should include
both the name of the server (e.g., <tt>ff_dods</tt>) as well
as the version
number. Since this information is typically divined by configure,
it's up to the executable to poke the correct value in using this
method.

Note that the -v switch that this class understands is deprecated
since it is usually called by Perl code. It makes more sense to have
the actual C++ software set the version string.

@param version A version string for this server. */
void
DODSFilter::set_cgi_version(string version)
{
    d_cgi_ver = version;
}

/** Return the version information passed to the instance when it was
created. This string is passed to the DODSFilter ctor using the -v
option.

@return The version string supplied at initialization. */
string
DODSFilter::get_cgi_version() const
{
    return d_cgi_ver;
}

/** Return the entire constraint expression in a string.  This
includes both the projection and selection clauses, but not the
question mark.

@brief Get the constraint expression.
@return A string object that contains the constraint expression. */
string
DODSFilter::get_ce() const
{
    return d_ce;
}

void
DODSFilter::set_ce(string _ce)
{
    d_ce = www2id(_ce, "%", "%20");
}

/** The ``dataset name'' is the filename or other string that the
filter program will use to access the data. In some cases this
will indicate a disk file containing the data.  In others, it
may represent a database query or some other exotic data
access method.

@brief Get the dataset name.
@return A string object that contains the name of the dataset. */
string
DODSFilter::get_dataset_name() const
{
    return d_dataset;
}

void
DODSFilter::set_dataset_name(const string ds)
{
    d_dataset = www2id(ds, "%", "%20");
}

/** Get the URL. This returns the URL, minus the constraint originally sent
to the server.
@return The URL. */
string
DODSFilter::get_URL() const
{
    return d_url;
}

/** Set the URL. Set the URL sent to the server.
@param url The URL, minus the constraint. */
void
DODSFilter::set_URL(const string &url)
{
    if (url.find('?') != url.npos)
        print_usage();  // Throws Error

    d_url = url;
}

/** To read version information that is specific to a certain
dataset, override this method with an implementation that does
what you want. By default, this returns an empty string.

@brief Get the version information for the dataset.
@return A string object that contains the dataset version
information.  */
string
DODSFilter::get_dataset_version() const
{
    return "";
}

/** Set the response to be returned. Valid response names are "DAS", "DDS",
"DataDDS, "Version".

@param r The name of the object.
@exception InternalErr Thrown if the response is not one of the valid
names. */
void DODSFilter::set_response(const string &r)
{
    if (r == "DAS" || r == "das") {
	d_response = DAS_Response;
	d_action = "das" ;
    }
    else if (r == "DDS" || r == "dds") {
	d_response = DDS_Response;
	d_action = "dds" ;
    }
    else if (r == "DataDDS" || r == "dods") {
	d_response = DataDDS_Response;
	d_action = "dods" ;
    }
    else if (r == "DDX" || r == "ddx") {
	d_response = DDX_Response;
	d_action = "ddx" ;
    }
    else if (r == "DataDDX" || r == "dataddx") {
	d_response = DataDDX_Response;
	d_action = "dataddx" ;
    }
    else if (r == "Version") {
	d_response = Version_Response;
	d_action = "version" ;
    }
    else
	print_usage();   // Throws Error
}

/** Get the enum name of the response to be returned. */
DODSFilter::Response
DODSFilter::get_response() const
{
    return d_response;
}

/** Get the string name of the response to be returned. */
string DODSFilter::get_action() const
{
    return d_action;
}

/** Get the dataset's last modified time. This returns the time at which
    the dataset was last modified as defined by UNIX's notion of
    modification. This does not take into account the modification of an
    ancillary DAS or DDS. Time is given in seconds since the epoch (1 Jan
    1970 00:00:00 GMT).

    This method perform a simple check on the file named by the dataset
    given when the DODSFilter instance was created. If the dataset is not
    a filter, this method returns the current time. Servers which provide
    access to non-file-based data should subclass DODSFilter and supply a
    more suitable version of this method.

    From the stat(2) man page: ``Traditionally, <tt>st_mtime</tt>
    is changed by mknod(2), utime(2), and write(2). The
    <tt>st_mtime</tt> is not changed for
    changes in owner, group, hard link count, or mode.''

    @return Time of the last modification in seconds since the epoch.
    @see get_das_last_modified_time()
    @see get_dds_last_modified_time() */
time_t
DODSFilter::get_dataset_last_modified_time() const
{
    return last_modified_time(d_dataset);
}

/** Get the last modified time for the dataset's DAS. This time, given in
    seconds since the epoch (1 Jan 1970 00:00:00 GMT), is the greater of
    the datasets's and any ancillary DAS' last modified time.

    @param anc_location A directory to search for ancillary files (in
    addition to the CWD).
    @return Time of last modification of the DAS.
    @see get_dataset_last_modified_time()
    @see get_dds_last_modified_time() */
time_t
DODSFilter::get_das_last_modified_time(const string &anc_location) const
{
    DBG(cerr << "DODSFilter::get_das_last_modified_time(anc_location="
        << anc_location << "call faf(das) d_dataset=" << d_dataset
        << " d_anc_file=" << d_anc_file << endl);

    string name
    = Ancillary::find_ancillary_file(d_dataset, "das",
                          (anc_location == "") ? d_anc_dir : anc_location,
                          d_anc_file);

    return max((name != "") ? last_modified_time(name) : 0,
               get_dataset_last_modified_time());
}

/** Get the last modified time for the dataset's DDS. This time, given in
    seconds since the epoch (1 Jan 1970 00:00:00 GMT), is the greater of
    the datasets's and any ancillary DDS' last modified time.

    @return Time of last modification of the DDS.
    @see get_dataset_last_modified_time()
    @see get_dds_last_modified_time() */
time_t
DODSFilter::get_dds_last_modified_time(const string &anc_location) const
{
    DBG(cerr << "DODSFilter::get_das_last_modified_time(anc_location="
        << anc_location << "call faf(dds) d_dataset=" << d_dataset
        << " d_anc_file=" << d_anc_file << endl);

    string name
    = Ancillary::find_ancillary_file(d_dataset, "dds",
                          (anc_location == "") ? d_anc_dir : anc_location,
                          d_anc_file);

    return max((name != "") ? last_modified_time(name) : 0,
               get_dataset_last_modified_time());
}

/** Get the last modified time to be used for a particular data request.
    This method should look at both the constraint expression and any
    ancillary files for this dataset. The implementation provided here
    returns the latest time returned by the <tt>get_dataset</tt>...(),
    <tt>get_das</tt>...() and <tt>get_dds</tt>...() methods and
    does not currently check the CE.

    @param anc_location A directory to search for ancillary files (in
    addition to the CWD).
    @return Time of last modification of the data.
    @see get_dataset_last_modified_time()
    @see get_das_last_modified_time()
    @see get_dds_last_modified_time() */
time_t
DODSFilter::get_data_last_modified_time(const string &anc_location) const
{
    DBG(cerr << "DODSFilter::get_das_last_modified_time(anc_location="
        << anc_location << "call faf(both) d_dataset=" << d_dataset
        << " d_anc_file=" << d_anc_file << endl);

    string dds_name
    = Ancillary::find_ancillary_file(d_dataset, "dds",
                          (anc_location == "") ? d_anc_dir : anc_location,
                          d_anc_file);
    string das_name
    = Ancillary::find_ancillary_file(d_dataset, "das",
                          (anc_location == "") ? d_anc_dir : anc_location,
                          d_anc_file);

    time_t m = max((das_name != "") ? last_modified_time(das_name) : (time_t)0,
                   (dds_name != "") ? last_modified_time(dds_name) : (time_t)0);
    // Note that this is a call to get_dataset_... not get_data_...
    time_t n = get_dataset_last_modified_time();

    return max(m, n);
}

/** Get the value of a conditional request's If-Modified-Since header.
    This value is used to determine if the request should get a full
    response or a Not Modified (304) response. The time is given in
    seconds since the Unix epoch (midnight, 1 Jan 1970). If no time was
    given with the request, this methods returns -1.

    @return If-Modified-Since time from a condition GET request. */
time_t
DODSFilter::get_request_if_modified_since() const
{
    return d_if_modified_since;
}

/** The <tt>cache_dir</tt> is used to hold the cached .dds and .das files.
    By default, this returns an empty string (store cache files in
    current directory.

    @brief Get the cache directory.
    @return A string object that contains the cache file directory.  */
string
DODSFilter::get_cache_dir() const
{
    return d_cache_dir;
}

/** Set the server's timeout value. A value of zero (the default) means no
    timeout.

    @param t Server timeout in seconds. Default is zero (no timeout). */
void
DODSFilter::set_timeout(int t)
{
    d_timeout = t;
}

/** Get the server's timeout value. */
int
DODSFilter::get_timeout() const
{
    return d_timeout;
}

/** Use values of this instance to establish a timeout alarm for the server.
    If the timeout value is zero, do nothing.

    @todo When the alarm handler is called, two CRLF pairs are dumped to the
    stream and then an Error object is sent. No attempt is made to write the
    'correct' MIME headers for an Error object. Instead, a savvy client will
    know that when an exception is thrown during a deserialize operation, it
    should scan ahead in the input stream for an Error object. Add this, or a
    sensible variant once libdap++ supports reliable error delivery. Dumb
    clients will never get the Error object... */

void
DODSFilter::establish_timeout(FILE *stream) const
{
#ifndef WIN32
    if (d_timeout > 0) {
        SignalHandler *sh = SignalHandler::instance();
        EventHandler *old_eh = sh->register_handler(SIGALRM, new AlarmHandler(stream));
        delete old_eh;
        alarm(d_timeout);
    }
#endif
}

void
DODSFilter::establish_timeout(ostream &stream) const
{
#ifndef WIN32
    if (d_timeout > 0) {
        SignalHandler *sh = SignalHandler::instance();
        EventHandler *old_eh = sh->register_handler(SIGALRM, new AlarmHandler(stream));
        delete old_eh;
        alarm(d_timeout);
    }
#endif
}

static const char *emessage = "DODS internal server error; usage error. Please report this to the dataset maintainer, or to the opendap-tech@opendap.org mailing list.";

/** This message is printed when the filter program is incorrectly
    invoked by the dispatch CGI.  This is an error in the server
    installation or the CGI implementation, so the error message is
    written to stderr instead of stdout.  A server's stderr messages
    show up in the httpd log file. In addition, an error object is
    sent back to the client program telling them that the server is
    broken.

    @brief Print usage information for a filter program. */
void
DODSFilter::print_usage() const
{
    // Write a message to the WWW server error log file.
    ErrMsgT(usage.c_str());

    throw Error(emessage);
}

/** This function formats and sends to stdout version
    information from the httpd server, the server dispatch scripts,
    the DODS core software, and (optionally) the dataset.

    @brief Send version information back to the client program. */
void
DODSFilter::send_version_info() const
{
    do_version(d_cgi_ver, get_dataset_version());
}

/** This function formats and prints an ASCII representation of a
    DAS on stdout.  This has the effect of sending the DAS object
    back to the client program.

    @brief Transmit a DAS.
    @param out The output FILE to which the DAS is to be sent.
    @param das The DAS object to be sent.
    @param anc_location The directory in which the external DAS file resides.
    @param with_mime_headers If true (the default) send MIME headers.
    @return void
    @see DAS */
void
DODSFilter::send_das(FILE *out, DAS &das, const string &anc_location,
                     bool with_mime_headers) const
{
    ostringstream oss;
    send_das(oss, das, anc_location, with_mime_headers);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

/** This function formats and prints an ASCII representation of a
    DAS on stdout.  This has the effect of sending the DAS object
    back to the client program.

    @brief Transmit a DAS.
    @param out The output stream to which the DAS is to be sent.
    @param das The DAS object to be sent.
    @param anc_location The directory in which the external DAS file resides.
    @param with_mime_headers If true (the default) send MIME headers.
    @return void
    @see DAS */
void
DODSFilter::send_das(ostream &out, DAS &das, const string &anc_location,
                     bool with_mime_headers) const
{
    time_t das_lmt = get_das_last_modified_time(anc_location);
    if (is_conditional()
        && das_lmt <= get_request_if_modified_since()
        && with_mime_headers) {
        set_mime_not_modified(out);
    }
    else {
        if (with_mime_headers)
            set_mime_text(out, dods_das, d_cgi_ver, x_plain, das_lmt);
        das.print(out);
    }
    out << flush ;
}

void
DODSFilter::send_das(DAS &das, const string &anc_location,
                     bool with_mime_headers) const
{
    send_das(cout, das, anc_location, with_mime_headers);
}

/** This function formats and prints an ASCII representation of a
    DDS on stdout.  When called by a CGI program, this has the
    effect of sending a DDS object back to the client
    program. Either an entire DDS or a constrained DDS may be sent.

    @brief Transmit a DDS.
    @param out The output FILE to which the DAS is to be sent.
    @param dds The DDS to send back to a client.
    @param eval A reference to the ConstraintEvaluator to use.
    @param constrained If this argument is true, evaluate the
    current constraint expression and send the `constrained DDS'
    back to the client.
    @param anc_location The directory in which the external DAS file resides.
    @param with_mime_headers If true (the default) send MIME headers.
    @return void
    @see DDS */
void
DODSFilter::send_dds(FILE *out, DDS &dds, ConstraintEvaluator &eval,
                     bool constrained,
                     const string &anc_location,
                     bool with_mime_headers) const
{
    ostringstream oss;
    send_dds(oss, dds, eval, constrained, anc_location, with_mime_headers);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

/** This function formats and prints an ASCII representation of a
    DDS on stdout.  When called by a CGI program, this has the
    effect of sending a DDS object back to the client
    program. Either an entire DDS or a constrained DDS may be sent.

    @brief Transmit a DDS.
    @param out The output stream to which the DAS is to be sent.
    @param dds The DDS to send back to a client.
    @param eval A reference to the ConstraintEvaluator to use.
    @param constrained If this argument is true, evaluate the
    current constraint expression and send the `constrained DDS'
    back to the client.
    @param anc_location The directory in which the external DAS file resides.
    @param with_mime_headers If true (the default) send MIME headers.
    @return void
    @see DDS */
void
DODSFilter::send_dds(ostream &out, DDS &dds, ConstraintEvaluator &eval,
                     bool constrained,
                     const string &anc_location,
                     bool with_mime_headers) const
{
    // If constrained, parse the constraint. Throws Error or InternalErr.
    if (constrained)
        eval.parse_constraint(d_ce, dds);

    if (eval.functional_expression())
        throw Error("Function calls can only be used with data requests. To see the structure of the underlying data source, reissue the URL without the function.");

    time_t dds_lmt = get_dds_last_modified_time(anc_location);
    if (is_conditional()
        && dds_lmt <= get_request_if_modified_since()
        && with_mime_headers) {
        set_mime_not_modified(out);
    }
    else {
        if (with_mime_headers)
            set_mime_text(out, dods_dds, d_cgi_ver, x_plain, dds_lmt);
        if (constrained)
            dds.print_constrained(out);
        else
            dds.print(out);
    }

    out << flush ;
}

void
DODSFilter::send_dds(DDS &dds, ConstraintEvaluator &eval,
                     bool constrained, const string &anc_location,
                     bool with_mime_headers) const
{
    send_dds(cout, dds, eval, constrained, anc_location, with_mime_headers);
}

// 'lmt' unused. Should it be used to supply a LMT or removed from the
// method? jhrg 8/9/05
void
DODSFilter::functional_constraint(BaseType &var, DDS &dds,
                                  ConstraintEvaluator &eval, FILE *out) const
{
    ostringstream oss;
    functional_constraint(var, dds, eval, oss);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

// 'lmt' unused. Should it be used to supply a LMT or removed from the
// method? jhrg 8/9/05
void
DODSFilter::functional_constraint(BaseType &var, DDS &dds,
                                  ConstraintEvaluator &eval, ostream &out) const
{
    out << "Dataset {\n" ;
    var.print_decl(out, "    ", true, false, true);
    out << "} function_value;\n" ;
    out << "Data:\n" ;

    out << flush ;

    // Grab a stream that encodes using XDR.
    XDRStreamMarshaller m( out ) ;

    try {
        // In the following call to serialize, suppress CE evaluation.
        var.serialize(eval, dds, m, false);
    }
    catch (Error &e) {
        throw;
    }
}

void
DODSFilter::dataset_constraint(DDS & dds, ConstraintEvaluator & eval,
                               FILE * out, bool ce_eval) const
{
    ostringstream oss;
    dataset_constraint(dds, eval, oss, ce_eval);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void
DODSFilter::dataset_constraint(DDS & dds, ConstraintEvaluator & eval,
                               ostream &out, bool ce_eval) const
{
    // send constrained DDS
    dds.print_constrained(out);
    out << "Data:\n" ;
    out << flush ;

    // Grab a stream that encodes using XDR.
    XDRStreamMarshaller m( out ) ;

    try {
        // Send all variables in the current projection (send_p())
        for (DDS::Vars_iter i = dds.var_begin(); i != dds.var_end(); i++)
            if ((*i)->send_p()) {
                DBG(cerr << "Sending " << (*i)->name() << endl);
                (*i)->serialize(eval, dds, m, ce_eval);
            }
    }
    catch (Error & e) {
        throw;
    }
}

void
DODSFilter::dataset_constraint_ddx(DDS & dds, ConstraintEvaluator & eval,
                               ostream &out, const string &boundary,
                               const string &start, bool ce_eval) const
{
    // Write the MPM headers for the DDX (text/xml) part of the response
    set_mime_ddx_boundary(out, boundary, start, dap4_ddx);

    // Make cid
    uuid_t uu;
    uuid_generate(uu);
    char uuid[37];
    uuid_unparse(uu, &uuid[0]);
    char domain[256];
    if (getdomainname(domain, 255) != 0 || strlen(domain) == 0)
	strncpy(domain, "opendap.org", 255);

    string cid = string(&uuid[0]) + "@" + string(&domain[0]);

    // Send constrained DDX with a data blob reference
    dds.print_xml_writer(out, true, cid);

    // Write the MPM headers for the data part of the response.
    set_mime_data_boundary(out, boundary, cid, dap4_data, binary);

    // Grab a stream that encodes using XDR.
    XDRStreamMarshaller m( out ) ;

    try {
        // Send all variables in the current projection (send_p())
        for (DDS::Vars_iter i = dds.var_begin(); i != dds.var_end(); i++)
            if ((*i)->send_p()) {
                DBG(cerr << "Sending " << (*i)->name() << endl);
                (*i)->serialize(eval, dds, m, ce_eval);
            }
    }
    catch (Error & e) {
        throw;
    }
}

/** Send the data in the DDS object back to the client program. The data is
    encoded using a Marshaller, and enclosed in a MIME document which is all sent
    to \c data_stream. If this is being called from a CGI, \c data_stream is
    probably \c stdout and writing to it has the effect of sending the
    response back to the client.

    @brief Transmit data.
    @param dds A DDS object containing the data to be sent.
    @param eval A reference to the ConstraintEvaluator to use.
    @param data_stream Write the response to this FILE.
    @param anc_location A directory to search for ancillary files (in
    addition to the CWD).  This is used in a call to
    get_data_last_modified_time().
    @param with_mime_headers If true, include the MIME headers in the response.
    Defaults to true.
    @return void */
void
DODSFilter::send_data(DDS & dds, ConstraintEvaluator & eval,
                      FILE * data_stream, const string & anc_location,
                      bool with_mime_headers) const
{
    ostringstream oss;
    send_data(dds, eval, oss, anc_location, with_mime_headers);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), data_stream);
}

/** Send the data in the DDS object back to the client program. The data is
    encoded using a Marshaller, and enclosed in a MIME document which is all sent
    to \c data_stream. If this is being called from a CGI, \c data_stream is
    probably \c stdout and writing to it has the effect of sending the
    response back to the client.

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
void
DODSFilter::send_data(DDS & dds, ConstraintEvaluator & eval,
                      ostream & data_stream, const string & anc_location,
                      bool with_mime_headers) const
{
    // If this is a conditional request and the server should send a 304
    // response, do that and exit. Otherwise, continue on and send the full
    // response.
    time_t data_lmt = get_data_last_modified_time(anc_location);
    if (is_conditional()
        && data_lmt <= get_request_if_modified_since()
        && with_mime_headers) {
        set_mime_not_modified(data_stream);
        return;
    }
    // Set up the alarm.
    establish_timeout(data_stream);
    dds.set_timeout(d_timeout);

    eval.parse_constraint(d_ce, dds);   // Throws Error if the ce doesn't
					// parse.

    dds.tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

    // Start sending the response...

    // Handle *functional* constraint expressions specially
#if 0
    if (eval.functional_expression()) {
        // Get the result and then start sending the headers. This provides a
        // way to send errors back to the client w/o colliding with the
        // normal response headers. There's some duplication of code with this
        // and the else-clause.
        BaseType *var = eval.eval_function(dds, d_dataset);
        if (!var)
            throw Error(unknown_error, "Error calling the CE function.");

       if (with_mime_headers)
            set_mime_binary(data_stream, dods_data, d_cgi_ver, x_plain, data_lmt);

	data_stream << flush ;

        functional_constraint(*var, dds, eval, data_stream);
        delete var;
        var = 0;
    }
#endif
    if (eval.function_clauses()) {
	DDS *fdds = eval.eval_function_clauses(dds);
        if (with_mime_headers)
            set_mime_binary(data_stream, dods_data, d_cgi_ver, x_plain, data_lmt);

        dataset_constraint(*fdds, eval, data_stream, false);
	delete fdds;
    }
    else {
        if (with_mime_headers)
            set_mime_binary(data_stream, dods_data, d_cgi_ver, x_plain, data_lmt);

        dataset_constraint(dds, eval, data_stream);
    }

    data_stream << flush ;
}

/** Send the DDX response. The DDX never contains data, instead it holds a
    reference to a Blob response which is used to get the data values. The
    DDS and DAS objects are built using code that already exists in the
    servers.

    @param dds The dataset's DDS \e with attributes in the variables.
    @param eval A reference to the ConstraintEvaluator to use.
    @param out Destination
    @param with_mime_headers If true, include the MIME headers in the response.
    Defaults to true. */
void
DODSFilter::send_ddx(DDS &dds, ConstraintEvaluator &eval, FILE *out,
                     bool with_mime_headers) const
{
    ostringstream oss;
    send_ddx(dds, eval, oss, with_mime_headers);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

/** Send the DDX response. The DDX never contains data, instead it holds a
    reference to a Blob response which is used to get the data values. The
    DDS and DAS objects are built using code that already exists in the
    servers.

    @param dds The dataset's DDS \e with attributes in the variables.
    @param eval A reference to the ConstraintEvaluator to use.
    @param out Destination
    @param with_mime_headers If true, include the MIME headers in the response.
    Defaults to true. */
void
DODSFilter::send_ddx(DDS &dds, ConstraintEvaluator &eval, ostream &out,
                     bool with_mime_headers) const
{
    // If constrained, parse the constraint. Throws Error or InternalErr.
    if (!d_ce.empty())
        eval.parse_constraint(d_ce, dds);

    if (eval.functional_expression())
        throw Error("Function calls can only be used with data requests. To see the structure of the underlying data source, reissue the URL without the function.");

    time_t dds_lmt = get_dds_last_modified_time(d_anc_dir);

    // If this is a conditional request and the server should send a 304
    // response, do that and exit. Otherwise, continue on and send the full
    // response.
    if (is_conditional() && dds_lmt <= get_request_if_modified_since()
        && with_mime_headers) {
        set_mime_not_modified(out);
        return;
    }
    else {
        if (with_mime_headers)
            set_mime_text(out, dap4_ddx, d_cgi_ver, x_plain, dds_lmt);
        dds.print_xml_writer(out, !d_ce.empty(), "");
    }
}

/** Send the data in the DDS object back to the client program. The data is
    encoded using a Marshaller, and enclosed in a MIME document which is all sent
    to \c data_stream. If this is being called from a CGI, \c data_stream is
    probably \c stdout and writing to it has the effect of sending the
    response back to the client.

    @brief Transmit data.

    @param dds A DDS object containing the data to be sent.
    @param eval A reference to the ConstraintEvaluator to use.
    @param data_stream Write the response to this stream.
    @param start
    @param boundary
    @param anc_location A directory to search for ancillary files (in
    addition to the CWD).  This is used in a call to
    get_data_last_modified_time().
    @param with_mime_headers If true, include the MIME headers in the response.
    Defaults to true.

    @return void */
void
DODSFilter::send_data_ddx(DDS & dds, ConstraintEvaluator & eval,
                      ostream & data_stream, const string &start,
                      const string &boundary, const string & anc_location,
                      bool with_mime_headers) const
{
    // If this is a conditional request and the server should send a 304
    // response, do that and exit. Otherwise, continue on and send the full
    // response.
    time_t data_lmt = get_data_last_modified_time(anc_location);
    if (is_conditional()
        && data_lmt <= get_request_if_modified_since()
        && with_mime_headers) {
        set_mime_not_modified(data_stream);
        return;
    }
    // Set up the alarm.
    establish_timeout(data_stream);
    dds.set_timeout(d_timeout);

    eval.parse_constraint(d_ce, dds);   // Throws Error if the ce doesn't
					// parse.

    dds.tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

    // Start sending the response...

    // Handle *functional* constraint expressions specially
#if 0
    if (eval.functional_expression()) {
        BaseType *var = eval.eval_function(dds, d_dataset);
        if (!var)
            throw Error(unknown_error, "Error calling the CE function.");

        if (with_mime_headers)
            set_mime_multipart(data_stream, boundary, start, dap4_data_ddx,
		d_cgi_ver, x_plain, data_lmt);
	data_stream << flush ;
	BaseTypeFactory btf;
	DDS var_dds(&btf, var->name());
	var->set_send_p(true);
	var_dds.add_var(var);
        dataset_constraint_ddx(var_dds, eval, data_stream, boundary, start);

        // functional_constraint_ddx(*var, dds, eval, data_stream, boundary);
        delete var;
        var = 0;
    }
#endif
    if (eval.function_clauses()) {
    	DDS *fdds = eval.eval_function_clauses(dds);
        if (with_mime_headers)
            set_mime_multipart(data_stream, boundary, start, dap4_data_ddx,
        	    d_cgi_ver, x_plain, data_lmt);
        data_stream << flush ;
        dataset_constraint(*fdds, eval, data_stream, false);
    	delete fdds;
    }
    else {
        if (with_mime_headers)
            set_mime_multipart(data_stream, boundary, start, dap4_data_ddx,
        	    d_cgi_ver, x_plain, data_lmt);
        data_stream << flush ;
        dataset_constraint_ddx(dds, eval, data_stream, boundary, start);
    }

    data_stream << flush ;

    if (with_mime_headers)
	data_stream << CRLF << "--" << boundary << "--" << CRLF;
}

} // namespace libdap

