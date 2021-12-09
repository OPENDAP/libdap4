
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

#ifndef _Regex_h
#define _Regex_h 1

#define USE_CPP_11_REGEX 0

#if USE_CPP_11_REGEX
#include <regex>
#else
#include <string>
#endif

namespace libdap
{


/**
 * @brief Regular expression matching
 *
 * This class provides an interface that mimics the libgnu C++ library
 * that was used with the first version of libdap (c. 1993). It can been
 * re-implemented several times, this last time using the C++-11 regex
 * class. We found this was faster than the unix regex_t (man(3)) that
 * was being used.
 *
 * @note Make sure to compile the regular expressions only when really
 * needed (e.g., make Regex instances const, etc., when possible) since
 * it is an expensive operation
 *
 * @author James Gallagher <jgallagher@opendap.org>
 */
class Regex
{
private:
#if USE_CPP_11_REGEX
    std::regex d_exp;

    void init(const char *s) { d_exp = std::regex(s); }
    void init(const std::string &s) { d_exp = std::regex(s); } // , std::regex::basic
#else
    // d_preg was a regex_t* but I needed to include both regex.h and config.h
    // to make the gnulib code work. Because this header is installed (and is
    // used by other libraries) it cannot include config.h, so I moved the 
    // regex.h and config.h (among other) includes to the implementation. It
    // would be cleaner to use a special class, but for one field that seems
    // like overkill.
    void *d_preg;

    void init(const char *t);
    void init(const std::string &s) { init(s.c_str()); } // std::regex::ECMAScript
#endif

public:
    /// @brief initialize a Regex with a C string
    explicit Regex(const char *s) { init(s); }
    /// @deprecated
    Regex(const char *s, int) { init(s); }
    /// @brief nitialize a Regex with a C++ string
    explicit Regex(const std::string &s) { init(s); }

#if USE_CPP_11_REGEX
    ~Regex() = default;
#else
    ~Regex();
#endif

    /// @brief Does the pattern match.
    int match(const char *s, int len, int pos = 0) const;
    /// @brief Does the pattern match.
    int match(const std::string &s) const;

    /// @brief How much of the string does the pattern match.
    int search(const char *s, int len, int &matchlen, int pos = 0) const ;
    /// @brief How much of the string does the pattern match.
    int search(const std::string &s, int &matchlen) const;
};

#if 0
class Regex
{
private:
    -    std::regex d_exp;
    -
    -    void init(const char *s) { d_exp = std::regex(s); }
    -    void init(const std::string &s) { d_exp = std::regex(s); } // , std::regex::basic
    +    // d_preg was a regex_t* but I needed to include both regex.h and config.h
    +    // to make the gnulib code work. Because this header is installed (and is
    +    // used by other libraries) it cannot include config.h, so I moved the 
    +    // regex.h and config.h (among other) includes to the implementation. It
    +    // would be cleaner to use a special class, but for one field that seems
    +    // like overkill.
    +    void *d_preg;
    +    void init(const char *t);

public:
    -    /// @brief initialize a Regex with a C string
    -    explicit Regex(const char *s) { init(s); }
    -    /// @deprecated
    -    Regex(const char *s, int) { init(s); }
    -    /// @brief nitialize a Regex with a C++ string
    -    explicit Regex(const std::string &s) { init(s); }
    -
    -    ~Regex() = default;
    -
    -    /// @brief Does the pattern match.
    -    int match(const char *s, int len, int pos = 0) const;
    -    /// @brief Does the pattern match.
    -    int match(const std::string &s) const;
    -
    -    /// @brief How much of the string does the pattern match.
    -    int search(const char *s, int len, int &matchlen, int pos = 0) const ;
    -    /// @brief How much of the string does the pattern match.
    -    int search(const std::string &s, int &matchlen) const;
    +    Regex(const char *t);
    +    Regex(const char *t, int dummy);
    +    ~Regex();
    +
    +    /// Does the pattern match.
    +    int match(const char* s, int len, int pos = 0);
    +    /// How much of the string does the pattern match.
    +    int search(const char* s, int len, int& matchlen, int pos = 0);
};
#endif

} // namespace libdap

#endif
