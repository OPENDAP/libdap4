
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
 
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Declarations for identifier escaping and un-escaping functions.

#ifndef _escaping_h
#define _escaping_h

#include <string>

using std::string;

string hexstring(unsigned char val);
string unhexstring(string s);
string octstring(unsigned char val);
string unoctstring(string s);

// The original set of allowed characeters was: [0-9a-zA-Z_%]
// The characters accepted in DODS ids: [^-a-zA-Z0-9_/%.#:+\\()]
// The characters allowable in an id in a URI (see RFC 2396): 
// [-A-Za-z0-9_.!~*'()].

string id2www(string s, const string &allowable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.!~*'()-");

string id2www_ce(string s, const string &allowable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.!~*'()-[]:{}&=<>,");

string www2id(string s, const string &escape = "%",
	      const string &except = "");

// Include these for compatibility with the old names. 7/19/2001 jhrg
#define id2dods id2www
#define dods2id www2id

string esc2underscore(string s, const string escape = "%[0-7][0-9a-fA-F]");
string char2ASCII(string s, const string escape = "%[0-7][0-9a-fA-F]");
string escattr(string s);
string unescattr(string s);

string munge_error_message(string msg);

// $Log: escaping.h,v $
// Revision 1.12  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.11  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.10  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.5.4.10  2002/10/08 05:33:45  jimg
// Removed unnecessary #ifdef WIN32.
//
// Revision 1.5.4.9  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.9  2002/06/03 22:21:16  jimg
// Merged with release-3-2-9
//
// Revision 1.5.4.8  2002/02/03 23:57:59  rmorris
// Minor mod.  Someone ifdef'd out "#include <string> and using std::string;
// These are needed under win32.  I #ifdef'd them back in for win32.
//
// Revision 1.5.4.7  2002/01/28 20:34:25  jimg
// *** empty log message ***
//
// Revision 1.8  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.5.4.6  2001/10/08 16:57:09  jimg
// Added munge_error_message().
//
// Revision 1.7  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.5.4.5  2001/08/22 06:26:35  jimg
// Expanded the set of chars allowed by id2www_ce to include =, <> and ,
// (comma). I hope this is the last of 'em.
//
// Revision 1.5.4.4  2001/08/22 06:19:51  jimg
// Added prototype for id2www_ce() which has a slightly larger set of allowed
// characters than id2www().
//
// Revision 1.5.4.3  2001/08/18 00:03:40  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.5.4.2  2001/07/28 00:44:04  jimg
// Changed the names of id2dods and dods2id to id2www and www2id. I always had
// to stop and think about just what id2dods really meant. Note that I also
// changed what those functions do; see escaping.cc
//
// Revision 1.6  2001/06/15 23:49:04  jimg
// Merged with release-3-2-4.
//
// Revision 1.5.4.1  2001/05/16 21:09:49  jimg
// Modified dods2id() so that it takes a third argument, except.
//
// Revision 1.5  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.4  2000/07/09 21:57:10  rmorris
// Mods's to increase portability, minimuze ifdef's in win32 and account
// for differences between the Standard C++ Library - most notably, the
// iostream's.
//
// Revision 1.3  2000/06/07 18:07:00  jimg
// Merged the pc port branch
//
// Revision 1.2.4.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
//
// Revision 1.2  2000/03/31 21:07:03  jimg
// Merged with release-3-1-5
//
// Revision 1.1.2.1  2000/03/20 22:22:59  jimg
// Added more prototypes.
//
// Revision 1.1  1999/07/23 21:51:15  jimg
// Added

#endif // _escaping_h

