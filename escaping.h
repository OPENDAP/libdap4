
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Declarations for identifier escaping and un-escaping functions.

// $Log: escaping.h,v $
// Revision 1.1  1999/07/23 21:51:15  jimg
// Added
//


string id2dods(string s, const string allowable = "[^0-9a-zA-Z_%]");
string dods2id(string s, const string escape = "%[0-7][0-9a-fA-F]");
string esc2underscore(string s, const string escape = "%[0-7][0-9a-fA-F]");
string escattr(string s);
string unescattr(string s);


