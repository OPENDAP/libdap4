// -*- C++ -*-

// This class contains information about a connection made to a netcdf data
// file through the dods-nc library. Each dataset accessed must be assigned a
// unique ID. This ID is used to determine whether the dataset is local or
// remote. If it is local, the ID is used to fetch the ncid that the local
// netcdf library function assigned to the open file. If the file is remote,
// the ID is used to access information about the remote connection.
//
// This class is a virtual base class because its ctor is used to make the
// connection to either a DODS data server or a local file (i.e., it might be
// used to pass the arguments from the user program's data access API
// straight into that API's open function. Thus, for each surrogate library,
// this class must be subclassed and that subclass must define a ctor with
// the proper type arguments for the API's open function. See the class
// NCConnect for an example ctor.
//
// jhrg 9/29/94

/* $Log: Connect.h,v $
/* Revision 1.2  1995/01/31 20:46:56  jimg
/* Added declaration of request_data() mfunc in Connect.
/*
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
    String _api_name;		// child ctors MUST set this.
    String _URL;		// URL to remote dataset; --> LOCAL is false
    DAS _das;			// dataset attribute structure --> !LOCAL
    DDS _dds;			// dataset descriptor structure --> ! LOCAL

    String make_url(const String &api, const String &cgi);
    void parse_url(const char *name);

protected:
    String _access;		// broken-out URL components
    String _host;
    String _path;
    String _anchor;

public:
    // child classes can use these ctors
    Connect(const String &name, const String &api); 
    Connect(const char *name, const String &api);
    virtual ~Connect();		// base classes should have virtual dtors

    bool is_local();
    const String &URL();
    const String &api_name();
    DAS &das();
    DDS &dds();
    
    bool request_das();		// get the das from the server in the URL
    bool request_dds();
    bool request_data();
};

typedef Connect * ConnectPtr;

#endif // _connect_h
