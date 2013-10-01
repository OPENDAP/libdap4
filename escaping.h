
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

namespace libdap
{

string hexstring(unsigned char val);
string unhexstring(string s);
string octstring(unsigned char val);
string unoctstring(string s);

// The original set of allowed characters was: [0-9a-zA-Z_%]
// The characters accepted in DAP2 ids: [-+a-zA-Z0-9_/%.\\#*]; everything
// else must be escaped.  Note that for some inscrutable reason, we've been
// escaping '*'.

// The characters allowable in an id in a URI (see RFC 2396):
// [-A-Za-z0-9_.!~*'()].

string id2www(string s, const string &allowable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-+_/.\\*");

// This is what DAP2 allows in a ce: [-+a-zA-Z0-9_/%.\\#]
string id2www_ce(string s, const string &allowable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-+_/.\\");

string www2id(const string &in, const string &escape = "%",
              const string &except = "");

// Include these for compatibility with the old names. 7/19/2001 jhrg
#define id2dods id2www
#define dods2id www2id

string octal_to_hex(const string &octal_digits);

string id2xml(string in, const string &not_allowed = "><&'\"");
string xml2id(string in);

string esc2underscore(string s);
string char2ASCII(string s, const string escape = "%[0-7][0-9a-fA-F]");
string escattr(string s);
string unescattr(string s);

string munge_error_message(string msg);

string unescape_double_quotes(string source);
string escape_double_quotes(string source);

} // namespace libdap

#endif // _escaping_h

