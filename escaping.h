
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

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
//[0-7][0-9a-fA-F]
string www2id(string s, const string &escape = "%",
	      const string &except = "");
// Include these for compatibility with the old names. 7/19/2001 jhrg
#define id2dods id2www
#define dods2id www2id
string esc2underscore(string s, const string escape = "%[0-7][0-9a-fA-F]");
string escattr(string s);
string unescattr(string s);

// $Log: escaping.h,v $
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

