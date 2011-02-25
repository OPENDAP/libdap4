// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2011 OPeNDAP, Inc.
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
#include "config.h"

static char rcsid[] not_used = { "$Id: ResponseBuilder.cc 23477 2010-09-02 21:02:59Z jimg $" };

#include <iostream>

//#define DODS_DEBUG

#include "Keywords2.h"
#include "Error.h"
#include "escaping.h"
#include "debug.h"

using namespace std;

namespace libdap {

Keywords::Keywords()
{
    // Load known keywords
    d_known_keywords.insert("dap");
}

Keywords::~Keywords()
{
}

/** Static function to parse the curly-brace keyword notation.
 * @param kw the keyword clause '<word> ( <value> )'
 * @param word (result) the word
 * @param value (result) the value
 */
static void f_parse_keyword(const string &kw, string &word, string &value)
{
    DBG(cerr << "f_parse_keyword, kw: " << kw << endl);

    word = "";
    value = "";
    string::size_type i = kw.find('(');
    if (i == string::npos)
	return;
    word = kw.substr(0, i);
    string::size_type j = kw.find(')');
    if (j == string::npos)
	return;
    ++i; // Move past the opening brace
    value = kw.substr(i, j-i);

    DBG(cerr << "i: " << i << ", j: " << j << endl);
    DBG(cerr << "word: " << word << ", value: " << value << endl);
}

/**
 * Add the keyword to the set of keywords that apply to this request.
 * @note Should call m_is_valid_keyword first.
 * @param s The keyword, as a string, including its value.
 */
void Keywords::m_add_keyword(const keyword &word, const keyword_value &value)
{
    d_parsed_keywords[word] = value;
}

/** Is the string a valid keyword clause? This checks for the syntax
 * '<word> ( <value> )'.
 * @param s
 * @return True if the string is valid keyword clause
 */
bool Keywords::m_is_valid_keyword(const keyword &word, const keyword_value &value) const
{
	return (d_known_keywords.count(word) != 0 && !value.empty());
}

/**
 * Is the word one of the known keywords for this version of libdap?
 * @param s As a string, including the value
 * @return true if the keyword is known
 */
bool Keywords::is_known_keyword(const string &word) const
{
	return d_known_keywords.count(word) != 0;
}

/**
 * Get a list of the strings that make up the set of current keywords for
 * this request.
 * @return The list of keywords as a list of string objects.
 */
list<Keywords::keyword> Keywords::get_keywords() const
{
    list<keyword> kws;
    map<keyword, keyword_value>::const_iterator i;
    for (i = d_parsed_keywords.begin(); i != d_parsed_keywords.end(); ++i)
    	kws.push_front((*i).first);

    return kws;
}


/**
 * Lookup a keyword_kind and return true if it has been set for this request,
 * otherwise return false.
 * @param kw Keyword
 * @return true if the keyword is set.
 */
bool Keywords::has_keyword(const keyword &kw) const
{
    return d_parsed_keywords.count(kw) != 0;
}

/** Look in the dictionary for the value associated with a given keyword.
 *
 * @param k
 * @return The value
 */
Keywords::keyword_value Keywords::get_keyword_value(const keyword &kw) const
{
    if (d_known_keywords.count(kw) != 1)
    	throw Error("Keyword not known (" + kw + ")");

    return d_parsed_keywords.at(kw);
}

/** Parse the constraint expression, removing all keywords. As a side effect,
 * return the remaining CE.
 * @param ce
 * @return The CE stripped of all recognized keywords.
 */
string Keywords::parse_keywords(const string &ce)
{
    // Get the whole CE
    string projection = www2id(ce, "%", "%20");
    string selection = "";

    // Separate the selection part (which follows/includes the first '&')
    string::size_type amp = projection.find('&');
    if (amp != string::npos) {
	selection = projection.substr(amp);
	projection = projection.substr(0, amp);
    }

    // Extract keywords; add to the Keywords keywords. For this, scan for
    // a known set of keywords and assume that anything else is part of the
    // projection and should be left alone. Keywords must come before variables
    // The 'projection' string will look like: '' or 'dap4.0' or 'dap4.0,u,v'
    while (!projection.empty()) {
	string::size_type i = projection.find(',');
	string next_word = projection.substr(0, i);
	string word, value;
	f_parse_keyword(next_word, word, value);
	if (m_is_valid_keyword(word, value)) {
	    m_add_keyword(word, value);
	    if (i != string::npos)
		projection = projection.substr(i + 1);
	    else
		projection = "";
	}
	else {
	    break; // exit on first non-keyword
	}
    }

    // The CE is whatever is left after removing the keywords
    return projection + selection;
}

}
