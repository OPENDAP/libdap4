
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//	dan		Dan Holloway (dan@hollywood.gso.uri.edu)
//	reza		Reza Nekovei (reza@intcomm.net)

// $Log: Connect.cc,v $
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
// Revision 1.66.4.2  1999/02/05 09:32:33  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code.
//
// Revision 1.66.4.1  1999/02/02 21:56:56  jimg
// String to string version
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
// Revision 1.15  1996/02/01 21:43:51  jimg
// Added mfuncs to maintain a list of DDSs and the constraint expressions
// that produced them.
// Added code in request_data to strip the incoming DDS from a data
// document.
// Fixed up bogus comments.
//
// Revision 1.14.2.3  1996/03/01 00:07:57  jimg
// Removed bad attempt at multiple connect implementation.
//
// Revision 1.14.2.2  1996/02/23 22:51:00  jimg
// Added const in prototype of netio files.
// Added libraries for solaris 2.x
//
// Revision 1.14.2.1  1996/02/23 21:38:35  jimg
// Updated for new configure.in.
//
// Revision 1.14  1995/07/09  21:20:44  jimg
// Fixed date in copyright (it now reads `Copyright 1995 ...').
//
// Revision 1.13  1995/07/09  21:14:45  jimg
// Added copyright.
//
// Revision 1.12  1995/06/27  19:33:47  jimg
// The mfuncs request_{das,dds,dods} accept a parameter which is appended to
// the URL and used by the data server CGI to select which filter program is
// run to handle a particular request. I changed the parameter name from cgi
// to ext to better represent what was going on (after getting confused
// several times myself).
//
// Revision 1.11  1995/06/01  16:46:35  jimg
// Removed old code.
//
// Revision 1.10  1995/05/30  18:42:45  jimg
// Modified the request_data member function so that it accepts the variable
// in addition to the existing arguments.
//
// Revision 1.9  1995/05/22  20:41:37  jimg
// Changed the usage of URLs: we now use straight URLs; no POSTs and no
// internal parsing of the URL. To select different documents from a DODS
// server an extension is appended to the URL.
//
// Revision 1.8  1995/04/17  03:19:22  jimg
// Added code which takes the cgi basename from the URL supplied by the
// user. Still, the cgi must be in `cgi-bin'.
//
// Revision 1.7  1995/03/09  20:36:07  jimg
// Modified so that URLs built by this library no longer supply the
// base name of the CGI. Instead the base name is stripped off the front
// of the pathname component of the URL supplied by the user. This class
// append the suffix _das, _dds or _serv when a Connect object is used to
// get the DAS, DDS or Data (resp).
//
// Revision 1.6  1995/02/22  21:04:00  reza
// Added version number capability using CGI status_line.
//
// Revision 1.5  1995/02/10  21:53:53  jimg
// Modified request_data() so that it takes an additional (optional)
// parameter which specifies synchronous (default) of Asynchronous
// behavior.
//
// Revision 1.4  1995/02/10  04:43:15  reza
// Fixed the request_data to pass arguments. The arguments string is added to
// the file name before being posted by NetConnect. Default arg. is null.
//
// Revision 1.3  1995/01/31  20:46:04  jimg
// Fixed problems with the return value (status, fp) in request_das,
// request_dds and request_data.
// Added declarations for set_xdrin() and set_xdr_out().
// Now that NetExecute forks and reads from a child, a temp file is no
// longer used. I removed that code which created and sebsequently deleted
// that temp file.
//
// Revision 1.2  1995/01/18  18:48:22  dan
// Added member function 'request_data' which makes a data read request
// to the remote api server and links the object's xdrin file-pointers
// to the data stream.  This function requires NetConnect which has
// been defined in the utility function netexec.c
//
// Revision 1.1  1995/01/09  16:03:26  dan
// These files constitute the JGOFS server and client library code
// for use with the DODS api.
//
// Revision 1.6  1994/12/06  01:14:13  reza
// Fixed a bug in the usage of url_comp.
//
// Revision 1.5  1994/11/18  21:22:27  reza
// Fixed error in an if condition.
//
// Revision 1.4  1994/11/03  05:36:09  reza
// Added the request_dds function and error checking for NetExecute calls.
//
// Revision 1.3  1994/10/06  16:14:07  jimg
// Added hard-coded path to cgi binaries in make_url (set to `cgi-bin').
//
// Revision 1.2  1994/10/05  20:23:26  jimg
// Fixed errors in *.h files comments - CVS bites again.
// Changed request_{das,dds} so that they use the field `_api_name'
// instead of requiring callers to pass the api name.
//

// Revision 1.1  1994/10/05  18:02:06  jimg
// First version of the connection management classes.
// This commit also includes early versions of the test code.
//

#include "config_dap.h"

static char rcsid[] not_used ={"$Id: Connect.cc,v 1.79 1999/07/22 17:11:50 jimg Exp $"};

#ifdef __GNUG__
#pragma "implemenation"
#endif

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>

#include <sys/types.h>		// Used by the semaphore code in fetch_url()
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#if HAVE_EXPECT
#include <expect.h>
#endif

#ifdef __GNUG__
#include <strstream>
#else
#include <sstream>
#endif
#include <fstream>

#include "debug.h"
#include "DataDDS.h"
#include "Connect.h"

#define SHOW_MSG (WWWTRACE || HTAlert_interactive())
#define DODS_KEEP_TEMP 0

#if defined(__svr4__)
#define CATCH_SIG
#endif

// Constants used for temporary files.

static const char DODS_PREFIX[]={"dods"};
static const int DEFAULT_TIMEOUT = 100; // Timeout in seconds.
int keep_temps = DODS_KEEP_TEMP;	// Non-zero to keep temp files.

static const char bad_decomp_msg[]={\
"The data returned by the server was compressed and the\n\
decompression program failed to start. Please report this\n\
error to the data server maintainer or to support@unidata.ucar.edu"}; 

HTList *Connect::_conv = 0;

#undef CATCH_SIG
#ifdef CATCH_SIG
#include <signal.h>

// This function sets up signal handlers. This might not be necessary to
// call if the application has its own handlers (lossage on SVR4)

static void 
SetSignal(void)
{
    // On some systems (SYSV) it is necessary to catch the SIGPIPE signal
    // when attemting to connect to a remote host where you normally should
    // get `connection refused' back

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
	if (PROT_TRACE) cerr << "HTSignal.... Can't catch SIGPIPE" << endl;
    } else {
	if (PROT_TRACE) cerr << "HTSignal.... Ignoring SIGPIPE" << endl;
    }
}
#endif /* CATCH_SIG */

// The next five functions (*not* mfuncs) are friends of Connect.

// This function is registered to handle the result of the request

int 
http_terminate_handler(HTRequest * request, HTResponse * /*response*/,
			void * /*param*/, int status) 
{
    if (status != HT_LOADED) {
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
    }
    
    return HT_OK;
}

// This function is registered to handle timeout in select eventloop

int 
timeout_handler(HTRequest *request)
{
    Connect *me = (Connect *)HTRequest_context(request);
    string cmd;

    if (!me->gui()->progress_visible())
	cmd = (string)"progress popup \"Request timeout...\"\r";
    else
	cmd = (string)"progress text \"Request timeout...\"\r";
	
    me->gui()->progress_visible(me->gui()->command(cmd));

    if (me->gui()->progress_visible()) {
	sleep(3);
	cmd = (string)"progress popdown\r";
	me->gui()->command(cmd);
	me->gui()->progress_visible(false);
    }

    HTRequest_kill(request);

    return 0;
}

// Use the GUI to report progress to the user.

BOOL
dods_progress (HTRequest * request, HTAlertOpcode op, int /* msgnum */, 
	       const char * /* dfault */, void * input,
	       HTAlertPar * /* reply */)
{
    if (!request) {
        if (WWWTRACE) cerr << "dods_rogress: NULL Request" << endl;
        return YES;
    }

    Connect *me = (Connect *)HTRequest_context(request);
    string cmd;

    switch (op) {
      case HT_PROG_DNS:
	if (!me->gui()->progress_visible())
	    cmd = (string)"progress popup \"Looking up " + (char *)input 
	          + "\"\r";
	else
	    cmd = (string)"progress text \"Looking up " + (char *)input 
	          + "\"\r";
	
	me->gui()->progress_visible(me->gui()->command(cmd));
        break;

      case HT_PROG_CONNECT:
	if (!me->gui()->progress_visible())
	    cmd = (string)"progress popup \"Contacting host...\"\r";
	else
	    cmd = (string)"progress text \"Contacting host...\"\r";
	
	me->gui()->progress_visible(me->gui()->command(cmd));
        break;

      case HT_PROG_ACCEPT:
	if (!me->gui()->progress_visible())
	    cmd = (string)"progress popup \"Waiting for connection...\"\r";
	else
	    cmd = (string)"progress text \"Waiting for connection...\"\r";
	
	me->gui()->progress_visible(me->gui()->command(cmd));
        break;

      case HT_PROG_READ: {
	  if (!me->gui()->progress_visible())
	      cmd = (string)"progress popup \"Reading...\"\r";
	  else
	      cmd = (string)"progress text \"Reading...\"\r";
	  me->gui()->progress_visible(me->gui()->command(cmd));
	  
	  if (!me->gui()->progress_visible()) // Bail if window won't popup
	      break;

	  long cl = HTAnchor_length(HTRequest_anchor(request));
	  if (cl >= 0) {
#ifdef LIBWWW_5_0
	      long b_read = HTRequest_bytesRead(request);
#else
	      long b_read = HTRequest_bodyRead(request);
#endif
	      double pro = (double) b_read/cl*100;
	      ostrstream cmd_s;
	      cmd_s << "progress bar " << pro << "\r" << ends;
	      (void)me->gui()->command(cmd_s.str());
	      cmd_s.rdbuf()->freeze(0);
	  }
	  else {
	      cmd = (string)"progress bar -1\r";
	      (void)me->gui()->command(cmd);
	  }
	  
	  break;
      }

      case HT_PROG_WRITE:
	// DODS *NEVER* writes. Ever. Well, it does write the request header...
	break;

      case HT_PROG_DONE:
	cmd = (string)"progress popdown\r";
	me->gui()->command(cmd);
	me->gui()->progress_visible(false);
        break;

      case HT_PROG_WAIT:
	if (!me->gui()->progress_visible())
	    cmd = (string)"progress popup \"Waiting for free socket...\"\r";
	else
	    cmd = (string)"progress text \"Waiting for free socket...\"\r";
	me->gui()->progress_visible(me->gui()->command(cmd));
        break;

      default:
        cerr << "UNKNOWN PROGRESS STATE" << endl;
        break;
    }

    return YES;
}

BOOL
dods_username_password (HTRequest * request, HTAlertOpcode /* op */,
			int /* msgnum */, const char * /* dfault */,
			void * /* input */, HTAlertPar * reply)
{
    if (!request) {
        if (WWWTRACE) cerr << "dods_username_password: Bad argument" << endl;
        return NO;
    }

    Connect *me = (Connect *)HTRequest_context(request);

    // Put the username in reply using HTAlert_setReplyMessage; use
    // _setReplySecret for the password.
    string cmd = "password\r";
    string response;

    if (!me->gui()->response(cmd, response))
	return NO;

    // Extract two words from RESPONSE; #1 is the username and #2 is the
    // password. Either may be missing, in which case return NO.

    string words[2];
    size_t white1 = response.find_first_of(" \t\r\n");
    size_t white2 = response.find_first_not_of(" \t\r\n", white1+1);
    words[0] = response.substr(0, white1);
    words[1] = response.substr(white2);
    if (response.find_first_of(" \t\r\n", white2) != response.npos) {
	DBG2(cerr << "Wrong number of words in response: " << response \
	     << endl);
	return NO;
    }

    HTAlert_setReplyMessage(reply, words[0].c_str());
    HTAlert_setReplySecret(reply, words[1].c_str());

    return YES;
}

static HTErrorMessage HTErrors[HTERR_ELEMENTS] = {HTERR_ENGLISH_INITIALIZER};

BOOL 
dods_error_print (HTRequest * request, HTAlertOpcode /* op */,
		  int /* msgnum */, const char * /* dfault */, void * input,
		  HTAlertPar * /* reply */)
{
    HTList *cur = (HTList *) input;
    HTError *pres;
    HTErrorShow showmask = HTError_show();
    HTChunk *msg = NULL;
    int code;

    if (WWWTRACE) cerr << "dods_error_print: Generating message" << endl;

    if (!request)
	if (WWWTRACE) cerr << "dods_error_print: Null request." << endl;

    if (!cur)
	if (WWWTRACE) cerr << "dods_error_print: Null error list." << endl;

    if (!request || !cur) 
	return NO;

    while ((pres = (HTError *) HTList_nextObject(cur))) {
        int index = HTError_index(pres);
	// An index of 
        if (HTError_doShow(pres)) {
            if (!msg) {
                HTSeverity severity = HTError_severity(pres);
                msg = HTChunk_new(128);
		switch (severity) {
		  case ERR_WARN:
                    HTChunk_puts(msg, "Warning: ");
		    break;
		  case ERR_NON_FATAL:
                    HTChunk_puts(msg, "Non Fatal Error: ");
		    break;
		  case ERR_FATAL:
		    if (request) {
			Connect *me = (Connect *)HTRequest_context(request);
			me->_type = web_error;
		    }
                    HTChunk_puts(msg, "Fatal Error: ");
		    break;
		  case ERR_INFO:
                    HTChunk_puts(msg, "Information: ");
		    break;
		  default:
                    if (WWWTRACE)
                        cerr << "Unknown Classification of Error (" 
			     << severity << ")" << endl;
                    HTChunk_delete(msg);
                    return NO;
                }

                /* Error number */
                if ((code = HTErrors[index].code) > 0) {
                    char buf[10];
                    sprintf(buf, "%d ", code);
                    HTChunk_puts(msg, buf);
                }

                HTChunk_putc(msg, '\n');
	    }
            else
                HTChunk_puts(msg, "\nReason: ");
	    
            HTChunk_puts(msg, HTErrors[index].msg); // Error message

            if (showmask & HT_ERR_SHOW_LOCATION) { // Error Location
                HTChunk_puts(msg, "This occured in ");
                HTChunk_puts(msg, HTError_location(pres));
                HTChunk_putc(msg, '\n');
            }

            /*
            ** Make sure that we don't get this error more than once even
            ** if we are keeping the error stack from one request to another
            */
            HTError_setIgnore(pres);
            
            /* If we only are show the most recent entry then break here */
            if (showmask & HT_ERR_SHOW_FIRST)
                break;
        }
    }

    if (msg) {
	string command = (string)"dialog \"" + (char *)HTChunk_data(msg) 
	    + "\" error\r";
	string response;	// Not used
	Connect *me = (Connect *)HTRequest_context(request);

	if (me->gui()->show_gui()) {
	    if (!me->gui()->response(command, response))
		cerr << "GUI Failure in dods_error_print()" << endl;
	}
	else {
	    // Connect used to route all www errors to stderr. That was a
	    // mistake; they should go to the Error object. To keep the old
	    // behavior I've added two mfuncs that set and get a property
	    // used to control if errors are sent to stderr. 6/3/98 jhrg
	    if (me->get_www_errors_to_stderr())
		cerr << (char *)HTChunk_data(msg) << endl;
	    // Load into the error object here. 
	    Error &e = me->error();
	    e.error_code(unknown_error);
	    string s = (char *)HTChunk_data(msg);
	    e.error_message(s);
	}

        HTChunk_delete(msg);
    }

    return YES;
}

static ObjectType
get_type(string value)
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
    
// This function is registered to handle unknown MIME headers

int 
description_handler(HTRequest *request, HTResponse */*response*/, 
		    const char *token, const char *val)
{
    string field = token, value = val;
    downcase(field);
    downcase(field);
    
    if (field == "content-description") {
	DBG(cerr << "Found content-description header" << endl);
	Connect *me = (Connect *)HTRequest_context(request);
	me->_type = get_type(value);
    }
    else {
	if (SHOW_MSG)
	    cerr << "Unknown header: " << token << endl;
    }

    return HT_OK;
}

int 
server_handler(HTRequest *request, HTResponse */*response*/, 
	       const char *token, const char *val)
{
    string field = token, value = val;
    downcase(field);
    downcase(value);
    
    if (field == "xdods-server") {
	DBG(cerr << "Found dods server header: " << value << endl);
	Connect *me = (Connect *)HTRequest_context(request);
	me->_server = value;
    }
    else if (field == "server") {
	DBG(cerr << "Found server header: " << value << endl);
	Connect *me = (Connect *)HTRequest_context(request);
	me->_server = value;
    }
    else {
	if (SHOW_MSG)
	    cerr << "Unknown header: " << token << endl;
    }

    return HT_OK;
}

// Use this for debugging only since various servers seem to add headers
// (even though the `NPH' mechanism precludes that ...??). This is especially
// true for error returns.

int 
header_handler(HTRequest *, HTResponse *, const char *token, const char *val)
{
    string field = token, value = val;
    downcase(field);
    downcase(value);
    
    cerr << "Unknown header: " << token << ": " << value <<endl;

    return HT_OK;
}

#define PUTBLOCK(b, l)	(*target->isa->put_block)(target, b, l)
struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};
 
int
xdods_accept_types_header_gen(HTRequest *pReq, HTStream *target)
{
    Connect *me = (Connect *)HTRequest_context(pReq);

    string types = "XDODS-Accept-Types: " + me->get_accept_types() + "\r\n";
    if (WWWTRACE) 
	HTTrace("DODS..... '%s'.\n", types.c_str());
    
    PUTBLOCK(types.c_str(), types.length());

    return HT_OK;
}

// Barely a parser... This is used when reading from local sources of DODS
// Data objects. It simulates the important actions of the libwww MIME header
// parser. Those actions fill in certain fields in the Connect object. jhrg
// 5/20/97
//
// Make sure that this parser reads from data_source without disturbing the
// information in data_source tat follows the MIME header. Since the DDS
// (which follows the MIME header) is parsed by a flex/bison scanner/parser,
// make sure to use I/O calls that will mesh with ANSI C I/O calls. In the
// old GNU libg++, the C++ calls were synchronized with the C calls, but that
// may no longer be the case. 5/31/99 jhrg

void
Connect::parse_mime(FILE *data_source)
{    
    char line[256];

    fgets(line, 255, data_source);
    line[strlen(line)-1] = '\0'; // remove the newline

    while ((string)line != "") {
	char h[256], v[256];
	sscanf(line, "%s %s\n", h, v);
	string header = h;
	string value = v;
	downcase(header);
	downcase(value);

	if (header == "content-description:") {
	    DBG(cout << header << ": " << value << endl);
	    _type = get_type(value);
	}
	else if (header == "xdods-server:") {
	    DBG(cout << header << ": " << value << endl);
	    _server = value;
	}
	else if (header == "server:") {
	    DBG(cout << header << ": " << value << endl);
	    _server = value;
	}
	
	fgets(line, 255, data_source);
	line[strlen(line)-1] = '\0';
    }
}

void
Connect::www_lib_init(bool www_verbose_errors, bool accept_deflate)
{
    // Initialize various parts of the library. This is in lieu of using one
    // of the profiles in HTProfil.c. 02/09/98 jhrg
    
    // Initial tcp and buffered_tcp transports
    HTTransportInit();

    // Set up http and cache protocols. Do this instead of
    // HTProtocolPreemtiveInit(). 
    HTProtocol_add("http", "buffered_tcp", HTTP_PORT, YES, HTLoadHTTP, NULL);
    HTProtocol_add("cache", "local", 0, YES, HTLoadCache, NULL);

    // Initialize various before and after filters. See HTInit.c.
    HTNetInit();

    // Add basic authentication.
    HTAAInit();

    // Add proxy handling here. In HTProfile_newPreem...()
    // HTProxy_getEnvVars() is called. 02/09/98 jhrg
    
    // Register the default set of converters.
    _conv = HTList_new();
    HTConverterInit(_conv);
    HTFormat_setConversion(_conv);

    // Register the default set of transfer encoders and decoders
    HTList *transfer_encodings = HTList_new();
    HTTransferEncoderInit(transfer_encodings);
    HTFormat_setTransferCoding(transfer_encodings);

    // Register the default set of content encoders and decoders
    if (accept_deflate) {
#ifdef HT_ZLIB
	HTList *content_encodings = HTList_new();
	HTCoding_add(content_encodings, "deflate", NULL, HTZLib_inflate, 1.0);
	HTFormat_setContentCoding(content_encodings);
#endif /* HT_ZLIB */
    }

    // Register MIME headers for HTTP 1.1
    HTMIMEInit();

#ifdef GUI
    // Add progress notification, etc.
    HTAlert_add(dods_progress, HT_A_PROGRESS);
    HTAlert_add(dods_error_print, HT_A_MESSAGE);
    HTAlert_add(dods_username_password, HT_A_USER_PW);
    HTAlert_setInteractive(YES);
#endif
    // Disable the cache.
    HTCacheTerminate();

    if (www_verbose_errors)
	HTError_setShow(HT_ERR_SHOW_INFO);
    else
	HTError_setShow(HT_ERR_SHOW_FATAL);
	
    // Add our own filter to update the history list.
    HTNet_addAfter(http_terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    // We add our own parsers for content-description and server so that
    // we can test for these fields and operate on the resulting document
    // without using the stream stack mechanism (which seems to be very
    // complicated). jhrg 11/20/96
    HTHeader_addParser("content-description", NO, description_handler);
    // Added DODS server header because `Server:' is used by Java. We check
    // first for `XDODS-Server:' and use that if found. Then look for
    // `Server:' and finally default to 0.0. 12/16/98 jhrg
    HTHeader_addParser("xdods-server", NO, server_handler);
    HTHeader_addParser("server", NO, server_handler);

    // Add xdods_accept_types header. 2/17/99 jhrg
    HTHeader_addGenerator(xdods_accept_types_header_gen);
}

// Before calling this mfunc memory for the timeval struct must be allocated.

void
Connect::clone(const Connect &src)
{
    // dup the _request, _anchor, ... members here
    _local = src._local;
    
    if (!_local) {
	_URL = src._URL;
	_das = src._das;
	_dds = src._dds;
	_error = src._error;

	// Initialize the anchor object.
	char *ref = HTParse(_URL.c_str(), (char *)0, PARSE_ALL);
	_anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
	HT_FREE(ref);

	_type = src._type;
	_encoding = src._encoding;

	// Copy the access method.
	_method = src._method;

#ifdef LIBWWW_5_0
	// Copy the timeout value.
	_tv->tv_sec = src._tv->tv_sec;
#endif

	// Open the file for non-truncating update.
	if (_output)
	    _output = fdopen(dup(fileno(src._output)), "r+");
	if (_source)
	    _source = new_xdrstdio(_output, XDR_DECODE);
    }
}

// Use the URL designated when the Connect object was created as the
// `base' URL so that the formal parameter to this mfunc can be relative.

bool
Connect::read_url(string &url, FILE *stream)
{
    assert(stream);

    int status = YES;

    HTRequest *_request = HTRequest_new();

    HTRequest_setContext (_request, this); // Bind THIS to request 

    HTRequest_setOutputFormat(_request, WWW_SOURCE);

    // Set timeout on sockets.
    // For libwww 5.1d this must be changed.
#ifdef LIBWWW_5_0
    HTEventrg_registerTimeout(_tv, _request, timeout_handler, NO);
#endif

    HTRequest_setAnchor(_request, (HTAnchor *)_anchor);

    HTRequest_setOutputStream(_request, HTFWriter_new(_request, stream, YES));

    status = HTLoadRelative(url.c_str(), _anchor, _request);

    if (status != YES) {
	if (SHOW_MSG) cerr << "Can't access resource" << endl;
	return false;
    }

    HTRequest_delete(_request);

    return status;
}

void
Connect::close_output()
{
    if (_output && _output != stdout) {
	fclose(_output);
	_output = 0;
    }

    if (_source) {
	delete_xdrstdio(_source);
	_source = 0;
    }
}

// This ctor is decalred private so that it won't ever be called by users,
// thus forcing them to create Connects which point somewhere.

Connect::Connect()
{
    assert(false);
}

// public mfuncs

Connect::Connect(string name, bool www_verbose_errors,
		 bool accept_deflate) : _www_errors_to_stderr(false)
{
#ifdef GUI
    _gui = new Gui;
#endif
    // Unless a client says otherwise, assume we can process all types.
    _accept_types = "All";
    name = prune_spaces(name);
    char *access_ref = HTParse(name.c_str(), NULL, PARSE_ACCESS);
    if (strcmp(access_ref, "http") == 0) { // access == http --> remote access
	// If there are no current connects, initialize the library
       	if (!_conv) {
	    www_lib_init(www_verbose_errors, accept_deflate);
	}
	size_t dotpos = name.find('?');
	if (dotpos!=name.npos) {
	    _URL = name.substr(0, dotpos);
	    string expr = name.substr(dotpos+1);

	    dotpos = expr.find('&');
	    if (dotpos!=expr.npos) {
		_proj = expr.substr(0, dotpos);
		_sel = expr.substr(dotpos); // XXX includes '&'
	    }
	    else {
		_proj = expr;
		_sel = "";
	    }
	}
	else {
	    _URL = name;
	    _proj = "";
	    _sel = "";
	}
	_local = false;

#ifdef LIBWWW_5_0
	_tv = new timeval;
	_tv->tv_sec = DEFAULT_TIMEOUT;
#endif
	_output = 0;
	_source = 0;
	_type = unknown_type;
	_encoding = unknown_enc;
	// Assume servers that don't announce themselves are old servers.
	_server = "dods/0.0";

	char *ref = HTParse(_URL.c_str(), (char *)0, PARSE_ALL);
	_anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
	HT_FREE(ref);
    }
    else {
	_URL = "";
	_local = true;
	_output = 0;
	_source = 0;
	_type = unknown_type;
	_encoding = unknown_enc;
#ifdef LIBWWW_5_0
	_tv = 0;
#endif
#ifdef GUI
	delete _gui;
#endif
    }

    HT_FREE(access_ref);
}

Connect::Connect(const Connect &copy_from) : _error(undefined_error, "")
{
#ifdef LIBWWW_5_0
    if (copy_from._tv)
	_tv = new timeval;
    else 
	_tv = 0;
#endif

    clone(copy_from);
}

Connect::~Connect()
{
    DBG2(cerr << "Entering the Connect dtor" << endl);

#ifdef LIBWWW_5_0
    delete _tv;
#endif
#ifdef GUI
    delete _gui;
#endif 

    close_output();

    DBG2(cerr << "Leaving the Connect dtor" << endl);
}

Connect &
Connect::operator=(const Connect &rhs)
{
    if (&rhs == this)
	return *this;
    else {
	clone(rhs);
	return *this;
    }
}

void
Connect::set_www_errors_to_stderr(bool state)
{
    _www_errors_to_stderr = state;
}

bool
Connect::get_www_errors_to_stderr()
{
    return _www_errors_to_stderr;
}

string 
Connect::get_accept_types()
{
    return _accept_types;
}

void
Connect::set_accept_types(const string &types)
{
    _accept_types = types;
}

// Dereference the URL and dump its contents into _OUTPUT. Note that
// read_url() does the actual dereferencing; this sets up the _OUTPUT sink.
//
// Due to a bug in the asynchorous code which makes it fail when used with
// compression, I'm forcing all accesses to be synchronous. Note that given
// the design of Connect, the caller of fetch_url will never know that their
// async operation was actually synchronous.

bool
Connect::fetch_url(string &url, bool)
{
    _encoding = unknown_enc;
    _type = unknown_type;
   
    /* NB: I've completely removed the async stuff for now. 2/18/97 jhrg */
  
    char *c = tempnam(NULL, DODS_PREFIX);
    FILE *stream = fopen(c, "w+"); // Open truncated for update.
    if (!keep_temps)
	unlink(c);		// When _OUTPUT is closed file is deleted.
    else
	cerr << "Temporary file for Data document: " << c << endl;
    free(c);			// tempnam uses malloc!

    if (!read_url(url, stream))
	return false;
   
    // Now rewind the stream so that we can read from the temp file
    if (fseek(stream, 0L, 0) < 0) {
	cerr << "Could not rewind stream" << endl;
	return false;
    }
   
    close_output();

    _output = stream;
   
    return true;
}

FILE *
Connect::output()
{
    // NB: Users should make sure they don't close stdout.
    return _output;		
}

XDR *
Connect::source()
{
    if (!_source)
	_source = new_xdrstdio(_output, XDR_DECODE);
    	
    return _source;
}


ObjectType
Connect::type()
{
    return _type;
}

EncodingType
Connect::encoding()
{
    return _encoding;
}

string
Connect::server_version()
{
    return _server;
}

// Added EXT which defaults to "das". jhrg 3/7/95

bool
Connect::request_das(bool gui_p, const string &ext)
{
#ifdef GUI
    (void)gui()->show_gui(gui_p);
#endif
    string das_url = _URL + "." + ext;
    if (_proj.length() + _sel.length())
	das_url = das_url + "?" + _proj + _sel;
    bool status = false;
    string value;

    status = fetch_url(das_url);
    if (!status)
	goto exit;

    switch (type()) {
      case dods_error: {
	  string correction;
	  if (!_error.parse(_output)) {
	      cerr << "Could not parse error object" << endl;
	      status = false;
	      break;
	  }
	  correction = _error.correct_error(gui());
	  status = false;
	  break;
      }

      case web_error:
	status = false;
	break;

      case dods_das:
      default:
	status = _das.parse(_output); // read and parse the das from a file 
	break;
    }

exit:
    close_output();
    return status;
}

// Added EXT which deafults to "dds". jhrg 3/7/95

bool
Connect::request_dds(bool gui_p, const string &ext)
{
#ifdef GUI
    (void)gui()->show_gui(gui_p);
#endif
    string dds_url = _URL + "." + ext;
    if (_proj.length() + _sel.length())
	dds_url = dds_url + "?" + _proj + _sel;
    bool status = false;

    status = fetch_url(dds_url);
    if (!status)
	goto exit;
    
    switch (type()) {
      case dods_error: {
	  string correction;
	  if (!_error.parse(_output)) {
	      cerr << "Could not parse error object" << endl;
	      status = false;
	      break;
	  }
	  correction = _error.correct_error(gui());
	  status = false;
	  break;
      }
	
      case web_error:
	status = false;
	break;

      case dods_dds:
      default:
	status = _dds.parse(_output); // read and parse the dds from a file 
	break;
    }

exit:
    close_output();
    return status;
}

// Assume that _output points to FILE * from which we can read the data
// object. 
// This is a private mfunc.

DDS *
Connect::process_data(bool async)
{
    switch (type()) {
      case dods_error: {
	  if (!_error.parse(_output)) {
	      cerr << "Could not parse error object" << endl;
	      break;
	  }
	  return 0;
      }

      case web_error:
	// Web errors (those reported in the return document's MIME header)
	// are processed by the WWW library.
	return 0;

      case dods_data:
      default: {
	  DataDDS *dds = new DataDDS("received_data", _server);

	  // Parse the DDS
	  if (!dds->parse(_output)) {
	      cerr << "Could not parse data DDS." << endl;
	      return 0;
	  }

	  // If !asynchronous, read data for all the variables from the
	  // document. 
	  if (!async) {
	      XDR *s = source();
	      for (Pix q = dds->first_var(); q; dds->next_var(q)) {
		  BaseType *v = dds->var(q);
		  // Because sequences have multiple rows, bail out and let
		  // the caller deserialize as they read the data.
		  if (v->type() == dods_sequence_c)
		      break;
		  if (!v->deserialize(s, dds))
		      return 0;
	      }
	  }

	  return dds;
      }
    }

    return 0;
}
      
// Read data from the server at _URL. If ASYNC is true, read asynchronously
// using NetConnect (which forks so that it can return *before* the read
// completes). Synchronous reads (using NetExecute) are the default. 
//
// NB: This function does not actually read the data (in either case), it
// just sets up the BaseType static class member so that data *can* be read. 
//
// Returns: A reference to the DDS object which contains the variable
// (BaseType *) generated from the DDS sent with the data. This variable is
// guaranteed to be large enough to hold the data, even if the constraint
// expression changed the type of the variable from that which appeared in the
// origianl DDS received from the dataset when this connection was made.

// WARNING: If EXPR contains several veraibles and one of the variables is a
// Sequence and it is not the last variable then the input stream will be
// corrupted. If you request N variables and M of them are Sequences, all the
// M sequences must follow the N-M other variables.

DDS *
Connect::request_data(string expr, bool gui_p, 
		      bool async, const string &ext)
{
#ifdef GUI
    (void)gui()->show_gui(gui_p);
#endif
    string proj, sel;
    size_t dotpos = expr.find('&');
    if (dotpos != expr.npos) {
	proj = expr.substr(0, dotpos);
	sel = expr.substr(dotpos);
    }
    else {
	proj = expr;
	sel = "";
    }

    string data_url = _URL + "." + ext + "?" + _proj + proj + _sel + sel;
    bool status = fetch_url(data_url, async);
	
    if (!status) {
	cerr << "Could not complete data request operation" << endl;
	return 0;
    }

    return process_data(async);
}

DDS *
Connect::read_data(FILE *data_source, bool gui_p, bool async)
{
#ifdef GUI
    (void)gui()->show_gui(gui_p);
#endif
    // Read from data_source and parse the MIME headers specific to DODS.
    parse_mime(data_source);

    _output = data_source;

    return process_data(async);
}

Gui *
Connect::gui()
{
#ifdef GUI
    return _gui;
#else
    return NULL;
#endif
}

bool
Connect::is_local()
{
    return _local;
}

string
Connect::URL(bool ce)
{
    if (_local) {
	cerr << "URL(): This call is only valid for a remote connection."
	     << endl;
	return "";
    }

    if (ce)
	return _URL + "?" + _proj + _sel;
    else 
	return _URL;
}

string 
Connect::CE()
{
    if (_local) {
	cerr << "CE(): This call is only valid for a remote connection."
	    << endl;
	return "";
    }
    
    return _proj + _sel;
}

DAS &
Connect::das()
{
    assert(!_local);

    return _das;
}

DDS &
Connect::dds()
{
    assert(!_local);

    return _dds;
}

Error &
Connect::error()
{
    return _error;
}





