
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//	dan		Dan Holloway (dan@hollywood.gso.uri.edu)
//	reza		Reza Nekovei (reza@intcomm.net)

// $Log: Connect.cc,v $
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

static char rcsid[]={"$Id: Connect.cc,v 1.29 1996/06/22 00:00:23 jimg Exp $"};

#ifdef __GNUG__
#pragma "implemenation"
#endif

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

#include <sys/types.h>		// Used by the semaphore code in fetch_url()
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include <expect.h>

#include <strstream.h>
#include <fstream.h>

#define DEBUG2 1		// WWW defines DEBUG; use DEBUG2 here.
#include "debug.h"
#include "Connect.h"
#include "config_dap.h"

// On a sun (4.1.3) these are not prototyped... Maybe other systems too?
#if (HAVE_SEM_PROTO == 0)
extern "C" {
    int semget(key_t key, int nsems, int flag);
    int semctl(int semid, int semnum, int cmd, union semun arg);
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

static const char *dods_root = getenv("DODS_ROOT") ? getenv("DODS_ROOT") 
    : DODS_ROOT;

// Constants used for temporary files.

static const char DODS_PREFIX[]={"dods"};
static const int DEFAULT_TIMEOUT = 100; // timeout in seconds

int Connect::_connects = false;
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

// This function is registered to handle access authentication,
// for example for HTTP

int
authentication_handler(HTRequest *request, int status)
{
    Connect *me = (Connect *)HTRequest_context(request);

    // Ask the authentication module for getting credentials
    if (HTAA_authentication(request) && HTAA_retryWithAuth(request)) {

	// Make sure we do a reload from cache
	HTRequest_setReloadMode(request, HT_FORCE_RELOAD);

	// Log current request
	if (HTLog_isOpen()) 
	    HTLog_add(request, status);

	// Start request with new credentials
	HTLoadAnchor((HTAnchor *) me->_anchor, request);
    } 
    else {
	cerr << "Access denied" << endl;
    }

    //Make sure this is the last callback in the list
    return HT_ERROR;		
}

// This function is registered to handle permanent and temporary
// redirections. Make sure this is the last callback in the list.

int
redirection_handler(HTRequest *request, int status)
{
    bool result = true;
    Connect *me = (Connect *)HTRequest_context(request);
    HTAnchor *new_anchor = HTRequest_redirection(request);

    // Make sure we do a reload from cache
    HTRequest_setReloadMode(request, HT_FORCE_RELOAD);

    // Log current request
    if (HTLog_isOpen()) 
	HTLog_add(request, status);

    // Start new request
    if (HTRequest_retry(request)) {
	result = HTLoadAnchor(new_anchor, request);
    } 
    else {
	cerr << "Too many redirections detected" << endl;
    }

    return HT_ERROR;
}

// This function is registered to handle the result of the request

int
terminate_handler(HTRequest *request, int status) 
{
    Connect *me = (Connect *)HTRequest_context(request);

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
dods_progress (HTRequest * request, HTAlertOpcode op, int msgnum, 
	       const char * dfault, void * input, HTAlertPar * reply)
{
    Connect *me = (Connect *)HTRequest_context(request);
    String cmd;

    if (!request) {
        if (WWWTRACE) TTYPrint(TDEST, "HTProgress.. Bad argument\n");
        return NO;
    }

    switch (op) {
      case HT_PROG_DNS:
	if (!me->gui()->progress_visible())
	    cmd = (String)"progress popup \"Looking up " + (char *)input + "\"\r";
	else
	    cmd = (String)"progress text \"Looking up " + (char *)input + "\"\r";
	
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
        TTYPrint(TDEST, "UNKNOWN PROGRESS STATE\n");
        break;
    }

    return YES;
}

BOOL
dods_username_password (HTRequest * request, HTAlertOpcode op, int msgnum, 
			const char * dfault, void * input, HTAlertPar * reply)
{
    if (!request) {
        if (WWWTRACE) TTYPrint(TDEST, "HTProgress.. Bad argument\n");
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

#include "www-error-msgs.h"

BOOL 
dods_error_print (HTRequest * request, HTAlertOpcode op,
		  int msgnum, CONST char * dfault, void * input,
		  HTAlertPar * reply)
{
    HTList *cur = (HTList *) input;
    HTError *pres;
    HTErrorShow showmask = HTError_show();
    HTChunk *msg = NULL;
    int code;

    if (WWWTRACE) 
	TTYPrint(TDEST, "HTError..... Generating message\n");

    if (!request || !cur) 
	return NO;

    while ((pres = (HTError *) HTList_nextObject(cur))) {
        int index = HTError_index(pres);
        if (HTError_doShow(pres)) {
            if (!msg) {
                HTSeverity severity = HTError_severity(pres);
                msg = HTChunk_new(128);
                if (severity == ERR_WARN)
                    HTChunk_puts(msg, "Warning: ");
                else if (severity == ERR_NON_FATAL)
                    HTChunk_puts(msg, "Non Fatal Error: ");
                else if (severity == ERR_FATAL)
                    HTChunk_puts(msg, "Fatal Error: ");
                else if (severity == ERR_INFO)
                    HTChunk_puts(msg, "Information: ");
                else {
                    if (WWWTRACE)
                        TTYPrint(TDEST, "HTError..... Unknown Classification of Error (%d)...\n", severity);
                    HTChunk_delete(msg);
                    return NO;
                }

                /* Error number */
                if ((code = HTErrors[index].code) > 0) {
                    char buf[10];
                    sprintf(buf, "%d ", code);
                    HTChunk_puts(msg, buf);
                }
            } else
                HTChunk_puts(msg, "\nReason: ");
	    
	    ostrstream os;
	    os << endl << "Could not access `" 
	       << HTAnchor_address((HTAnchor *)HTRequest_anchor(request)) 
		   << "': " << ends;
	    
	    HTChunk_puts(msg, os.str());

            HTChunk_puts(msg, HTErrors[index].msg);         /* Error message */

#if 0
            if (showmask & HT_ERR_SHOW_PARS) {           /* Error parameters */
                int length;
                int cnt;                
                char *pars = (char *) HTError_parameter(pres, &length);
                if (length && pars) {
                    HTChunk_puts(msg, " (");
                    for (cnt=0; cnt<length; cnt++) {
                        char ch = *(pars+cnt);
                        if (ch < 0x20 || ch >= 0x7F)
                            HTChunk_putc(msg, '#');
                        else
                            HTChunk_putc(msg, ch);
                    }
                    HTChunk_puts(msg, ") ");
                }
            }
#endif

            if (showmask & HT_ERR_SHOW_LOCATION) {         /* Error Location */
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
header_handler(HTRequest *request, const char *token)
{
    String field, value;
    istrstream line(token);
    line >> field; field.downcase();
    line >> value; value.downcase();
    
    if (field == "content-description:") {
	DBG2(cerr << "Found content-description header" << endl);
	Connect *me = (Connect *)HTRequest_context(request);
	me->_type = get_type(value);
    }
#if 0
    // Parsed by HTMIME.c (wwwlib).
    else if (field == "content-encoding:") {
	DBG(cerr << "Found content-encoding header" << endl);
	Connect *me = (Connect *)HTRequest_context(request);
	me->_encoding = get_encoding(value);
    }
#endif
    else {
	if (SHOW_MSG)
	    cerr << "Unknown header: " << token << endl;
    }

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

    // Initiate W3C Reference Library
    HTLibInit(CNAME, CVER);

    // Initialize the protocol modules
    HTProtocol_add("http", YES, HTLoadHTTP, NULL);
    HTProtocol_add("file", YES, HTLoadFile, NULL);

    // Initialize set of converters

    _conv = HTList_new();

    // GENERIC converters

    HTConversion_add(_conv,"multipart/*", "*/*", HTBoundary, 1.0, 0.0, 0.0);
    HTConversion_add(_conv,"message/rfc822", "*/*", HTMIMEConvert, 
		     1.0, 0.0, 0.0);
    HTConversion_add(_conv,"text/x-http", "*/*", HTTPStatus_new,
		     1.0, 0.0, 0.0);

    HTFormat_setConversion(_conv);

    // Initialize bindings between file suffixes and media types
    HTFileInit();

    // Get any proxy or gateway environment variables
    HTProxy_getEnvVar();

#ifdef CATCH_SIG
    SetSignal();
#endif

    HTAlert_setInteractive(YES);

    // Register our User Prompts etc in the Alert Manager
    if (HTAlert_interactive()) {
	HTAlert_add(dods_progress, HT_A_PROGRESS);
	HTAlert_add(dods_error_print, HT_A_MESSAGE);
	HTAlert_add(dods_username_password, HT_A_USER_PW);
    }

    // Register a call back function for the Net Manager
    HTNetCall_addBefore(HTLoadStart, 0);
    HTNetCall_addAfter(authentication_handler, HT_NO_ACCESS);
    HTNetCall_addAfter(redirection_handler, HT_PERM_REDIRECT);
    HTNetCall_addAfter(redirection_handler, HT_TEMP_REDIRECT);
    HTNetCall_addAfter(terminate_handler, HT_ALL);
    
    // Register our own MIME header handler for extra headers
    HTHeader_addParser("*", NO, header_handler);
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
    unlink(c);
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

	// Set timeout on sockets
    HTEvent_registerTimeout(_tv, _request, timeout_handler, NO);

    HTRequest_setAnchor(_request, (HTAnchor *)_anchor);

    HTRequest_setOutputStream(_request, HTFWriter_new(_request, stream, YES));

    status = HTLoadRelative((const char *)url, _anchor, _request);

    if (status != YES) {
	if (SHOW_MSG) cerr << "Can't access resource" << endl;
	return false;
    }

    // LoadRelative uses a different anchor than the one bound to the request
    // in this function. Extract what you need from the anchor used in that
    // call.
    HTEncoding enc = HTAnchor_encoding(HTRequest_anchor(_request));
    _encoding = get_encoding((char *)HTAtom_name(enc));

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

    if (_source)
	delete_xdrstdio(_source);
}

// This ctor is decalred private so that it won't ever be called by users,
// thus forcing them to create Connects which point somewhere.

Connect::Connect()
{
    assert(false);
}

// public mfuncs

Connect::Connect(const String &name)
{
    _gui = new Gui;
    char *ref = HTParse(name, NULL, PARSE_ALL);
    if (ref) {
	// If there are no current connects, initialize the library
       	if (_connects == 0)
	    www_lib_init();

	_connects++;		// Record the connect.

	_URL = name;
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
    // Release resources for this object.
    HTAnchor_delete(_anchor);
    delete _tv;

    if (_logfile != "") 
	HTLog_close();

    close_output();

    _connects--;
    
    // If this is the last Connect, close WWW Library.
    if (_connects == 0) {
	HTList_delete(_conv);
	HTLibTerminate();
    }
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

static FILE *
decompress(FILE *input)
{
    int pid, data[2];

    if (pipe(data) < 0) {
	cerr << "Could not create IPC channel for decompresser process" 
	     << endl;
	return NULL;
    }
    
    if ((pid = fork()) < 0) {
	cerr << "Could not fork to create decompresser process" << endl;
	return NULL;
    }

    // The parent process closes the write end of the Pipe, and creates a
    // FILE * using fdopen(). The FILE * is used by the calling program to
    // access the read end of the Pipe.

    if (pid > 0) {
	close(data[1]);
	FILE *output = fdopen(data[0], "r");
	if (!output) {
	    cerr << "Parent process could not open channel for decompression"
		 << endl;
	    return NULL;
	}
	return output;
    }
    else {
	close(data[0]);
	dup2(fileno(input), 0);	// Read from FILE *input 
	dup2(data[1], 1);	// Write to the pipe

	DBG2(cerr << "Opening decompression stream." << endl);

	// First try to run gzip using DODS_ROOT (the value read from the
	// DODS_ROOT environment variable takes precedence over the value set
	// at build time. If that fails, try the users PATH.
	String gzip = (String)dods_root + "/etc/gzip";
	(void) execl(gzip, "gzip", "-cd", NULL);

	(void) execlp("gzip", "gzip", "-cd", NULL);

	cerr << "Could not start decompresser!" << endl;
	cerr << "gzip must be in DODS_ROOT/etc or on your PATH" << endl;
	_exit(127);		// Only get here if an error
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
	unlink(c);		// When _OUTPUT is closed file is deleted.
	
	if (!read_url(url, stream))
	    return false;

	// Now rewind the stream so that we can read from the temp file
	if (fseek(stream, 0L, 0) < 0) {
	    cerr << "Could not rewind stream" << endl;
	    return false;
	}

	if (encoding() == x_gzip) {
	    _output = decompress(stream);
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
	int semaphore = semget(IPC_PRIVATE, 1, 0);
	if (semaphore < 0) {
	    cerr << "Could not create semaphore." << endl;
	    return false;
	}
	union semun semun;
	semun.val = -1;		// Wait for 0 value... 
	if (semctl(semaphore, 0, SETVAL, semun) < 0) {
	    cerr << "Could not initialize semaphore." << endl;
	    return false;
	}
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

	    // Semaphore wait for completion of read_url(...) in child.
	    struct sembuf sembuf[1] = {0, 0, SEM_UNDO};
	    if (semop(semaphore, sembuf, 1) < 0) {
		cerr << "Could not wait for child to read URL." << endl;
		return false;
	    }

	    if (encoding() == x_gzip) {
		_output = decompress(stream);
		if (!_output)
		    return false;
	    }
	    else {
		// Since we are reading asynchronously, do not rewind stream.
		_output = stream;
	    }

	    return true;
	}
	else {
	    close(data[0]);
	    _output = fdopen(data[1], "w");
	    if (!_output)  {
		cerr << "Child process could not open IPC channel" << endl;
		_exit(127);
	    }
	    bool status = read_url(url, _output);

	    // Set the semaphore so that the parent will stop blocking
	    struct sembuf sembuf[1] = {0, 1, SEM_UNDO};
	    if (semop(semaphore, sembuf, 1) < 0) {
		cerr << "Could not wait for child to read URL." << endl;
		return false;
	    }

	    if (!status) {
		cerr << "Child process could not read data from the URL"
		     << endl;
		_exit(127);
	    }
	    exit(0);		// successful completion
	}
    }
}


FILE *
Connect::output()
{
    if (_output && _output != stdout) {
	FILE *ret_val = fdopen(dup(fileno(_output)), "r");
	if (!ret_val) {
	    cerr << "Could not duplicate the object's output sink." << endl;
	    return NULL;
	}
	
	// Close the object's stream pointer so that when the user closes the
	// copy returned here the stream goes away. 
	close_output();

	return ret_val;
    }
    else
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

    String das_url = _URL + "." + ext;
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

    String dds_url = _URL + "." + ext;
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

DDS &
Connect::request_data(const String expr, bool gui_p = true, 
		      bool async = false, const String &ext = "dods")
{
    (void)gui()->show_gui(gui_p);

    String data_url = _URL + "." + ext + "?" + expr;
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
    // If asynchronous, just return the DDS and leave the reading to t he
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
    if (!_data.empty() && p)
	return _data(p)._expression;
}

DDS &
Connect::constraint_dds(Pix p)
{
    if (!_data.empty() && p)
	return _data(p)._dds;
}

DDS &
Connect::append_constraint(String expr, DDS &dds)
{
    constraint c(expr, dds);

    return _data.rear()._dds;
}
