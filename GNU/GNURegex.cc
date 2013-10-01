
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.


//#define DODS_DEBUG

#include <config.h>

#ifndef WIN32
#include <alloca.h>
#endif
#include <stdlib.h>
 
#include <sys/types.h>
#include <regex.h>

#include <new>
#include <string>
#include <vector>
#include <stdexcept>

#include "GNURegex.h"
#include "Error.h"
#include "util.h"
#include "debug.h"


using namespace std;

namespace libdap {

void
Regex::init(const char *t)
{
    DBG( cerr << "Regex::init() - BEGIN" << endl);

    DBG( cerr << "Regex::init() - creating new regex..." << endl);
    d_preg = static_cast<void*>(new regex_t);

    DBG( cerr << "Regex::init() - Calling regcomp()..." << endl);
    int result = regcomp(static_cast<regex_t*>(d_preg), t, REG_EXTENDED);

    if  (result != 0) {
        DBG( cerr << "Regex::init() - Call to regcomp FAILED" << endl);
        DBG( cerr << "Regex::init() - Calling regerror()..." << endl);
        size_t msg_len = regerror(result, static_cast<regex_t*>(d_preg),
                                  static_cast<char*>(NULL),
                                  static_cast<size_t>(0));

        DBG( cerr << "Regex::init() - Creating message" << endl);
        vector<char> msg(msg_len+1);
        //char *msg = new char[msg_len+1];
        DBG( cerr << "Regex::init() - Calling regerror() again..." << endl);
        regerror(result, static_cast<regex_t*>(d_preg), &msg[0], msg_len);
        DBG( cerr << "Regex::init() - Throwing libdap::Error" << endl);
        throw Error(string("Regex error: ") + string(&msg[0]));
        //delete[] msg;
        //throw e;
    }
    DBG( cerr << "Regex::init() - Call to regcomp() SUCCEEDED" << endl);
    DBG( cerr << "Regex::init() - END" << endl);
}

Regex::~Regex()
{
    regfree(static_cast<regex_t*>(d_preg));
    delete static_cast<regex_t*>(d_preg); d_preg = 0;

}

/** Initialize a POSIX regular expression (using the 'extended' features).

    @param t The regular expression pattern. */
Regex::Regex(const char* t)
{
    init(t);
}

/** Compatability ctor.
    @see Regex::Regex(const char* t) */
Regex::Regex(const char* t, int)
{
    init(t);
}

/** Does the regular expression match the string? 

    @param s The string
    @param len The length of string to consider
    @param pos Start looking at this position in the string
    @return The number of characters that match, -1 if there's no match. */
int 
Regex::match(const char* s, int len, int pos)
{
   if (len > 32766)	// Integer overflow protection
    	return -1;
    	
    regmatch_t *pmatch = new regmatch_t[len+1];
    string ss = s;

    int result = regexec(static_cast<regex_t*>(d_preg), 
                         ss.substr(pos, len-pos).c_str(), len, pmatch, 0);
	int matchnum;
    if (result == REG_NOMATCH)
        matchnum = -1;
	else
		matchnum = pmatch[0].rm_eo - pmatch[0].rm_so;
		
	delete[] pmatch; pmatch = 0;

    return matchnum;
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
	// sanitize allocation
    if (!size_ok(sizeof(regmatch_t), len+1))
    	return -1;
    	
    // alloc space for len matches, which is theoretical max.
    // Problem: If somehow 'len' is very large - say the size of a 32-bit int,
    // then len+1 is a an integer overflow and this might be exploited by
    // an attacker. It's not likely there will be more than a handful of
    // matches, so I am going to limit this value to 32766. jhrg 3/4/09
    if (len > 32766)
    	return -1;

    regmatch_t *pmatch = new regmatch_t[len+1];
    string ss = s;
     
    int result = regexec(static_cast<regex_t*>(d_preg),
                         ss.substr(pos, len-pos).c_str(), len, pmatch, 0);
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

} // namespace libdap

