
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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
 
// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation of the DODSFilter class. This class is used to build dods
// filter programs which, along with a CGI program, comprise DODS servers.
// jhrg 8/26/97

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: DODSFilter.cc,v 1.31 2003/02/09 10:07:32 rmorris Exp $"};

#include <iostream>
#include <strstream>
#include <string>
#include <algorithm>
#if 0
#include <sys/ddi.h>
#endif

#include <GetOpt.h>

#include "DAS.h"
#include "DDS.h"
#include "debug.h"
#include "cgi_util.h"
#include "util.h"
#include "escaping.h"
#include "DODSFilter.h"
#include "InternalErr.h"

using std::ostrstream;

//  Using std::max under win32 causes a conflict.  MS suggests using _MAX
//  instead.  We do that, then for non-WIN32 define _MAX to max.
#ifndef WIN32
#define _MAX max
#endif

/** Create an instance of DODSFilter using the command line
    arguments passed by the CGI (or other) program.  The default
    constructor is private; this and the copy constructor (which is
    just the default copy constructor) are the only way to create an
    instance of DODSFilter.

    These are the valid options:

    <dl>
    <dt><i>filename</i><dd>
    The name of the file on which the filter is to operate.  Usually
    this would be the file whose data has been requested.

    <dt><tt>-c</tt><dd>
    Send compressed data. Data are compressed using the deflate program.
    The W3C's libwww will recognize this and automatically decompress
    these data.

    <dt><tt>-e</tt> <i>expression</i><dd>
    This option specifies a non-blank constraint expression used to
    subsample a dataset.

    <dt><tt>-v</tt> <i>cgi-version</i><dd> Set the CGI/Server version to
    <tt>cgi-version</tt>. This is a way for the caller to set version
    information passed back to the client either as the response to a
    version request of in the response headers.

    <dt><tt>-V</tt><dd> Specifies that this request is just for version
    information. Servers can check to see if this was given using the
    <tt>version</tt> mfunc. Note that version information is sent from within
    DODSFilter so that sophisticated servers can support versioning data
    sources inaddition to the server software.

    <dt><tt>-d</tt> <i>ancdir</i><dd>
    Specifies that ancillary data be sought in the <i>ancdir</i>
    directory. 

    <dt><tt>-f</tt> <i>ancfile</i><dd>
    Specifies that ancillary data may be found in a file called 
    <i>ancfile</i>.

    <dt><tt>-r</tt> <i>cache directory</i><dd>
    Specify a directory to use if/when files are to be cached. Not all
    handlers support caching and each uses its own rules tailored to a
    specific file or data type.

    <dt><tt>-t</tt> <i>list of types</i><dd> Specifies a list of
    types accepted by 
    the client. This information is passed to a server by a client using
    the XDODS-Accept-Types header. The comma separated list contains each
    type the client can understand <i>or</i>, each type the client does
    <i>not</i> understand. In the latter case the type names are prefixed
    by a {\tt !}. If the list contains only the keyword `All', then the
    client is declaring that it can understand all DODS types.

    <dt><tt>-l</tt> <i>time</i><dd> Indicates that the request is
    a conditional request; send a complete response if and only if
    the data has changed since <i>time</i>. If it has not changed
    since <i>time</i>, then send a 304 (Not Modified)
    response. The <i>time</i> parameter is the
    <tt>Last-Modified</tt> time from an If-Modified-Since
    condition GET request.  It is given in seconds since the start
    of the Unix epoch (Midnight, 1 Jan 1970).

    </dl>

    @brief DODSFilter constructor. */

DODSFilter::DODSFilter(int argc, char *argv[]) : comp(false), ver(false), 
    bad_options(false), d_conditional_request(false), dataset(""), ce(""),
    cgi_ver(""), anc_dir(""), anc_file(""), cache_dir(""),
    accept_types("All"), d_anc_das_lmt(0), d_anc_dds_lmt(0),
    d_if_modified_since(-1)
{
    program_name = argv[0];

    int option_char;
    GetOpt getopt (argc, argv, "ce:v:Vd:f:r:t:l:");

    while ((option_char = getopt()) != EOF)
	switch (option_char) {
	  case 'c': comp = true; break;
	  case 'e': ce = getopt.optarg; break;
	  case 'v': cgi_ver = getopt.optarg; break;
	  case 'V': ver = true; break;
	  case 'd': anc_dir = getopt.optarg; break;
	  case 'f': anc_file = getopt.optarg; break;
	  case 'r': cache_dir = getopt.optarg; break;
	  case 't': accept_types = getopt.optarg; break;
	  case 'l': 
	    d_conditional_request = true;
	    d_if_modified_since 
		= static_cast<time_t>(strtol(getopt.optarg, NULL, 10));
	    break;
	  default: bad_options = true; break;
	}

    int next_arg = getopt.optind;
    if(next_arg < argc)
	dataset = argv[next_arg];
    else if (!ver)
	bad_options = true;

    // Both dataset and ce could be set at this point (dataset must be, ce
    // might be). If they contain any WWW-style esacpes (%<hex digit>,hex
    // digit>) then undo that escaping.
    dataset = www2id(dataset, "%", "%20");
    ce = www2id(ce, "%", "%20");

    DBG(cerr << "comp: " << comp << endl);
    DBG(cerr << "ce: " << ce << endl);
    DBG(cerr << "cgi_ver: " << cgi_ver << endl);
    DBG(cerr << "ver: " << ver << endl);
    DBG(cerr << "anc_dir: " << anc_dir << endl);
    DBG(cerr << "anc_file: " << anc_file << endl);
    DBG(cerr << "cache_dir: " << cache_dir << endl);
    DBG(cerr << "accept_types: " << accept_types << endl);
    DBG(cerr << "d_conditional_request: " << d_conditional_request << endl);
    DBG(cerr << "d_if_modified_since: " << d_if_modified_since << endl);
}

DODSFilter::~DODSFilter()
{
}

/** Use this function to test whether the options passed via argc
    and argv are valid. 

    @brief Check whether the DODSFilter was initialized with valid
    arguments. 
    @return True if the class state is OK, false otherwise. */
bool
DODSFilter::OK()
{

    return !bad_options;
}

/** Use this function to check whether the client requested version
    information.  In addition to returning version information about
    the DODS software, the server can also provide version
    information about the dataset itself.

    @brief Should the filter send version information to the client
    program?

    @return TRUE if the -v option was given indicating that the filter
    should send version information back to the client, FALSE
    otherwise. 
    @see DODSFilter::send_version_info */
bool
DODSFilter::version()
{
    return ver;
}

/** @brief Is this request conditional? 

    @return True if the request is conditional.
    @see get_request_if_modified_since(). */
bool
DODSFilter::is_conditional()
{
    return d_conditional_request;
}

/** Set the CGI/Server version number. Servers use this when answering
    requests for version information. The vesion `number' should include
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
    cgi_ver = version;
}

/** Return the version information passed to the instance when it was
    created. This string is passed to the DODSFilter ctor using the -v
    option.

    @return The version string supplied at initialization. */
string
DODSFilter::get_cgi_version()
{
    return cgi_ver;
}

/** Return the entire constraint expression in a string.  This
    includes both the projection and selection clauses, but not the
    question mark.

    @brief Get the constraint expression. 
    @return A string object that contains the constraint expression. */
string
DODSFilter::get_ce()
{
    return ce;
}

void
DODSFilter::set_ce(string _ce)
{
    ce = www2id(_ce, "%", "%20");
}

/** The ``dataset name'' is the filename or other string that the
    filter program will use to access the data. In some cases this
    will indicate a disk file containing the data.  In others, it
    may represent a database query or some other exotic data
    access method. 

    @brief Get the dataset name. 
    @return A string object that contains the name of the dataset. */
string
DODSFilter::get_dataset_name()
{
    return dataset;
}

void
DODSFilter::set_dataset_name(const string _dataset)
{
  dataset = _dataset;
}

/** To read version information that is specific to a certain
    dataset, override this method with an implementation that does
    what you want. By default, this returns an empty string.

    @brief Get the version information for the dataset.  
    @return A string object that contains the dataset version
    information.  */ 
string
DODSFilter::get_dataset_version()
{
    return "";
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
DODSFilter::get_dataset_last_modified_time()
{
    return last_modified_time(dataset);
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
DODSFilter::get_das_last_modified_time(const string &anc_location)
{
    string name = find_ancillary_file(dataset, "das", anc_location, anc_file);
    return std::_MAX((name != "") ? last_modified_time(name) : 0,
		    get_dataset_last_modified_time()); 
}

/** Get the last modified time for the dataset's DDS. This time, given in
    seconds since the epoch (1 Jan 1970 00:00:00 GMT), is the greater of
    the datasets's and any ancillary DDS' last modified time.

    @return Time of last modification of the DDS.
    @see get_dataset_last_modified_time()
    @see get_dds_last_modified_time() */
time_t
DODSFilter::get_dds_last_modified_time(const string &anc_location)
{
    string name = find_ancillary_file(dataset, "dds", anc_location, anc_file);
    return std::_MAX((name != "") ? last_modified_time(name) : 0,
		    get_dataset_last_modified_time()); 
}

/** Get the last modified time to be used for a particular data request.
    This method should look at both the contraint expression and any
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
DODSFilter::get_data_last_modified_time(const string &anc_location)
{
    string dds_name = find_ancillary_file(dataset, "dds", anc_location,
					  anc_file);
    string das_name = find_ancillary_file(dataset, "dds", anc_location,
					  anc_file);
    time_t m = std::_MAX((das_name != "") ? last_modified_time(das_name) : (time_t)0,
			(dds_name != "") ? last_modified_time(dds_name) : (time_t)0);
    // Note that this is a call to get_dataset_... not get_data_...
    time_t n = get_dataset_last_modified_time();

    return std::_MAX(m, n); 
}

/** Get the value of a conditional request's If-Modified-Since header.
    This value is used to determine if the request should get a full
    response or a Not Modified (304) response. The time is given in
    seconds since the Unix epoch (midnight, 1 Jan 1970). If no time was
    given with the request, this methods returns -1.

    @return If-Modified-Since time from a condition GET request. */
time_t
DODSFilter::get_request_if_modified_since()
{
    return d_if_modified_since;
}

/** The <tt>cache_dir</tt> is used to hold the cached .dds and .das files.
    By default, this returns an empty string (store cache files in
    current directory.

    @brief Get the cache directory.
    @return A string object that contains the cache file directory.  */
string
DODSFilter::get_cache_dir()
{
  return cache_dir;
}

/** Get the list of accepted datatypes sent by the client. If no list was
    sent, return the string `All'. 

    NB: The funny spelling `accept types' instead of `accepted types'
    mirrors the name of the HTTP request header field name which in turn
    mirrors the common practice of using `accept' over `accepted'.

    @see DODSFilter
    @return A string containing a list of the accepted types. */
string
DODSFilter::get_accept_types()
{
    return accept_types;
}

/** Read the ancillary DAS information and merge it into the input
    DAS object.

    @brief Test if ancillary data must be read.
    @param das A DAS object that will be augmented with the
    ancillary data attributes.
    @param anc_location The directory in which the external DAS file resides.
    @return void
    @see DAS */
void
DODSFilter::read_ancillary_das(DAS &das, string anc_location)
{
    string name = find_ancillary_file(dataset, "das", 
			      (anc_location == "") ? anc_dir : anc_location, 
				      anc_file);

    FILE *in = fopen(name.c_str(), "r");
    if (in) {
	das.parse(in);
	int res = fclose( in ) ;
	if( res ) {
	    DBG(cerr << "DODSFilter::read_ancillary_das - Failed to close file " << (void *)in << endl ;) ;
	}
    }
}

/** Read the ancillary DDS information and merge it into the input
    DDS object. 

    @brief Test if ancillary data must be read.
    @param dds A DDS object that will be augmented with the
    ancillary data properties.
    @param anc_location The directory in which the external DAS file resides.
    @return void
    @see DDS */
void
DODSFilter::read_ancillary_dds(DDS &dds, string anc_location)
{
    string name = find_ancillary_file(dataset, "dds", 
			      (anc_location == "") ? anc_dir : anc_location, 
				      anc_file);
    FILE *in = fopen(name.c_str(), "r");
    if (in) {
	dds.parse(in);
	int res = fclose( in ) ;
	if( res ) {
	    DBG(cerr << "DODSFilter::read_ancillary_dds - Failed to close " << (void *)in << endl ;) ;
	}
    }
}

static const char *emessage = \
"DODS internal server error; usage error. Please report this to the dataset \
maintainer, or to support@unidata.ucar.edu.";

/** This message is printed when the filter program is incorrectly
    invoked by the dispatch CGI.  This is an error in the server
    installation or the CGI implementation, so the error message is
    written to stderr instead of stdout.  A server's stderr messages
    show up in the httpd log file. In addition, an error object is
    sent back to the client program telling them that the server is
    broken. 

    @brief Print usage information for a filter program. */
void 
DODSFilter::print_usage()
{
    // Write a message to the WWW server error log file.
    string oss="";
    oss+= "Usage: " +program_name+ " -V | [-c] [-v <cgi version>] [-e <ce>]";
    oss+= "       [-d <ancillary file directory>] [-f <ancillary file name>]";
    oss+= "       <dataset>\n";
    ErrMsgT(oss.c_str());

    throw Error(unknown_error, emessage);
}

/** This function formats and sends to stdout version
    information from the httpd server, the server dispatch scripts,
    the DODS core software, and (optionally) the dataset.

    @brief Send version information back to the client program. */ 
void 
DODSFilter::send_version_info()
{
    fprintf( stdout, "HTTP/1.0 200 OK\n" ) ;
    fprintf( stdout, "XDODS-Server: %s\n", cgi_ver.c_str() ) ;
    fprintf( stdout, "Content-Type: text/plain\n" ) ;
    fprintf( stdout, "\n" ) ;

    fprintf( stdout, "DODS server core software: %s\n", DVR ) ;

    if (cgi_ver != "")
	fprintf( stdout, "Server vision: %s\n", cgi_ver.c_str() ) ;

    string v = get_dataset_version();
    if (v != "")
	fprintf( stdout, "Dataset version: %s\n", v.c_str() ) ;
    
    fflush( stdout ) ;
}

// I've written a few unit tests for this method (see DODSFilterTest.cc) but
// it's very hard to test well. 5/1/2001 jhrg
/** This function formats and prints an ASCII representation of a
    DAS on stdout.  This has the effect of sending the DAS object
    back to the client program.

    @brief Transmit a DAS.
    @param os The output stream to which the DAS is to be sent.
    @param das The DAS object to be sent.
    @param anc_location The directory in which the external DAS file resides.
    @return void
    @see DAS */
void
DODSFilter::send_das(ostream &os, DAS &das, const string &anc_location)
{
    time_t das_lmt = get_das_last_modified_time(anc_location);
    if (is_conditional()
	&& das_lmt <= get_request_if_modified_since()) {
	set_mime_not_modified(os);
    }
    else {
	set_mime_text(os, dods_das, cgi_ver, x_plain, das_lmt);
	das.print(os);
    }
}

void
DODSFilter::send_das(FILE *out, DAS &das, const string &anc_location)
{
    time_t das_lmt = get_das_last_modified_time(anc_location);
    if (is_conditional()
	&& das_lmt <= get_request_if_modified_since()) {
	set_mime_not_modified(out);
    }
    else {
	set_mime_text(out, dods_das, cgi_ver, x_plain, das_lmt);
	das.print(out);
    }
    fflush( stdout ) ;
}

void
DODSFilter::send_das(DAS &das, const string &anc_location)
{
    send_das(stdout, das, anc_location);
}

/** This function formats and prints an ASCII representation of a
    DDS on stdout.  When called by a CGI program, this has the
    effect of sending a DDS object back to the client
    program. Either an entire DDS or a constrained DDS may be sent.

    @brief Transmit a DDS.
    @param os The output stream to which the DAS is to be sent.
    @param dds The DDS to send back to a client.
    @param constrained If this argument is true, evaluate the
    current constraint expression and send the `constrained DDS'
    back to the client. 
    @param anc_location The directory in which the external DAS file resides.
    @return void
    @see DDS */
void
DODSFilter::send_dds(ostream &os, DDS &dds, bool constrained,
		     const string &anc_location)
{
    // If constrained, parse the constriant. Throws Error or InternalErr.
    if (constrained)
	dds.parse_constraint(ce, os, true);

    time_t dds_lmt = get_dds_last_modified_time(anc_location);
    if (is_conditional() 
	&& dds_lmt <= get_request_if_modified_since()) {
	set_mime_not_modified(os);
    }
    else {
	set_mime_text(os, dods_dds, cgi_ver, x_plain, dds_lmt);
	if (constrained)
	    dds.print_constrained(os);
	else
	    dds.print(os);
    }
}

void
DODSFilter::send_dds(FILE *out, DDS &dds, bool constrained,
		     const string &anc_location)
{
    // If constrained, parse the constriant. Throws Error or InternalErr.
    if (constrained)
	dds.parse_constraint(ce, out, true);

    time_t dds_lmt = get_dds_last_modified_time(anc_location);
    if (is_conditional() 
	&& dds_lmt <= get_request_if_modified_since()) {
	set_mime_not_modified(out);
    }
    else {
	set_mime_text(out, dods_dds, cgi_ver, x_plain, dds_lmt);
	if (constrained)
	    dds.print_constrained(out);
	else
	    dds.print(out);
    }

    fflush( stdout ) ;
}

void
DODSFilter::send_dds(DDS &dds, bool constrained, const string &anc_location)
{
    send_dds(stdout, dds, constrained, anc_location);
}

/** Send the data in the DDS object back to the client
    program.  The data is encoded in XDR format, and enclosed in a
    MIME document which is all sent to stdout.  This has the effect
    of sending it back to the client.

    @brief Transmit data.
    @param dds A DDS object containing the data to be sent.
    @param data_stream A pointer to the XDR sink into which the data
    is to be put for encoding and transmission.
    @param anc_location A directory to search for ancillary files (in
    addition to the CWD).  This is used in a call to 
    get_data_last_modified_time(). 
    @return void
    @see DDS */
void
DODSFilter::send_data(DDS &dds, FILE *data_stream, const string &anc_location)
{
    bool compress = comp && deflate_exists();
    time_t data_lmt = get_data_last_modified_time(anc_location);

    // If this is a conditional request and the server should send a 304
    // response, do that and exit. Otherwise, continue on ans send the full
    // response. 
    if (is_conditional()
	&& data_lmt <= get_request_if_modified_since()) {
	set_mime_not_modified(data_stream);
	return;
    }

    // Jose Garcia
    // DDS::send may return false or throw an exception
    if (!dds.send(dataset, ce, data_stream, compress, cgi_ver,
		  data_lmt)) {
	ErrMsgT((compress) ? "Could not send compressed data" : 
		"Could not send data");
	throw InternalErr("could not send data");
    }
}

// $Log: DODSFilter.cc,v $
// Revision 1.31  2003/02/09 10:07:32  rmorris
// Fixed a conflict using std::max under win32.
//
// Revision 1.30  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.29  2003/01/13 22:55:07  jimg
// When a filter program got the -V option without a dataset, It was returning
// usage information. Fixed.
//
// Revision 1.28  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.23.2.19  2002/12/20 00:46:58  jimg
// I removed the include of sys/ddi.h. This header was not found on my
// machine (Red Hat 7.3).
//
// Revision 1.23.2.18  2002/12/17 22:35:02  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.23.2.17  2002/12/01 14:37:52  rmorris
// Smalling changes for the win32 porting and maintenance work.
//
// Revision 1.23.2.16  2002/11/21 21:24:17  pwest
// memory leak cleanup and file descriptor cleanup
//
// Revision 1.23.2.15  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.23.2.14  2002/06/18 22:50:06  jimg
// Added include of util.h. This was necessary because I removed the include of
// Connect.h from cgi_util.h.
//
// Revision 1.27  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.26  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.23.2.13  2002/03/29 18:40:20  jimg
// Updated comments and/or removed dead code.
//
// Revision 1.23.2.12  2002/01/28 20:34:25  jimg
// *** empty log message ***
//
// Revision 1.25  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.23.2.11  2001/08/21 14:54:29  dan
// Added a set_dataset_name method to provide a mechanism to change the
// dataset name in the DODSFilter class, which currently can only be set
// by running the constructor.   This method was required for a modification
// to the jg-dods server which now support relative pathnames as part of
// the object name.
//
// Revision 1.23.2.10  2001/08/18 00:18:41  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.23.2.9  2001/07/28 01:04:46  jimg
// Rewrote calls to www2id since it now uses strings and not Regexs.
// Removed old code.
//
// Revision 1.24  2001/06/15 23:49:02  jimg
// Merged with release-3-2-4.
//
// Revision 1.23.2.8  2001/06/14 21:31:15  jimg
// Modified the handling of version requests as signaled by this class'
// client. The version number printed from this class is for the core software.
// The cgi_version property should be set by the client so that the
// send_version() method will write both the core and server (CGI) version
// numbers.
//
// Revision 1.23.2.7  2001/06/08 21:57:49  jimg
// Really fixed the usage message this time...
//
// Revision 1.23.2.6  2001/06/08 21:37:02  jimg
// Corrected the usage message. Added to text in the Error thrown as well.
//
// Revision 1.23.2.5  2001/05/16 21:12:26  jimg
// Modified the ctor and set_ce() so that www2id() does not replace %20s
// in either the dataset name or the CE. This keeps the parsers from gaging
// on spaces (which they think are word separators).
//
// Revision 1.23.2.4  2001/05/07 17:19:08  jimg
// The dataset name and CE are now passed through www2id() which removes WWW
// escape sequences (those %<hex digit><hex digit> things) and replaces them
// with the correct ASCII characters.
// Fixed a bug where DAS::parse(FILE *) and DDS::parse(FILE *) were often called
// with a null FILE *.
//
// Revision 1.23.2.3  2001/05/03 23:36:31  jimg
// Removed all the catch clauses for Error and InternalErr. Code that uses
// DODSFilter should catch (and probably serialize) Error and its children. All
// of the current URI servers do this.
//
// Revision 1.23.2.2  2001/05/03 20:23:47  jimg
// Added the methods is_conditional() and get_request_if_modified_since(). These
// are used to determine if the request was conditional and, if so, to get the
// value of the If-Modified-Since header.
// Modified the send_das(), send_dds() and send_data() methods so that a 304
// (not modified) response is sent if appropriate.
//
// Revision 1.23.2.1  2001/04/23 22:34:46  jimg
// Added support for the Last-Modified MIME header in server responses.`
//
// Revision 1.23  2000/10/30 17:21:27  jimg
// Added support for proxy servers (from cjm).
//
// Revision 1.22  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.21  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.20  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.19  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.18  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.17.10.1  2000/06/02 18:16:48  rmorris
// Mod's for port to Win32.
//
// Revision 1.16.2.3  2000/05/18 20:45:27  jimg
// added set_ce(). Maybe add more set methods?
//
// Revision 1.17.4.3  2000/03/08 00:09:04  jgarcia
// replace ostrstream with string;added functions to convert from double and
// long to string
//
// Revision 1.17.4.2  2000/02/17 05:03:12  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.17.4.1  2000/02/07 21:11:36  jgarcia
// modified prototypes and implementations to use exceeption handling
//
// Revision 1.16.2.2  1999/09/08 22:36:51  jimg
// Fixed the spelling of version (was vision) and the usage line (did not
// include -V).
//
// Revision 1.17  1999/09/03 22:07:44  jimg
// Merged changes from release-3-1-1
//
// Revision 1.16.2.1  1999/08/28 06:43:04  jimg
// Fixed the implementation/interface pragmas and misc comments
//
// Revision 1.16  1999/07/22 17:11:51  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.15.4.1  1999/06/01 15:43:51  jimg
// Made dods/3.0 the default version number. This makes is simpler to debug dods
// servers since running the server filter programs will generate valid headers
// now. Before you had to remember to use the -v option and give a version
// string/number or the MIME header would not be valid. This confused the MIME
// header parse which hosed the data stream.
//
// Revision 1.15  1999/05/26 17:37:02  jimg
// Added a bit where, before sending caught Error objects to the client, we
// write the message to t eh httpd's error_log.
//
// Revision 1.14  1999/05/25 21:57:52  dan
// Added an optional second argument to read_ancillary_dds to support
// JGOFS usage.
//
// Revision 1.13  1999/05/25 21:54:19  dan
// Added an optional argument to read_ancillary_das to support JGOFS
// data object usage, where the location of the ancillary DAS file isn't
// readily available through the 'dataset' argument of the command line.
//
// Revision 1.12  1999/05/21 17:15:46  jimg
// Added instrumentation to the ctor. This simplifies debugging the interaction
// between the filter programs and the perl script.
//
// Revision 1.11  1999/05/19 23:56:57  jimg
// Changed the support address from @dods to @unidata
//
// Revision 1.10  1999/05/05 00:36:36  jimg
// Added the -V option. -v now is used to pass the version information from the
// CGI to the C++ software; -V triggers output of the version message. This
// allows the DODSFilter class to pass information about the server's version to
// the core software.
// All set_mime_*() functions are now passes the CGI version information so that
// all MIME headers contain information about the server's version.
// Added the get_cgi_version() member function so that clients of DODSFilter can
// find out the version number.
//
// Revision 1.9  1999/05/04 19:47:21  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.8  1999/04/29 02:29:28  jimg
// Merge of no-gnu branch
//
// Revision 1.7  1999/02/22 22:59:07  jimg
// Added the get_accept_types() accessor.
// Changed the ctor so that the -t option is recognized.
//
// Revision 1.6  1998/12/16 19:10:53  jimg
// Added support for XDODS-Server MIME header. This fixes a problem where our
// use of Server clashed with Java.
//
// Revision 1.5  1998/11/10 01:04:42  jimg
// Added `ends' to strings made with ostrstream (fixes a bug found with
// purify).
// Added catch for throws from within the CE evaluation functions.
//
// Revision 1.4.2.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.4.2.1  1999/02/02 21:56:57  jimg
// String to string version
//
// Revision 1.4  1998/08/06 16:12:30  jimg
// Added cache dir methods and stuff to ctor (from jeh)
//
// Revision 1.3  1998/03/19 23:34:21  jimg
// Fixed calls to set_mime_*().
// Removed the compression code (it is now in DDS::send().
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.2  1998/02/11 22:00:46  jimg
// Added call to util.cc:deflate_exists() to send_data(). This means that
// send_data() will only try to start the compressor if an executable copy of
// deflate can be found. If, for any reason a copy of deflate cannot be found
// data is sent without trying to compress it.
//
// Revision 1.1  1997/08/28 20:39:02  jimg
// Created
//

