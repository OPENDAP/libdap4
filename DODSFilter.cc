
// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation of the DODSFilter class. This class is used to build dods
// filter programs which, along with a CGI program, comprise DODS servers.
// jhrg 8/26/97

// $Log: DODSFilter.cc,v $
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: DODSFilter.cc,v 1.19 2000/06/07 19:33:21 jimg Exp $"};

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
#include "DODSFilter.h"

DODSFilter::DODSFilter(int argc, char *argv[]) : comp(false), ver(false), 
    bad_options(false), dataset(""), ce(""), cgi_ver("dods/3.0"),
    anc_dir(""), anc_file(""), cache_dir(""), accept_types("All")
{
    program_name = argv[0];

    int option_char;
    GetOpt getopt (argc, argv, "ce:v:Vd:f:r:t:");

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
	  default: bad_options = true; break;
	}

    int next_arg = getopt.optind;
    if(next_arg < argc)
	dataset = argv[next_arg];
    else
	bad_options = true;

    DBG(cerr << "comp: " << comp << endl);
    DBG(cerr << "ce: " << ce << endl);
    DBG(cerr << "cgi_ver: " << cgi_ver << endl);
    DBG(cerr << "ver: " << ver << endl);
    DBG(cerr << "anc_dir: " << anc_dir << endl);
    DBG(cerr << "anc_file: " << anc_file << endl);
    DBG(cerr << "cache_dir: " << cache_dir << endl);
    DBG(cerr << "accept_types: " << accept_types << endl);
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
  ce = _ce;
}

string
DODSFilter::get_dataset_name()
{
    return dataset;
}

string
DODSFilter::get_dataset_version()
{
    return "";
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

bool
DODSFilter::read_ancillary_das(DAS &das, string anc_location)
{
    if ( anc_location == "" ) anc_location = anc_dir;

    string name = find_ancillary_file(dataset, "das", anc_location, anc_file);
    FILE *in = fopen(name.c_str(), "r");
 
    if (in) {
	int status = das.parse(in);
	fclose(in);
    
	if(!status) {
	    string msg = "Parse error in external file " + dataset + ".das";

	    // server error message
	    ErrMsgT(msg);

	    // client error message
	    set_mime_text(cout, dods_error, cgi_ver);
	    Error e(malformed_expr, msg);
	    e.print(cout);

	    return false;
	}
    }

    return true;
}

bool
DODSFilter::read_ancillary_dds(DDS &dds, string anc_location)
{
    if ( anc_location == "" ) anc_location = anc_dir;

    string name = find_ancillary_file(dataset, "dds", anc_location, anc_file);
    FILE *in = fopen(name.c_str(), "r");
 
    if (in) {
	int status = dds.parse(in);
	fclose(in);
    
	if(!status) {
	    string msg = "Parse error in external file " + dataset + ".dds";

	    // server error message
	    ErrMsgT(msg);

	    // client error message
	    set_mime_text(cout, dods_error, cgi_ver);
	    Error e(malformed_expr, msg);
	    e.print(cout);

	    return false;
	}
    }

    return true;
}

static const char *emessage = \
"DODS internal server error. Please report this to the dataset maintainer, \
or to support@unidata.ucar.edu.";

void 
DODSFilter::print_usage()
{
    // Write a message to the WWW server error log file.
    ostrstream oss;
    oss << "Usage: " << program_name
	<< " [-c] [-v <cgi version>] [-e <ce>]"
	<< " [-d <ancillary file directory>] [-f <ancillary file name>]"
	<< " <dataset>" << ends;
    ErrMsgT(oss.str());
    oss.rdbuf()->freeze(0);

    // Build an error object to return to the user.
    Error e(unknown_error, emessage);
    set_mime_text(cout, dods_error, cgi_ver);
    e.print(cout);
}

void 
DODSFilter::send_version_info()
{
    cout << "HTTP/1.0 200 OK" << endl
	 << "XDODS-Server: " << cgi_ver << endl
	 << "Content-Type: text/plain" << endl
	 << endl;
    
    cout << "Core version: " << DVR << endl;

    if (cgi_ver != "")
	cout << "Server vision: " << cgi_ver << endl;

    string v = get_dataset_version();
    if (v != "")
	cout << "Dataset version: " << v << endl;
}

bool
DODSFilter::send_das(DAS &das)
{
    set_mime_text(cout, dods_das, cgi_ver);
    das.print(cout);

    return true;
}

bool
DODSFilter::send_dds(DDS &dds, bool constrained)
{
    if (constrained) {
	if (!dds.parse_constraint(ce, cout, true)) {
	    string m = program_name + ": parse error in constraint: " 
		+  ce;
	    ErrMsgT(m);
	    
	    set_mime_text(cout, dods_error, cgi_ver);
	    Error e(unknown_error, m);
	    e.print(cout);

	    return false;
	}
	set_mime_text(cout, dods_dds, cgi_ver);
	dds.print_constrained(cout);  // send constrained DDS    
    }
    else {
	set_mime_text(cout, dods_dds, cgi_ver);
	dds.print(cout);
    }

    return true;
}

bool
DODSFilter::send_data(DDS &dds, FILE *data_stream)
{
    bool compress = comp && deflate_exists();

    // This catch is a quick & dirty hack for exceptions thrown by the new
    // (11/6/98) projection functions in CEs. I might add exceptions to other
    // parts of the CE parser and evaluator. Eventually, the C++ classes
    // should switch to exceptions. 11/6/98 jhrg
    //
    // I'm not sure we should catch errors from dds.send and its callees
    // here. I think they should be caught in the outer layer (e.g.,
    // ff_dods). 5/26/99 jhrg
    try {
	if (!dds.send(dataset, ce, data_stream, compress, cgi_ver)) {
	    ErrMsgT((compress) ? "Could not send compressed data" : 
		    "Could not send data");
	    return false;
	}
    }
    catch (Error &e) {
	ErrMsgT((compress) ? "Could not send compressed data" : 
		"Could not send data");
	set_mime_text(cout, dods_error, cgi_ver);
	e.print(cout);

	return false;
    }
	
    return true;
}













