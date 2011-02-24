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

#ifndef KEYWORDS_H_
#define KEYWORDS_H_

#include <string>
#include <set>
#include <list>

using namespace std;

namespace libdap {

/**
 * Manage keywords for libdap. These are passed in to the library using the
 * constraint expression - in fact they are an extension of the CE and this
 * class implements the parsing needed to remove them from the CE. The result
 * can then be used by the ConstraintExpression class.
 */
class Keywords {
private:
    set<string> d_keywords; 	/// Holds all of the keywords passed in the CE
    set<string> d_known_keywords; /// Holds all of the keywords libdap understands.

    void m_init();
    // Not needed w/o pointers Keyword &clone(const Keyword &rhs);

public:
    Keywords();
    virtual ~Keywords();

    virtual string parse_keywords(const string &ce);

    virtual void add_keyword(const string &kw);
    virtual bool is_keyword(const string &kw) const;
    virtual list<string> get_keywords() const;
    // This method holds all of the keywords that this version of libdap groks
    virtual bool is_known_keyword(const string &w) const;
};

}

#endif /* KEYWORDS_H_ */
