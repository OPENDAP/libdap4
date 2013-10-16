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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
#include "config.h"

#include <iostream>
#include <vector>

//#define DODS_DEBUG

#include "Keywords2.h"
#include "Error.h"
#include "escaping.h"
#include "debug.h"

using namespace std;

namespace libdap {

Keywords::Keywords()
{
    // Load known keywords and their allowed values
    vector<string> v1(7);
    v1[0] = "2"; v1[1] = "2.0"; v1[2] = "3.2"; v1[3] = "3.3"; v1[4] = "3.4";
    v1[5] = "4"; v1[6] = "4.0";
    value_set_t vs = value_set_t(v1.begin(), v1.end());
    d_known_keywords["dap"] = vs;

    vector<string> v2(4);
    v2[0] = "md5"; v2[1] = "MD5"; v2[2] = "sha1"; v2[3] = "SHA1";
    value_set_t vs2 = value_set_t(v2.begin(), v2.end());
    d_known_keywords["checksum"] = vs2;
}

Keywords::~Keywords()
{
}

/** Static function to parse the keyword notation.
 * @param kw the keyword clause '<word> ( <value> )'
 * @param word (result) the word
 * @param value (result) the value
 * @return True if the parse was successful (word and value contain useful
 * results) and False otherwise.
 */
static bool f_parse_keyword(const string &kw, string &word, string &value)
{
    word = "";
    value = "";
    string::size_type i = kw.find('(');
    if (i == string::npos)
	return false;
    word = kw.substr(0, i);
    string::size_type j = kw.find(')');
    if (j == string::npos)
	return false;
    ++i; // Move past the opening paren
    value = kw.substr(i, j-i);

    return (!word.empty() && !value.empty());
}

/**
 * Add the keyword to the set of keywords that apply to this request.
 * @note Should call m_is_valid_keyword first.
 * @param word The keyword/function name
 * @param value The keyword/function value
 */
void Keywords::m_add_keyword(const keyword &word, const keyword_value &value)
{
    d_parsed_keywords[word] = value;
}

/** Is the string a valid keyword clause? Assumption: the word and value have
 * already been successfully parsed.
 * @param word The keyword/function name
 * @param value The keyword/function value
 * @return True if the string is valid keyword and the value is one of the
 * allowed values.
 */
bool Keywords::m_is_valid_keyword(const keyword &word, const keyword_value &value) const
{
    map<keyword, value_set_t>::const_iterator ci = d_known_keywords.find(word);
    if (ci == d_known_keywords.end())
	return false;
    else {
	value_set_t vs = ci->second;

	if (vs.find(value) == vs.end())
	    throw Error("Bad value passed to the keyword/function: " + word);
    }

    return true;
}

/**
 * Is the word one of the known keywords for this version of libdap?
 * @param s As a string, including the value
 * @return true if the keyword is known
 */
bool Keywords::is_known_keyword(const string &word) const
{
	return d_known_keywords.count(word) == 1;
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
    return d_parsed_keywords.count(kw) == 1;
}

/** Look at the parsed keywords for the value associated with a given keyword.
 *
 * @param k
 * @return The value
 */
Keywords::keyword_value Keywords::get_keyword_value(const keyword &kw) const
{
    if (d_known_keywords.find(kw) == d_known_keywords.end())
    	throw Error("Keyword not known (" + kw + ")");

    return d_parsed_keywords.find(kw)->second;
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
	if (f_parse_keyword(next_word, word, value)
	    && m_is_valid_keyword(word, value)) {
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
