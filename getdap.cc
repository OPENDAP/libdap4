
// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// This is the source to `geturl'; a simple tool to exercise the Connect
// class. It can be used to get naked URLs as well as the DODS DAS and DDS
// objects.  jhrg.

// $Log: getdap.cc,v $
// Revision 1.43  2000/07/18 03:43:42  rmorris
// Change to set stdout to binary mode under win32 so that the output of geturl
// yields the exact same thing as it does Unix - i.e., no cr-nl translation
// of nl's within the output.
//
// Revision 1.42  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.41  2000/06/07 18:07:01  jimg
// Merged the pc port branch
//
// Revision 1.40.10.1  2000/06/02 18:39:03  rmorris
// Mod's for port to win32.
//
// Revision 1.40  1999/08/23 18:57:46  jimg
// Merged changes from release 3.1.0
//
// Revision 1.39.2.1  1999/08/09 22:57:51  jimg
// Removed GUI code; reactivate by defining GUI
//
// Revision 1.39  1999/08/09 18:27:35  jimg
// Merged changes from Brent for the Gui code (progress indicator)
//
// Revision 1.38  1999/07/22 17:11:52  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.37.4.2  1999/06/11 10:21:50  rmorris
// Removed a couple of debugging lines accidentally returned to cvs
//
// Revision 1.37.4.1  1999/06/04 22:34:36  rmorris
// Fixed a line that was causing core dumps during the generation and running
// of the hdf server test suites.  It looks like there was a simple cut-and-paste
// error made by someone earlier involving the outputting of Structures - they
// were being treated like sequences.
//
// Revision 1.37  1999/05/26 17:35:01  jimg
// The Gui option is now disabled; gui is always false.
// One call to gui()->... has been removed with #if 0 ... #endif.
//
// Revision 1.36  1999/04/29 02:29:36  jimg
// Merge of no-gnu branch
//
// Revision 1.35  1999/04/22 22:30:52  jimg
// Uses dynamic_cast
//
// Revision 1.34  1999/02/22 22:45:10  jimg
// Added -T option: Use this to send a list of accepted types to the server
// using the new XDODS-Accept-Types request header. The list must not contain
// spaces *even* if it is quoted.
//
// Revision 1.33  1998/11/10 00:47:53  jimg
// Delete the dds object (created with new) after calling process_data().
//
// Revision 1.32  1998/09/08 22:23:51  jimg
// Removed PERF macro calls.
//
// Revision 1.31.6.2  1999/02/05 09:32:36  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.31.6.1  1999/02/02 21:57:08  jimg
// String to string version
//
// Revision 1.31  1998/04/03 17:46:04  jimg
// Patch from Jake Hamby; fixed bug where Structures which contained sequences
// did not print properly.
//
// Revision 1.30  1998/03/20 00:23:25  jimg
// Improved the error message reporting.
//
// Revision 1.29  1998/02/19 19:45:00  jimg
// Changed error messages so that ones from geturl that are not obviously from
// geturl are prefixed by `geturl: '.
//
// Revision 1.28  1998/02/11 22:34:23  jimg
// Added support for on-the-fly decompression.
// Added -z flag to suppress requesting data be compressed.
// Added new trace options (libwww core, etc.).
//
// Revision 1.27  1998/02/05 20:14:04  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.26  1997/10/09 22:19:32  jimg
// Resolved conflicts in merge of 2.14c to trunk.
//
// Revision 1.25  1997/10/04 00:33:06  jimg
// Release 2.14c fixes
//
// Revision 1.24  1997/09/22 22:26:34  jimg
// Rewrote the handling of received Sequences in `constrained-trans()'.
//
// Revision 1.23.6.1  1997/09/23 15:44:33  jimg
// Now prints newline at end of data output
//
// Revision 1.23  1997/06/05 23:08:51  jimg
// Modified so that data can be read from stdin (similar to writeval).
//
// Revision 1.22  1997/03/05 08:29:32  jimg
// Silly bug where show_gui() was not called in some cases.
//
// Revision 1.21  1997/02/17 20:28:15  jimg
// Changed output of verbose option so it is consistent for DAS, DDS and data
//
// Revision 1.20  1997/02/13 17:30:17  jimg
// Added printout of the DODS version number stamp (read from the MIME header).
// Use the verbose option to enable display.
//
// Revision 1.19  1997/02/13 00:21:28  jimg
// Added version switch. Made compatible with writeval's command line options.
//
// Revision 1.18  1997/02/12 21:45:03  jimg
// Added use of the optional parameter to Connect's ctor; if -t (trace) is
// given on the command line, then print www library informational messages.
// Changed trace option switch from -v (which is now used for verbose - an
// option that is entirely separate from trace) to -t.
//
// Revision 1.17  1997/02/10 02:36:10  jimg
// Modified usage of request_data() to match new return type.
//
// Revision 1.16  1996/12/18 18:43:32  jimg
// Tried to fix the online help - maybe I succeeded?
//
// Revision 1.15  1996/11/27 22:12:33  jimg
// Expanded help to include all the verbose options.
// Added PERF macros to code around request_data() call.
//
// Revision 1.14  1996/11/20 00:37:47  jimg
// Modified -D option to correctly process the Asynchronous option.
//
// Revision 1.13  1996/11/16 00:20:47  jimg
// Fixed a bug where multiple urls failed.
//
// Revision 1.12  1996/10/31 22:24:05  jimg
// Fix the help message so that -D is not described as -A...
//
// Revision 1.11  1996/10/08 17:13:09  jimg
// Added test for the -D option so that a Ce can be supplied either with the
// -c option or using a ? at the end of the URL.
//
// Revision 1.10  1996/09/19 16:53:35  jimg
// Added code to process sequences correctly when given the -D (get data)
// option.
//
// Revision 1.9  1996/09/05 16:28:06  jimg
// Added a new option to geturl, -D, which uses the Connect::request_data
// member function to get data. Thus -a, -d and -D can be used to get the
// das, dds and data from a DODS server. Because the request_data member
// function requires a constraint expression I also added the option -c; this
// option takes as a single argument a constraint expression and passes it to
// the request_data member function. You must use -c <expr> with -D.
//
// Revision 1.8  1996/08/13 20:13:36  jimg
// Added not_used to definition of char rcsid[].
// Added code so that local `URLs' are skipped.
//
// Revision 1.7  1996/06/22 00:11:20  jimg
// Modified to accomodate the new Gui class.
//
// Revision 1.6  1996/06/18 23:55:33  jimg
// Added support for the GUI progress indicator.
//
// Revision 1.5  1996/06/08 00:18:38  jimg
// Initialized vcode to null.
//
// Revision 1.4  1996/05/29 22:05:57  jimg
// Fixed up the copyright header.
//
// Revision 1.3  1996/05/28 17:35:40  jimg
// Fixed verbose arguments.
//
// Revision 1.2  1996/05/28 15:49:55  jimg
// Removed code that read from the stream after the das/dds parser built the
// internal object (since there was nothing in that stream after the parse
// geturl would always crash).
//
// Revision 1.1  1996/05/22 23:34:21  jimg
// First version. Built to test the new WWW code in the class Connect.
//

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: getdap.cc,v 1.43 2000/07/18 03:43:42 rmorris Exp $"};

#include <stdio.h>
#include <assert.h>

#include <GetOpt.h>
#include <string>

#include "Connect.h"

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

const char *version = "$Revision: 1.43 $";
extern int keep_temps;		// defined in Connect.cc

void
usage(string name)
{
    cerr << "Usage: " << name 
	 << "[dDagVvk] [c <expr>] [t <codes>] [m <num>] [-T <list>] <url> [<url> ...]" 
	 << endl
	 << "[gVvk] [t <codes>] [T <list>] <file> [<file> ...]" 
	 << endl;
    cerr << "In the first form of the command, dereference the URL" << endl
	 << "perform the requested operations. In the second, assume" << endl
	 << "the files are DODS data objects (stored in files or read" << endl
	 << "from pipes) and process them as if -D were given." <<endl;
    cerr << "        d: For each URL, get the DODS DDS." << endl;
    cerr << "        a: For each URL, get the DODS DAS." << endl;
    cerr << "        D: For each URL, get the DODS Data." << endl;
    cerr << "        g: Show the progress GUI." << endl;
    cerr << "        v: Verbose." << endl;
    cerr << "        V: Version." << endl;
    cerr << "        c: <expr> is a contraint expression. Used with -D."
	 << endl;
    cerr << "           NB: You can use a `?' for the CE also." << endl;
    cerr << "        k: Keep temporary files created by DODS core" << endl;
    cerr << "        m: Request the same URL <num> times." << endl;
    cerr << "        z: Don't ask the server to compress data." << endl;
    cerr << "        T: <list> List of `Accepted Types'. Translating servers use this." << endl;
    cerr << "        t: <options> trace output; use -td for default." 
         << endl;
    cerr << "          a: show anchor trace." << endl;
    cerr << "          A: show app trace." << endl;
    cerr << "          b: show bind trace." << endl;
    cerr << "          c: show cache trace." << endl;
    cerr << "          h: show auth trace." << endl;
    cerr << "          i: show pics trace." << endl;
    cerr << "          k: show core trace." << endl;
    cerr << "          l: show sgml trace." << endl;
    cerr << "          m: show mem trace." << endl;
    cerr << "          p: show protocol trace." << endl;
    cerr << "          s: show stream trace." << endl;
    cerr << "          t: show thread trace." << endl;
    cerr << "          u: show uri trace." << endl;
    cerr << "          U: show util trace." << endl;
    cerr << "          x: show mux trace." << endl;
    cerr << "          z: show all traces." << endl;
    cerr << "       Without D, d or a, print the URL." << endl;
}

bool
read_data(FILE *fp)
{
    char c;
    
    if (!fp) {
	cerr << "geturl: Whoa!!! Null stream pointer." << endl;
	return false;
    }

    // Changed from a loop that used getc() to one that uses fread(). getc()
    // worked fine for transfers of text information, but *not* for binary
    // transfers. fread() will handle both.

    while (fread(&c, 1, 1, fp))
	printf("%c", c);	// stick with stdio 

    return true;
}

static void
process_data(Connect &url, DDS *dds, bool verbose = false, bool async = false)
{

    if (verbose)
	cerr << "Server version: " << url.server_version() << endl;

    cout << "The data:" << endl;

    bool sequence_found = false;
    for (Pix q = dds->first_var(); q; dds->next_var(q)) {
	BaseType *v = dds->var(q);
	switch (v->type()) {
	    // Sequences present a special case because I let
	    // their semantics get out of hand... jhrg 9/12/96
	  case dods_sequence_c: {
	    Sequence *s = dynamic_cast<Sequence *>(v);
	    s->print_all_vals(cout, url.source(), dds);
	    sequence_found = true;
	    break;
	  }
	  case dods_structure_c: {
	    Structure *s = dynamic_cast<Structure *>(v);
	    s->print_all_vals(cout, url.source(), dds);
	    break;
	  }
	  default:
	    if ((sequence_found || async) && !v->deserialize(url.source(), dds)) {
		cerr << "Asynchronous read failure." << endl;
		exit(1);
	    }
	    v->print_val(cout);
	    break;
	}
    }
    
    cout << endl;
}

#ifdef WIN32
void
#else
int
#endif
main(int argc, char * argv[])
{
    GetOpt getopt (argc, argv, "AdaDgVvkc:t:m:zT:");
    int option_char;
    bool async = false;
    bool get_das = false;
    bool get_dds = false;
    bool get_data = false;
    bool gui = false;
    bool cexpr = false;
    bool verbose = false;
    bool trace = false;
    bool multi = false;
    bool accept_deflate = true;
    string accept_types = "All";
    int times = 1;
    char *tcode = NULL;
    char *expr = "";  // can't use NULL or C++ string conversion will crash
    int topts = 0;

#ifdef WIN32
	_setmode(_fileno(stdout), _O_BINARY);
#endif

    while ((option_char = getopt()) != EOF)
	switch (option_char)
	    {
              case 'A': async = true; break;
              case 'd': get_dds = true; break;
	      case 'a': get_das = true; break;
	      case 'D': get_data = true; break;
	      case 'V': cerr << "geturl version: " << version << endl; exit(0);
	      case 'v': verbose = true; break;
	      case 'g': gui = true; break;
	      case 'k': keep_temps =1; break; // keep_temp is in Connect.cc
	      case 'c':
		cexpr = true; expr = getopt.optarg; break;
	      case 't': 
		trace = true;
		topts = strlen(getopt.optarg);
		if (topts) {
		    tcode = new char[topts + 1];
		    strcpy(tcode, getopt.optarg); 
		}
		break;
	      case 'm': multi = true; times = atoi(getopt.optarg); break;
	      case 'z': accept_deflate = false; break;
	      case 'T': accept_types = getopt.optarg; break;
	      case 'h':
              case '?':
	      default:
		usage(argv[0]); exit(1); break;
	    }

    char c, *cc = tcode;
    if (trace && topts > 0)
	while ((c = *cc++))
	    switch (c) {
	      case 'a': WWWTRACE |= SHOW_ANCHOR_TRACE; break;
	      case 'A': WWWTRACE |= SHOW_APP_TRACE; break;
	      case 'b': WWWTRACE |= SHOW_BIND_TRACE; break;
	      case 'c': WWWTRACE |= SHOW_CACHE_TRACE; break;
	      case 'h': WWWTRACE |= SHOW_AUTH_TRACE; break;
	      case 'i': WWWTRACE |= SHOW_PICS_TRACE; break;
	      case 'k': WWWTRACE |= SHOW_CORE_TRACE; break;
	      case 'l': WWWTRACE |= SHOW_SGML_TRACE; break;
	      case 'm': WWWTRACE |= SHOW_MEM_TRACE; break;
	      case 'p': WWWTRACE |= SHOW_PROTOCOL_TRACE; break;
	      case 's': WWWTRACE |= SHOW_STREAM_TRACE; break;
	      case 't': WWWTRACE |= SHOW_THREAD_TRACE; break;
	      case 'u': WWWTRACE |= SHOW_URI_TRACE; break;
	      case 'U': WWWTRACE |= SHOW_UTIL_TRACE; break;
	      case 'x': WWWTRACE |= SHOW_MUX_TRACE; break;
	      case 'z': WWWTRACE = SHOW_ALL_TRACE; break;
	      default:
		cerr << "Unrecognized trace option: `" << c << "'" << endl;
		break;
	    }
    
    delete tcode;

    // If after processing all the command line options there is nothing left
    // (no URL oor file) assume that we should read from stdin.
    if (getopt.optind == argc) {
	if (verbose)
	    cerr << "Assuming standard input is a DODS data stream." << endl;

	Connect url("stdin", trace, accept_deflate);
	url.set_accept_types(accept_types);

	DDS *dds = url.read_data(stdin, gui, async);
	process_data(url, dds, verbose, async);
    }

    for (int i = getopt.optind; i < argc; ++i) {
	if (verbose)
	    cerr << "Fetching: " << argv[i] << endl;
	
	string name = argv[i];
	Connect url(name, trace, accept_deflate);
	url.set_accept_types(accept_types);

	if (url.is_local()) {
	    if (verbose) 
		cerr << "Assuming that the argument " << argv[i] 
		     << " is a file" << endl 
		     << "that contains a DODS data object; decoding." << endl;

	    DDS *dds = url.read_data(fopen(argv[i], "r"), gui, async);
	    process_data(url, dds, verbose, async);
	    continue;		// Do not run the following IF stmt.
	}

	if (get_das) {
	    for (int j = 0; j < times; ++j) {
		if (!url.request_das(gui))
		    continue;
		if (verbose) {
		    cerr << "Server version: " << url.server_version() 
			<< endl;
		    cerr << "DAS:" << endl;
		}
		url.das().print();
	    }
	}

	if (get_dds) {
	    for (int j = 0; j < times; ++j) {
		if (!url.request_dds(gui))
		    continue;
		if (verbose) {
		    cerr << "Server version: " << url.server_version() 
			<< endl;
		    cerr << "DDS:" << endl;
		}
		url.dds().print();
	    }
	}

	if (get_data) {
	    if (!(expr || name.find('?') != name.npos)) {
		cerr << "Must supply a constraint expression with -D."
		     << endl;
		continue;
	    }
	    for (int j = 0; j < times; ++j) {
		DDS *dds = url.request_data(expr, gui, async);
		if (!dds) {
		    cerr << "Error: " << url.error().error_message() << endl;
		    continue;
		}
		process_data(url, dds, verbose, async);
		delete dds;
	    }
	}

	if (!get_das && !get_dds && !get_data) {
#ifdef GUI
	    url.gui()->show_gui(gui);
#endif
	    string url_string = argv[i];
	    for (int j = 0; j < times; ++j) {
		if (!url.fetch_url(url_string, async))
		    continue;
		if (verbose)
		    cerr << "Server version: " << url.server_version() 
			<< endl;
		FILE *fp = url.output();
		if (!read_data(fp))
		    continue;
		fclose(fp);
	    }
	}	    
    }
#ifdef WIN32
	return;
#endif
}

