/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

// Implementation for Connect -- a virtual base class that manages
// connections to DODS servers or local files (the later case really isn't a
// connection, but DODS still uses a Connect object to refer to the open
// file).
// 
// jhrg 9/29/94
//
// Also includes the implementation for the class Connections,  which is used
// to hold and managed a set of open Connect *'s. 
//
// jhrg 9/30/94

// $Log: Connect.cc,v $
// Revision 1.15  1996/02/01 21:43:51  jimg
// Added mfuncs to maintain a list of DDSs and the constraint expressions
// that produced them.
// Added code in request_data to strip the incoming DDS from a data
// document.
// Fixed up bogus comments.
//
// Revision 1.14  1995/07/09  21:20:44  jimg
// Fixed date in copyright (it now reads `Copyright 1995 ...').
//
// Revision 1.13  1995/07/09  21:14:45  jimg
// Added copyright.
//
// Revision 1.12  1995/06/27  19:33:47  jimg
// The mfuncs request_{das,dds,dods} accept a parameter which is appended to the
// URL and used by the data server CGI to select which filter program is run to
// handle a particular request. I changed the parameter name from cgi to ext to
// better represent what was going on (after getting confused several times
// myself).
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

static char rcsid[]={"$Id: Connect.cc,v 1.15 1996/02/01 21:43:51 jimg Exp $"};

#ifdef __GNUG__
#pragma "implemenation"
#endif

#include <stdio.h>

#include "config_netio.h"
#include "errmsg.h"
#include "url_comp.h"		// C code to parse a URL

#include "Connect.h"

extern "C" FILE *NetExecute(char *); // defined in netexec.c
extern "C" FILE *NetConnect(char *); // defined in netexec.c
extern "C" FILE *move_dds(FILE *in); // defined in netexec.c
extern void set_xdrin(FILE *in); // defined in BaseType.cc
extern void set_xdrout(FILE *out); // define in BaseType.cc

// Private mfunc

// Given that the Connect object has been construcuted, return a URL that
// will get the correct document from the DODS server (which is a CGI script
// or program and one or more programs which read data sets). The type of
// document returned is selected using the `MIME type' part of a URL (i.e.,
// the URl's extension).
//
// NB: This mfunc has been changed significantly since the last revision.
//
// Returns: The URL of a CGI in a String object. 

void
Connect::parse_url(const char *name)
{
    URL_COMP *uc = url_comp(name);

    if (uc == NULL) {		// local file
	_local = true;
	_URL = "";		// null string
	// das & dds are initialized by children of Connect
    }
    else {
	_local = false;
	_URL = name;
    }

    free_url_comp(uc);
}

// public mfuncs

Connect::Connect(const String &name)
{
    parse_url((const char *)name);
}

Connect::~Connect()
{
}

// Added EXT which defaults to "das". jhrg 3/7/95

bool
Connect::request_das(const String &ext)
{
    // get the das 

    String das_url = _URL + "."  + ext;
    bool status = false;

    FILE *fp = NetExecute(das_url);

    if( fp ) 
      status = _das.parse(fp);    // read and parse the das from a file 

    fclose(fp);

    return status;
}

// Added EXT which deafults to "dds". jhrg 3/7/95

bool
Connect::request_dds(const String &ext)
{
    // get the dds 

    String dds_url = _URL + "." + ext;
    bool status = false;

    FILE *fp = NetExecute(dds_url);

    if( fp ) 
      status = _dds.parse(fp);    // read and parse the dds from a file 
      
    fclose(fp);
   
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
Connect::request_data(const String expr, bool async, const String &ext)
{
    String data_url = _URL + "." + ext + "?" + expr;
    FILE *fp;

    if (async)
	fp = NetConnect(data_url);
    else
	fp = NetExecute(data_url);
	
    if (!fp) {
	cerr << "Could not complete data request operation" << endl;
	exit(1);
    }

    // First read the DDS into a new object (using a file to store the DDS
    // temporarily - the parser/scanner won't stop reading until an EOF is
    // found, this fixes that problem).
    DDS dds;
    FILE *dds_fp = move_dds(fp);
    if (!dds.parse(dds_fp)) {
	cerr << "Could not parse return data description" << endl;
	exit (1);
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

    set_xdrin(fp);
   
    return d;
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
	err_quit("Connect::A URL is only valid for a remote connection");

    return _URL;		// if _local returns ""
}

DAS &
Connect::das()
{
    if (_local)
	err_quit("Connect::A das is only vaild for a remote connection");

    return _das;
}

DDS &
Connect::dds()
{
    if (_local)
	err_quit("Connect::A dds is only vaild for a remote connection");

    return _dds;
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
    constraint c;

    c._expression = expr;
    c._dds = dds;

    _data.append(c);

    return _data.rear()._dds;
}
