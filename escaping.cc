
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1996, California Institute of Technology.
// ALL RIGHTS RESERVED.   U.S. Government Sponsorship acknowledged.
//
// Please read the full copyright notice in the file COPYRIGHT
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
#include <strstream>
#include <iomanip>
#include <string>
#include <Regex.h>

using std::string;
using std::ostrstream;
using std::istrstream;
using std::setfill;
using std::hex;
using std::setw;
using std::oct;
using std::ends;

const int MAXSTR = 256;

// The next four functions were originally defined static, but I removed that
// to make testing them (see generalUtilTest.cc) easier to write. 5/7/2001
// jhrg

string hexstring(unsigned char val) {
    static char buf[MAXSTR];

    ostrstream(buf,MAXSTR) << hex << setw(2) << setfill('0') <<
	(unsigned int)val << ends;

    return (string)buf;
}

string unhexstring(string s) 
{
#if 0
    // Originally this code returned val, the string declared below. This
    // does not work with g++ 2.95.2 (and I'm not sure that it ever did
    // work...). Note the hoops you must jump through to convert a single
    // char into a string object. 5/4/2001 jhrg 
    string val;
#endif
    int val;
    istrstream(s.c_str(),MAXSTR) >> hex >> val;
    static char tmp_str[2];
    tmp_str[0] = static_cast<char>(val);
    tmp_str[1] = '\0';
    return string(tmp_str);
}

string octstring(unsigned char val) {
    static char buf[MAXSTR];

    ostrstream(buf,MAXSTR) << oct << setw(3) << setfill('0') <<
	(unsigned int)val << ends;

    return (string)buf;
}

string unoctstring(string s) {
    int val;

    istrstream(s.c_str(),MAXSTR) >> oct >> val;

    static char tmp_str[2];
    tmp_str[0] = static_cast<char>(val);
    tmp_str[1] = '\0';
    return string(tmp_str);
}

/** Replace characters that are not allowed in DODS identifiers.

    @param s The string in which to replace characters.
    @param allowable The set of characters that are allowed in a URI.
    default: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz
    0123456789_.!~*'()-";
    @return The modified identifier. */
string
id2www(string in, const string &allowable)
{
    static const string ESC = "%";
    string::size_type i = 0;

    while ((i = in.find_first_not_of(allowable, i)) != string::npos) {
	in.replace(i, 1, ESC + hexstring(in[i]));
	i++;
    }

    // We cannot undo this with certainty. 7/20/2001 jhrg
    // I think this was added before the parsers could handle names that
    // began with digits. Note the IDs still cannot. 7/26/2001 jhrg
#if 0
    if (isdigit(in[0]))
	in.insert(0, "_");
#endif

    return in;
}

/** Replace characters that are not allowed in WWW URLs. This function bends
    the rules so that characters that we've been sending in CEs will be sent
    unencoded. This is needed to keep the newer clients from breaking the old
    servers. The only difference between this function and id2www is that
    #[]:{}&# have been added to the allowable set of characters.

    @param s The string in which to replace characters.
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

    @param s The string to modify.
    @param escape The character used to signal the begining of an escape
    sequence. default: "%"
    @param except If there is some escape code that should not be removed by
    this call (e.g., you might not want to remove spaces, %20) use this
    parameter to specify that code. The function will then transform all
    escapes \em{except} that one. default: ""
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

/** Return a string that has all the `%<hex digit><hex digit>' sequences
    replaced with underscores (`_').
    @param s The string to transform
    @param escape A regular expression which matches the `%<hd><hd>' pattern.
    By default this is the string "%[0-7][0-9a-fA-F]". Replacing the pattern
    allows the function to be used to map other patterns to an underscore.
    @return The modified string. */
string 
esc2underscore(string s, const string escape = "%[0-7][0-9a-fA-F]") {
  Regex escregx(escape.c_str(), 1);

    int index=0, matchlen;
    while ((index = escregx.search(s.c_str(), s.size(), matchlen, index)) != -1)
      s.replace(index, matchlen, "_");

    return s;
}

/** Escape non-printable characters and quotes from an HDF attribute.
    @param s The attribute to modify.
    @return The modified attribute. */
string escattr(string s) {
    static Regex nonprintable("[^ !-~]", 1);
    const string ESC = "\\";
    const string QUOTE = "\"";
    const string ESCQUOTE = ESC + QUOTE;

    // escape non-printing characters with octal escape
    int index = 0, matchlen;
    while ( (index = nonprintable.search(s.c_str(), s.size(), matchlen, index)) != -1)
	s.replace(index,1, ESC + octstring(s[index]));

    // escape " with backslash
    string::size_type ind = 0;
    while ( (ind = s.find(QUOTE, ind)) != s.npos) {
	s.replace(ind, 1, ESCQUOTE);
	ind += ESCQUOTE.length();
    }

    return s;
}

/** Un-escape special characters, quotes and backslashes from an HDF attribute.
    <p>
    Note: A regex to match one \ must be defined as: Regex foo = "\\\\";
    because both C++ strings and GNU's Regex also employ \ as an escape
    character! 
    @param s The escaped attribute.
    @return The unescaped attribute. */
string unescattr(string s) {
    static Regex escregx("\\\\[01][0-7][0-7]", 1);  // matches 4 characters
    static Regex escquoteregex("[^\\\\]\\\\\"", 1);  // matches 3 characters
    static Regex escescregex("\\\\\\\\",1);      // matches 2 characters
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

// $Log: escaping.cc,v $
// Revision 1.18  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.16.2.5  2001/08/22 06:21:19  jimg
// Added id2www_ce(). This function calls id2www but uses an allowable
// character set that larger than id2www(). It allows []{}: and & in the CE
// part of a DODS URL. This was added to keep the new clients from breaking old
// servers.
//
// Revision 1.16.2.4  2001/07/28 00:49:26  jimg
// Changed the names and function of id2dods/dods2id. The new functions are
// called id2www and www2id. They use C++ strings, not Regexs. They scan their
// inputs and escape/unescape characters for the WWW. The character set that's
// allowed in a URL is defined by RFC 2396, ``Universal Resource Identifiers
// (URI): Generic Syntax.'' We're using that to determine which characters must
// be escaped and which are OK as is. For example, a dash (-) is allowed in a
// URL. Note that dashes are now processed by our parsers, too. Spaces, hashes
// (#), et c. are not allowed in a URL and will be escaped by id2www and
// unescaped by www2id.
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

