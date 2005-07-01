
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
// $RCSfile: escaping.cc,v $ - Miscellaneous routines for DODS HDF server
//
// These two routines are for escaping/unescaping strings that are identifiers
// in DODS
// id2www() -- escape (using WWW hex codes) non-allowable characters in a
// DODS identifier
// www2id() -- given an WWW hexcode escaped identifier, restore it
// 
// These two routines are for escaping/unescaping strings storing attribute
// values.  They use traditional octal escapes (\nnn) because they are
// intended to be viewed by a user
// escattr() -- escape (using traditional octal backslash) non-allowable
// characters in the value of a DODS attribute
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

/** Replace characters that are not allowed in DODS identifiers.

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
www2id(string in, const string &escape, const string &except)
{
    string::size_type i = 0;

    while ((i = in.find_first_of(escape, i)) != string::npos) {
	if (in.substr(i, 3) == except) {
	    i += 3;
	    continue;
	}
	in.replace(i, 3, unhexstring(in.substr(i + 1, 2)));
    }

    return in;
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
    @param escape A regular expression which matches the 
    \c %&lt;hd&gt;&lt;hd&gt; pattern.
    By default this is the string "%[0-7][0-9a-fA-F]". Replacing the pattern
    allows the function to be used to map other patterns to an underscore.
    @return The modified string. */
string 
esc2underscore(string s, const string escape = "%[0-7][0-9a-fA-F]")
{
    Regex escregx(escape.c_str(), 1);

    int index=0, matchlen;
    while ((index = escregx.search(s.c_str(), s.size(), matchlen, index)) != -1)
	s.replace(index, matchlen, "_");

    return s;
}

/** Given a string, replace any characters that match the regular expression
    with \c _<ASCII code> where \c <ASCII code> is the ASCII code for the
    characters replaced. If the pattern to be replaced is a WWW escape
    sequence, then replace the % with an _ and keep the two digit ASCII code
    that follows it. Note that the ASCII code is in hex.

    To escape all non-alphanumeric characters, use "[^A-Za-z0-9_]" for
    \e escape.

    @param s The string to transform
    @param escape A regular expression which matches the characters to
    replace. Note: Make sure that any expression given matches only a single
    character at a time (the default regex is handled specially).
    @return The modified string. */
string 
char2ASCII(string s, const string escape = "%[0-7][0-9a-fA-F]") 
{
    Regex escregx(escape.c_str(), 1);

    int i=0, matchlen;
    while ((i = escregx.search(s.c_str(), s.size(), matchlen, i)) != -1) {
	if (escape == "%[0-7][0-9a-fA-F]" && matchlen == 3)
	    s.replace(i, 1, "_");
	else {
	    if (matchlen != 1)
		throw InternalErr(__FILE__, __LINE__, "A caller supplied value for the regular expression in escape should match exactly one character.");

	    unsigned char ascii = *(s.substr(i, 1).data());
	    ostringstream ostr;
	    ostr << "_" << hexstring(ascii);
	    s.replace(i, matchlen, ostr.str());
	}
    }

    return s;
}

/** Escape non-printable characters and quotes from an HDF attribute.
    @param s The attribute to modify.
    @return The modified attribute. */
string 
escattr(string s) {
    Regex nonprintable("[^ !-~]", 1);
    const string ESC = "\\";
    const string DOUBLE_ESC = ESC + ESC;
    const string QUOTE = "\"";
    const string ESCQUOTE = ESC + QUOTE;

    // escape non-printing characters with octal escape
    int index = 0, matchlen;
    while ( (index = nonprintable.search(s.c_str(), s.size(), matchlen, index)) != -1)
	s.replace(index,1, ESC + octstring(s[index]));

    // escape " with backslash
    string::size_type ind = 0;
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
    Regex escregx("\\\\[01][0-7][0-7]", 1);  // matches 4 characters
    Regex escquoteregex("[^\\\\]\\\\\"", 1);  // matches 3 characters
    Regex escescregex("\\\\\\\\",1);      // matches 2 characters
    const string ESC = "\\";
    const string QUOTE = "\"";
    const string ESCQUOTE = ESC + QUOTE;

    // unescape any octal-escaped ASCII characters
    int index = 0, matchlen;
    while ( (index = escregx.search(s.c_str(), s.size(), matchlen, index)) != -1) {
	s.replace(index,4, unoctstring(s.substr(index+1,3)));
	index++;
    }

    // unescape any escaped quotes
    index = 0;
    while ( (index = escquoteregex.search(s.c_str(), s.size(), matchlen, index)) != -1) {
	s.replace(index+1,2, QUOTE);
	index++;
    }

    // unescape any escaped backslashes
    index = 0;
    while ( (index = escescregex.search(s.c_str(), s.size(), matchlen, index)) != -1) {
	s.replace(index,2, ESC);
	index++;
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

// $Log: escaping.cc,v $
// Revision 1.30  2005/01/28 17:25:13  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.26.2.2  2005/01/18 23:13:50  jimg
// FIxed documentation.
//
// Revision 1.29  2004/02/05 23:46:51  jimg
// Fix for bug 696; removed an errant 'ends' call in octstring(). Never use ends
// with the stringstream classes.
//
// Revision 1.28  2003/12/08 18:02:30  edavis
// Merge release-3-4 into trunk
//
// Revision 1.26.2.1  2003/09/06 23:00:24  jimg
// Updated the documentation.
//
// Revision 1.27  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.26  2003/04/23 21:33:53  jimg
// Changes for the unit tests. This involved merging Rob's VC++ changes
// and fixing a bug in escaping.cc (a call to string::insert invalidated
// an iterator in a loop).
//
// Revision 1.25  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.24  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.23.2.1  2003/02/21 00:10:08  jimg
// Repaired copyright.
//
// Revision 1.23  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.22  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.16.2.12  2002/12/27 19:32:54  jimg
// Updated the documentation for char2ASCII (to reflect Dan's modification).
//
// Revision 1.16.2.11  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.16.2.10  2002/08/06 22:29:54  jimg
// Fixed functions here so that they are all MT-Safe. Mostly this meant that
// static Regex objects are now automatic variables. There might be a slight
// performance penalty, but it's a lot easier than adding pthread_once calls
// (something we can always do later on... assuming there isn't a better way to
// handle these implementations that doesn't involve Regex).
//
// Revision 1.21  2002/06/03 22:21:16  jimg
// Merged with release-3-2-9
//
// Revision 1.16.2.9  2002/02/13 20:29:25  dan
// Modified char2ASCII so that it returns '_20' rather
// '_32', the hex representation of escaped characters
// instead of the decimal ascii representation.
//
// Revision 1.16.2.8  2002/01/28 22:52:38  jimg
// I chagned escattr so that it now escapes single backslashes.
//
// Revision 1.16.2.7  2002/01/28 20:34:25  jimg
// *** empty log message ***
//
// Revision 1.20  2001/10/29 21:17:46  jimg
// Fixed some errors in the comments.
//
// Revision 1.19  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.16.2.6  2001/10/08 16:58:17  jimg
// Added munge_error_message(). This function ensures that the message is
// wrapped in double quotes and that any internal double quotes are escaped.
//
// Revision 1.18  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.16.2.5  2001/08/22 06:21:19  jimg
// Added id2www_ce(). This function calls id2www but uses an allowable
// character set that larger than id2www(). It allows []{}: and & in the CE
// part of a DODS URL. This was added to keep the new clients from breaking
// old servers.
//
// Revision 1.16.2.4  2001/07/28 00:49:26  jimg
// Changed the names and function of id2dods/dods2id. The new functions are
// called id2www and www2id. They use C++ strings, not Regexs. They scan
// their inputs and escape/unescape characters for the WWW. The character set
// that's allowed in a URL is defined by RFC 2396, ``Universal Resource
// Identifiers (URI): Generic Syntax.'' We're using that to determine which
// characters must be escaped and which are OK as is. For example, a dash (-)
// is allowed in a URL. Note that dashes are now processed by our parsers,
// too. Spaces, hashes (#), et c. are not allowed in a URL and will be
// escaped by id2www and unescaped by www2id.
//
// Revision 1.17  2001/06/15 23:49:04  jimg
// Merged with release-3-2-4.
//
// Revision 1.16.2.3  2001/05/16 21:09:19  jimg
// Modified www2id so that one escape code can be considered exceptional
// and not transformed back into its ASCII equivalent. This is used by
// DODSFilter to suppress turning %20 into a space before the CE is feed
// into its parser. This means that somewhere further down the line the
// strings need to be scanned again for escapes (which should then be only
// %20s).
//
// Revision 1.16.2.2  2001/05/07 23:01:47  jimg
// Fixed the same bug in unoctstring as was fixed in unhexstring.
//
// Revision 1.16.2.1  2001/05/07 17:16:18  jimg
// Fixes to the unhexstring function.
//
// Revision 1.16  2000/10/03 05:00:21  rmorris
// string.insert(), for names that begin with 0-9 was causing an exception
// in that case.  Observed in the hdf server on modis data.  replaced
// string.insert with string = string("_") + string to fix - in id2www().
//
// Revision 1.15  2000/08/29 21:22:55  jimg
// Merged with 3.1.9
//
// Revision 1.9.6.4  2000/08/25 23:44:55  jimg
// Fixed an error in esc2underscore. A static instance of Regex was used but
// the value of that Regex was set using one of the method's parameters. This
// meant that while the first call to the method worked, all subsequent calls
// used whatever regular expression was given for the first call. I actually
// fixed this in several methods.
//
// Revision 1.9.6.3  2000/03/31 18:03:11  jimg
// Fixed bugs with the string class
//
// Revision 1.9.6.2  2000/01/27 00:03:05  jimg
// Fixed the return type of string::find.
//
// Revision 1.9.6.1  1999/08/16 23:32:01  jimg
// Added esc2underscore
//
// Revision 1.9  1999/04/29 02:29:36  jimg
// Merge of no-gnu branch
//
// Revision 1.8  1998/09/10 23:37:11  jehamby
// Forgot to update hexstring() to generate correct high-ASCII escapes.
//
// Revision 1.7  1998/09/10 19:38:03  jehamby
// Update escaping routines to not mangle high-ASCII characters with toascii()
// and to generate a correct escape sequence in octstring() for such characters
// through judicious casting (cast to a unsigned char, then an unsigned int).
//
// Revision 1.6.6.1  1999/02/02 21:57:07  jimg
// String to string version
//
// Revision 1.6  1998/03/19 23:29:47  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.5  1998/02/05 20:14:02  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.4  1997/02/14 04:18:10  jimg
// Added allowable and escape parameters to id2www and www2id so that the
// builtin regexs can be overridden if needed.
// Switched to the `fast compile' mode for the Regex objects.
//
// Revision 1.3  1997/02/14 02:24:44  jimg
// Removed reliance on the dods-hdf code.
// Introduced a const int MAXSTR with value 256. This matches the length of
// ID_MAX in the parser.h header (which I did not include since it defines
// a lot of software that is irrelevant to this module).
//
// Revision 1.2  1997/02/14 02:18:16  jimg
// Added to DODS core
//
// Revision 1.2  1996/10/07 21:15:17  todd
// Changes escape character to % from _.
//
// Revision 1.1  1996/09/24 22:38:16  todd
// Initial revision
//
//
/////////////////////////////////////////////////////////////////////////////

