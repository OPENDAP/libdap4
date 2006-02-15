
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

//
// jhrg 9/19/97

#include "config.h"

static char rcsid[] not_used = {"$Id$"};


#include <iostream>
#include <sstream>
#include <string>

#include "DataDDS.h"
#include "debug.h"

using namespace std;

// private

/** Parse the version string. A string that does not parse causes the
    version to default to 0.0. This is better than throwing an Error since
    this method is called from a constructor. */
void
DataDDS::m_version_string_to_numbers()
{
    string num = d_server_version.substr(d_server_version.find('/')+1);
 
    if (!num.empty() && num.find('.') != string::npos) {
        istringstream iss(num);
        char c;
 
        iss >> d_server_version_major;
        iss >> c;               // This reads the `.' in the version string
        iss >> d_server_version_minor;
 
        // Did it parse?
        if (!(c == '.' && d_server_version_major > 0
              && d_server_version_minor > 0)) {

	    d_server_version_major = 0;
	    d_server_version_minor = 0;
        }
    }
    else {
        d_server_version_major = 0;
        d_server_version_minor = 0;
    }
 
    DBG(cerr << "Server version: " << d_server_version_major << "." \
        << d_server_version_minor << endl);
}

/** Parse the protocol string. A string that does not parse causes the
    version to default to 2.0. This is better than throwing an Error since
    this method is called from a constructor. */
void
DataDDS::m_protocol_string_to_numbers()
{
 
    if (!d_protocol_version.empty() && d_protocol_version.find('.') 
        != string::npos) {
        istringstream iss(d_protocol_version);
        char c;
 
        iss >> d_server_protocol_major;
        iss >> c;               // This reads the `.' in the version string
        iss >> d_server_protocol_minor;
 
        // Did it parse?
        if (!(c == '.' && d_server_protocol_major > 0)) {
            d_server_protocol_major = 2;
            d_server_protocol_minor = 0;
        }
    }
    else {
        d_server_protocol_major = 2;
        d_server_protocol_minor = 0;
    }
 
    DBG(cerr << "Server version: " << d_server_version_major << "." \
        << d_server_version_minor << endl);
}

// public

/** @brief Make an instance of DataDDS
    A DataDDS instance is a DDS with aditional information about the version
    of the server from which the data came. 
    @param factory Use this BaseTypeFactory to instantiate the variables.
    Caller must free; can also be set using the set_factory() method.
    @param n The name of the dataset. Can also be set using the
    set_dataset_name() method.
    @param v The server version.
    @param p The protocol version. */

DataDDS::DataDDS(BaseTypeFactory *factory, const string &n, const string &v,
        const string &p)
    : DDS(factory, n), d_server_version(v), d_protocol_version(p)
{
    m_version_string_to_numbers();
    m_protocol_string_to_numbers();
}
#if 0
// #ifdef DEFAULT_BASETYPE_FACTORY
/** @brief Make an instance of DataDDS
    A DataDDS instance is a DDS with aditional information about the version
    of the server from which the data came. This version of the constructor
    uses the default BaseTypeFactory class (which instantiates the Byte, ...,
    Grid classes defined here).
    @param n The name of the dataset. Can also be set using the
    set_dataset_name() method.
    @param v The version ctraing read from the server's response headers. */
DataDDS::DataDDS(const string &n, const string &v)
    : DDS(n), d_server_version(v)
{
    m_version_string_to_numbers();
}
// #endif
#endif
// $Log: DataDDS.cc,v $
// Revision 1.22  2005/03/30 21:34:32  jimg
// Now uses the BaseTypeFactory class.
//
// Revision 1.21  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.17.2.3  2004/07/02 20:41:51  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.20  2003/12/10 21:11:57  jimg
// Merge with 3.4. Some of the files contains erros (some tests fail). See
// the ChangeLog for information about fixes.
//
// Revision 1.19  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.17.2.2  2003/11/18 22:04:52  jimg
// I patched the parser for server versions so that the default values are 0.0.
// This default is assigned to the major and minor versions for any strings that
// fail to parse. I also removed old code for the Sequence level stuff.
//
// Revision 1.17.2.1  2003/06/05 20:15:26  jimg
// Removed many uses of strstream and replaced them with stringstream.
//
// Revision 1.18  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.17  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.16  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.15.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.15  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.14  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.10.4.7  2002/12/24 00:19:35  jimg
// I removed the Regex code from the implementation of server_version(). It
// seemed to serve no purpose. Ultimately, I'd like to remove all use of the
// Regex class...
//
// Revision 1.10.4.6  2002/12/01 14:37:52  rmorris
// Smalling changes for the win32 porting and maintenance work.
//
// Revision 1.10.4.5  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.10.4.4  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.10.4.3  2002/08/06 23:12:37  jimg
// Fixed this class so that it's MT-Safe by changing the static Regex in
// _version_string_to_numbers to an automatic variable.
//
// Revision 1.13  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.12  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.11  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.10.4.2  2001/08/18 00:18:26  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.10.4.1  2001/06/18 23:11:44  jimg
// Fixed the regex used to recognize version strings sent by servers. The new
// version string regex will enable parsing of both 3.2's version string with
// the server name and the older version strings (which just said `dods'). The
// new code also allows for a version/release name to be included in
// parentheses.
//
// Revision 1.10  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.9  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.8  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.7.20.1  2000/06/02 18:21:26  rmorris
// Mod's for port to Win32.
//
// Revision 1.7  1999/05/05 00:40:11  jimg
// Modified the DataDDS class so that a version string may begin with any
// character sequence, not just `dods'. This means that each server can
// identify itself in the version string.
//
// Revision 1.6  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.5  1998/11/10 01:01:11  jimg
// version_regex now fast compiled.
//
// Revision 1.4.4.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.4.4.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.4  1998/06/05 21:29:54  jimg
// Changed the version regex used to check for the version string pattern so
// that it includes a `wildcard' at the end. Thus versions like 2.15.1 and
// 2.15a can be read (both as major version == 2 and minor version == 15).
//
// Revision 1.3  1998/02/05 20:13:52  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.2  1997/12/16 00:37:14  jimg
// Changed _version_string_to_numbers() so that it does something sensible
// when the version string is hosed.
//
// Revision 1.1  1997/09/22 22:19:27  jimg
// Created this subclass of DDS to hold version information in the data DDS
//

