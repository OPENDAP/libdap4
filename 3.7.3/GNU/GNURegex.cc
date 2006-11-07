
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2005 OPeNDAP, Inc.
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

#include <config.h>

#include <new>
#include <string>
#include <stdexcept>

#include <GNURegex.h>

using namespace std;

void
Regex::init(const char *t) throw(Error)
{
    d_preg = new regex_t;
    int result = regcomp(d_preg, t, REG_EXTENDED);

    if  (result != 0) {
        size_t msg_len = regerror(result, d_preg, (char *)NULL, (size_t)0);
        char *msg = new char[msg_len+1];
        regerror(result, d_preg, msg, msg_len);
        Error e(string("Regex error: ") + string(msg));
        delete[] msg;
        throw e;
    }
}

Regex::~Regex()
{
    regfree(d_preg);
    delete d_preg; d_preg = 0;

}

/** Initialize a POSIX regular expression (using the 'extended' features).

    @param t The regular expression pattern. */
Regex::Regex(const char* t) throw(Error)
{
    init(t);
}

/** Compatability ctor.
    @see Regex::Regex(const char* t) */
Regex::Regex(const char* t, int) throw(Error)
{
    init(t);
}

/** Does the regular expression match the string? 

    @param s The string
    @param len The length of string to consider
    @param pos Start looking at this position in the string
    @return The number of characters that match, -1 if there's no match. */
int 
Regex::match(const char*s, int len, int pos)
{
    regmatch_t pmatch[1];
    string ss = s;
    
    int result = regexec(d_preg, ss.substr(pos, len).c_str(), 1, pmatch, 0);
    if (result == REG_NOMATCH)
        return -1;

    
    return pmatch[0].rm_eo - pmatch[0].rm_so;
}

/** Does the regular expression match the string? 

    @param s The string
    @param len The length of string to consider
    @param matchlen Return the length of the matched portion in this 
    value-result parameter.
    @param pos Start looking at this position in the string
    @return The start position of the first match. This is different from 
    POSIX regular expressions, whcih return the start position of the 
    longest match. */
int 
Regex::search(const char* s, int len, int& matchlen, int pos)
{
    // alloc space for len matches, which is theoretical max.
    regmatch_t *pmatch = new regmatch_t[len];
    string ss = s;
     
    int result = regexec(d_preg, ss.substr(pos, len).c_str(), len, pmatch, 0);
    if (result == REG_NOMATCH) {
        delete[] pmatch; pmatch = 0;
        return -1;
    }

    // Match found, find the first one (pmatch lists the longest first)
    int m = 0;
    for (int i = 1; i < len; ++i)
        if (pmatch[i].rm_so != -1 && pmatch[i].rm_so < pmatch[m].rm_so)
            m = i;
            
    matchlen = pmatch[m].rm_eo - pmatch[m].rm_so;
    int matchpos = pmatch[m].rm_so;
    
    delete[] pmatch; pmatch = 0;
    return matchpos;
}

