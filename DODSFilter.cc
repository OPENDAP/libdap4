
// (c) COPYRIGHT URI/MIT 1997
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation of the DODSFilter class. This class is used to build dods
// filter programs which, along with a CGI program, comprise DODS servers.
// jhrg 8/26/97

// $Log: DODSFilter.cc,v $
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
#pragma "implemenation"
#endif

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: DODSFilter.cc,v 1.3 1998/03/19 23:34:21 jimg Exp $"};

#include <iostream.h>
#include <strstream.h>
#include <String.h>
#include <GetOpt.h>

#include "DAS.h"
#include "DDS.h"
#include "debug.h"
#include "cgi_util.h"
#include "DODSFilter.h"

DODSFilter::DODSFilter(int argc,char *argv[]) : comp(false), ver(false), 
    bad_options(false), dataset(""), ce(""), cgi_ver(""),
    anc_dir(""), anc_file("")
{
    program_name = argv[0];

    int option_char;
    GetOpt getopt (argc, argv, "ce:v:d:f:");

    while ((option_char = getopt()) != EOF)
	switch (option_char) {
	  case 'c': comp = true; break;
	  case 'e': ce = getopt.optarg; break;
	  case 'v': ver = true; cgi_ver = getopt.optarg; break;
	  case 'd': anc_dir = getopt.optarg; break;
	  case 'f': anc_file = getopt.optarg; break;
	  default: bad_options = true; break;
	}

    int next_arg = getopt.optind;
    if(next_arg < argc)
	dataset = argv[next_arg];
    else
	bad_options = true;
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

String
DODSFilter::get_ce()
{
    return ce;
}

String
DODSFilter::get_dataset_name()
{
    return dataset;
}

String
DODSFilter::get_dataset_version()
{
    return "";
}

bool
DODSFilter::read_ancillary_das(DAS &das)
{
    String name = find_ancillary_file(dataset, "das", anc_dir, anc_file);
    FILE *in = fopen((const char *)name, "r");
 
    if (in) {
	int status = das.parse(in);
	fclose(in);
    
	if(!status) {
	    String msg = "Parse error in external file " + dataset + ".das";

	    // server error message
	    ErrMsgT(msg);

	    // client error message
	    set_mime_text(cout, dods_error);
	    Error e(malformed_expr, msg);
	    e.print(cout);

	    return false;
	}
    }

    return true;
}

bool
DODSFilter::read_ancillary_dds(DDS &dds)
{
    String name = find_ancillary_file(dataset, "dds", anc_dir, anc_file);
    FILE *in = fopen((const char *)name, "r");
 
    if (in) {
	int status = dds.parse(in);
	fclose(in);
    
	if(!status) {
	    String msg = "Parse error in external file " + dataset + ".dds";

	    // server error message
	    ErrMsgT(msg);

	    // client error message
	    set_mime_text(cout, dods_error);
	    Error e(malformed_expr, msg);
	    e.print(cout);

	    return false;
	}
    }

    return true;
}

static const char *emessage = \
"DODS internal server error. Please report this to the dataset maintainer, \
or to support@dods.gso.uri.edu.";

void 
DODSFilter::print_usage()
{
    // Write a message to the WWW server error log file.
    ostrstream oss;
    oss << "Usage: " << program_name
	<< " [-c] [-v <cgi version>] [-e <ce>]"
	<< " [-d <ancillary file directory>] [-f <ancillary file name>]"
	<< " <dataset>";
    ErrMsgT(oss.str());
    oss.freeze(0);

    // Build an error object to return to the user.
    Error e(unknown_error, emessage);
    set_mime_text(cout, dods_error);
    e.print(cout);
}

void 
DODSFilter::send_version_info()
{
    cout << "HTTP/1.0 200 OK" << endl
	 << "Server: " << DVR << endl
	 << "Content-Type: text/plain" << endl
	 << endl;
    
    cout << "Core software version: " << DVR << endl;

    if (cgi_ver != "")
	cout << "Server Script Revision: " << cgi_ver << endl;

    String v = get_dataset_version();
    if (v != "")
	cout << "Dataset version: " << v << endl;
}

bool
DODSFilter::send_das(DAS &das)
{
    set_mime_text(cout, dods_das);
    das.print(cout);

    return true;
}

bool
DODSFilter::send_dds(DDS &dds, bool constrained)
{
    if (constrained) {
	if (!dds.parse_constraint(ce, cout, true)) {
	    String m = program_name + ": parse error in constraint: " 
		+  ce;
	    ErrMsgT(m);
	    
	    set_mime_text(cout, dods_error);
	    Error e(unknown_error, m);
	    e.print(cout);

	    return false;
	}
	set_mime_text(cout, dods_dds);
	dds.print_constrained(cout);  // send constrained DDS    
    }
    else {
	set_mime_text(cout, dods_dds);
	dds.print(cout);
    }

    return true;
}

bool
DODSFilter::send_data(DDS &dds, FILE *data_stream)
{
    bool compress = comp && deflate_exists();

    if (!dds.send(dataset, ce, data_stream, compress)) {
	ErrMsgT((compress) ? "Could not send compressed data" : "Could not send data");
	return false;
    }

    return true;
}

