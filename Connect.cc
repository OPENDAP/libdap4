
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

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

static char rcsid[]={"$Id: Connect.cc,v 1.21 1996/05/31 23:29:30 jimg Exp $"};

#ifdef __GNUG__
#pragma "implemenation"
#endif

#include <stdio.h>
#include <assert.h>

#include <strstream.h>
#include <fstream.h>

#include "config_netio.h"

#include "Connect.h"

const char DODS_PREFIX[]={"dods"};
const int DEFAULT_TIMEOUT = 100; // timeout in seconds

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

    if (SHOW_MSG) 
	cerr << "Request timeout..." << endl;

    HTRequest_kill(request);

    return 0;
}

static ObjectType
get_type(String &value)
{
    if (value == "das")
	return dods_das;
    else if (value == "dds")
	return dods_dds;
    else if (value == "data")
	return dods_data;
    else if (value == "error")
	return dods_error;
    else
	return unknown;
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
    HTLibInit(NAME, VERSION);

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
	HTAlert_add(HTProgress, HT_A_PROGRESS);
	HTAlert_add(HTError_print, HT_A_MESSAGE);
	HTAlert_add(HTPromptUsernameAndPassword, HT_A_USER_PW);
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

	// Copy the access method.
	_method = src._method;

	// Copy the timeout value.
	_tv->tv_sec = src._tv->tv_sec;

	// Open the file for non-truncating update.
	_output = fdopen(dup(fileno(src._output)), "r+");
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
Connect::read_url(String &url)
{
    int status = YES;

    HTRequest *request = HTRequest_new();

    HTRequest_setContext (request, this); // Bind THIS to request 

    HTRequest_setOutputFormat(request, WWW_SOURCE);

    // Set timeout on sockets
    HTEvent_registerTimeout(_tv, request, timeout_handler, NO);

    HTRequest_setAnchor(request, (HTAnchor *)_anchor);

    HTRequest_setOutputStream(request, HTFWriter_new(request, _output, YES));

    status = HTLoadRelative((const char *)url, _anchor, request);
    if (status != YES) {
	if (SHOW_MSG) cerr << "Can't access resource" << endl;
	return false;
    }

    HTRequest_delete(request);

    return status;
}

void
Connect::close_output()
{
    if (_output && _output != stdout) {
	fclose(_output);
	_output = 0;
    }
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
    char *ref = HTParse(name, NULL, PARSE_ALL);
    if (ref) {
	// If there are no current connects, initialize the library
       	if (_connects == 0)
	    www_lib_init();

	_connects++;		// record the connect

	_URL = name;
	_local = false;

	_tv = new timeval;

	_tv->tv_sec = DEFAULT_TIMEOUT;
	_output = 0;

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

// Dereference the URL and dump its contents into _OUTPUT. Note that
// read_url() does the actual dereferencing; this sets up the _OUTPUT sink.

bool
Connect::fetch_url(String &url, bool async = false)
{
    close_output();

    if (!async) {
	char *c = tempnam(NULL, DODS_PREFIX);
	_output = fopen(c, "w+"); // Open truncated for update.
	unlink(c);		// When _OUTPUT is closed file is deleted.
	
	bool status = read_url(url);

	// Now rewind the stream so that we can read from the temp file
	status = fseek(_output, 0L, 0) == 0;
	if (!status) {
	    cerr << "Could not rewind stream" << endl;
	}

	return status;
    }
    else {
	int pid, data[2];

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
	    _output = fdopen(data[0], "r");
	    if (!_output) {
		cerr << "Parent process could not open IPC channel for reading"
		     << endl;
		return false;
	    }
	    return true;
	}
	else {
	    close(data[0]);
	    _output = fdopen(data[1], "w");
	    if (!_output)  {
		cerr << "Child process could not open IPC channel" << endl;
		exit(0);
	    }
	    bool status = read_url(url);
	    if (!status) {
		cerr << "Child process could not read data from the URL"
		     << endl;
		exit(0);
	    }
	    exit(1);		// successful completion
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

ObjectType
Connect::type()
{
    return _type;
}

// Added EXT which defaults to "das". jhrg 3/7/95

bool
Connect::request_das(const String &ext = "das")
{
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
	    
	String correction = _error.correct_error();
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
Connect::request_dds(const String &ext = "dds")
{
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
	    
	String correction = _error.correct_error();
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
Connect::request_data(const String expr, bool async = false, 
		      const String &ext = "dods")
{
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
	    
	String correction = _error.correct_error();
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

    // now arrange to read the data via the appropriate variable.  NB:
    // Since all BaseTypes share I/O, this works. However, it will have
    // to be changed when BaseType is modified to handle several
    // simultaneous reads.

    set_xdrin(_output);

    return d;
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
