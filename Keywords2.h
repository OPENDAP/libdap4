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

#ifndef KEYWORDS_H_
#define KEYWORDS_H_

#include <string>
#include <set>
#include <map>
#include <list>

using namespace std;

namespace libdap {

/**
 * Manage keywords for libdap. These are passed in to the library using the
 * constraint expression - in fact they are an extension of the CE and this
 * class implements the parsing needed to remove them from the CE so that
 * the ConstraintExpression evaluator can parse it (because the keywords are
 * not identifiers in the DDS, they will cause a parse error.
 *
 * @note If pointers are added to this code, modify DDS so copying still works!
 *
 * @note The keywords are used to specify the DAP version(s) that the client
 * can understand.
 *
 * @note Keywords are parsed and used by the BES in Hyrax - libdap never makes
 * calls to these methods.
 */
class Keywords {
public:
    // convenience types
    typedef string keyword;
    typedef string keyword_value;
    typedef set<keyword_value> value_set_t;

private:
    /// Holds the keywords and value of the keywords passed in the CE
    map<keyword, keyword_value> d_parsed_keywords;

    /// Holds all of the keywords
    map<keyword, value_set_t> d_known_keywords;

    virtual void m_add_keyword(const keyword &word, const keyword_value &value);
    virtual bool m_is_valid_keyword(const keyword &word, const keyword_value &value) const;

public:
    Keywords();
    virtual ~Keywords();

    virtual string parse_keywords(const string &ce);

    // Is this keyword in the dictionary?
    virtual bool is_known_keyword(const string &s) const;

    // Get a list of all of the keywords parsed
    virtual list<keyword> get_keywords() const;
    // Has a particular keyword been parsed
    virtual bool has_keyword(const keyword &kw) const;

    // Get the parsed keyword (and it's dictionary value) of a particular kind
    virtual keyword_value get_keyword_value(const keyword &kw) const;
};

}

#endif /* KEYWORDS_H_ */
