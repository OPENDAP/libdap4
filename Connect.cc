
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//	dan		Dan Holloway (dan@hollywood.gso.uri.edu)
//	reza		Reza Nekovei (reza@intcomm.net)

// $Log: Connect.cc,v $
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

static char rcsid[] __unused__ ={"$Id: Connect.cc,v 1.40 1996/11/22 00:14:38 jimg Exp $"};

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

#include <strstream.h>
#include <fstream.h>

#include "debug.h"
#include "Connect.h"

#define SHOW_MSG (WWWTRACE || HTAlert_interactive())

#if defined(__svr4__)
#define CATCH_SIG
#endif

#define USE_SEM 0

#if USE_SEM
// On a sun (4.1.3) these are not prototyped... Maybe other systems too?
#if (HAVE_SEM_PROTO == 0)
extern "C" {
    int semget(key_t key, int nsems, int flag);
    int semctl(int semid, int semnum, int cmd, ...);
    int semop(int semid, struct sembuf sb[], size_t nops);
}
#endif

// osf-3.0 fails to define this in <sys/sem.h>
#if (HAVE_SEM_UNION == 0)
union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};
#endif

const int SEM_KEY = 123456L;
const int SEM_PERMS = 0666;
#endif /* USE_SEM */

// In cases where the DODS libraries are not linked with g++ this code won't
// work properly; dods_root may be NULL or (worse) undefined. jhrg 9/19/96.

static const char *dods_root = getenv("DODS_ROOT") ? getenv("DODS_ROOT") 
    : DODS_ROOT;

// Constants used for temporary files.

static const char DODS_PREFIX[]={"dods"};
static const int DEFAULT_TIMEOUT = 100; // Timeout in seconds.
static int keep_temps = DODS_KEEP_TEMP;	// Non-zero to keep temp files.

// Initially, _connects is -1 to indicate that no connection has been made
// (and thus that the WWW library has not yet been inititalize). Once
// _connects is > -1 there is no need to initialize the WWW library.

int Connect::_connects = -1;
String Connect::_logfile = "";
HTList *Connect::_conv = 0;

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
terminate_handler (HTRequest * request, HTResponse * /*response*/,
		   void * /*param*/, int status) 
{
    if (status != HT_LOADED) {
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf) (*cbf)(request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
			HTRequest_error(request), NULL);
    }

    if (HTLog_isOpen()) 
	HTLog_add(request, status);

    return HT_OK;
}

// This function is registered to handle timeout in select eventloop

int 
timeout_handler(HTRequest *request)
{
    Connect *me = (Connect *)HTRequest_context(request);
    String cmd;

    if (!me->gui()->progress_visible())
	cmd = (String)"progress popup \"Request timeout...\"\r";
    else
	cmd = (String)"progress text \"Request timeout...\"\r";
	
    me->gui()->progress_visible(me->gui()->command(cmd));

    if (me->gui()->progress_visible()) {
	sleep(3);
	cmd = (String)"progress popdown\r";
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
    String cmd;

    switch (op) {
      case HT_PROG_DNS:
	if (!me->gui()->progress_visible())
	    cmd = (String)"progress popup \"Looking up " + (char *)input 
	          + "\"\r";
	else
	    cmd = (String)"progress text \"Looking up " + (char *)input 
	          + "\"\r";
	
	me->gui()->progress_visible(me->gui()->command(cmd));
        break;

      case HT_PROG_CONNECT:
	if (!me->gui()->progress_visible())
	    cmd = (String)"progress popup \"Contacting host...\"\r";
	else
	    cmd = (String)"progress text \"Contacting host...\"\r";
	
	me->gui()->progress_visible(me->gui()->command(cmd));
        break;

      case HT_PROG_ACCEPT:
	if (!me->gui()->progress_visible())
	    cmd = (String)"progress popup \"Waiting for connection...\"\r";
	else
	    cmd = (String)"progress text \"Waiting for connection...\"\r";
	
	me->gui()->progress_visible(me->gui()->command(cmd));
        break;

      case HT_PROG_READ: {
	  if (!me->gui()->progress_visible())
	      cmd = (String)"progress popup \"Reading...\"\r";
	  else
	      cmd = (String)"progress text \"Reading...\"\r";
	  me->gui()->progress_visible(me->gui()->command(cmd));
	  
	  if (!me->gui()->progress_visible()) // Bail if window won't popup
	      break;

	  long cl = HTAnchor_length(HTRequest_anchor(request));
	  if (cl >= 0) {
	      long b_read = HTRequest_bytesRead(request);
	      double pro = (double) b_read/cl*100;
	      ostrstream cmd_s;
	      cmd_s << "progress bar " << pro << "\r" << ends;
	      (void)me->gui()->command(cmd_s.str());
	      cmd_s.freeze(0);
	  }
	  else {
	      cmd = (String)"progress bar -1\r";
	      (void)me->gui()->command(cmd);
	  }
	  
	  break;
      }

      case HT_PROG_WRITE:
	// DODS *NEVER* writes. Ever. Well, it does write the request header...
	break;

      case HT_PROG_DONE:
	cmd = (String)"progress popdown\r";
	me->gui()->command(cmd);
	me->gui()->progress_visible(false);
        break;

      case HT_PROG_WAIT:
	if (!me->gui()->progress_visible())
	    cmd = (String)"progress popup \"Waiting for free socket...\"\r";
	else
	    cmd = (String)"progress text \"Waiting for free socket...\"\r";
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
    String cmd = "password\r";
    String response;

    if (!me->gui()->response(cmd, response))
	return NO;

    // Extract two words from RESPONSE; #1 is the username and #2 is the
    // password. Either may be missing, in which case return NO.

    String words[2];
    if (split(response, words, 2, RXwhite) != 2) {
	DBG2(cerr << "Wrong number of words in response: " << response \
	     << endl);
	return NO;
    }

    HTAlert_setReplyMessage(reply, (const char *)words[0]);
    HTAlert_setReplySecret(reply, (const char *)words[1]);

    return YES;
}

static HTErrorMessage HTErrors[HTERR_ELEMENTS] = {HTERR_ENGLISH_INITIALIZER};

BOOL 
dods_error_print (HTRequest * request, HTAlertOpcode /* op */,
		  int /* msgnum */, CONST char * /* dfault */, void * input,
		  HTAlertPar * /* reply */)
{
    HTList *cur = (HTList *) input;
    HTError *pres;
    HTErrorShow showmask = HTError_show();
    HTChunk *msg = NULL;
    int code;

    if (WWWTRACE) cerr << "dods_error_print: Generating message" << endl;

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
	String command = (String)"dialog \"" + (char *)HTChunk_data(msg) 
	    + "\" error\r";
	String response;	// Not used
	Connect *me = (Connect *)HTRequest_context(request);

	if (me->gui()->show_gui()) {
	    if (!me->gui()->response(command, response))
		cerr << "GUI Failure in dods_error_print()" << endl;
	}
	else
	    cerr << (char *)HTChunk_data(msg) << endl;

        HTChunk_delete(msg);
    }

    return YES;
}

static ObjectType
get_type(String value)
{
    if (value == "dods_das")
	return dods_das;
    else if (value == "dods_dds")
	return dods_dds;
    else if (value == "dods_data")
	return dods_data;
    else if (value == "dods_error")
	return dods_error;
    else
	return unknown_type;
}
    
static EncodingType
get_encoding(String value)
{
    if (value == "x-plain")
	return x_plain;
    else if (value == "x-gzip")
	return x_gzip;
    else
	return unknown_enc;
}
    
// This function is registered to handle unknown MIME headers

int 
description_handler(HTRequest *request, HTResponse *response, 
		    const char *token, const char *val)
{
    String field = token, value = val;
    field.downcase();
    value.downcase();
    
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
encoding_handler(HTRequest *request, HTResponse *response, const char *token,
		 const char *val)
{
    String field = token, value = val;
    field.downcase();
    value.downcase();
    
    if (field == "content-encoding") {
	DBG(cerr << "Found content-encoding header" << endl);
	Connect *me = (Connect *)HTRequest_context(request);
	me->_encoding = get_encoding(value);
    }
    else {
	if (SHOW_MSG)
	    cerr << "Unknown header: " << token << endl;
    }

    return HT_OK;
}

int 
header_handler(HTRequest *, HTResponse *, const char *token, const char *val)
{
    String field = token, value = val;
    field.downcase();
    value.downcase();
    
    cerr << "Unknown header: " << token << ": " << value <<endl;

    return HT_OK;
}
 

void
Connect::www_lib_init()
{
    // Starts Mac GUSI socket library
#ifdef GUSI
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef __MWERKS__
    InitGraf((Ptr) &qd.thePort); 
    InitFonts(); 
    InitWindows(); 
    InitMenus(); TEInit(); 
    InitDialogs(nil); 
    InitCursor();
    SIOUXSettings.asktosaveonclose = false;
#endif

    // Initiate W3C Reference Library with a client profile.
    HTProfile_newPreemptiveClient(CNAME, CVER);

    // Add progress notification, etc.
    HTAlert_add(dods_progress, HT_A_PROGRESS);
    HTAlert_add(dods_error_print, HT_A_MESSAGE);
    HTAlert_add(dods_username_password, HT_A_USER_PW);

    HTError_setShow(HT_ERR_SHOW_FATAL);

    // Add our own filter to update the history list.
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    // We add our own parsers for content-description and -encoding so that
    // we can test for these fields and operate on the resulting document
    // without using the stream stack mechanism (which seems to be very
    // complicated). jhrg 11/20/96
    HTHeader_addParser("content-description", NO, description_handler);
    HTHeader_addParser("content-encoding", NO, encoding_handler);
    HTHeader_addRegexParser("*", NO, header_handler);
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
	char *ref = HTParse(_URL, (char *)0, PARSE_ALL);
	_anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
	HT_FREE(ref);

	_type = src._type;
	_encoding = src._encoding;

	// Copy the access method.
	_method = src._method;

	// Copy the timeout value.
	_tv->tv_sec = src._tv->tv_sec;

	// Open the file for non-truncating update.
	if (_output)
	    _output = fdopen(dup(fileno(src._output)), "r+");
	if (_source)
	    _source = new_xdrstdio(_output, XDR_DECODE);
    }
}

// Read the DDS from the data stream. Leave the binary information behind. The
// DDS is moved, without parsing it, into a file and a pointer to that FILE is
// returned. The argument IN (the input FILE stream) is positioned so that the
// next byte is the binary data.
//
// The binary data follows the text `Data:', which itself starts a line.
//
// Returns: a FILE * which contains the DDS describing the binary information
// in IF.

FILE *
Connect::move_dds(FILE *in)
{

    char *c = tempnam(NULL, DODS_PREFIX);
    if (!c) {
	cerr << "Could not create temporary file name: " << strerror(errno)
	    << endl;
	return NULL;
    }

    FILE *fp = fopen(c, "w+");
    if (!keep_temps)
	unlink(c);
    else
	cerr << "Temporary file for Data Document DDS: " << c << endl;

    if (!fp) {
	cerr << "Could not open anonymous temporary file: " 
	     << strerror(errno) << endl;
	return NULL;
    }

    int data = FALSE;
    char s[256], *sp;
    
    sp = &s[0];
    while (!feof(in) && !data) {
	sp = fgets(s, 255, in);
	if (strcmp(s, "Data:\n") == 0)
	    data = TRUE;
	else
	    fputs(s, fp);
    }

    if (fseek(fp, 0L, 0) < 0) {
	cerr << "Could not rewind data DDS stream: " << strerror(errno)
	    << endl;
	return NULL;
    }
    
    free(c);			// tempnam uses malloc
    return fp;
}

// Use the URL designated when the Connect object was created as the
// `base' URL so that the formal parameter to this mfunc can be relative.

bool
Connect::read_url(String &url, FILE *stream)
{
    assert(stream);

    int status = YES;

    HTRequest *_request = HTRequest_new();

    HTRequest_setContext (_request, this); // Bind THIS to request 

    HTRequest_setOutputFormat(_request, WWW_SOURCE);

    // Set timeout on sockets.
    HTEventrg_registerTimeout(_tv, _request, timeout_handler, NO);

    HTRequest_setAnchor(_request, (HTAnchor *)_anchor);

    HTRequest_setOutputStream(_request, HTFWriter_new(_request, stream, YES));

    status = HTLoadRelative((const char *)url, _anchor, _request);

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

Connect::Connect(String name)
{
    _gui = new Gui;
    char *access_ref = HTParse(name, NULL, PARSE_ACCESS);
    if (strcmp(access_ref, "http") == 0) { // access == http --> remote access
	// If there are no current connects, initialize the library
       	if (_connects == -1) {
	    www_lib_init();
	    _connects = 1;
	}
	else {
	    _connects++;		// Record the connect.
	}

	if (name.contains("?")) {
	    _URL = name.before("?");
	    String expr = name.after("?");
	    if (expr.contains("&")) {
		_proj = expr.before("&");
		_sel = expr.after(_proj);
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

	_tv = new timeval;

	_tv->tv_sec = DEFAULT_TIMEOUT;
	_output = 0;
	_source = 0;
	_type = unknown_type;
	_encoding = unknown_enc;

	char *ref = HTParse(_URL, (char *)0, PARSE_ALL);
	_anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
	HT_FREE(ref);
    }
    else {
	_URL = "";
	_local = true;
    }

    HT_FREE(access_ref);
}

Connect::Connect(const Connect &copy_from)
{
    _tv = new timeval;

    clone(copy_from);
    
    if (!_local) {
	// if COPY_FROM is remote, then there must be at least one connect
	if (_connects == 0)	
	    assert(false);

	_connects++;		// record the connect
    }
}

Connect::~Connect()
{
    DBG2(cerr << "Entering the Connect dtor" << endl);

    // Release resources for this object.
    delete _tv;

    _connects--;
    
    // If this is the last connect, close the log file.
    if (_connects == 0 &&_logfile != "") 
	HTLog_close();

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

// Dereference the URL and dump its contents into _OUTPUT. Note that
// read_url() does the actual dereferencing; this sets up the _OUTPUT sink.

bool
Connect::fetch_url(String &url, bool async = false)
{
    close_output();
    _encoding = unknown_enc;
    _type = unknown_type;

    if (!async) {
	char *c = tempnam(NULL, DODS_PREFIX);
	FILE *stream = fopen(c, "w+"); // Open truncated for update.
	if (!keep_temps)
	    unlink(c);		// When _OUTPUT is closed file is deleted.
	else
	    cerr << "Temporary file for Data document: " << c << endl;

	if (!read_url(url, stream))
	    return false;

	// Now rewind the stream so that we can read from the temp file
	if (fseek(stream, 0L, 0) < 0) {
	    cerr << "Could not rewind stream" << endl;
	    return false;
	}

	if (encoding() == x_gzip) {
	    _output = decompressor(stream);
	    if (!_output)
		return false;
	}
	else {
	    _output = stream;
	}

	return true;
    }
    else {
	int pid, data[2];
#if USE_SEM
	int semaphore = semget(SEM_KEY, 1, IPC_CREAT | SEM_PERMS);
	if (semaphore < 0) {
	    cerr << "Could not create semaphore." << endl;
	    return false;
	}
	union semun semun;
	semun.val = 1;		// Set to 1, wait for child to set to 0.
	if (semctl(semaphore, 0, SETVAL, semun) < 0) {
	    cerr << "Could not initialize semaphore." << endl;
	    return false;
	}
#endif

	if (pipe(data) < 0) {
	    cerr << "Could not create IPC channel for receiver process" 
		 << endl;
	    return false;
	}
    
	if ((pid = fork()) < 0) {
	    cerr << "Could not fork to create receiver process" << endl;
	    return false;
	}

	// The parent process closes the write end of the Pipe, and creates a
	// FILE * using fdopen(). The FILE * is used by the calling program to
	// access the read end of the Pipe.

	if (pid > 0) {
	    close(data[1]);
	    FILE *stream = fdopen(data[0], "r");
	    if (!stream) {
		cerr << "Parent process could not open IPC channel for reading"
		     << endl;
		return false;
	    }

#if USE_SEM
	    // Semaphore wait for completion of read_url(...) in child.
	    struct sembuf sembuf[1] = {{0, 0, SEM_UNDO}};
	    if (semop(semaphore, sembuf, 1) < 0) {
		cerr << "Could not wait for child to read URL." << endl;
		return false;
	    }
#endif
	    if (encoding() == x_gzip) {
		_output = decompressor(stream);
		if (!_output)
		    return false;
	    }
	    else {
		// Since we are reading asynchronously, do not rewind stream.
		_output = stream;
	    }

#if USE_SEM
	    // Remove the semaphore.
	    // SEMUN is q dummy parameter in this call.
	    if (semctl(semaphore, 0, IPC_RMID, semun) < 0) {
		cerr << "Could not initialize semaphore." << endl;
		return false;
	    }
#endif
	    
	    return true;
	}
	else {
	    close(data[0]);
	    _output = fdopen(data[1], "w");
	    if (!_output)  {
		cerr << "Child process could not open IPC channel" << endl;
		_exit(127);
	    }

#if USE_SEM
	    // Set the semaphore so that the parent will stop blocking
	    struct sembuf sembuf[1] = {{0, -1, SEM_UNDO}};
	    if (semop(semaphore, sembuf, 1) < 0) {
		cerr << "Could not decrement semaphore in child." << endl;
		return false;
	    }
#endif

	    bool status = read_url(url, _output);
	    if (!status) {
		cerr << "Child process could not read data from the URL"
		     << endl;
		_exit(127);
	    }

	    exit(0);		// successful completion
	}
    }
}

// Remove the duplication of _connect and subsequent close(). This would hav
// the side-effect of spuriously closing the data channel. jhrg 7/16/96

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

// Added EXT which defaults to "das". jhrg 3/7/95

bool
Connect::request_das(bool gui_p = false, const String &ext = "das")
{
    (void)gui()->show_gui(gui_p);

    String das_url = _URL + "." + ext + "?" + _proj + _sel;
    bool status = false;
    String value;

    status = fetch_url(das_url);

    if (!status) {
	goto exit;
    }

    if (type() == dods_error) {
	if (!_error.parse(_output)) {
	    cerr << "Could not parse error object" << endl;
	    status = false;
	    goto exit;
	}
	    
	String correction = _error.correct_error(gui());
	// put the test for various error codes here
	status = false;
    }
    else
	status = _das.parse(_output); // read and parse the das from a file 

exit:
    close_output();
    return status;
}

// Added EXT which deafults to "dds". jhrg 3/7/95

bool
Connect::request_dds(bool gui_p = false, const String &ext = "dds")
{
    (void)gui()->show_gui(gui_p);

    String dds_url = _URL + "." + ext + "?" + _proj + _sel;
    bool status = false;

    status = fetch_url(dds_url);

    if (!status)
	return false;
    
    if (type() == dods_error) {
	if (!_error.parse(_output)) {
	    cerr << "Could not parse error object" << endl;
	    status = false;
	    goto exit;
	}
	    
	String correction = _error.correct_error(gui());
	// put the test for various error codes here
	status = false;
    }
    else
	status = _dds.parse(_output); // read and parse the dds from a file 

exit:
    close_output();
    return status;
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

DDS &
Connect::request_data(String expr, bool gui_p = true, 
		      bool async = false, const String &ext = "dods")
{
    (void)gui()->show_gui(gui_p);

    String proj, sel;
    if (expr.contains("&")) {
      proj = expr.before("&");
      sel = expr.after(proj);
    }
    else {
      proj = expr;
      sel = "";
    }

    String data_url = _URL + "." + ext + "?" + _proj + proj + _sel + sel;
    bool status = fetch_url(data_url, async);
	
    if (!status) {
	cerr << "Could not complete data request operation" << endl;
	exit(1);
    }

    if (type() == dods_error) {
	if (!_error.parse(_output)) {
	    cerr << "Could not parse error object" << endl;
	    exit(1);
	}
	    
	String correction = _error.correct_error(gui());
	// put the test for various error codes here
	exit(1);		// improve this!!
    }
      
    // First read the DDS into a new object.

    DDS dds;
    FILE *dds_fp = move_dds(_output);
    if (!dds_fp || !dds.parse(dds_fp)) {
	cerr << "Could not parse data DDS." << endl;
	exit(1);
    }
    fclose(dds_fp);

    // Save the newly created DDS (which now has a variable (BaseType *)
    // that can hold the data) along with the constraint expression in a
    // list.

    DDS &d = append_constraint(expr, dds);

    // If the transmission is synchronous, read all the data into the DDS D.
    // If asynchronous, just return the DDS and leave the reading to to he
    // caller. 
    if (!async) {
	XDR *s = source();
	for (Pix q = d.first_var(); q; d.next_var(q))
	    if (!d.var(q)->deserialize(s))
		break;
    }

    return d;
}

Gui *
Connect::gui()
{
    return _gui;
}

bool
Connect::is_local()
{
    return _local;
}

String
Connect::URL(bool ce = true)
{
    if (_local) {
	cerr << "URL(): This call is only valid for a remote connection."
	     << endl;
	return "";
    }

    if (ce)
	return _URL;
    else 
	return _URL.before("?");
}

String 
Connect::CE()
{
    if (_local) {
	cerr << "CE(): This call is only valid for a remote connection."
	    << endl;
	return "";
    }
    
    return _URL.after("?");
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

Pix 
Connect::first_constraint()
{
    return _data.first();
}

void
Connect::next_constraint(Pix &p)
{
    if (!_data.empty() && p)
	_data.next(p);
}

String
Connect::constraint_expression(Pix p)
{
    assert(!_data.empty() && p);

    return _data(p)._expression;
}

DDS &
Connect::constraint_dds(Pix p)
{
    assert(!_data.empty() && p);

    return _data(p)._dds;
}

DDS &
Connect::append_constraint(String expr, DDS &dds)
{
    constraint c(expr, dds);

    _data.append(c);

    return _data.rear()._dds;
}
