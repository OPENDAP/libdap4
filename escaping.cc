
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
 
// Copyright (c) 1996, California Institute of Technology.
// ALL RIGHTS RESERVED.   U.S. Government Sponsorship acknowledged.
//
// Please read the full copyright notice in the file COPYRIGHT_URI
// in this directory.
//
// Author: Todd Karakashian, NASA/Jet Propulsion Laboratory
//         Todd.K.Karakashian@jpl.nasa.gov
//
// $RCSfile: escaping.cc,v $ - Miscellaneous routines for OPeNDAP HDF server
//
// These two routines are for escaping/unescaping strings that are identifiers
// in DAP2
// id2www() -- escape (using WWW hex codes) non-allowable characters in a
// DAP2 identifier
// www2id() -- given an WWW hexcode escaped identifier, restore it
// 
// These two routines are for escaping/unescaping strings storing attribute
// values.  They use traditional octal escapes (\nnn) because they are
// intended to be viewed by a user
// escattr() -- escape (using traditional octal backslash) non-allowable
// characters in the value of a DAP2 attribute
// unescattr() -- given an octally escaped string, restore it
// 
// These are routines used by the above, not intended to be called directly:
// 
// hexstring()
// unhexstring()
// octstring()
// unoctstring()
// 
// -Todd

#include <ctype.h>

#include <iomanip>
#include <string>
#include <sstream>

#include "GNURegex.h"
#include "Error.h"
#include "InternalErr.h"
// #define DODS_DEBUG
#include "debug.h"

using namespace std;

// The next four functions were originally defined static, but I removed that
// to make testing them (see generalUtilTest.cc) easier to write. 5/7/2001
// jhrg

string 
hexstring(unsigned char val) 
{
    ostringstream buf; 
    buf << hex << setw(2) << setfill('0') 
	<< static_cast<unsigned int>(val);

    return buf.str();
}

string 
unhexstring(string s) 
{
    int val;
    istringstream ss(s);
    ss >> hex >> val;
    char tmp_str[2];
    tmp_str[0] = static_cast<char>(val);
    tmp_str[1] = '\0';
    return string(tmp_str);
}

string 
octstring(unsigned char val) 
{
    ostringstream buf;
    buf << oct << setw(3) << setfill('0') 
	<< static_cast<unsigned int>(val);

    return buf.str();
}

string
unoctstring(string s) 
{
    int val;

    istringstream ss(s);
    ss >> oct >> val;

    char tmp_str[2];
    tmp_str[0] = static_cast<char>(val);
    tmp_str[1] = '\0';
    return string(tmp_str);
}

/** Replace characters that are not allowed in DAP2 identifiers.

    @param in The string in which to replace characters.
    @param allowable The set of characters that are allowed in a URI.
    default: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz
    0123456789_.!~*'()-";
    @return The modified identifier. */
string
id2www(string in, const string &allowable)
{
    string::size_type i = 0;

    while ((i = in.find_first_not_of(allowable, i)) != string::npos) {
	in.replace(i, 1, "%" + hexstring(in[i]));
	i++;
    }

    return in;
}

/** Replace characters that are not allowed in WWW URLs. This function bends
    the rules so that characters that we've been sending in CEs will be sent
    unencoded. This is needed to keep the newer clients from breaking the old
    servers. The only difference between this function and id2www is that
    <code>[]:{}&</code> have been added to the allowable set of characters.

    @param in The string in which to replace characters.
    @param allowable The set of characters that are allowed in a URI.
    default: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz
    0123456789_.!~*'()-[]:{}&";
    @return The modified identifier. */
string
id2www_ce(string in, const string &allowable)
{
    return id2www(in, allowable);
}

/** Given a string that contains WWW escape sequences, translate those escape
    sequences back into ASCII characters. Return the modified string. 

    @param in The string to modify.
    @param escape The character used to signal the begining of an escape
    sequence. default: "%"
    @param except If there is some escape code that should not be removed by
    this call (e.g., you might not want to remove spaces, %20) use this
    parameter to specify that code. The function will then transform all
    escapes \e except that one. default: ""
    @return The modified string. */
string 
www2id(const string &in, const string &escape, const string &except)
{
    string::size_type i = 0;
    string res = in;
    while ((i = res.find_first_of(escape, i)) != string::npos) {
	if (res.substr(i, 3) == except) {
	    i += 3;
	    continue;
	}
	res.replace(i, 3, unhexstring(res.substr(i + 1, 2)));
    }

    return res;
}

static string
entity(char c)
{
    switch (c) {
      case '>': return "&gt;";
      case '<': return "&lt;";
      case '&': return "&amp;";
      case '\'': return "&apos;";
      case '\"': return "&quot;";
      default:
	throw InternalErr(__FILE__, __LINE__, "Unrecognized character.");
    }
}

/** Replace characters that are not allowed in XML

    @param in The string in which to replace characters.
    @param not_allowed The set of characters that are not allowed in XML.
    default: ><&'(single quote)"(double quote)
    @return The modified identifier. */
string
id2xml(string in, const string &not_allowed)
{
    string::size_type i = 0;

    while ((i = in.find_first_of(not_allowed, i)) != string::npos) {
	in.replace(i, 1, entity(in[i]));
	i++;
    }

    return in;
}

/** Given a string that contains XML escape sequences (i.e., entities),
    translate those back into ASCII characters. Return the modified string.

    @param in The string to modify.
    @return The modified string. */
string 
xml2id(string in)
{
    string::size_type i = 0;

    while ((i = in.find("&gt;", i)) != string::npos)
	in.replace(i, 4, ">");

    i = 0;
    while ((i = in.find("&lt;", i)) != string::npos)
	in.replace(i, 4, "<");

    i = 0;
    while ((i = in.find("&amp;", i)) != string::npos)
	in.replace(i, 5, "&");

    i = 0;
    while ((i = in.find("&apos;", i)) != string::npos)
	in.replace(i, 6, "'");

    i = 0;
    while ((i = in.find("&quot;", i)) != string::npos)
	in.replace(i, 6, "\"");

    return in;
}

/** Return a string that has all the \c %&lt;hex digit&gt;&lt;hex digit&gt; 
    sequences replaced with underscores (`_').

    @param s The string to transform
    @return The modified string. */
string 
esc2underscore(string s)
{
    string::size_type pos;
    while ((pos = s.find('%')) != string::npos)
        s.replace(pos, 3, "_");
    
    return s;
}


/** Escape non-printable characters and quotes from an HDF attribute.
    @param s The attribute to modify.
    @return The modified attribute. */
string 
escattr(string s) {
    const string printable = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~`!@#$%^&*()_-+={[}]|\\:;<,>.?/'\"";
    const string ESC = "\\";
    const string DOUBLE_ESC = ESC + ESC;
    const string QUOTE = "\"";
    const string ESCQUOTE = ESC + QUOTE;

    // escape non-printing characters with octal escape
    string::size_type ind = 0;
    while ( (ind = s.find_first_not_of(printable, ind)) != s.npos)
        s.replace(ind, 1, ESC + octstring(s[ind]));

    // escape " with backslash
    ind = 0;
    while ( (ind = s.find(ESC, ind)) != s.npos) {
	s.replace(ind, 1, DOUBLE_ESC);
	ind += DOUBLE_ESC.length();
    }

    ind = 0;
    while ( (ind = s.find(QUOTE, ind)) != s.npos) {
	s.replace(ind, 1, ESCQUOTE);
	ind += ESCQUOTE.length();
    }

    return s;
}

/** Un-escape special characters, quotes and backslashes from an HDF
    attribute.

    Note: A regex to match one \ must be defined as: Regex foo = "\\\\";
    because both C++ strings and GNU's Regex also employ \ as an escape
    character! 

    @param s The escaped attribute. @return The unescaped attribute. */
string 
unescattr(string s) {
    Regex escregx("\\\\[01][0-7][0-7]");  // matches 4 characters
    Regex escquoteregex("[^\\\\]\\\\\"");  // matches 3 characters
    Regex escescregex("\\\\\\\\");      // matches 2 characters
    const string ESC = "\\";
    const string QUOTE = "\"";
    const string ESCQUOTE = ESC + QUOTE;

    // unescape any octal-escaped ASCII characters
    int matchlen;
    unsigned int index = 0;
    index = escregx.search(s.c_str(), s.size(), matchlen, index);
    while (index < s.length()) {
	s.replace(index,4, unoctstring(s.substr(index+1,3)));
	index += 3;
	index = escregx.search(s.c_str(), s.size(), matchlen, index);
    }

    // unescape any escaped quotes
    index = 0;
    index = escquoteregex.search(s.c_str(), s.size(), matchlen, index);
    while (index < s.length()) {
	s.replace(index+1, 2, QUOTE);
	index = escquoteregex.search(s.c_str(), s.size(), matchlen, ++index);
    }

    // unescape any escaped backslashes
    index = 0;
    index = escescregex.search(s.c_str(), s.size(), matchlen, index);
    while (index < s.length()) {
	s.replace(index, 2, ESC);
	index = escescregex.search(s.c_str(), s.size(), matchlen, ++index);
    }

    return s;
}

string
munge_error_message(string msg)
{
    // First, add enclosing quotes if needed.
    if (*msg.begin() != '"')
	msg.insert(msg.begin(), '"');
    if (*(msg.end()-1) != '"')
	msg += "\"";

    // Now escape any internal double quotes that aren't escaped.
    string::iterator miter;
    for (miter = msg.begin()+1; miter != msg.end()-1; miter++)
	if (*miter == '"' && *(miter-1) != '\\')
	    miter = msg.insert(miter, '\\');

    return msg;
}

