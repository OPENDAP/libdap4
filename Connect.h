// -*- C++ -*-

// This class contains information about a connection made to a netcdf data
// file through the dods-nc library. Each dataset accessed must be assigned a
// unique ID. This ID is used to determine whether the dataset is local or
// remote. If it is local, the ID is used to fetch the ncid that the local
// netcdf library function assigned to the open file. If the file is remote,
// the ID is used to access information about the remote connection.
//
// This class should be used as a base class because its ctor is used to make
// the CONNECT object for either a DODS data server or a local file (i.e., it
// might be used to pass the arguments from the user program's data access API
// straight into that API's open function). Thus, for each surrogate library,
// this class must be subclassed and that subclass must define a ctor with
// the proper type of arguments for the API's open function. See the class
// NCConnect for an example ctor.
//
// jhrg 9/29/94

/* $Log: Connect.h,v $
/* Revision 1.8  1995/05/30 18:42:47  jimg
/* Modified the request_data member function so that it accepts the variable
/* in addition to the existing arguments.
/*
 * Revision 1.7  1995/05/22  20:43:12  jimg
 * Removed a paramter from the REQUEST_DATA member function: POST is not
 * needed since we no longer use the post mechanism.
 *
 * Revision 1.6  1995/04/17  03:20:52  jimg
 * Removed the `api' field.
 *
 * Revision 1.5  1995/03/09  20:36:09  jimg
 * Modified so that URLs built by this library no longer supply the
 * base name of the CGI. Instead the base name is stripped off the front
 * of the pathname component of the URL supplied by the user. This class
 * append the suffix _das, _dds or _serv when a Connect object is used to
 * get the DAS, DDS or Data (resp).
 *
 * Revision 1.4  1995/02/10  21:54:52  jimg
 * Modified definition of request_data() so that it takes an additional
 * parameter specifying sync or async behavior.
 *
 * Revision 1.3  1995/02/10  04:43:17  reza
 * Fixed the request_data to pass arguments. The arguments string is added to
 * the file name before being posted by NetConnect. Default arg. is null.
 *
 * Revision 1.2  1995/01/31  20:46:56  jimg
 * Added declaration of request_data() mfunc in Connect.
 *
 * Revision 1.1  1995/01/10  16:23:04  jimg
 * Created new `common code' library for the net I/O stuff.
 *
 * Revision 1.2  1994/10/05  20:23:28  jimg
 * Fixed errors in *.h files comments - CVS bites again.
 * Changed request_{das,dds} so that they use the field `_api_name'
 * instead of requiring callers to pass the api name.
 *
 * Revision 1.1  1994/10/05  18:02:08  jimg
 * First version of the connection management classes.
 * This commit also includes early versions of the test code.
 */

#ifndef _connect_h
#define _connect_h

#ifdef __GNUG__
#pragma "interface"
#endif

#include <String.h>

#include "DAS.h"
#include "DDS.h"

class Connect {
private:
    bool _local;		// is this a local connection
    String _URL;		// URL to remote dataset; --> LOCAL is false
    DAS _das;			// dataset attribute structure --> !LOCAL
    DDS _dds;			// dataset descriptor structure --> ! LOCAL

#ifdef NEVER
    String make_url(const String &cgi);
#endif
    void parse_url(const char *name);

protected:
    String _access;		// broken-out URL components
    String _host;
    String _cgi_basename;	// base name of CGI (e.g., def --> def_das)
    String _path;
    String _anchor;

public:
    // child classes can use these ctors
    Connect(const String &name, const String &api = ""); 
    virtual ~Connect();		// base classes should have virtual dtors

    bool is_local();
    const String &URL();
    const String &api_name();	// deprecated
    DAS &das();
    DDS &dds();
    
    // get the DAS, DDS and data from the server/cgi comb using the URL
    bool request_das(const String &cgi = "das");
    bool request_dds(const String &cgi = "dds");
    bool request_data(const String expr, bool async = false, 
		      const String &cgi = "dods");
};

typedef Connect * ConnectPtr;

#endif // _connect_h
