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

#include "Keywords.h"
#include "Error.h"
#include "escaping.h"

using namespace std;

namespace libdap {

Keywords::Keywords()
{
    m_init();
}

void Keywords::m_init()
{
    // Load known_keywords
    m_insert_tuple("dap2", dap_version, "2.0");
    m_insert_tuple("dap2.0", dap_version, "2.0");

    m_insert_tuple("dap3.2", dap_version, "3.2");
    m_insert_tuple("dap3.3", dap_version, "3.3");

    m_insert_tuple("dap4", dap_version, "4.0");
    m_insert_tuple("dap4.0", dap_version, "4.0");

#if 0
    d_known_keywords.insert(pair<string,double>(,2.0));
    d_known_keywords.insert(pair<string,double>("dap2.0",2.0));

    d_known_keywords.insert(pair<string,double>("dap3.2",3.2));
    d_known_keywords.insert(pair<string,double>("dap3.3",3.3));

    d_known_keywords.insert(pair<string,double>("dap4",4.0));
    d_known_keywords.insert(pair<string,double>("dap4.0",4.0));
#endif
}

Keywords::~Keywords()
{
}

void Keywords::m_insert_tuple(const keyword &k, keyword_kind kind, const keyword_value &v)
{
    kind_value_t kind_value = kind_value_t(kind,v);
    d_known_keywords.insert(pair<keyword,  kind_value_t >(k, kind_value));
}

/**
 * Add the keyword to the set of keywords that apply to this request.
 * @param kw The keyword
 */
void Keywords::add_keyword(const string &kw)
{
    if (!is_known_keyword(kw))
	throw Error("Keyword not known (" + kw + ")");

    kind_value_t kind_value = d_known_keywords.at(kw);
    keyword_value_t keyword_value = keyword_value_t (kw,kind_value.second);

    d_parsed_keywords[kind_value.first] = keyword_value;
}

/**
 * Is the word one of the known keywords for this version of libdap?
 * @param w
 * @return true if the keyword is known
 */
bool Keywords::is_known_keyword(const string &kw) const
{
    return d_known_keywords.count(kw) != 0;
}

/**
 * Get a list of the strings that make up the set of current keywords for
 * this request.
 * @return The list of keywords as a list of string objects.
 */
list<string> Keywords::get_keywords() const
{
    list<string> kws;
    map<keyword_kind, pair<string, string> >::const_iterator i;
    for (i = d_parsed_keywords.begin(); i != d_parsed_keywords.end(); ++i)
	kws.push_front((*i).second.first);

    return kws;
}


/**
 * Lookup a keyword_kind and return true if it has been set for this request,
 * otherwise return false.
 * @param kw Keyword
 * @return true if the keyword is set.
 */
bool Keywords::has_keyword_kind(const keyword_kind &kind) const
{
    return d_parsed_keywords.count(kind) != 0;
}

/** Look up the value for a keyword_kind that has been parsed.
 *
 * @param kind
 * @return The value
 */
string Keywords::get_kind_value(const keyword_kind &kind) const
{
    if (d_parsed_keywords.count(kind) == 0)
	return "";
    else
	return d_parsed_keywords.at(kind).second;
}

/** Look up the keyword (parsed) associated with a given keyword_kind.
 * @param kind
 * @return The keyword
 */
string Keywords::get_kind_keyword(const keyword_kind &kind) const
{
    if (d_parsed_keywords.count(kind) == 0)
	return "";
    else
	return d_parsed_keywords.at(kind).first;
}

/** Look in the dictionary for the value associated with a given keyword.
 *
 * @param k
 * @return The value
 */
string Keywords::get_keyword_value(const keyword &kw) const
{
    if (!is_known_keyword(kw))
	throw Error("Keyword not known (" + kw + ")");

    return d_known_keywords.at(kw).second;
}

/** Look in the dictionary for the keyword_kind assoicated with a given
 * keyword.
 * @param k
 * @return The keyword_kind
 */
Keywords::keyword_kind Keywords::get_keyword_kind(const keyword &kw) const
{
    if (!is_known_keyword(kw))
	throw Error("Keyword not known (" + kw + ")");

    return d_known_keywords.at(kw).first;
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
	if (is_known_keyword(next_word)) {
	    add_keyword(next_word);
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
