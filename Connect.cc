
// (c) COPYRIGHT URI/MIT 1994-2001
// Please read the full copyright statement in the file COPYRIGHT.
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
    { "$Id: Connect.cc,v 1.118 2002/06/18 15:36:24 tom Exp $" };

#ifdef GUI
#include "Gui.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <signal.h>
#include <assert.h>
#include <errno.h>

#if (__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif

#include <fstream>

#include "debug.h"
#include "DataDDS.h"
#include "Connect.h"
#include "escaping.h"
#include "RCReader.h"

#define SHOW_MSG (WWWTRACE || HTAlert_interactive())
#define DODS_KEEP_TEMP 0	// set to 1 for debugging

#if defined(__svr4__)
#define CATCH_SIG
#endif

using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
#ifdef WIN32
using std::iterator;
using std::vector<string>;
#else
using std::vector;
#endif

/*  Same under UNIX and win32 because libwww with       */
/*  cache: and file: protocols don't require backslash, */
/*  even under win32.  In fact, using the backslash     */
/*  will bust client-side caching because of how '\'    */
/*  makes .dodsrc get misinterpreted.                   */
#define DIR_SEP_STRING	"/"
#define DIR_SEP_CHAR	'/'

// Constants used for temporary files.

static const int DEFAULT_TIMEOUT = 100;		// Timeout in seconds.
int keep_temps = DODS_KEEP_TEMP;	// Non-zero to keep temp files.

static const char bad_decomp_msg[] =
    { "The data returned by the server was compressed and the\n\
decompression program failed to start. Please report this\n\
error to the data server maintainer or to support@unidata.ucar.edu"
};

HTList *Connect::_conv = 0;
int Connect::_num_remote_conns = 0;
bool Connect::_cache_enabled = 0;
char *Connect::_cache_root = 0;

// Constants used by the cache. 
// NB: NEVER_DEFLATE: I added this (12/1/99 jhrg) because libwww 5.2.9 cannot
// process compressed (i.e., deflated) documents in the cache. Users must be
// able to choose whether they want compressed data that will always be
// refreshed or uncompressed data that will be cached. When set this flag
// overrides the value passed into the Connect object's constructor. This
// gives users control over the value. Previously, this could only be set by
// the program that called Connect(...). 
// Note that I've now (4/6/2000 jhrg) fixed libwww so this parameter is no
// longer needed.
static const int
 DODS_USE_CACHE = 1;		// 0- Disabled 1- Enabled
static const int
 DODS_CACHE_MAX = 20;		// Max cache size in Mbytes
static const int
 DODS_CACHED_OBJ = 5;		// Max cache entry size in Mbytes
static const int
 DODS_IGN_EXPIRES = 0;		// 0- Honor expires 1- Ignore them
static const int
 DODS_NEVER_DEFLATE = 0;	// 0- allow deflate, 1- disallow
static const int
 DODS_DEFAULT_EXPIRES = 86400;	// 24 hours in seconds
static const int DODS_ALWAYS_VALIDATE = 0; // Let libwww decide by default.

// This function is registered to handle the result of the request

int
http_terminate_handler(HTRequest * request, HTResponse * /*response */ ,
		       void * /*param */ , int status)
{
    if (WWWTRACE)
	HTTrace("DODS........ HTTP Terminate handler.\n");

    if (status != HT_LOADED) {
	HTAlertCallback *cbf = HTAlert_find(HT_A_MESSAGE);
	if (cbf)
	    (*cbf) (request, HT_A_MESSAGE, HT_MSG_NULL, NULL,
		    HTRequest_error(request), NULL);
    }

    return HT_OK;
}

// This function is registered to handle timeout in select eventloop

int 
timeout_handler(HTRequest * request)
{
    if (WWWTRACE)
	HTTrace("DODS........ Timeout handler.\n");

#ifdef GUI
    Connect *me = (Connect *) HTRequest_context(request);
    string cmd;

    if (!me->_gui->progress_visible())
	cmd = (string) "popup \"Request timeout...\"";
    else
	cmd = (string) "text \"Request timeout...\"";

    me->_gui->progress_visible(me->_gui->command(cmd));

    if (me->_gui->progress_visible()) {
	sleep(3);
	cmd = (string) "popdown";
	me->_gui->command(cmd);
	me->_gui->progress_visible(false);
    }
#endif

    HTRequest_kill(request);

    return 0;
}

// Use the GUI to report progress to the user.

BOOL 
dods_progress(HTRequest * request, HTAlertOpcode op,
	      int /* msgnum */ ,
	      const char * /* dfault */ , void *input,
	      HTAlertPar * /* reply */ )
{
    if (WWWTRACE)
	HTTrace("DODS........ Progress display handler.\n");

    if (!request) {
	if (WWWTRACE)
	    HTTrace("DODS........ dods_progress: NULL Request.\n");
	return YES;
    }

#ifdef GUI
    Connect *me = (Connect *) HTRequest_context(request);
    string cmd;
    int usr_cancel = 0;

    switch (op) {
    case HT_PROG_DNS:
	if (!me->_gui->progress_visible())
	    cmd = (string) "popup \"Looking up " + (char *) input + "\"";
	else
	    cmd = (string) "text \"Looking up " + (char *) input + "\"";

	me->_gui->progress_visible(me->_gui->command(cmd));
	break;

    case HT_PROG_CONNECT:
	if (!me->_gui->progress_visible())
	    cmd = (string) "popup \"Contacting host...\"";
	else
	    cmd = (string) "text \"Contacting host...\"";

	me->_gui->progress_visible(me->_gui->command(cmd));
	break;

    case HT_PROG_ACCEPT:
	if (!me->_gui->progress_visible())
	    cmd = (string) "popup \"Waiting for connection...\"";
	else
	    cmd = (string) "text \"Waiting for connection...\"";

	me->_gui->progress_visible(me->_gui->command(cmd));
	break;

    case HT_PROG_READ:
	{
	    if (!me->_gui->progress_visible())
		cmd = (string) "popup \"Reading...\"";
	    else
		cmd = (string) "text \"Reading...\"";
	    me->_gui->progress_visible(me->_gui->command(cmd));

	    if (!me->_gui->progress_visible())	// Bail if window won't popup
		break;

	    long cl = HTAnchor_length(HTRequest_anchor(request));
	    double pro = -1.0;
	    if (cl >= 0) {
		long b_read = HTRequest_bodyRead(request);
		pro = (double) b_read / cl * 100;
	    }
	    (void) me->_gui->percent_bar(pro, &usr_cancel);
	    if (usr_cancel == 1) {	// the usr wants to bail
		if (WWWTRACE)
		    HTTrace("DODS........ Killing the request.\n");
		if (request)
		    HTRequest_kill(request);
	    }

	    break;
	}

    case HT_PROG_WRITE:
	// DODS *NEVER* writes. Ever. Well, it does write the request header...
	break;

    case HT_PROG_DONE:
	cmd = (string) "popdown";
	me->_gui->command(cmd);
	me->_gui->progress_visible(false);
	break;

    case HT_PROG_INTERRUPT:
	if (!me->_gui->progress_visible())
	    cmd = (string) "popup \"Request interrupted.\"";
	else
	    cmd = (string) "text \"Request interrupted.\"";
	me->_gui->progress_visible(me->_gui->command(cmd));
	break;

    case HT_PROG_OTHER:
	if (!me->_gui->progress_visible())
	    cmd = (string) "popup \"Message: " + (char *) input + ".\"";
	else
	    cmd = (string) "text \"Message: " + (char *) input + ".\"";
	me->_gui->progress_visible(me->_gui->command(cmd));
	break;

    case HT_PROG_TIMEOUT:
	if (!me->_gui->progress_visible())
	    cmd = (string) "popup \"Request timeout.\"";
	else
	    cmd = (string) "text \"Request timeout.\"";
	me->_gui->progress_visible(me->_gui->command(cmd));
	break;

    default:
	break;
    }
#endif

    return YES;
}

// See HTAABrow.c in libwww for calls that generate the authentication
// information. It seems easier to have the callback handle inserting that
// info even when the username and password are given with the URL (or with
// Connect's ctor). This is especially so since it *looks* as if servers will
// ask for Basic authentication but accept digest (this appears to be the
// case for Apache; run geturl with -t and look at w3chttp.out). However,
// there's one slight complication. If this callback is used to add the
// authentication information, we must ensure that static auth info (that is,
// when the stuff is given with a URL) does not result in an infinite loop.
// 2/8/2001 jhrg

BOOL 
dods_username_password(HTRequest * request, HTAlertOpcode /* op */ ,
		       int /* msgnum */ , const char * /* dfault */ ,
		       void * /* input */ , HTAlertPar * reply)
{
    if (WWWTRACE)
	HTTrace("DODS........ Entering username/password callback.\n");

    if (!request) {
	if (WWWTRACE)
	    HTTrace("dods_username_password: Bad argument.\n");
	return NO;
    }

    Connect *me = (Connect *) HTRequest_context(request);

    // Already tried this username/password pair? If we're using the popup,
    // let this cycle 10 times. That should be enough for most folks but will
    // still prevent infinite loops. If we're not using the popup, one shot
    // is all you get. 2/8/2001 jhrg
#ifdef GUI
    if (((!me->_gui->show_gui() && me->_password_attempt >= 1)
	 || (me->_gui->show_gui() && me->_password_attempt >= 10)))
	return NO;
#else
    if (me->_password_attempt >= 1) 
	return NO;
#endif    

    // This is inside an ifdef so that it is possible to build the library
    // without the _gui symbol and thus not link in all the X11 code.
#ifdef GUI
    if ((me->_username.length() == 0) || (me->_password.length() == 0)) {
	if (!me->_gui->password(me->_username, me->_password))
	    return NO;
    }
#endif

    if ((me->_username.length() == 0) || (me->_password.length() == 0))
	return NO;

    // Put the username in reply using HTAlert_setReplyMessage; use
    // _setReplySecret for the password. This callback is triggered by a
    // an After filter; the caller knows to take the stuff in the HTAlert_Par
    // and add it to the request header. This is all part of libwww's
    // operation. 2/9/2001 jhrg
    HTAlert_setReplyMessage(reply, me->_username.c_str());
    HTAlert_setReplySecret(reply, me->_password.c_str());

    ++(me->_password_attempt);

    return YES;
}

static HTErrorMessage
    HTErrors[HTERR_ELEMENTS] = { HTERR_ENGLISH_INITIALIZER };

// To process errors reported by the remote httpd, I use a function called
// from read_url() instead of a callback because throwing an exception from
// within a callback always results in a core dump. At least that's the case
// with libwww callbacks... In a more normal use of libwww these errors would
// be displayed to a user who would then do something with them. The dap++
// library must support both interactive and noninteractive use. 2/8/2001
// jhrg

void 
process_www_errors(HTList *listerr, HTRequest *request) throw(Error)
{
    HTError *the_error;
    HTErrorShow showmask = HTError_show();

    if (WWWTRACE)
	HTTrace("DODS........ Generating error message.\n");

    while ((the_error = (HTError *) HTList_nextObject(listerr))) {
	int index = HTError_index(the_error);
	HTSeverity severity = HTError_severity(the_error);
	switch (severity) {
	  case ERR_INFO:
	  case ERR_WARN:
	  case ERR_NON_FATAL:
	    // Ignore Informational messages.
	    break;

	  case ERR_FATAL: {
	      // I think this `if' will go away once Connect is refactored.
	      // It is left over from when this code was called in a
	      // callback. Exceptions are a much better way to send fatal
	      // error reports than the embedded Error object we were using.
	      // So, I'll make this function a friend rather than a method in
	      // hopes that the architecture of the whole class will change
	      // soon enough. 2/8/2001 jhrg
	    if (request) {
		Connect *me = (Connect *)HTRequest_context(request);
		me->_type = web_error;
	    }
	    string msg = "Fatal Error: ";
	    msg += (string)HTErrors[index].msg + (string)" (";
	    msg += long_to_string(HTErrors[index].code, 10);
	    msg += (string)")";
	    switch (HTError_index(the_error)) {
	      case HTERR_UNAUTHORIZED:
		throw Error(no_authorization, msg);
		break;
	      default:
		throw Error(msg);
		break;
	    }
	    break;
	  }
	  default:
	    if (WWWTRACE)
		HTTrace("DODS........ Unknown Classification of Error (%d)\n",
			severity);
	  }

	// Make sure that we don't get this error more than once even
	// if we are keeping the error stack from one request to another.
	HTError_setIgnore(the_error);

	// If we only are showing the most recent entry then break here.
	if (showmask & HT_ERR_SHOW_FIRST)
	    break;
    }
}

static
ObjectType get_type(string value)
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
description_handler(HTRequest * request, HTResponse *,
		    const char *token, const char *val)
{
    if (WWWTRACE)
	HTTrace("DODS........ Description header handler.\n");

    string field = token, value = val;
    downcase(field);
    downcase(field);

    if (field == "content-description") {
	DBG(cerr << "Found content-description header" << endl);
	Connect *me = (Connect *) HTRequest_context(request);
	me->_type = get_type(value);
    } else {
	if (SHOW_MSG)
	    cerr << "Unknown header: " << token << endl;
    }

    return HT_OK;
}

int
server_handler(HTRequest * request, HTResponse *,
	       const char *token, const char *val)
{
    if (WWWTRACE)
	HTTrace("DODS........ Server header handler.\n");

    string field = token, value = val;
    downcase(field);
    downcase(value);
    Connect *me = (Connect *) HTRequest_context(request);

    if (field == "xdods-server") {
	DBG(cerr << "Found dods server header: " << value << endl);
	me->_server = value;
    } 
    // The test for `server' is a hold over from the pre-Java days when I
    // used that field name. It is actually a reserved header in http. We're
    // testing for it solely for compatibility with older servers (very old
    // servers, in fact). However, when a server sends both an XDODS-Server
    // and Server header and the Server header follows the XDODS-Server
    // header, the latter overwrites the former's value. That breaks
    // Sequences, which rely on version inforamtion to choose the correct
    // deserialization routine. FIX: if we only set the value of _server when
    // XDODS-Server has *not* yet been set we are sure to not overwrite its
    // value. We can set _server using the value of Server; if an
    // XDODS-Server follows it will overwrite that value. If not then the
    // Server header is all we've got. 4/2/2002 jhrg
    else if (field == "server") {
	DBG(cerr << "Found server header: " << value << endl);
	if (me->_server == "dods/0.0") {
	    DBG(cerr << "Setting server header: " << value << endl);
	    me->_server = value;
	}
    } else {
	if (SHOW_MSG)
	    cerr << "Unknown header: " << token << endl;
    }

    return HT_OK;
}

// Use this for debugging only since various servers seem to add headers
// (even though the `NPH' mechanism precludes that ...??). This is especially
// true for error returns.

int
header_handler(HTRequest *, HTResponse *, const char *token,
	       const char *val)
{
    if (WWWTRACE)
	HTTrace("DODS........ Header handler.\n");

    string field = token, value = val;
    downcase(field);
    downcase(value);

    if (SHOW_MSG)
	cerr << "Unknown header: " << token << ": " << value << endl;

    return HT_OK;
}

#define PUTBLOCK(b, l)	(*target->isa->put_block)(target, b, l)
struct _HTStream {
    const HTStreamClass *isa;
    /* ... */
};

int 
xdods_accept_types_header_gen(HTRequest * pReq, HTStream * target)
{
    Connect *me = (Connect *) HTRequest_context(pReq);

    string types = "XDODS-Accept-Types: " + me->get_accept_types() + "\r\n";
    if (WWWTRACE)
	HTTrace("DODS........ %s", types.c_str());

    PUTBLOCK(types.c_str(), types.length());

    return HT_OK;
}

int 
cache_control_header_gen(HTRequest * pReq, HTStream * target)
{
    Connect *me = (Connect *) HTRequest_context(pReq);

    // If the requestor does not want a cache control header, don't write
    // one. 12/1/99 jhrg
    if (me->get_cache_control() == "")
	return HT_OK;

    string control = "Cache-Control: " + me->get_cache_control() + "\r\n";
    if (WWWTRACE)
	HTTrace("DODS........ %s", control.c_str());

    PUTBLOCK(control.c_str(), control.length());

    return HT_OK;
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

/** Use when you cannot use libwww. */
void 
Connect::parse_mime(FILE * data_source)
{
    char line[256];

    fgets(line, 255, data_source);
    line[strlen(line) - 1] = '\0';	// remove the newline

    while ((string) line != "") {
	char h[256], v[256];
	sscanf(line, "%s %s\n", h, v);
	string header = h;
	string value = v;
	downcase(header);
	downcase(value);

	if (header == "content-description:") {
	    DBG(cout << header << ": " << value << endl);
	    _type = get_type(value);
	} else if (header == "xdods-server:") {
	    DBG(cout << header << ": " << value << endl);
	    _server = value;
	} else if (_server == "dods/0.0" && header == "server:") {
	    DBG(cout << header << ": " << value << endl);
	    _server = value;
	}

	fgets(line, 255, data_source);
	line[strlen(line) - 1] = '\0';
    }
}

// I moved this code from fetch_url() because at some point I think it should
// go in Connect's ctor. I cannot get that to work right now; don't know why.
// 2/8/2001 jhrg
/** If a URL contains a username/password pair (in the convention
    established by Netscape, et al.) then extract that information from
    the URL and load it into the this object. Remove the information from
    the URL.
    @brief Extract a username and password from a URL.
    @param url The url on which to operate. Note that this is a parameter
    only to support the method <tt>fetch_url</tt>. */
void 
Connect::extract_auth_info(string &url)
{
    string::size_type start_pos = 0, end_pos, colon_pos;

    // look for a user / password pair which may [or may not] be in the url --
    // look for a '@', If present there are two cases: http:// or file:/
    // get the user and password substr after these and if neither is 0
    // length, put them in the Connect object. Rebuild the URL without them.
    if ((end_pos = url.find('@')) != url.npos) {
        if (url.find("http://") == 0)
            start_pos = 7;
        else if (url.find("file:/") == 0)
            start_pos = 6;
        colon_pos = url.find(":", start_pos);
        if (end_pos > colon_pos) {
            _username = url.substr(start_pos, colon_pos - start_pos);
            _password = url.substr(colon_pos + 1, end_pos - (colon_pos+1));
            url.erase(start_pos, end_pos - start_pos + 1);
        }
    }
}

/** Initialize the W3C WWW Library. This should only be called when a
    Connect object is created and there are no other Connect objects in
    existence. */
void 
Connect::www_lib_init(bool www_verbose_errors, bool accept_deflate)
{
    // Initialize various parts of the library. This is in lieu of using one
    // of the profiles in HTProfil.c. 02/09/98 jhrg
   
#ifdef WIN32
    HTEventInit();
#endif

    // Grab a pointer to the reader for the .dodsrc file.
    RCReader *rcr = RCReader::instance();

#if 0
    // I think there's a typo in the line below; the 2nd = should be ==.
    // However, I think the code can be made simpler... 4/30/2002 jhrg
    _accept_deflate = accept_deflate = rcr->get_never_deflate() ? false : true;
#endif
    _accept_deflate = accept_deflate && !rcr->get_never_deflate();
    _always_validate = rcr->get_always_validate();

    HTProxy_add(rcr->get_proxy_server_protocol().c_str(),
		rcr-> get_proxy_server_host_url().c_str());
    
    HTProxy_addRegex(rcr->get_proxy_for_regexp().c_str(),
		     rcr->get_proxy_for_proxy_host_url().c_str(),
		     rcr->get_proxy_for_regexp_flags()); 
    
    HTNoProxy_add(rcr->get_no_proxy_for_protocol().c_str(),
		  rcr->get_no_proxy_for_host().c_str(),
		  rcr->get_no_proxy_for_port());


    HTLibInit(CNAME, CVER);	// These constants are in config_dap.h

    // Initialize tcp and buffered_tcp transports
    HTTransportInit();

    // Set up http and cache protocols. Do this instead of
    // HTProtocolPreemtiveInit(). 
    HTProtocol_add("http", "buffered_tcp", HTTP_PORT, YES, HTLoadHTTP,
		   NULL);
    HTProtocol_add("file", "local", 0, YES, HTLoadFile, NULL);
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
    if (_accept_deflate) {
#ifdef HT_ZLIB
	HTList *content_encodings = HTList_new();
	HTContentEncoderInit(content_encodings);
	// HTContentEncoderInit adds `deflate' if libwww was built with
	// HT_ZLIB defined. 3/28/2000 jhrg
	HTFormat_setContentCoding(content_encodings);
#endif				/* HT_ZLIB */
    }

    // Register MIME headers for HTTP 1.1
    HTMIMEInit();

#ifdef GUI
    // Add progress notification for popup.
    HTAlert_add(dods_progress, HT_A_PROGRESS);
    HTAlert_setInteractive(YES);
#endif

    HTAlert_add(dods_username_password, HT_A_USER_PW);

    if (!rcr->get_use_cache()) {
	// Disable the cache. 
	HTCacheTerminate();
	_cache_enabled = false;
    } else {

	//  We have to escape spaces.  Utilizing the escape functionality
	//  forces us, in turn, to use the "file:" convention for URL's.
#ifdef WIN32
	string croot = string("file:/") + rcr->get_dods_cache_root(); // cache:/ ???
	croot = id2www(string(croot));
#else
	// Changed to cache: from file:; both work but cache: is closer to
	// the truth. 9/25/2001 jhrg
	string croot = string("cache:") +  rcr->get_dods_cache_root();
#endif
	// I removed this line since using it screws up the UNIX code. libwww
	// will undo the %xx escapes but not the cache: or file: protocol
	// identifier. It thinks that it's supposed to create a directory
	// called `file:/...' or `cache:/...' which is not true. 
	_cache_root = new char[strlen(rcr->get_dods_cache_root().c_str()) + 1];
	strcpy(_cache_root,rcr->get_dods_cache_root().c_str());
	if (HTCacheInit(croot.c_str(),rcr->get_max_cache_size()) == YES) {
	    HTCacheMode_setMaxCacheEntrySize(rcr->get_max_cached_obj());
	    if (rcr->get_ignore_expires())
		HTCacheMode_setExpires(HT_EXPIRES_IGNORE);
	    else
		HTCacheMode_setExpires(HT_EXPIRES_AUTO);
	    HTCacheMode_setDefaultExpiration(rcr->get_default_expires());
	    _cache_enabled = true;
	} else {
	    // Disable the cache. 
	    HTCacheTerminate();
	    _cache_enabled = false;
	}
    }

    // I think this is pretty much broken. I'm not sure how it would be used
    // except for debugging and even then, I think tracing is more practicle.
    // The bottom line is that this code should only deal with Fatal errors.
    // 2/8/2001 jhrg
    if (www_verbose_errors)
	HTError_setShow(HT_ERR_SHOW_INFO);
    else
	HTError_setShow(HT_ERR_SHOW_FATAL);

    // Add our own filter to update the history list.
    HTNet_addAfter(http_terminate_handler, NULL, NULL, HT_ALL,
		   HT_FILTER_LAST);

    // We add our own parsers for content-description and server so that
    // we can test for these fields and operate on the resulting document
    // without using the stream stack mechanism (which seems to be very
    // complicated). jhrg 11/20/96
    HTHeader_addParser("content-description", NO,
		       (HTParserCallback *) description_handler);

    // Added DODS server header because `Server:' is used by Java. We check
    // first for `XDODS-Server:' and use that if found. Then look for
    // `Server:' and finally default to 0.0. 12/16/98 jhrg
    HTHeader_addParser("xdods-server", NO,
		       (HTParserCallback *) server_handler);
    HTHeader_addParser("server", NO, (HTParserCallback *) server_handler);

    // Add xdods_accept_types header. 2/17/99 jhrg
    HTHeader_addGenerator(xdods_accept_types_header_gen);
    // Add Cache-Control header. Use this to suppres caching of compressed
    // data reqeusts which, for some reason, are broken. 12/1/99 jhrg
    HTHeader_addGenerator(cache_control_header_gen);
}

/** Create a new Connect object. */
void 
Connect::clone(const Connect & src)
{
    _local = src._local;

    if (!_local) {
	_type = src._type;
	_encoding = src._encoding;
	_server = src._server;

	_das = src._das;
	_dds = src._dds;
	_error = src._error;

#ifdef WIN32
	_tfname = src._tfname;
#endif

#ifdef GUI
	_gui = new Gui();
#endif
	_URL = src._URL;
	_proj = src._proj;
	_sel = src._sel;
	_accept_types = src._accept_types;
	_cache_control = src._cache_control;

	// Initialize the anchor object.
	char *ref = HTParse(_URL.c_str(), (char *) 0, PARSE_ALL);
	_anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
	HT_FREE(ref);
	// Copy the access method.
	_method = src._method;
	// Open the file for non-truncating update.
	if (_output)
	    _output = fdopen(dup(fileno(src._output)), "r+b");
	if (_source)
	    _source = new_xdrstdio(_output, XDR_DECODE);

	_www_errors_to_stderr = src._www_errors_to_stderr;
	_accept_deflate = src._accept_deflate;
    }
}

// Use the URL designated when the Connect object was created as the
// `base' URL so that the formal parameter to this mfunc can be relative.

/** Assume that the object's _OUTPUT stream has been set
    properly. Error signals something's wrong. */
void
Connect::read_url(string & url, FILE * stream) throw(Error)
{
    assert(stream);

    int status = YES;

    HTRequest *_request = HTRequest_new();

    HTRequest_setContext(_request, this);	// Bind THIS to request 

    HTRequest_setOutputFormat(_request, WWW_SOURCE);

    HTRequest_setAnchor(_request, (HTAnchor *) _anchor);

    HTRequest_setOutputStream(_request,
			      HTFWriter_new(_request, stream, YES));

    // Set this request to use the cache if possible. 
    // CJM used HT_CACHE_VALIDATE which forces validation; HT_CACHE_OK uses a
    // more complex algorithm (See HTCache.c:HTCache_isFresh and
    // :HTCacheFilter) for the complete scoop. 5/2/2001 jhrg
    if (_cache_enabled) {
	HTRequest_setReloadMode(_request, 
		   _always_validate ? HT_CACHE_VALIDATE : HT_CACHE_OK);
    }

    status = HTLoadRelative(url.c_str(), _anchor, _request);
    HTList *listerr=HTRequest_error(_request);
    if (listerr)
	process_www_errors(listerr, _request); // throws Error

    if (_cache_enabled)
	HTCacheIndex_write(_cache_root);

    if (status != YES) {
	if (SHOW_MSG)
	    cerr << "DODS...... Can't access resource" << endl;
	throw Error("Cannot access resource from web server.");
    }

    if (_cache_enabled)
	HTCacheIndex_write(_cache_root);

    HTRequest_delete(_request);
}

/** This ctor is declared private so that it won't ever be called by users,
    thus forcing them to create Connects which point somewhere. */
Connect::Connect()
{
    assert(false);
}

// public mfuncs

/** The Connect constructor requires a <tt>name</tt>, which is the
    URL to which the connection is to be made.  You can specify that
    you want to see the <tt>verbose</tt> form of any WWW library
    errors.  This can be useful for debugging.  The default is to
    suppress these errors. Callers can use the
    <tt>accept_deflate</tt> parameter to request that servers are
    told the client (caller of Connect ctor) <i>can</i> process return
    documents that are compressed with gzip.

    @param name The URL for the virtual connection.
    @param www_verbose_errors False: show only WWW Fatal errors, True: show
    WWW informational messages, too. This affects message display but not
    exceptions. If Connect is compiled to throw exceptions for certain WWW
    errors, it will do so regardless of the value of this parameter.
    @param accept_deflate Provides compile-time control for on-the-fly
    compression. If True clients will ask servers to compress responses.
    @param uname If given along woth password, supply this as the Username
    in all HTTP requests.
    @param password Use this as the password with <tt>uname</tt> above.
    @brief Create an instance of Connect. */
Connect::Connect(string name, bool www_verbose_errors, bool accept_deflate, 
		 string uname, string password) :
    _accept_types("All"), _cache_control(""), _username(uname),
    _password(password), _always_validate(DODS_ALWAYS_VALIDATE), 
    _www_errors_to_stderr(false), _accept_deflate(accept_deflate)
{
    name = prune_spaces(name);
    char *access_ref = HTParse(name.c_str(), NULL, PARSE_ACCESS);
    // Instantiate Gui here so that both local and remote connects have a
    // valid object. This was done for remote connects only which caused
    // local accessed to crash sometimes. 5/22/2001 jhrg
#ifdef GUI
    _gui = new Gui;
#endif

    if (strcmp(access_ref, "http") == 0) { // access == http --> remote access
	// If there are no current connects, initialize the library
	if (_num_remote_conns == 0) {
	    www_lib_init(www_verbose_errors, accept_deflate);
	}
	_num_remote_conns++;
	// NB: _cache_enabled and _cache_root are set in www_lib_init.
	// 12/14/99 jhrg

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
	_output = 0;
	_source = 0;
	_type = unknown_type;
	_encoding = unknown_enc;
	// Assume servers that don't announce themselves are old servers.
	_server = "dods/0.0";

	char *ref = HTParse(_URL.c_str(), (char *) 0, PARSE_ALL);
	_anchor = (HTParentAnchor *) HTAnchor_findAddress(ref);
	HT_FREE(ref);
    } else {
	_URL = "";
	_local = true;
	_output = 0;
	_source = 0;
	_type = unknown_type;
	_encoding = unknown_enc;
    }
    _username = "";
    _password = "";
    _password_attempt = 0;


    if (access_ref)
	HT_FREE(access_ref);
}

/** The Connect copy constructor. */
Connect::Connect(const Connect & copy_from):_error(undefined_error, "")
{
    clone(copy_from);
    if (!_local)
	_num_remote_conns++;
}

Connect::~Connect()
{
    DBG2(cerr << "Entering the Connect dtor" << endl);

    // Don't count local connections as those that affect whether libwww
    // needs to be shutdown (or initialized, for that matter). This fixes a
    // bug where several local connections opening and closing would result
    // in a negative value for _num_remote_conns (since that field is not
    // *incremented* for local connections. This meant that remote
    // connections made after one or more local connections were made and
    // then broken, were done with an uninitialized libwww. Note surprisingly,
    // this was bad... 4/17/2000 jhrg
    if (!_local) {
#ifdef GUI
	delete _gui;
	_gui = 0;
#endif
	_num_remote_conns--;
    }

#ifdef WIN32
    HTEventTerminate();

    //  Get rid of any intermediate files
    vector < string >::const_iterator i;
    for (i = _tfname.begin(); i != _tfname.end(); i++)
	remove((*i).c_str());
#endif

    // Calling this ensures that the WWW library Cache gets updated and the
    // .index file is written. 11/22/99 jhrg
    if (_num_remote_conns == 0) {
	if (_cache_enabled)
	    HTCacheTerminate();
	HTList_delete(_conv);
	if (HTLib_isInitialized())
	    HTLibTerminate();
	_conv = 0;
	delete[] _cache_root;
	_cache_root = 0;
    } else if (_cache_enabled)
	HTCacheIndex_write(_cache_root);

    close_output();

    DBG2(cerr << "Leaving the Connect dtor" << endl);
}

Connect & 
Connect::operator = (const Connect & rhs) 
{
    if (&rhs == this)
	return *this;
    else {
	clone(rhs);
	return *this;
    }
}

/** @brief Sets the <tt>www_errors_to_stderr</tt> property.

    This controls where http errors get printed.

    @see get_www_errors_to_stderr
    @param state The desired state of the property.  TRUE means
    that http errors get printed on stderr as well as in the Error
    object.  FALSE prints them only in the Error object. */
void 
Connect::set_www_errors_to_stderr(bool state)
{
    _www_errors_to_stderr = state;
}

/** @brief Gets the state of the <tt>www_errors_to_stderr</tt>
    property. 

    If TRUE this means that http errors will be printed
    to stderr in addition to being reported in the Error
    object. If FALSE only the Error object will be 	used. 

    @return TRUE if WWW errors should got to stderr, FALSE if only the
    Error object should be used. */
bool 
Connect::get_www_errors_to_stderr()
{
    return _www_errors_to_stderr;
}

/** Sets the list of accepted types. This string is meant to list all of
    the DODS datatypes that the client can grok and is sent to
    the server using the XDODS-Accept-Types MIME header. The server will
    try to only send back to the client datatypes that are listed. If the
    value of this header is `All', then the server assumes that the
    client can process all of the DODS datatypes. If only one or two
    types are <i>not</i> understood, then they can be listed, each one
    prefixed by `!'. Thus, if a client does not understand `Sequences',
    it could set types to `!Sequences' as opposed to listing all of the
    DODS datatypes. Multiple values are separated by commas (,).

    Not all servers will honor this and some requests may not be possible
    to express with a very limited set of datatypes.

    @note By default (if this function is never called) the value
    `All' is used. 

    @brief Set the types a client can accept.
    @param types The string listing datatypes understood by this client.
*/
void 
Connect::set_accept_types(const string & types)
{
    _accept_types = types;
}

/** Gets the current string of `accepted types.' This string lists all of
    the DODS datatypes that the client can grok. 

    @brief Retrieve a list of types a DODS client can understand.
    @see set_accepted_types
    @return A string listing the types this client declares to servers it
    can understand. */
string 
Connect::get_accept_types()
{
    return _accept_types;
}

/** @brief Set the cache control header value.

    http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html

    @see get_cache_control
    @see HTTP/1.1 Specification (RFC 2616), sec. 14.9.
    @param caching A string containing the desired caching
    directive. Should be <tt>no-cache</tt> to disable caching. */
void 
Connect::set_cache_control(const string & caching)
{
    _cache_control = caching;
}

/** Get the string which describes the default cache control value. This
    is sent with all outgoing messages.

    @note N.b. The libwww 5.2.9 cache does not honor this.

    @brief Get the cache control directive.
    @return The cache control header value. */
string 
Connect::get_cache_control()
{
    return _cache_control;
}

// Dereference the URL and dump its contents into _OUTPUT. Note that
// read_url() does the actual dereferencing; this sets up the _OUTPUT sink.
//
// Due to a bug in the asynchorous code which makes it fail when used with
// compression, I'm forcing all accesses to be synchronous. Note that given
// the design of Connect, the caller of fetch_url will never know that their
// async operation was actually synchronous.

/** Fetch the contents of the indicated URL and put its contents
    into an output file.  A pointer to this file can be retrieved
    with the <tt>output()</tt> function. <i>A program that uses that
    FILE pointer must be sure not to close it with
    <tt>fclose()</tt>.</i> Instead, use
    <tt>Connect::close_output()</tt>. If <i>async</i> is TRUE, then
    the operation is asynchronous, and the function returns before
    the data transfer is complete. This method is here so that
    Connect can be used to read from any URL, not just URLs which
    return DAS, DDS or DataDDS objects. Because of this,
    <tt>fetch_url</tt> neither automatically appends a suffix nor
    does it route the response through any of the parsers which
    decode responses from a DODS server.

    This method also scans the URL for a username/passwd. If present,
    extracts them, places them in the Connect object, and rebuilds the URL
    without them.

    This method is here so that Connect can be used to read from any URL,
    not just URLs which return DAS, DDS or DataDDS objects. Because of
    this, <tt>fetch_url</tt> neither automatically appends a suffix nor does it
    route the response through any of the parsers which decode responses
    from a DODS server. In addition, <tt>fetch_url</tt> also does <i>not</i>
    escape any characters in the URL. That is the responsibility of the
    caller. This includes characters that break URLs and those that break
    DODS CEs.

    @note Note that the asynchronous transfer feature of DODS is not
    currently enabled.  All invocations of this function will be
    synchronous, no matter what the value of the <i>async</i>
    parameter. 

    @brief Dereference a URL.  
    @return Always returns true.
    @exception Error indicates some problem reading from the web server
    (not the DODS server, other methods report those errors).
    @param url A string containing the URL to be dereferenced.  The
    data referred to by this URL will wind up available through a
    file pointer retrieved from the <tt>output()</tt> function.
    @param async Not currently implemented.
    @see Connect::output */
bool 
Connect::fetch_url(string & url, bool) throw(Error)
{
    _encoding = unknown_enc;
    _type = unknown_type;

    /* NB: I've completely removed the async stuff for now. 2/18/97 jhrg */
    
    // get_tempfile_template uses new, must call delete
    char *dods_temp = get_tempfile_template("dodsXXXXXX");
    // Open truncated for update. NB: mkstemp() returns a file descriptor.
#ifdef WIN32
    FILE *stream = fopen(_mktemp(dods_temp),"wb");
#else
    FILE *stream = fdopen(mkstemp(dods_temp), "wb");
#endif

    // This is here instead of Connect's ctor because it's possible a client
    // of Connect will call this method with a URL other than the one used to
    // initialize the class. 2/8/2001 jhrg
    extract_auth_info(url); 

    read_url(url, stream);	// Throws Error.

    // Workaround for Linux 2.2 (only?). Using fseek() did not work for URLs
    // with between 61 and 64 characters in them. I have no idea why, but it
    // appeared in ddd as if the eof was not being reported correctly. Since
    // dumping all the data to an intermediate file is sort of a hack, I
    // decided to close and then reopen the file to see if that would cure
    // the problem. It seems to. The real solution is to pass the socket back
    // via _output. However, this means figuring that out and getting
    // multi-part MIME docs working in DODS. 8/2/2000 jhrg
    fclose(stream);
    stream = fopen(dods_temp, "rb");
    if (!keep_temps)
	unlink(dods_temp);	// When _OUTPUT is closed file is deleted
    else
	cerr << "Temporary file for Data document: " << dods_temp << endl;

    delete dods_temp;

    _output = stream;

    return true;		// Faux status; for compat with old code.
}

/** Returns a file pointer which can be used to read the data
    fetched from a URL.

    Note that occasionally this may be directed to <tt>stdout</tt>.  If this
    is the case, users should avoid closing it.

    @brief Access the information contained in this Connect instance.
    @see Connect::fetch_url
    @return A <tt>(FILE *)</tt> indicating a file containing the data
    received from a dereferenced URL.  */
FILE *
Connect::output()
{
    // NB: Users should make sure they don't close stdout.
    return _output;
}

/** The data retrieved from a remote DODS server will be in XDR
    format.  Use this function to initialize an XDR decoder for that
    data and to return an XDR pointer to the data.

    @brief Access the XDR input stream (source) for this connection.

    @return Returns a XDR pointer tied to the current output
    stream.  
    @see Connect::output
*/
XDR *
Connect::source()
{
    if (!_source)
	_source = new_xdrstdio(_output, XDR_DECODE);

    return _source;
}

/** Close the output stream of the Connect object. This closes the FILE
    pointer returned by <tt>output()</tt>. In addition, it also deletes the
    internal XDR stream object, although users should not have to know
    about that.

    @brief Close the object's output stream if it is not NULL or
    STDOUT. */
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

/** During the parse of the message headers returned from the
    dereferenced URL, the object type is set. Use this function to
    read that type information. This will be valid <i>before</i> the
    return object is completely parsed so it can be used to decide
    which parser to call to read the data remaining in
    the input stream.

    The object types are Data, DAS, DDS, Error, and undefined.

    @brief What type is the most recent object sent from the
    server?
    @return The type of the object.
    @see ObjectType */
ObjectType 
Connect::type()
{
    return _type;
}

/** During the parse of the message headers returned from the
    dereferenced URL, the encoding type is set. Use this function to
    read that type information. This will be valid <i>before</i> the
    return object is completely parsed so it can be used to decide
    which decoder to call (if any) to read the data remaining in
    the input stream.

    The encoding types are currently limited to x-plain (no special
    decoding required) and x-gzip (compressed using GNU's gzip).  

    @brief What type of encoding was used on the data in the stream? 
    @return The type of the compression.
    @see EncodingType
*/
EncodingType 
Connect::encoding()
{
    return _encoding;
}

/** Returns a string containing the version of DODS used by the
    server. */
string 
Connect::server_version()
{
    return _server;
}

// Added EXT which defaults to "das". jhrg 3/7/95

/** Reads the DAS corresponding to the dataset in the Connect
    object's URL. Although DODS does not support usig CEs with DAS
    requests, if present in the Connect object's instance, they will be
    escaped and passed as the query string of the request.

    @brief Get the DAS from a server.
    @return TRUE if the DAS was successfully received. FALSE
    otherwise. 
    @param gui_p If TRUE, use the client GUI.  Most DAS's are too
    small to make this worthwhile.
    @param ext The extension to append to the URL to retrieve the
    dataset DAS.  This parameter is included for compatibility with
    future versions of the DODS software.  It currently defaults to
    the only possible working value, ``das''.
*/
bool 
Connect::request_das(bool gui_p, const string & ext) throw(Error, InternalErr)
{
#ifdef GUI
    (void) _gui->show_gui(gui_p);
#endif
    string das_url = _URL + "." + ext;
    if (_proj.length() + _sel.length())
	das_url = das_url + "?" + id2www_ce(_proj + _sel);

    // We need to catch Error exceptions to ensure calling close_output.
    try {
	fetch_url(das_url);
    }
    catch (...) {
	close_output();
#ifdef GUI
	_gui->command("popdown");
	_gui->progress_visible(false);
#endif
	throw;
    }

    switch (type()) {
      case dods_error: {
#ifdef GUI
	  _gui->command("popdown");
	  _gui->progress_visible(false);
#endif
	  if (!_error.parse(_output)) {
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
	    _das.parse(_output);	// read and parse the das from a file 
	}
	catch (...) {
	    close_output();
#ifdef GUI
	    _gui->command("popdown");
	    _gui->progress_visible(false);
#endif
	    throw;
	}
	    
	break;
    }

    close_output();
    return true;
}

// Added EXT which deafults to "dds". jhrg 3/7/95

/** Reads the DDS corresponding to the dataset in the Connect
    object's URL. Although CEs are rarely used with this method, if present
    it will be escaped.

    @brief Get the DDS from a server.
    @return TRUE if the DDS was successfully received. FALSE
    otherwise. 
    @param gui_p If TRUE, use the client GUI.  Most DDS's are too
    small to make this worthwhile.
    @param ext The extension to append to the URL to retrieve the
    dataset DDS.  This parameter is included for compatibility with
    future versions of the DODS software.  It currently defaults to
    the only possible working value, ``dds''.
*/
bool 
Connect::request_dds(bool gui_p, const string & ext) throw(Error, InternalErr)
{
#ifdef GUI
    (void) _gui->show_gui(gui_p);
#endif
    string dds_url = _URL + "." + ext;
    if (_proj.length() + _sel.length())
	dds_url = dds_url + "?" + id2www_ce(_proj + _sel);

    // We need to catch Error exceptions to ensure calling close_output.
    try {
	fetch_url(dds_url);
    }
    catch (...) {
	close_output();
#ifdef GUI
	_gui->command("popdown");
	_gui->progress_visible(false);
#endif
	throw;
    }

    switch (type()) {
      case dods_error: {
#ifdef GUI
	  _gui->command("popdown");
	  _gui->progress_visible(false);
#endif
	  if (!_error.parse(_output)) {
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
	    _dds.parse(_output);	// read and parse the dds from a file 
	}
	catch (...) {
	    close_output();
#ifdef GUI
	    _gui->command("popdown");
	    _gui->progress_visible(false);
#endif
	    throw;
	}
	break;
    }

    close_output();
    return true;
}

// Assume that _output points to FILE * from which we can read the data
// object. 
// This is a private mfunc.

DDS *
Connect::process_data(bool async) throw(Error, InternalErr)
{
    switch (type()) {
      case dods_error:
#ifdef GUI
	  _gui->command("popdown");
	  _gui->progress_visible(false);
#endif
	if (!_error.parse(_output))
	    throw InternalErr(__FILE__, __LINE__,
	      "Could not parse the Error object returned by the server!");
	throw _error;
	return 0;

      case web_error:
	// Web errors (those reported in the return document's MIME header)
	// are processed by the WWW library.
	return 0;

      case dods_data:
      default: {
	  DataDDS *dds = new DataDDS("received_data", _server);

	  // Parse the DDS; throw an exception on error.
	  dds->parse(_output);

	  for (Pix q = dds->first_var(); q; dds->next_var(q)) {
	      dds->var(q)->deserialize(source(), dds);
#if 0
	      // I think that nesting these makes messages that look goofy.
	      // If users have to see out mistakes we might as well make the
	      // messages look professional... 1/30/2002 jhrg
	      try {
		  dds->var(q)->deserialize(source(), dds);
	      }
	      catch (InternalErr &ie) {
		  string msg = "I cought the following Internal Error:\n";
		  msg += NEWLINE + ie.get_error_message();
		  throw InternalErr(__FILE__, __LINE__, msg);
	      }
#endif
	  }

	  return dds;
      }
    }
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

/** Reads data from the Connect object's server.  This method sets
    up the BaseType variables in a DDS, and sends a request using
    <tt>fetch_url()</tt>.  Upon return, it caches the data on a disk, then
    unpacks it into the DDS storage. Unlike <tt>fetch_url</tt>, this method
    escapes the CE part of the URL.

    @brief Calls <tt>fetch_url()</tt> to retrieve data from the server.
    @return A reference to the DataDDS object which contains the
    variables (BaseType pointers) generated from the DDS sent with
    the data. These variables are guaranteed to be large enough to
    hold the data, even if the constraint expression changed the
    type of the variable from that which appeared in the original
    DDS received from the dataset when this connection was made.
    @param expr A string containign a constraint expression.  The
    function adds the clauses of this constraint expression to the
    Connect object's original CE.  If the constraint expression
    contains one or more Sequences, these must be the <i>last</i>
    objects specified in the projection clause.  If you request N
    variables and M of them are Sequences, all the M sequences must
    follow the N-M other variables. 
    @param gui_p If this is TRUE, use the DODS client GUI.  See the
    Gui class for a description of this feature.
    @param async  If this is TRUE, this function reads data
    asynchronously, returning before the read completes. Synchronous
    reads are the default, and the only possible action as of DODS
    version 2.15.
    @param ext The extension to append to the URL to retrieve the
    dataset data.  This parameter is included for compatibility with
    future versions of the DODS software.  It currently defaults to
    the only possible working value, ``dods''.
    @see DataDDS
    @see Gui */
DDS *
Connect::request_data(string expr, bool gui_p, bool async, 
		      const string & ext) throw(Error, InternalErr)
{
#ifdef GUI
    (void) _gui->show_gui(gui_p);
#endif
    string proj, sel;
    string::size_type dotpos = expr.find('&');
    if (dotpos != expr.npos) {
	proj = expr.substr(0, dotpos);
	sel = expr.substr(dotpos);
    } else {
	proj = expr;
	sel = "";
    }

    string data_url = _URL + "." + ext + "?" 
	+ id2www_ce(_proj + proj + _sel + sel);

    // We need to catch Error exceptions to ensure calling close_output.
    try {
	fetch_url(data_url);

	return process_data(async);
    }
    catch (...) {
	close_output();
#ifdef GUI
	_gui->command("popdown");
	_gui->progress_visible(false);
#endif
	throw;
    }
}

/** @brief Read cached data from a disk file.

    @return A reference to the DataDDS object which contains the
    variables (BaseType pointers) generated from the DDS sent with
    the data. 
    @param data_source A file pointer to the cache file.
    @param gui_p If this is TRUE, use the DODS client GUI.  See the
    Gui class for a description of this feature.
    @param async  If this is TRUE, this function reads data
    asynchronously, returning before the read completes. Synchronous
    reads are the default, and the only possible action as of DODS
    version 2.15.
    @see DataDDS
    @see Gui */
DDS *
Connect::read_data(FILE * data_source, bool gui_p, bool async) 
    throw(Error, InternalErr)
{
    if (!data_source) 
	throw InternalErr(__FILE__, __LINE__, "data_source is null.");

#ifdef GUI
    _gui->show_gui(gui_p);
#endif
    // Read from data_source and parse the MIME headers specific to DODS.
    parse_mime(data_source);

    _output = data_source;

    try {
	return process_data(async);
    }
    catch (...) {
	close_output();
#ifdef GUI
	_gui->command("popdown");
	_gui->progress_visible(false);
#endif
	throw;
    }
}

/** The DODS client can display graphic information to a user with
    the DODS Graphical User Interface (GUI).  Typically used for a
    progress indicator, the GUI is simply a shell capable of
    interpreting arbitrary graphical commands (with tcl/tk).  The
    Gui object is created anew when the Connect object is first
    created.  This function returns a pointer to the Gui object, so
    you can modify the GUI as desired.

    This member will be removed since its presence makes it hard to build
    Gui and non-gui versions of the DAP. The Gui object is accessed is in
    Connect and Error, but in the later case an instance of Gui is always
    passed to the instance of Error. Thus, even though it is a dubious
    design, we can use the private member <tt>_gui</tt> and pass the pointer to
    outside classes. Eventually, Connect must be redesigned.

    @brief Returns a pointer to a Gui object.
    @return a pointer to the Gui object associated with this
    connection. 
    @deprecated 
    @see Gui */
void *
Connect::gui()
{
#ifdef GUI
    return _gui;
#else
    return NULL;
#endif
}

/** The Connect class can be used for ``connections'' to local
    files.  This means that local files can continue to be accessed
    with a DODS-compliant API.

    @brief Does this object refer to a local file?  

    @return Return TRUE if the Connect object refers to a local
    file, otherwise returns FALSE.  */
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
    if (_local) {
	cerr << "CE(): This call is only valid for a remote connection."
	    << endl;
	return "";
    }

    return _proj + _sel;
}

/** All DODS datasets define a Data Attribute Structure (DAS), to
    hold a variety of information about the variables in a
    dataset. This function returns the DAS for the dataset indicated
    by this Connect object.

    @brief Return a reference to the Connect's DAS object. 
    @return A reference to the DAS object.
    @see DAS 
*/
DAS & 
Connect::das()
{
    assert(!_local);

    return _das;
}

/** All DODS datasets define a Data Descriptor Structure (DDS), to
    hold the data type of each of the variables in a dataset.  This
    function returns the DDS for the dataset indicated by this
    Connect object.

    @brief Return a reference to the Connect's DDS object. 
    @return A reference to the DDS object.
    @see DDS 
*/
DDS & 
Connect::dds()
{
    assert(!_local);

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
    @see Error 
*/
Error & 
Connect::error()
{
    return _error;
}

/** @brief Set the credentials for responding to challenges while dereferencing
    URLs. 
    @param u The username.
    @param p The password. 
    @see extract_auth_info() */
void 
Connect::set_credentials(string u, string p)
{
    _username = u;
    _password = p;
}

/** Disable any further use of the client-side cache. In a future version
    of this software, this should be handled so that the www library is
    not initialized with the cache running by default. */
void
Connect::disable_cache()
{
    HTCacheTerminate();
    _cache_enabled = false;
}

// $Log: Connect.cc,v $
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
// Revision 1.109  2001/02/09 22:49:47  jimg
// Merged Jose's and Brent's authentication code, modifying it in the process.
// There are some significant changes in the functions/methods:
// dods_username_password, process_www_errors, extract_auth_info, read_url,
// fetch_url:
//
// dods_username_password() can now be used by both a popup and non-popup
// version of the class. The function knows how to provide humans with
// several chances to get it right but only allows statically supplied
// credentials one shot (thus avoiding an infinite loop).
// process_www_errors() is new: it reads libwww errors and throws Error if it
// finds a fatal error in the list. If it finds an authentication error, it
// sets the Error object's error_code to no_authorization. It does nothing if
// the errors are not fatal. extract_auth_info() is Brent's code moved to its
// own home. read_url() now calls process_www_errors. This is necessary
// because process_www_errors throws exceptions and those don't (seem to)
// work from within libwww callbacks. Understandable since libwww is C
// code... fetch_url() now calls extract_auth_info. I'd like to move this
// Connect's ctor, but this will have to do for now.
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
// Revision 1.81  1999/08/23 18:57:44  jimg
// Merged changes from release 3.1.0
//
// Revision 1.80.2.2  1999/08/28 06:43:03  jimg
// Fixed the implementation/interface pragmas and misc comments
//
// Revision 1.80.2.1  1999/08/09 22:57:49  jimg
// Removed GUI code; reactivate by defining GUI
//
// Revision 1.80  1999/08/09 18:27:33  jimg
// Merged changes from Brent for the Gui code (progress indicator)
//
// Revision 1.79  1999/07/22 17:11:50  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.78.4.2  1999/07/29 05:46:17  brent
// call Tcl / GUI directly from Gui.cc, abandon expect, and consolidate Tcl
// files.
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
