
// Implementation for Connect -- a virtual base class that manages
// connections to DODS servers or local files (the later case really isn't a
// connection, DODS still uses a Connect object to refer to the open file.
// 
// jhrg 9/29/94
//
// Also includes the implementation for the class Connections,  which is used
// to hold and managed a set of open Connect *'s. 
//
// jhrg 9/30/94

// $Log: Connect.cc,v $
// Revision 1.2  1995/01/18 18:48:22  dan
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

static char rcsid[]={"$Id: Connect.cc,v 1.2 1995/01/18 18:48:22 dan Exp $"};

#ifdef __GNUG__
#pragma "implemenation"
#endif

#include <stdio.h>

#include "errmsg.h"
#include "url_comp.h"		// C code to parse a URL

#include "Connect.h"

extern "C" int NetExecute(char *, char *, FILE *); // defined in NetExec.c
extern "C" FILE *NetConnect(char *, char *);       // defined in NetExec.c

// Private mfunc

String
Connect::make_url(const String &api, const String &cgi)
{
    String url = _access + "://" + _host + "/cgi-bin/" + api + "_" + cgi;
    
    return url;
}

void
Connect::parse_url(const char *name)
{
    URL_COMP *uc = url_comp(name);

    if (uc == NULL) {		// local file
	_local = true;
	_URL = "";		// null string
	_access = "";
	_host = "";
	_path = "";
	_anchor = "";
	// das & dds are initialized by children of Connect
    }
    else {
	_local = false;
	_URL = name;
	_access = uc->access;
	_host = uc->host;
	_path = uc->path;
	_anchor = uc->anchor;
    }

    free(uc);
}

// public mfuncs

Connect::Connect(const String &name, const String &api)
{
    parse_url((const char *)name);
    _api_name = api;
}

Connect::Connect(const char *name, const String &api)
{
    parse_url(name);
    _api_name = api;
}

Connect::~Connect()
{
}

bool
Connect::request_das()
{
    // get the das 

    String das_url = make_url(_api_name, "das");
    bool status;

    // NULL means P_tmpdir in stdio or TMPDIR environment variable
    //char *tmp = tempnam(NULL, "das");
    FILE *fp = NetConnect(das_url, _path);

    if( fp ) 
      status = _das.parse(fp);    // read and parse the das from a file 

#ifdef NEVER
    cerr << "das url: " << das_url << endl;
    cerr << "tmp: " << tmp << endl;
#endif

    //unlink(tmp);
    //free(tmp);			// malloc'd in tempnam(3)

    return status;
}

bool
Connect::request_dds()
{
    // get the dds 

    String dds_url = make_url(_api_name, "dds");
    bool status;

    // NULL means P_tmpdir in stdio or TMPDIR environment variable
    //char *tmp = tempnam(NULL, "dds");
    FILE *fp = NetConnect(dds_url, _path);

    if( fp ) 
      status = _dds.parse(fp);    // read and parse the das from a file 
   
#ifdef NEVER
    cerr << "dds url: " << dds_url << endl;
    cerr << "tmp: " << tmp << endl;
#endif

    //unlink(tmp);
    //free(tmp);			// malloc'd in tempnam(3)

    return status;
}
bool
Connect::request_data()
{
    // get the dds 

    String data_url = make_url(_api_name, "server");
    bool status;

    // NULL means P_tmpdir in stdio or TMPDIR environment variable
    //char *tmp = tempnam(NULL, "server");
    FILE *fp = NetConnect(data_url, _path);

    if ( fp ) 
      set_xdrin(fp);

    return status;
}

bool
Connect::is_local()
{
    return _local;
}

const String &
Connect::URL()
{
    if (_local)
	err_quit("Connect::URL is only valid for a remote connection");

    return _URL;		// if _local returns ""
}

const String &
Connect::api_name()
{
    if (_api_name == "")
	err_quit("Connect: api_name not set by child class");
	
    return _api_name;
}

DAS &
Connect::das()
{
    if (_local)
	err_quit("Connect::das is only vaild for a remote connection");

    return _das;
}

DDS &
Connect::dds()
{
    if (_local)
	err_quit("Connect::dds is only vaild for a remote connection");

    return _dds;
}
