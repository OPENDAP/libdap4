
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// This is the source to `geturl'; a simple tool to exercise the Connect
// class. It can be used to get naked URLs as well as the DODS DAS and DDS
// objects.  jhrg.

// $Log: getdap.cc,v $
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
// Added __unused__ to definition of char rcsid[].
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

static char rcsid[] __unused__ = {"$Id: getdap.cc,v 1.15 1996/11/27 22:12:33 jimg Exp $"};

#include <stdio.h>
#include <assert.h>

#include <GetOpt.h>
#include <String.h>

#include "Connect.h"

void
usage(String name)
{
    cerr << "Usage: " << name 
	 << "[AdDa] [c <expr>] [v <codes>] [m <num>] <url> [<url> ...]" 
	 << endl;
    cerr << "       " << "A: Use Connect's asynchronous mode." << endl;
    cerr << "       " << "d: For each URL, get the DODS DDS." << endl;
    cerr << "       " << "a: For each URL, get the DODS DAS." << endl;
    cerr << "       " << "D: For each URL, get the DODS Data." << endl;
    cerr << "       " << "g: Show the progress GUI." << endl;
    cerr << "       " << "c: <expr> is a contraint expression. Used with -D."
	 << endl;
         << "       " << "   NB: You can use a `?' for the CE also."
	 << endl;
    cerr << "       " << "v: <options> Verbose output; use -vd for default." 
         << endl;
    cerr << "       " << "   a: show_anchor_trace." << endl;
    cerr << "       " << "   b: show_bind_trace." << endl;
    cerr << "       " << "   c: show_cache_trace." << endl;
    cerr << "       " << "   l: show_sgml_trace." << endl;
    cerr << "       " << "   m: show_mem_trace." << endl;
    cerr << "       " << "   p: show_protocol_trace." << endl;
    cerr << "       " << "   s: show_stream_trace." << endl;
    cerr << "       " << "   t: show_thread_trace." << endl;
    cerr << "       " << "   u: show_uri_trace." << endl;
    cerr << "       " << "m: Request the same URL <num> times." << endl;
    cerr << "       " << "Without A, use the synchronous mode." << endl;
    cerr << "       " << "Without d or a, print the URL." << endl;
}

bool
read_data(FILE *fp)
{
    char c;
    
    if (!fp) {
	cerr <<"Whoa!!! Null stream pointer." << endl;
	return false;
    }

    // Changed from a loop that used getc() to one that uses fread(). getc()
    // worked fine for transfers of text information, but *not* for binary
    // transfers. fread() will handle both.

    while (fread(&c, 1, 1, fp))
	printf("%c", c);	// stick with stdio 

    return true;
}


int
main(int argc, char * argv[])
{
    GetOpt getopt (argc, argv, "AdaDgc:v:m:");
    int option_char;
    bool async = false;
    bool get_das = false;
    bool get_dds = false;
    bool get_data = false;
    bool gui = false;
    bool cexpr = false;
    bool verbose = false;
    bool multi = false;
    int times = 1;
    char *vcode = NULL;
    char *expr = NULL;
    int vopts = 0;

    while ((option_char = getopt()) != EOF)
	switch (option_char)
	    {
              case 'A': async = true; break;
              case 'd': get_dds = true; break;
	      case 'a': get_das = true; break;
	      case 'D': get_data = true; break;
	      case 'g': gui = true; break;
	      case 'c':
		cexpr = true; expr = getopt.optarg; break;
	      case 'v': 
		verbose = true;
		vopts = strlen(getopt.optarg);
		if (vopts) {
		    vcode = new char[vopts + 1];
		    strcpy(vcode, getopt.optarg); 
		}
		break;
	      case 'm': multi = true; times = atoi(getopt.optarg); break;
	      case 'h':
              case '?':
	      default:
		usage(argv[0]); exit(1); break;
	    }

    char c, *cc = vcode;
    if (verbose && vopts > 0)
	while ((c = *cc++))
	    switch (c) {
	      case 'a': WWWTRACE |= SHOW_ANCHOR_TRACE; break;
	      case 'b': WWWTRACE |= SHOW_BIND_TRACE; break;
	      case 'c': WWWTRACE |= SHOW_CACHE_TRACE; break;
	      case 'l': WWWTRACE |= SHOW_SGML_TRACE; break;
	      case 'm': WWWTRACE |= SHOW_MEM_TRACE; break;
	      case 'p': WWWTRACE |= SHOW_PROTOCOL_TRACE; break;
	      case 's': WWWTRACE |= SHOW_STREAM_TRACE; break;
	      case 't': WWWTRACE |= SHOW_THREAD_TRACE; break;
	      case 'u': WWWTRACE |= SHOW_URI_TRACE; break;
	      case 'd': break;
	      default:
		cerr << "Unrecognized verbose option: `" << c << "'" << endl;
		break;
	    }
    
    delete vcode;

    for (int i = getopt.optind; i < argc; ++i) {
	if (verbose)
	    cerr << "Fetching " << argv[i] << ":" << endl;
	
	String name = argv[i];
	Connect url(name);

	if (url.is_local()) {
	    cerr << "Skipping the URL `" << argv[i] 
		 << "' because it lacks the `http' access protocol." 
		 << endl; 
	    continue;
	}

	if (get_das) {
	    for (int j = 0; j < times; ++j) {
		if (!url.request_das(gui))
		    exit(1);
		if (verbose)
		    cerr << "DAS:" << endl;
		url.das().print();
	    }
	}

	if (get_dds) {
	    for (int j = 0; j < times; ++j) {
		if (!url.request_dds(gui))
		    exit(1);
		if (verbose)
		    cerr << "DDS:" << endl;
		url.dds().print();
	    }
	}

	if (get_data) {
	    if (!(expr || name.contains("?"))) {
		cerr << "Must supply a constraint expression with -D."
		     << endl;
		exit(1);
	    }
	    for (int j = 0; j < times; ++j) {
		DDS &dds = url.request_data(expr, gui, async);

		cout << "The data:" << endl;
		for (Pix q = dds.first_var(); q; dds.next_var(q)) {
		    BaseType *v = dds.var(q);
		    switch (v->type()) {
			// Sequences present a special case because I let
			// their semantics get out of hand... jhrg 9/12/96
		      case dods_sequence_c:
			((Sequence *)v)->print_all_vals(cout, url.source());
			break;
		      default:
			PERF(cerr << "Deserializing: " << dds.var(q).name() \
			     << endl);
			if (async && !dds.var(q)->deserialize(url.source())) {
			    cerr << "Asynchronous read failure." << endl;
			    exit(1);
			}
			PERF(cerr << "Deserializing complete" << endl);
			dds.var(q)->print_val(cout);
			break;
		    }
		}
	    }
	}

	if (!get_das && !get_dds && !get_data) {
	    if (gui)
		url.gui()->show_gui(gui);
	    String url_string = argv[i];
	    for (int j = 0; j < times; ++j) {
		if (!url.fetch_url(url_string, async))
		    exit(1);
		FILE *fp = url.output();
		if (!read_data(fp))
		    exit(1);
		fclose(fp);
	    }
	}	    
    }
}
