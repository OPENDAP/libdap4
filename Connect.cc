
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//         Dan Holloway <dholloway@gso.uri.edu>
//         Reza Nekovei <reza@intcomm.net>
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
 
// (c) COPYRIGHT URI/MIT 1994-2002
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>
//      dan             Dan Holloway <dholloway@gso.uri.edu>
//      reza            Reza Nekovei <reza@intcomm.net>

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used =
    { "$Id: Connect.cc,v 1.127 2003/04/22 19:40:27 jimg Exp $" };

#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include <strstream>
#include <fstream>

#include "debug.h"
#include "DataDDS.h"
#include "Connect.h"
#include "escaping.h"
#include "RCReader.h"

using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;

extern ObjectType get_type(const string &value);

/** This private method process data from both local and remote sources. It
    exists to eliminate duplication of code. */
void
Connect::process_data(DataDDS &data, Response *rs) 
    throw(Error, InternalErr)
{
    data.set_version(rs->get_version());

    DBG(cerr << "Entring process_data: d_stream = " << rs << endl);
    switch (rs->get_type()) {
      case dods_error:
	if (!_error.parse(rs->get_stream()))
	    throw InternalErr(__FILE__, __LINE__,
	      "Could not parse the Error object returned by the server!");
	throw _error;

      case web_error:
	// Web errors (those reported in the return document's MIME header)
	// are processed by the WWW library.
	return;

      case dods_data:
      default: {
	  // Parse the DDS; throw an exception on error.
	  data.parse(rs->get_stream());
	  XDR *xdr_stream = new_xdrstdio(rs->get_stream(), XDR_DECODE);

	  // Load the DDS with data.
	  try {
	      for (DDS::Vars_iter i = data.var_begin(); i != data.var_end(); 
		   i++) {
		  (*i)->deserialize(xdr_stream, &data);
	      }
	  }
	  catch(...) {
	      delete_xdrstdio(xdr_stream);
	      throw;
	  }

	  delete_xdrstdio(xdr_stream);
      }
    }
}

// This function returns the ObjectType value that matches the given string.
ObjectType 
get_type(const string &value)
{
    if (value == "dods_das")
	return dods_das;
    else if (value == "dods_dds")
	return dods_dds;
    else if (value == "dods_data")
	return dods_data;
    else if (value == "dods_error")
	return dods_error;
    else if (value == "web_error")
	return web_error;
    else
	return unknown_type;
}

// Barely a parser... This is used when reading from local sources of DODS
// Data objects. It simulates the important actions of the libwww MIME header
// parser. Those actions fill in certain fields in the Connect object. jhrg
// 5/20/97
//
// Make sure that this parser reads from data_source without disturbing the
// information in data_source that follows the MIME header. Since the DDS
// (which follows the MIME header) is parsed by a flex/bison scanner/parser,
// make sure to use I/O calls that will mesh with ANSI C I/O calls. In the
// old GNU libg++, the C++ calls were synchronized with the C calls, but that
// may no longer be the case. 5/31/99 jhrg

/** Use when you cannot use libcurl.
    @param data_source Read from this stream.
    @param rs Value/Result parameter. Dump version and type information here.
    */ 
void 
Connect::parse_mime(FILE *data_source, Response *rs)
{
    rs->set_version("dods/0.0"); // initial value; for backward compat.

    char line[256];
    fgets(line, 255, data_source);
    line[strlen(line) - 1] = '\0';	// remove the newline

    while (line[0] != '\0') {
	char h[256], v[256];
	sscanf(line, "%s %s\n", h, v);
	string header = h;
	string value = v;
	downcase(header);
	downcase(value);

	if (header == "content-description:") {
	    DBG(cout << header << ": " << value << endl);
	    rs->set_type(get_type(value));
	} else if (header == "xdods-server:") {
	    DBG(cout << header << ": " << value << endl);
	    rs->set_version(value);
	} else if (rs->get_version() == "dods/0.0" && header == "server:") {
	    DBG(cout << header << ": " << value << endl);
	    rs->set_version(value);
	}

	fgets(line, 255, data_source);
	line[strlen(line) - 1] = '\0';
    }
}

// public mfuncs

/** The Connect constructor requires a <tt>name</tt>, which is the URL to
    which the connection is to be made. 

    @param name The URL for the virtual connection.
    @param www_verbose_errors Ignored
    @param accept_deflate Does this client accept deflated responses? True by
    default. 
    @param uname Use this username for authentication. Null by default.
    @param password Passwrod to use for authentication. Null by default.
    @brief Create an instance of Connect. */
Connect::Connect(const string &n, bool www_verbose_errors, 
		 bool accept_deflate, string uname, string password) 
    throw (Error, InternalErr)
    : d_http(0), d_version("unknown")
{
    string name = prune_spaces(n);
    
    // Figure out if the URL starts with 'http', if so, make sure that we
    // talk to an instance of HTTPConnect.
    if (name.find("http") == 0) {
	DBG(cerr << "Connect: The identifier is an http URL" << endl);
	d_http = new HTTPConnect(RCReader::instance());
	DBG2(cerr << "Initialized d_http to: " << hex << d_http << dec << endl);

	// Find and store any CE given with the URL.
	string::size_type dotpos = name.find('?');
	if (dotpos != name.npos) {
	    _URL = name.substr(0, dotpos);
	    string expr = name.substr(dotpos + 1);

	    dotpos = expr.find('&');
	    if (dotpos != expr.npos) {
		_proj = expr.substr(0, dotpos);
		_sel = expr.substr(dotpos);	// XXX includes '&'
	    } else {
		_proj = expr;
		_sel = "";
	    }
	} else {
	    _URL = name;
	    _proj = "";
	    _sel = "";
	}

	_local = false;
    } 
    else {
	DBG(cerr << "Connect: The identifier is a local data source." << endl);

	d_http = 0;
	_URL = "";
	_local = true;		// local in this case means non-DODS
    }

    set_accept_deflate(accept_deflate);

    set_credentials(uname, password);
}

Connect::~Connect()
{
    DBG2(cerr << "Entering the Connect dtor" << endl);

    if (d_http)
	delete d_http;

    DBG2(cerr << "Leaving the Connect dtor" << endl);
}

/** Reads the DAS corresponding to the dataset in the Connect
    object's URL. Although DODS does not support usig CEs with DAS
    requests, if present in the Connect object's instance, they will be
    escaped and passed as the query string of the request.

    @brief Get the DAS from a server.
    @param das Result. */
void
Connect::request_das(DAS &das) throw(Error, InternalErr)
{
    string das_url = _URL + ".das";
    if (_proj.length() + _sel.length())
	das_url = das_url + "?" + id2www_ce(_proj + _sel);

    Response *rs = 0;
    try {
	rs = d_http->fetch_url(das_url);
    }
    catch (...) {
	delete rs;
	throw;
    }

    d_version = rs->get_version(); // Improve this design!

    switch (rs->get_type()) {
      case dods_error: {
	  if (!_error.parse(rs->get_stream())) {
	      throw InternalErr(__FILE__, __LINE__, 
			"Could not parse error returned from server.");
	      break;
	  }
	  throw _error;
	  break;
      }

      case web_error:
	// We should never get here; a web error should be picked up read_url
	// (called by fetch_url) and result in a thrown Error object.
	break;

      case dods_das:
      default:
	// DAS::parse throws an exception on error.
	try {
	    das.parse(rs->get_stream()); // read and parse the das from a file 
	}
	catch (...) {
	    delete rs;
	    throw;
	}
	    
	break;
    }

    delete rs;
}

/** Reads the DDS corresponding to the dataset in the Connect object's URL.
    If present in the Connect object's instance, a CE will be escaped,
    combined with \c expr and passed as the query string of the request.

    @brief Get the DDS from a server.
    @param dds Result.
    @param expr Send this constraint expression to the server. */
void
Connect::request_dds(DDS &dds, string expr) throw(Error, InternalErr)
{
    string proj, sel;
    string::size_type dotpos = expr.find('&');
    if (dotpos != expr.npos) {
	proj = expr.substr(0, dotpos);
	sel = expr.substr(dotpos);
    } else {
	proj = expr;
	sel = "";
    }

    string dds_url = _URL + ".dds" + "?" 
	+ id2www_ce(_proj + proj + _sel + sel);

    Response *rs = 0;
    try {
	rs = d_http->fetch_url(dds_url);
    }
    catch (...) {
	delete rs;
	throw;
    }

    d_version = rs->get_version(); // Improve this design!

    switch (rs->get_type()) {
      case dods_error: {
	  if (!_error.parse(rs->get_stream())) {
	      throw InternalErr(__FILE__, __LINE__, 
			"Could not parse error returned from server.");
	      break;
	  }
	  throw _error;
	  break;
      }

      case web_error:
	// We should never get here; a web error should be picked up read_url
	// (called by fetch_url) and result in a thrown Error object.
	break;

      case dods_dds:
      default:
	// DDS::prase throws an exception on error.
	try {
	    dds.parse(rs->get_stream()); // read and parse the dds from a file 
	}
	catch (...) {
	    delete rs;
	    throw;
	}
	break;
    }

    delete rs;
}

/** Reads the DataDDS object corresponding to the dataset in the Connect
    object's URL. If present in the Connect object's instance, a CE will be
    escaped, combined with \c expr and passed as the query string of the
    request. The result is a DataDDS which contains the data values bound to
    variables.

    @brief Get the DAS from a server.
    @param data Result.
    @param expr Send this constraint expression to the server. */
void
Connect::request_data(DataDDS &data, string expr) throw(Error, InternalErr)
{
    string proj, sel;
    string::size_type dotpos = expr.find('&');
    if (dotpos != expr.npos) {
	proj = expr.substr(0, dotpos);
	sel = expr.substr(dotpos);
    } else {
	proj = expr;
	sel = "";
    }

    string data_url = _URL + ".dods?" 
	+ id2www_ce(_proj + proj + _sel + sel);

    Response *rs = 0;
    // We need to catch Error exceptions to ensure calling close_output.
    try {
	rs = d_http->fetch_url(data_url);
	d_version = rs->get_version(); // Improve this design!

	process_data(data, rs);
	delete rs;
    }
    catch (...) {
	delete rs;
	throw;
    }
}

/** This is a place holder. A better implementation for reading objects from
    the local file store is to write FileConnect and have it support the same
    interface as HTTPConnect.

    @param data Result.
    @param data_source Read from this open file/stream. */
void
Connect::read_data(DataDDS &data, FILE *data_source) throw(Error, InternalErr)
{
    if (!data_source) 
	throw InternalErr(__FILE__, __LINE__, "data_source is null.");

    Response *rs = 0;

    try {
	rs = new Response(data_source);
	// Read from data_source and parse the MIME headers specific to DODS.
	parse_mime(data_source, rs);
    
	process_data(data, rs);

	delete rs;
    }
    catch (...) {
	delete rs;
	throw;
    }
}

bool 
Connect::is_local()
{
    return _local;
}

/** Return the Connect object's URL in a string.  The URL was set by
    the class constructor, and may not be reset.  If you want to
    open another URL, you must create another Connect object.  There
    is a Connections class created to handle the management of
    multiple Connect objects.

    @brief Get the object's URL.
    @see Connections
    @return A string containing the URL of the data to which the
    Connect object refers.  If the object refers to local data,
    the function returns the null string.  
    @param ce If TRUE, the returned URL will include any constraint
    expression enclosed with the Connect object's URL (including the
    <tt>?</tt>).  If FALSE, any constraint expression will be removed from
    the URL.  The default is TRUE.
*/
string 
Connect::URL(bool ce)
{
    if (_local)
	throw InternalErr(__FILE__, __LINE__, 
		  "URL(): This call is only valid for a DODS data source.");

    if (ce)
	return _URL + "?" + _proj + _sel;
    else
	return _URL;
}

/** Return the constraint expression (CE) part of the Connect URL. Note
    that this CE is supplied as part of the URL passed to the
    Connect's constructor.  It is not the CE passed to the 
    <tt>request_data()</tt> function.

    @brief Get the Connect's constraint expression.
    @return A string containing the constraint expression (if any)
    submitted to the Connect object's constructor.  */
string 
Connect::CE()
{
    if (_local)
	throw InternalErr(__FILE__, __LINE__, 
		  "CE(): This call is only valid for a DODS data source.");

    return _proj + _sel;
}

/** @brief Set the credentials for responding to challenges while dereferencing
    URLs. 
    @param u The username.
    @param p The password. 
    @see extract_auth_info() */
void 
Connect::set_credentials(string u, string p)
{
    if (d_http)
	d_http->set_credentials(u, p);
}

/** Set the <c>accept deflate</c> property. 
    @param deflate True if the client can accept compressed responses, Flase
    otherwise. */
void 
Connect::set_accept_deflate(bool deflate)
{
    if (d_http)
	d_http->set_accept_deflate(deflate);
}

/** Disable any further use of the client-side cache. In a future version
    of this software, this should be handled so that the www library is
    not initialized with the cache running by default. */
void
Connect::set_cache_enabled(bool cache)
{
    if (d_http)
	d_http->set_cache_enabled(cache);
}

bool
Connect::is_cache_enabled()
{
    bool status;
    DBG(cerr << "Entering is_cache_enabled (" << hex << d_http << dec 
	<< ")... ");
    if (d_http)
	status = d_http->is_cache_enabled();
    else
	status = false;
    DBGN(cerr << "exiting" << endl);
    return status;
}

/** @name Remove these...
    All of these are deprecated and will be removed in a futire version of
    this code. */
//@{
/** All DODS datasets define a Data Attribute Structure (DAS), to
    hold a variety of information about the variables in a
    dataset. This function returns the DAS for the dataset indicated
    by this Connect object.

    @brief Return a reference to the Connect's DAS object. 
    @return A reference to the DAS object.
    @deprecated
    @see DAS 
*/
DAS & 
Connect::das()
{
    if (_local)
	throw InternalErr(__FILE__, __LINE__,
			  "Cannot access DAS for a non-DODS data source.");

    return _das;
}

/** All DODS datasets define a Data Descriptor Structure (DDS), to
    hold the data type of each of the variables in a dataset.  This
    function returns the DDS for the dataset indicated by this
    Connect object.

    @brief Return a reference to the Connect's DDS object. 
    @return A reference to the DDS object.
    @deprecated
    @see DDS 
*/
DDS & 
Connect::dds()
{
    if (_local)
	throw InternalErr(__FILE__, __LINE__,
			  "Cannot access DDS for a non-DODS data source.");

    return _dds;
}

/** The DODS server uses Error objects to signal error conditions to
    the client.  If an error condition has occurred while fetching a
    URL, the Connect object will contain an Error object with
    information about that error.  The Error object may also contain
    a program to run to remedy the error.  This function returns the
    latest Error object received by the Connect object.

    @brief Get a reference to the last Error object.
    @return The last Error object sent from the server. If no error has
    been sent from the server, returns a reference to an empty error
    object. 
    @deprecated
    @see Error 
*/
Error & 
Connect::error()
{
    return _error;
}
//@}

// $Log: Connect.cc,v $
// Revision 1.127  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.126  2003/03/14 00:01:00  jimg
// Added DBG statements to is_cache_enabled(). This helped find a bug that
// defied the debugger...
//
// Revision 1.125  2003/03/04 21:45:17  jimg
// Removed code in #if 0 ... #endif. Added get_version(). Fixed constructor
// documentation.
//
// Revision 1.124  2003/03/04 17:54:52  jimg
// Removed many old methods (methods that were used with libwww). Switched to
// the new Response objects.
//
// Revision 1.123  2003/02/27 23:22:30  jimg
// Removed old code (code inside #if 0 ... #endif) and added a call to
// HTTPConnect::set_credentials() and set_accept_deflate() in the constructor.
//
// Revision 1.122  2003/02/26 06:39:13  jimg
// Fixed documentation comments.
//
// Revision 1.121  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.120.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.120  2003/01/23 00:22:23  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.119  2003/01/10 19:46:39  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1  1994/10/05  18:02:06  jimg
// First version of the connection management classes.
// This commit also includes early versions of the test code.
// Revision 1.105.2.41  2002/12/24 00:14:53  jimg
// Changed the output() method so that it works with the new changes to
// HTTPConnect.
// Made output(), close_output(), source() and close_source() private. These
// four methods made up a deprecated interface to data. All data accessed using
// this class is now read directly from a returned DataDDS instance.
//
// Revision 1.105.2.40  2002/12/17 22:35:02  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.105.2.39  2002/12/05 20:36:19  pwest
// Corrected problems with IteratorAdapter code, making methods non-inline,
// creating source files and template instantiation file. Cleaned up file
// descriptors and memory management problems. Corrected problem in Connect
// where the xdr source was not being cleaned up or a new one created when a
// new file was opened for reading.
//
// Revision 1.105.2.38  2002/11/21 21:24:17  pwest
// memory leak cleanup and file descriptor cleanup
//
// Revision 1.105.2.37  2002/10/28 21:17:43  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.105.2.36  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.105.2.35  2002/08/22 21:23:23  jimg
// Fixes for the Win32 Build made at ESRI by Vlad Plenchoy and myslef.
//
// Revision 1.105.2.34  2002/08/08 06:54:56  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.105.2.33  2002/07/06 19:33:34  jimg
// I fixed some of the access functions that provide information that servers
// send in the headers of a response. If we're accessing a local response (a
// DataDDS dumped to a file or read from stdin) the behavior of HTTP is
// simulated (something that we should change but can stay for now). In some
// cases it was possible for a client of Connect to ask the HTTPConnect object
// for information when that object had not been initialized because the data
// source was a local file. The implementation of Connect still needs work...
//
// Revision 1.105.2.32  2002/06/21 22:23:29  jimg
// I revised the basic interface to this class. The old interface is implemented
// using the new one. Methods to get objects from DODS objects now take
// references to the containers for those objects as formal parameters. This
// enables a user of Connect to control how/when/where those containers are
// created and managed.
//
// Revision 1.105.2.31  2002/06/21 00:12:51  jimg
// Added a new request_dds() method that takes a reference to a DDS and dumps
// the retrieved info into it instead of the DDS that's held locally by this
// class. This is a much more flexible approach to reading responses. I also
// reimplemented the older version of request_dds using the new method (easy
// since the GUI stuff is disabled).
//
// Revision 1.105.2.30  2002/06/20 06:21:27  jimg
// Fixed local access. For now access to OPENDAP objects stored in local files
// uses a local FILE *. Methods test _local to determine whether to use the
// HTTPConnect object or the local FILE *.
//
// Revision 1.105.2.29  2002/06/20 03:18:48  jimg
// Fixes and modifications to the Connect and HTTPConnect classes. Neither
// of these two classes is complete, but they should compile and their
// basic functions should work.
//
// Revision 1.105.2.28  2002/06/18 23:03:43  jimg
// Added extern declaration of ObjectType get_type(...).
//
// Revision 1.118  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.117  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.105.2.23  2002/04/29 04:31:51  rmorris
// Considerable patching to make win32 client-side caching work.  We need
// to document that .dodsrc cannot use backslashes in filenames, must use
// forward slash.  Also need to document that pointing env vars for caching
// to places with spaces in the directory name is a no-no.
//
// Revision 1.105.2.22  2002/04/27 22:51:44  rmorris
// Change to turn client-side caching on under win32.  This is still a
// hack around the "spaces in directory names" problem.  For now we are
// going to hardcode the cache to be in C:\TEMP\DODS, as the user-specific
// directories are going to have spaces in them.  There is a porential
// small, subtle problem if more than one dods user was active at the
// same time on a win32 box (unlikely).
//
// Revision 1.105.2.21  2002/04/02 18:36:17  jimg
// Fixed a bug (#416) in server_handler(). If a server sent a response that
// included the Server header *after* it included the XDODS-Server header, the
// value of Server was used in preference to XDODS-Server. This broke reading
// Sequences since clients thought they were talking to an old server.
//
// Revision 1.105.2.20  2002/02/04 00:19:36  rmorris
// Ported an occurance of "mkstemp" in fetch_url to use something compatible]
// with VC++.  There is no mkstemp in win32.
//
// Revision 1.105.2.19  2002/01/30 18:59:55  jimg
// I changed request_das, _dds and _data so that they take into account various
// exceptions that may be thrown by the DAS and DDS parsers as well as
// fetch_url. Now each of these methods catch *any* exception, close up the
// output stream if its open and popdown the progress indicator if its up. Then
// the exception is thrown further up the call chain.
// I also removed an explicit catch of InternalErr in request_data which was
// adding some more text (useless and goofy looking) to the exception and re
// throwing it. Now the text is not added.
// I also made read_data() (the somewhat broken method for reading from files)
// catch (...) and close the output stream and progress indicator.
//
// Revision 1.105.2.18  2002/01/28 20:34:25  jimg
// *** empty log message ***
//
// Revision 1.105.2.17  2002/01/17 00:42:02  jimg
// I added a new method to disable use of the cache. This provides a way
// for a client to suppress use of the cache even if the user wants it
// (or doesn't say they don't want it).
//
// Revision 1.105.2.16  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.116  2001/10/29 21:24:39  jimg
// Removed catch and display of Error objects thrown/returned when accessing
// data. These Error objects must now be caught by the code that uses
// libdap++.a.
//
// Revision 1.115  2001/10/25 21:23:23  jgarcia
// Modified the www_lib_init method so now it uses the class RCReader. This simplifies a lot the implementation of this class.
//
// Revision 1.114  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.105.2.15  2001/10/08 17:19:31  jimg
// Changed error handling scheme; this class never catches an Error object to
// display it. It catches some to add information. It always relies on the
// caller to handle the display of information about the error. This changes the
// way the read methods work slightly. Their return code are now bogus; they use
// exceptions to return error information to the caller.
//
// Revision 1.113  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.105.2.14  2001/09/26 00:45:46  jimg
// Minor change to www_lib_init while trying to fix the seg fault on cancel bug.
// The actual fix is in libwww (HTReader.c). However, I removed a line of
// unnecessary code in the process of tracking down the bug.
//
// Revision 1.105.2.13  2001/09/25 21:45:35  jimg
// Fixed a bug in the cache directory pathname code. Using the id2www function
// on the pathname breaks libwww; it correctly escapes the %xx sequences but
// does not then correctly simplify the pathname (which means, in this case,
// removing the `file:' or `cache:' prefix). Apparently the WIN32 code needs
// this so I moved the call to id2www into the WIN32 section of the www_lib_init
// method.
//
// Revision 1.105.2.12  2001/09/25 20:36:42  jimg
// Added debugging code for cancel button seg fault.
//
// Revision 1.105.2.11  2001/09/07 00:38:34  jimg
// Sequence::deserialize(...) now reads all the sequence values at once.
// Its call semantics are the same as the other classes' versions. Values
// are stored in the Sequence object using a vector<BaseType *> for each
// row (those are themselves held in a vector). Three new accessor methods
// have been added to Sequence (row_value() and two versions of var_value()).
// BaseType::deserialize(...) now always returns true. This matches with the
// expectations of most client code (the seqeunce version returned false
// when it was done reading, but all the calls for sequences must be changed
// anyway). If an XDR error is found, deserialize throws InternalErr.
//
// Revision 1.112  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.105.2.10  2001/08/22 06:13:56  jimg
// CEs are not sent to id2www_ce() which is more relaxed about the characters
// it allows. This was done so that new clients would not break old servers.
//
// Revision 1.105.2.9  2001/07/28 01:10:41  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.105.2.8  2001/07/11 05:29:11  jimg
// Connect::read_data() throws an InternalErr if it is passed a null data_source
// pointer (FILE *). This is a fix for clients that used the `read from a file
// or stdin' feature of Connect and don't check this themselves.
//
// Revision 1.111  2001/06/15 23:49:01  jimg
// Merged with release-3-2-4.
//
// Revision 1.105.2.7  2001/05/23 16:47:56  jimg
// Changed the initialization of _gui (an instance of Gui) so that it takes
// place for Connect instances that access both local and remote data
// sources. It was being set up for remote accesses only. This meant that a
// libdap++ compiled with GUI defined that was used to access a local data
// source (e.g., piped output from a server filter program) would crash when
// it tried to use the Gui instance to display something. I changed the
// function definitions so that the return types are on one line and the name
// and arguments are on the following line. Tags are easier to use with this
// style.
//
// Revision 1.110  2001/05/04 00:08:43  jimg
// Fixed a bug where an Error object was created with an error code of
// undefined_error (which make OK() throw and exception).
//
// Revision 1.105.2.6  2001/05/03 19:04:16  jimg
// Added use of the _always_validate field. This configuration file is read
// and this filed is set if a value is given for ALWAYS_VALIDATE. The default
// value for the filed is zero (don't force validation). The _always_validate
// field value is used to tell libwww to always validate cache entries (value
// of 1) or to use either the expires header value or a heuristic value
// derived from the current time and the last modified time sent from the
// origin server. If no LM time was sent from the origin server, then the
// value of DEFAULT_EXPIRES (from the configuration file) is used.
//
// Revision 1.105.2.5  2001/04/16 17:06:21  jimg
// Changed the call to Error's ctor on 390 so that it uses unknown_error as
// the error constant. It was using undefined_error and this caused Error's
// invariant to barf.
//
// Revision 1.105.2.4  2001/02/16 21:18:42  jimg
// Removed some excess variables in fetch_url().
//
// Revision 1.105.2.3  2001/02/14 00:10:04  jimg
// Merged code from the trunk's HEAD revision for this/these files onto
// the release-3-2 branch. This moves the authentication software onto the
// release-3-2 branch so that it will be easier to get it in the 3.2 release.
//
// Other changes:
// SetSignal() removed; libwww handles this now.
// I removed redundant initialization of libwww Before and After filters.
// I added calls to HTLibInit and HTLibTerminate in www_lib_init() and
// ~Connect(). The LibInit call sets the client name and version number. That
// information will appear in httpd access logs.
// I've added comments about how Connect works in places where it seemed
// pretty obscure.
//
// Revision 1.108  2001/02/05 18:57:44  jgarcia
// Added support so a Connect object can be created with credentials to be
// able to resolve challenges issued by web servers (Basic Authentication).
// Added exception to notify "No Authorization".
//
// Revision 1.107  2001/01/26 19:48:09  jimg
// Merged with release-3-2-3.
//
// Revision 1.105.2.2  2000/12/06 18:47:35  jimg
// Fixed processing of the .dodsrc file. Comments were not ignored, instead
// they caused the file reader to stop. This meant that comments at the top
// of the file prevented any of the parameters from being read.
//
// Revision 1.106  2000/11/25 00:44:47  jgarcia
// In Connect::read_url added an exception in case there is a fatal error
// loading an URL. It is required for supporting Connect objects that support
// HTTP Authorization.
//
// Revision 1.105.2.1  2000/11/22 05:35:09  brent
// allow username/password in URL for secure data sets
//
// Revision 1.105  2000/10/30 17:21:27  jimg
// Added support for proxy servers (from cjm).
//
// Revision 1.104  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.103  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.102  2000/08/29 21:22:54  jimg
// Merged with 3.1.9
//
// Revision 1.80.2.4  2000/08/02 23:18:38  jimg
// Fixed a bug that shows up on Linux (2.2, maybe others) where URLs with 61
// 65 characters hang. See fetch_url() for the gory details.
//
// Revision 1.101  2000/07/26 12:24:01  rmorris
// Modified intermediate (dod*) file removal under win32 to take into account
// a 1-to-n correspondence between connect objects and intermediate files.
// Implemented solution through vector of strings containing the intermediate
// filenames that are removed when the connect obj's destructor is invoked.
// Might consider using the same code for unix in the future.  Previous
// win32 solution incorrectly assumed the correspondence was 1-to-1.
//
// Revision 1.100  2000/07/24 18:49:50  rmorris
// Just added a notation that indicates what was tried to get around
// libwww bugs in regards to spaces in pathnames.  Client-side caching
// disabled until further notice - next version of libwww may help.
//
// Revision 1.99  2000/07/21 14:26:24  rmorris
// Remove client-side caching entired under win32 in lieu of a permanent
// fix (soon).  Fixed what I broke for client-side caching under unix.
//
// Revision 1.98  2000/07/18 12:49:04  rmorris
// Fixed failure to initialize a structure element appropriately when
// retrieving the Win32 OS version information for the WIN95_CACHE_HACK.
//
// Revision 1.96  2000/07/18 03:56:09  rmorris
// Changes made in an attempt to debug client-side caching under win95-based
// systems.  Is currently unsuccessful, but these changes made the code somewhat
// more generic.
//
// Revision 1.95  2000/07/13 07:09:05  rmorris
// Changed the approach to delete the intermediate file in the case
// of win32 (unlink() not the same under win32, needed another approach).
//
// Revision 1.94  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.93  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.92.4.2  2000/06/02 22:29:21  rmorris
// Fixed bug in bug fix that allowed spaces in paths via escape sequences.
// The bug within a bug was that we were translating a file path into a
// url by prepending it with "file:/".  This allows libwww to recognize
// escape sequence.  Under UNIX, "file:" is correct, not "file:/"
//
// Revision 1.92.4.1  2000/06/02 18:14:43  rmorris
// Mod for port to win32.
//
// Revision 1.92  2000/04/17 22:13:37  jimg
// Fixed problems with the _gui member and local connections. The _gui object
// was not initialized (correct) for local connections but *was* destroyed for
// them (because the code never checked for local connections in the dtor).
//
// Revision 1.91  2000/04/17 21:25:00  jimg
// Fixed an error where local connections affected the status of the
// __num_remote_conns field. This caused remote connections, made after one or
// more local connections were closed, to fail because libwww was not
// initialized correctly.
//
// Revision 1.90  2000/04/07 00:19:04  jimg
// Merged Brent's changes for the progress gui - he added a cancel button.
// Also repaired the last of the #ifdef Gui bugs so that we can build Gui
// and non-gui versions of the library that use one set of header files.
//
// Revision 1.89.2.1  2000/04/04 05:00:24  brent
// put a Cancel button the Tcl/Tk GUI
//
// Revision 1.89  2000/03/28 16:18:17  jimg
// Added a DEFAULT_EXPIRES parameter to the .dodsrc file. The default
// expiration time is now set by connect, using the value read from .dodsrc,
// rather than use the value compiled into libwww. To do this I added a new
// function to HTCache.c,h in libwww.
//
// Revision 1.88  2000/03/17 00:11:39  jimg
// I fixed the bug in libwww which made caching of compressed documents fail.
// I removed the hacks in this file that prevented data documents from being
// cached. I also have removed the code that wrote NEVER_DEFLATE to the rc
// file. The NEVER_DEFLATE option still works; I'm just not including it in
// the rc file by default.
//
// Revision 1.86.2.1  2000/02/17 05:03:12  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.87  2000/01/27 06:29:55  jimg
// Resolved conflicts from merge with release-3-1-4
//
// Revision 1.80.2.3  2000/01/26 23:55:50  jimg
// Fixed the return type of string::find.
//
// Revision 1.86  1999/12/31 00:55:10  jimg
// Fixed up the progress indicator
//
// Revision 1.85  1999/12/15 01:14:10  jimg
// More fixes for caching. Caching now works correctly for programs that use
// multiple Connect objects. The Cache index is now updated more frequently.
//
// Revision 1.84  1999/12/01 21:27:05  jimg
// Substantial changes for the caching software. Added a call to `terminate'
// the cache once we're done with the libwww code. This writes the .index
// file required by the cache. Additionally, changed the cache mode from
// validate to OK. The later forces the cache to not validate every request.
// Instead expiration is used and the libwww code implements a fall back in
// those cases where servers don't supply a Date header. Finally, compressed
// responses break the cache (I think this is libwww's bug) and I've disabled
// caching compressed data responses. So that users can cache data responses,
// I've added a new flag in the dodsrc file called NEVER_DEFLATE which allows
// users to override the clients wishes regarding compression (i.e., users
// can turn it off). Data responses can thus be cached.
//
// Revision 1.83  1999/10/22 04:17:25  cjm
// Added support for caching.  Most of the code is in www_lib_init(), there
// is also a modification to read_url() to make use of the cache if it is
// enabled. 
//
// Revision 1.82  1999/09/03 22:07:44  jimg
// Merged changes from release-3-1-1
//
// Revision 1.80.2.2  1999/08/28 06:43:03  jimg
// Fixed the implementation/interface pragmas and misc comments
//
// Revision 1.81  1999/08/23 18:57:44  jimg
// Merged changes from release 3.1.0
//
// Revision 1.80.2.1  1999/08/09 22:57:49  jimg
// Removed GUI code; reactivate by defining GUI
//
// Revision 1.80  1999/08/09 18:27:33  jimg
// Merged changes from Brent for the Gui code (progress indicator)
//
// Revision 1.78.4.2  1999/07/29 05:46:17  brent
// call Tcl / GUI directly from Gui.cc, abandon expect, and consolidate Tcl
// files.
//
// Revision 1.79  1999/07/22 17:11:50  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.78.4.1  1999/06/01 15:40:54  jimg
// Ripped out dead wood in parse_mime(...).
//
// Revision 1.78  1999/05/26 17:30:24  jimg
// Added the calls to Error::correct_error(...). These were removed because
// they use the Gui object. However, they access it through the Connect::Gui()
// member function which will return NULL until we fix the Gui. Calling the
// correct_error with a NULL Gui object works in that the error message is
// displayed on stderr and the Gui stuff is ignored.
//
// Revision 1.77  1999/05/21 20:39:23  dan
// Disabled the Gui interface in the Connect objects.  Primarily
// this was done in www_libc_init, constructor, destructor, and request_
// calls using 'ifdef GUI' directives.  To regain use of this code
// use the '-DGUI' compiler flag.
//
// Revision 1.76  1999/05/21 17:22:04  jimg
// Removed debugging instrumentation left in by accident.
//
// Revision 1.75  1999/05/21 00:46:42  jimg
// Using ifstream in parse_mime(...) confuses the downstream parser since the
// FILE * is not advanced. I switched back to the fgets(...) code and the some
// problems with the asciival client went away.
//
// Revision 1.74  1999/04/29 03:04:51  jimg
// Merged ferret changes
//
// Revision 1.73  1999/04/29 02:29:27  jimg
// Merge of no-gnu branch
//
// Revision 1.72.8.1  1999/04/14 22:31:36  jimg
// Removed old code.
// Fixed the delete of member _tv. timeval _tv was used by libwww 5.0 but is no
// longer needed. I wrapped all code that touched this in #ifdef LIBWWW_5_0 and
// removed the member from Connect using the same conditional. This fixes a
// problem where _tv is deleted without being allocated when local files are
// accessed.
//
// Revision 1.72  1999/02/23 01:32:59  jimg
// Removed more of the code in process_data. Because of fixes in the scanner,
// this code no longer needs to rewind after parsing the DDS of a data
// document. The scanner no longer reads into the binary doc. Note that real
// MP/MIME code would solve this by giving us two streams to work with. Some
// day...
//
// Revision 1.71  1999/02/18 19:21:40  jimg
// Added support for the DODS experimental MIME header XDODS-Accept-Types.
// This will be used to send a lists of `accepted types' from the client to a
// server. The list tells a server which datatypes the requesting client can
// understand. This information may be used by both the DDS and DataDDS
// objects to trigger translations from one type to another.
//
// Revision 1.66.4.2  1999/02/05 09:32:33  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code.
//
// Revision 1.66.4.1  1999/02/02 21:56:56  jimg
// String to string version
//
// Revision 1.70  1999/01/15 17:07:01  jimg
// Removed use of the move_dds() member function. The DDS parser now
// recognizes the `Data:' separator string as marking the end of the DDS part
// of a data document. This means that Connect no longer needs to copy the
// DDS part of the data document to a separate (temporary) text file before
// parsing it.
//
// Revision 1.69  1998/12/16 19:10:53  jimg
// Added support for XDODS-Server MIME header. This fixes a problem where our
// use of Server clashed with Java
//
// Revision 1.68  1998/11/10 01:08:34  jimg
// Patched memory leaks found with Purify.
//
// Revision 1.67  1998/09/08 22:27:11  jimg
// Removed PERF macro.
//
// Revision 1.66  1998/06/04 06:29:11  jimg
// Added two new member functions to set/get the new www_errors_to_stderr
// property. This controls whether www errors (like host not found) are
// reported on stderr in addition to the Error object. The default is to NOT
// report them to stderr. WWW errors are now recorded in the Error object.
//
// Revision 1.65  1998/04/07 22:14:31  jimg
// Added a call to prune_spaces to the default ctor. Removing spaces prevents
// various crashes. Note that CEs can themselves contain spaces but *leading*
// spaces caused problems.
//
// Revision 1.64  1998/04/03 17:39:07  jimg
// Fixed a bug in process_data where sequences were not handled properly. Patch
// from Jake Hamby.
//
// Revision 1.63  1998/03/26 00:19:24  jimg
// Changed from converters to the _conv member in www_lib_init.
//
// Revision 1.62  1998/03/19 23:48:24  jimg
// Removed old code associated with the (bogus) caching scheme.
// Removed the _connects field.
// Used _conv as a flag to ensure the www library is intialized only once.
//
// Revision 1.61  1998/02/11 21:56:20  jimg
// Mayor modifications for libwww 5.1 compression support. I removed lots of
// old code that was superfluous and changed the way the library is initialized
// to make that more efficient.
// Removed the old Semaphore hacks.
// Removed the content-encoding handler (that is now done by libwww).
// The Ctor and www_lib_init now take a flag that controls whether the server
// is told that the client can decompress data. Note that this does not mean
// data *will* be compressed, just that the client can process it if it is.
//
// Revision 1.60  1998/02/05 20:13:50  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.59  1997/12/16 00:40:07  jimg
// Fixed what may have been a lingering problem with version number strings
// in the server_handler() function.
// Added initialization of _server to `dods/0.0' in ctor. Thus _server will
// have a value even for older servers that don't support server version
// numbers.
//
// Revision 1.58  1997/09/22 23:06:52  jimg
// Changed code so that the new DataDDS objects are used/created when
// accessing data.
//
// Revision 1.57  1997/06/06 17:52:08  jimg
// Last minute changes for version 2.14
//
// Revision 1.56  1997/06/06 03:09:15  jimg
// Added parse_mime(); a tiny parser for the fields DODS uses. Not a real MIME
// parser.
// Added process_data to facilitate processing data from stdin.
// Modified request_data() member function so that it works when reading from
// stdin.
// Fixed a bug in the destructor where _output and _source were not set to
// null.
//
// Revision 1.55  1997/05/13 23:36:38  jimg
// Added calls to close_output() in the dtor. This ensures that all files
// will be closed when an Connect is destroyed.
//
// Revision 1.54  1997/05/07 22:10:37  jimg
// Fixed a bug where the last decompressor process exit was not handled
// properly. The fix was to make sure that calls for data interleaved
// with calls for the DAS and/or DDS objects properly wait for the exit
// status of the last decompression process. In addition, ensure that
// Connect's dtor waits for the exit status of the very last
// decompression process.
//
// Revision 1.53  1997/03/23 19:39:21  jimg
// Added temporary fix for decompression bug. When decompressing `zombie'
// processes were created which would fill the system's process table. The fix
// explicitly catches the exit of child processes. This code can be removed
// when/if we switch to version 5.1 of the WWW library.
//
// Revision 1.52  1997/03/05 08:24:33  jimg
// Fixed the logfile bug; when linking with ld or cc static objects are not
// initialized. The _logfile member was a static global object and caused core
// dumps when it was not initialized.
//
// Revision 1.51  1997/02/19 02:11:34  jimg
// Fixed ctors (removed lame error object initializers).
//
// Revision 1.50  1997/02/17 20:21:44  jimg
// Fixed a bug in the ctors introduced by making Error's ctor private.
//
// Revision 1.49  1997/02/13 17:33:11  jimg
// Added MIME header `handler' for the server header.
// Added mfuncs to access the server information (which in DODS is the version
// number of the core software).
//
// Revision 1.48  1997/02/13 05:49:53  reza
// Fixed concatenation of _proj and _sel members into request_das and
// request_dds URLs.
//
// Revision 1.47  1997/02/12 21:42:29  jimg
// Fixed handling of non fatal errors reported by the www library. Now an
// optional parameter to the class ctor enables display of these informational
// messages. However, they are not registered as `web_errors' in the
// type field of the Connect object.
// Fixed a bug which introduced extraneous '?'s into URLs.
//
// Revision 1.46  1997/02/10 02:27:10  jimg
// Fixed processing of error returns.
// Changed return type of request_data() (and related functions) from DDS & to
// DDS *. The member function now return NULL if an error is detected.
//
// Revision 1.45  1997/02/04 22:44:31  jimg
// Fixed bugs in URL() and CE() where the _URL, _proj and _sel members were
// misused.
//
// Revision 1.44  1997/01/28 17:15:19  jimg
// Wrapped the generic header_handler() in DBG() so that it is only used
// while debugging.
//
// Revision 1.43  1996/12/18 19:17:20  jimg
// Removed the DODS_PERF define.
//
// Revision 1.42  1996/12/02 23:10:10  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.41  1996/11/25 03:37:34  jimg
// Added USE_SEM control macro - since asynchronous connects are massively
// broken the semaphores are not used.
// The _connects field is not initialized to -1 and handled so that
// HTLibTerminate() is never called but so that the web library is initialized
// before the first use of connect. Apparently calling HTLibInit() more than
// once (even with an interleaving call to HTLibTerminate()) breaks the 5.0a
// version of the library.
// Added the constant web_error the the set of content-descriptions.
// Removed use of the disk cache. Added it in later; read_url() must be
// modified.
// Never use the broken asynchronous code.
// Fixed processing of content-description so that web-error and dods-error
// messages are handled correctly.
//
// Revision 1.40  1996/11/22 00:14:38  jimg
// Removed decompress() function.
// Switched to decompressor function in util.cc
// Wrapped semaphore code in USE_SEM preprocessor define - the semaphore code
// may not be necessary...
//
// Revision 1.39  1996/11/20 22:29:29  jimg
// Fixed header parsing. Now I use my own header parsers for the
// content-description and -encoding headers. Once the values of these headers
// have been stored in the Connect object it is easy to operate on the data
// stream. This is simpler than using libwww's stream stack (at least for
// decompression and error document routing).
//
// Revision 1.38  1996/11/20 00:55:29  jimg
// Fixed a bug with HTLibTerminate() where multiple URLs caused a core dump.
// Fixed the progress indicator.
// Ripped out the semaphore stuff used with the asynchronous connects - those
// connects now work, at least on the Sun.
//
// Revision 1.37  1996/11/13 18:53:00  jimg
// Updated so that this now works with version 5.0a of the WWW library from
// the W3c.
// Fixed handling of certain types of http/www errors.
//
// Revision 1.36  1996/10/18 16:40:09  jimg
// Changed request_das() and request_dds() so that they now pass any initial
// constraint to the DAS and DDS servers.
//
// Revision 1.35  1996/10/08 16:27:47  jimg
// Added code so that a constraint expression appended to a URL is properly
// handled. The CE is stored in in the Connect object. When a CE is passed to
// the request_data member function, Connect correctly combines the projection
// and selection parts of that CE with the matching parts of any initial CE.
//
// Revision 1.34  1996/09/18 23:06:28  jimg
// Fixed a bug in the dtor which caused the _anchor WWW lib object to be
// deleted twice under some (likely) conditions. The efence library found the
// error. 
//
// Revision 1.33  1996/08/26 21:12:52  jimg
// Changes for version 2.07
//
// Revision 1.32  1996/08/13 17:53:34  jimg
// Corrected misuse of the istrstream class; added calls to the freeze member
// function where needed.
// Fixed test for URL -vs- filename in the object ctor.
//
// Revision 1.31  1996/07/17 22:27:20  jimg
// Removed copy of the _output FILE * and bad logic on output() member
// function. Added reset of _source to 0 in close_output().
//
// Revision 1.30  1996/07/10 21:25:32  jimg
// *** empty log message ***
//
// Revision 1.29  1996/06/22 00:00:23  jimg
// Added Gui pointer to the Error oject's correct_error mfunc call.
//
// Revision 1.28  1996/06/21 23:15:03  jimg
// Removed GUI code to a new class - Gui.
//
// Revision 1.27  1996/06/20 15:59:24  jimg
// Added conditional definition of union semun {};
//
// Revision 1.26  1996/06/18 23:43:42  jimg
// Added support for a GUI. The GUI is actually contained in a separate program
// that is run in a subprocess. The core `talks' to the GUI using a pty and a
// simple command language.
// Removed GZIP preprocessor define and added DODS_ROOT define. Added checks in
// the code to use the environment variable DODS_ROOT in preference to the
// compile-time value (if non-null).
//
// Revision 1.25  1996/06/08 00:08:47  jimg
// Fixed comments.
//
// Revision 1.24  1996/06/08 00:07:19  jimg
// Added support for compression. The Content-Encoding header is used to
// determine if the incoming document is compressed (values: x-plain; no
// compression, x-gzip; gzip compression). The gzip program is used to
// decompress the document. The new software uses UNIX IPC and a separate
// subprocess to perform the decompression.
//
// revision 1.23  1996/06/06 17:07:57  jimg
// Added support for wwwlib 4.0.
// Added support for object types.
//
// revision 1.22  1996/06/04 21:33:15  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// revision 1.21  1996/05/31 23:29:30  jimg
// Updated copyright notice.
//
// Revision 1.20  1996/05/29 21:47:51  jimg
// Added Content-Description header parsing.
// Removed Event loop code (HTEvent_loop()).
// Fixed bug where a copy of _OUTPUT was created using _OUTPUT's file
// descriptor. When _OUTPUT was closed the copy no longer referenced a valid
// data source.
// Fixed problems with asserts and error messaging.
//
// Revision 1.19  1996/05/22 18:05:04  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.18  1996/05/21 23:46:32  jimg
// Added support for URLs directly to the class. This uses version 4.0D of
// the WWW library from W3C.
//
// Revision 1.17  1996/04/05 01:25:39  jimg
// Merged changes from version 1.1.1.
//
// Revision 1.16  1996/03/05 23:21:27  jimg
// Added const to char * parameters and function prototypes.
//
// Revision 1.14.2.3  1996/03/01 00:07:57  jimg
// Removed bad attempt at multiple connect implementation.
//
// Revision 1.14.2.2  1996/02/23 22:51:00  jimg
// Added const in prototype of netio files.
// Added libraries for solaris 2.x
//
// Revision 1.2  1994/10/05  20:23:26  jimg
// Fixed errors in *.h files comments - CVS bites again.
// Changed request_{das,dds} so that they use the field `_api_name'
// instead of requiring callers to pass the api name.
//
// Revision 1.15  1996/02/01 21:43:51  jimg
// Added mfuncs to maintain a list of DDSs and the constraint expressions
// that produced them.
// Added code in request_data to strip the incoming DDS from a data
// document.
// Fixed up bogus comments.
//
//
