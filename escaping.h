
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Declarations for identifier escaping and un-escaping functions.

// $Log: escaping.h,v $
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
//

#ifdef WIN32
using namespace std;
#endif

string hexstring(unsigned char val);
string unhexstring(string s);
string octstring(unsigned char val);
string unoctstring(string s);

string id2dods(string s, const string allowable = "[^0-9a-zA-Z_%]");
string dods2id(string s, const string escape = "%[0-7][0-9a-fA-F]");
string esc2underscore(string s, const string escape = "%[0-7][0-9a-fA-F]");
string escattr(string s);
string unescattr(string s);


