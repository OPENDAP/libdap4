
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Declarations for identifier escaping and un-escaping functions.

// $Log: escaping.h,v $
// Revision 1.2  2000/03/31 21:07:03  jimg
// Merged with release-3-1-5
//
// Revision 1.1.2.1  2000/03/20 22:22:59  jimg
// Added more prototypes.
//
// Revision 1.1  1999/07/23 21:51:15  jimg
// Added
//

string hexstring(unsigned char val);
char unhexstring(string s);
string octstring(unsigned char val);
char unoctstring(string s);

string id2dods(string s, const string allowable = "[^0-9a-zA-Z_%]");
string dods2id(string s, const string escape = "%[0-7][0-9a-fA-F]");
string esc2underscore(string s, const string escape = "%[0-7][0-9a-fA-F]");
string escattr(string s);
string unescattr(string s);


