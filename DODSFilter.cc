
// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation of the DODSFilter class. This class is used to build dods
// filter programs which, along with a CGI program, comprise DODS servers.
// jhrg 8/26/97

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: DODSFilter.cc,v 1.25 2001/08/24 17:46:22 jimg Exp $"};

#include <iostream>
#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif
#include <string>
#include <GetOpt.h>

#include "DAS.h"
#include "DDS.h"
#include "debug.h"
#include "cgi_util.h"
#include "escaping.h"
#include "DODSFilter.h"
#include "InternalErr.h"

using std::endl;
using std::ends;
using std::ostrstream;

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
    else
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

bool
DODSFilter::OK()
{

    return !bad_options;
}

bool
DODSFilter::version()
{
    return ver;
}

bool
DODSFilter::is_conditional()
{
    return d_conditional_request;
}

void
DODSFilter::set_cgi_version(string version)
{
    cgi_ver = version;
}

string
DODSFilter::get_cgi_version()
{
    return cgi_ver;
}

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

string
DODSFilter::get_dataset_version()
{
    return "";
}

time_t
DODSFilter::get_dataset_last_modified_time()
{
    return last_modified_time(dataset);
}

time_t
DODSFilter::get_das_last_modified_time(const string &anc_location)
{
    string name = find_ancillary_file(dataset, "das", anc_location, anc_file);
    return max((name != "") ? last_modified_time(name) : 0,
	       get_dataset_last_modified_time()); 
}

time_t
DODSFilter::get_dds_last_modified_time(const string &anc_location)
{
    string name = find_ancillary_file(dataset, "dds", anc_location, anc_file);
    return max((name != "") ? last_modified_time(name) : 0,
	       get_dataset_last_modified_time()); 
}

time_t
DODSFilter::get_data_last_modified_time(const string &anc_location)
{
    string dds_name = find_ancillary_file(dataset, "dds", anc_location,
					  anc_file);
    string das_name = find_ancillary_file(dataset, "dds", anc_location,
					  anc_file);
    time_t m = max((das_name != "") ? last_modified_time(das_name) : (time_t)0,
		   (dds_name != "") ? last_modified_time(dds_name) : (time_t)0);
    // Note that this is a call to get_dataset_... not get_data_...
    time_t n = get_dataset_last_modified_time();
    // g++ did not compile `max(max(x,y),z)' 4/23/2001 jhrg
    return max(m, n); 
}

time_t
DODSFilter::get_request_if_modified_since()
{
    return d_if_modified_since;
}

string
DODSFilter::get_cache_dir()
{
  return cache_dir;
}

string
DODSFilter::get_accept_types()
{
    return accept_types;
}

void
DODSFilter::read_ancillary_das(DAS &das, string anc_location)
{
    string msg = "Parse error in external file " + dataset + ".das";
  
    if ( anc_location == "" ) anc_location = anc_dir;

    string name = find_ancillary_file(dataset, "das", anc_location, anc_file);
    FILE *in = fopen(name.c_str(), "r");
    // Jose Garcia
    // If while calling DAS::parse we get an exception of type 
    // InternalErr we proceed to execute the code that logs the error
    // into the httpd log plus let the client know that
    // we have failed because of an internal error, at least it is 
    // a clean shutdown.
    // If the exception is of type Error, we let the client know that
    // it is a user error so in this case set_mime_text will 
    // have ObjectType as dods_error.
    // No matter the kind of exception we rethrow it so the initial 
    // caller of DAS::read_ancillary_das should get it and decide how to
    // terminate. 
    //
    // I removed the catch(). Callers are responsible for doing something
    // sensible with the Error objects (such as writing to system logs and
    // sending the error message back to the caller). 7/11/2001 jhrg
    if (in)
	das.parse(in);
}

void
DODSFilter::read_ancillary_dds(DDS &dds, string anc_location)
{
    string msg = "Parse error in external file " + dataset + ".dds";
    if ( anc_location == "" ) anc_location = anc_dir;

    string name = find_ancillary_file(dataset, "dds", anc_location, anc_file);
    FILE *in = fopen(name.c_str(), "r");
    
    // Jose Garcia
    // Same comments for DAS::read_ancillary_das apply here.
    if (in)
	dds.parse(in);
}

static const char *emessage = \
"DODS internal server error; usage error. Please report this to the dataset \
maintainer, or to support@unidata.ucar.edu.";

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

void 
DODSFilter::send_version_info()
{
    cout << "HTTP/1.0 200 OK" << endl
	 << "XDODS-Server: " << cgi_ver << endl
	 << "Content-Type: text/plain" << endl
	 << endl;

    cout << "DODS server core software: " << DVR << endl;

    if (cgi_ver != "")
	cout << "Server vision: " << cgi_ver << endl;

    string v = get_dataset_version();
    if (v != "")
	cout << "Dataset version: " << v << endl;
}

// I've written a few unit tests for this method (see DODSFilterTest.cc) but
// it's very hard to test well. 5/1/2001 jhrg
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
DODSFilter::send_das(DAS &das, const string &anc_location)
{
    send_das(cout, das, anc_location);
}

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
DODSFilter::send_dds(DDS &dds, bool constrained, const string &anc_location)
{
    send_dds(cout, dds, constrained, anc_location);
}

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

