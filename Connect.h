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
/* Revision 1.10  1996/02/01 21:45:33  jimg
/* Added list of DDSs and constraint expressions that produced them.
/* Added mfuncs to manage DDS/CE list.
/*
 * Revision 1.9  1995/06/27  19:33:49  jimg
 * The mfuncs request_{das,dds,dods} accept a parameter which is appended to the
 * URL and used by the data server CGI to select which filter program is run to
 * handle a particular request. I changed the parameter name from cgi to ext to
 * better represent what was going on (after getting confused several times
 * myself).
 *
 * Revision 1.8  1995/05/30  18:42:47  jimg
 * Modified the request_data member function so that it accepts the variable
 * in addition to the existing arguments.
 *
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
#include <SLList.h>

#include "DAS.h"
#include "DDS.h"

class Connect {
private:
    struct constraint {
	String _expression;
	DDS _dds;
    };

    bool _local;		// is this a local connection

    String _URL;		// URL to remote dataset; --> LOCAL is false
    DAS _das;			// dataset attribute structure --> !LOCAL
    DDS _dds;			// dataset descriptor structure --> ! LOCAL

    SLList<constraint> _data;	// list of expressions & DDSs

    void parse_url(const char *name);

protected:
    String _access;		// broken-out URL components
    String _host;
    String _cgi_basename;	// base name of CGI (e.g., def --> def_das)
    String _path;
    String _anchor;

public:
    Connect(const String &name); 
    virtual ~Connect();		// base classes should have virtual dtors

    bool is_local();

    const String &URL();
    DAS &das();
    DDS &dds();
    
    // get the DAS, DDS and data from the server/cgi comb using a URL
    bool request_das(const String &ext = "das");
    bool request_dds(const String &ext = "dds");

    DDS &request_data(const String expr, bool async = false, 
		      const String &ext = "dods");

    // For each data access there is an associated constraint expression
    // (even if it is null) and a resulting DDS which describes the type(s)
    // of the variables that result from evaluating the CE in the environment
    // of the dataset referred to by a particular instance of Connect.
    Pix first_constraint();
    void next_constraint(Pix &p);
    String constraint_expression(Pix p);
    DDS &constraint_dds(Pix p);

    // For every new data read initiated using this connect, there is a DDS
    // and constraint expression. The data itself is stored in the dds in the
    // constraint object.
    DDS &append_constraint(String expr, DDS &dds);
};

#endif // _connect_h
