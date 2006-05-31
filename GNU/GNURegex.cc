
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

#include <new>
#include <string>
#include <stdexcept>

#include <GNURegex.h>
#include <Error.h>

using namespace std;

void
Regex::init(const char *t) throw(Error)
{
    int result = regcomp(&d_preg, t, REG_EXTENDED);

    if  (result != 0) {
        size_t msg_len = regerror(result, &d_preg, (char *)NULL, (size_t)0);
        char *msg = new char[msg_len+1];
        regerror(result, &d_preg, msg, msg_len);
        Error e(string("Regex error: ") + string(msg));
        delete[] msg;
        throw e;
    }
}

Regex::~Regex()
{
    regfree(&d_preg);
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
    <p><b> We Need To Fix A Bug in This Code That is Held At Bay By A Workaround
    In The Code. See In Line Comments For More Detail.</b></p>
    @param s The string
    @param len The length of string to consider
    @param pos Start looking at this position in the string
    @return The number of characters that match, -1 if there's no match. */
int 
Regex::match(const char*s, int len, int pos) const
{

    // @todo Fix this regex bug for real, stop using the workaround!
    //
    // The declaration of pmatch is a hack to work around a problem encountered 
    // on Potter's Mac (PowerMac G5, quad-processor, 2GB RAM, OS-X 10.4.6). 
    // The array declaration should be for one element, not two. However for
    // some reason when it's one the call to regexec works, but the call in the
    // return statement:
    //
    //     return pmatch[0].rm_eo - pmatch[0].rm_so; 
    //
    // causes a:
    //
    //     Program received signal EXC_BAD_ACCESS, Could not access memory.
    //     Reason: KERN_INVALID_ADDRESS at address: 0xfffffffc
    // Here:
    //    (gdb) where
    //    #0  0x947b20b0 in __gnu_cxx::__exchange_and_add ()
    //    #1  0x9479ce84 in std::string::_Rep::_M_dispose ()
    //    #2  0x9479f070 in std::basic_string<char, std::char_traits<char>, std::allocator<char> >::~basic_string ()
    //    #3  0x019125c8 in Regex::match (this=0xbfffe998, s=0x24026ec "/nc/123.nc", len=10, pos=0) at GNU/GNURegex.cc:117
    //
    // That's nasty, and we don't know why.
    // We need to fix it for real at some point.
    //
    // ndp 5/31/2006

    regmatch_t pmatch[2];
    string ss = s;
    
    const char* foo = ss.substr(pos, len).c_str();
    
    int result = regexec(&d_preg, foo, 1, pmatch, 0);
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
Regex::search(const char* s, int len, int& matchlen, int pos) const
{
    // alloc space for len matches, which is theoretical max.
    regmatch_t *pmatch = new regmatch_t[len];
    string ss = s;
     
    int result = regexec(&d_preg, ss.substr(pos, len).c_str(), len, pmatch, 0);
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

