// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002 OPeNDAP, Inc.
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
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112

// This file was derived from the libwww source code of 1998/08/20. The
// copyright for the source of this derivative work can be found in the file
// COPYRIGHT_W3C.


#include "config.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
//#include <string>
#include <ctype.h>

#ifndef TM_IN_SYS_TIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

#include "util_mit.h"

using std::cerr;
using std::endl;
using std::string;

#include "debug.h"

namespace libdap {

static const char * months[12] =
    {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

#ifndef HAVE_STRFTIME
static const char * wkdays[7] =
    {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
#endif

/* Upper- and Lowercase macros

   The problem here is that toupper(x) is not defined officially unless
   isupper(x) is. These macros are CERTAINLY needed on #if defined(pyr) ||
   define(mips) or BDSI platforms. For safety, we make them mandatory.
*/

#ifndef TOLOWER
#define TOLOWER(c) tolower((int) (c))
#define TOUPPER(c) toupper((int) (c))
#endif

static int
strncasecomp(const char *a, const char *b, int n)
{
    const char *p = a;
    const char *q = b;

    for (p = a, q = b;; p++, q++) {
        int diff;
        if (p == a + n) return 0; /*   Match up to n characters */
        if (!(*p && *q)) return *p - *q;
        diff = TOLOWER(*p) - TOLOWER(*q);
        if (diff) return diff;
    }
    /*NOTREACHED*/
    return -1; // silence gcc
}

static int
make_month(char * s, char ** ends)
{
    char * ptr = s;
    while (!isalpha((int) *ptr)) ptr++;
    if (*ptr) {
        int i;
        *ends = ptr + 3;
        for (i = 0; i < 12; i++)
            if (!strncasecomp(months[i], ptr, 3)) return i;
    }
    return 0;
}

/** Parse a string in GMT format to a local time time_t representation
    Four formats are accepted:
 Wkd, 00 Mon 0000 00:00:00 GMT  (rfc1123)
 Weekday, 00-Mon-00 00:00:00 GMT  (rfc850)
 Wkd Mon 00 00:00:00 0000 GMT  (ctime)
 1*DIGIT     (delta-seconds)

    Copied from libwww. 09/19/02 jhrg

    @param str The time string.
    @param expand If the time is given in delta seconds, adjust it to seconds
    since midnight 1 Jan 1970 if this is true. If false, simply convert the
    string to a time_t and return. The default value is true.
    @return The time in seconds since midnight 1 Jan 1970. */
time_t
parse_time(const char * str, bool expand)
{
    char * s;
    struct tm tm;
    time_t t;

    if (!str) return 0;

    if ((s = (char *)strchr(str, ','))) {  /* Thursday, 10-Jun-93 01:29:59 GMT */
        s++;    /* or: Thu, 10 Jan 1993 01:29:59 GMT */
        while (*s && *s == ' ') s++;
        if (strchr(s, '-')) {         /* First format */
            DBG(cerr << "Format...... Weekday, 00-Mon-00 00:00:00 GMT"
                << endl);
            if ((int)strlen(s) < 18) {
                DBG(cerr << "ERROR....... Not a valid time format \""
                    << s << "\"" << endl);
                return 0;
            }
            tm.tm_mday = strtol(s, &s, 10);
            tm.tm_mon = make_month(s, &s);
            ++s;
            tm.tm_year = strtol(s, &s, 10);
            tm.tm_hour = strtol(s, &s, 10);
            ++s;
            tm.tm_min = strtol(s, &s, 10);
            ++s;
            tm.tm_sec = strtol(s, &s, 10);

        }
        else {         /* Second format */
            DBG(cerr << "Format...... Wkd, 00 Mon 0000 00:00:00 GMT" << endl);
            if ((int)strlen(s) < 20) {
                DBG(cerr << "ERROR....... Not a valid time format \""
                    << s << "\"" << endl);
                return 0;
            }
            tm.tm_mday = strtol(s, &s, 10);
            tm.tm_mon = make_month(s, &s);
            tm.tm_year = strtol(s, &s, 10) - 1900;
            tm.tm_hour = strtol(s, &s, 10);
            ++s;
            tm.tm_min = strtol(s, &s, 10);
            ++s;
            tm.tm_sec = strtol(s, &s, 10);
        }
    }
    else if (isdigit((int) *str)) {

        if (strchr(str, 'T')) { /* ISO (limited format) date string */
            DBG(cerr << "Format...... YYYY.MM.DDThh:mmStzWkd" << endl);
            s = (char *) str;
            while (*s && *s == ' ') s++;
            if ((int)strlen(s) < 21) {
                DBG(cerr << "ERROR....... Not a valid time format \""
                    << s << "\"" << endl);
                return 0;
            }
            tm.tm_year = strtol(s, &s, 10) - 1900;
            ++s;
            tm.tm_mon  = strtol(s, &s, 10);
            ++s;
            tm.tm_mday = strtol(s, &s, 10);
            ++s;
            tm.tm_hour = strtol(s, &s, 10);
            ++s;
            tm.tm_min  = strtol(s, &s, 10);
            ++s;
            tm.tm_sec  = strtol(s, &s, 10);

        }
        else {         /* delta seconds */
            t = expand ? time(NULL) + atol(str) : atol(str);

            return t;
        }

    }
    else {       /* Try the other format:  Wed Jun  9 01:29:59 1993 GMT */
        DBG(cerr << "Format...... Wkd Mon 00 00:00:00 0000 GMT" << endl);
        s = (char *) str;
        while (*s && *s == ' ') s++;
        DBG(cerr << "Trying...... The Wrong time format: " << s << endl);
        if ((int)strlen(s) < 24) {
            DBG(cerr << "ERROR....... Not a valid time format \""
                << s << "\"" << endl);
            return 0;
        }
        tm.tm_mon = make_month(s, &s);
        tm.tm_mday = strtol(s, &s, 10);
        tm.tm_hour = strtol(s, &s, 10);
        ++s;
        tm.tm_min = strtol(s, &s, 10);
        ++s;
        tm.tm_sec = strtol(s, &s, 10);
        tm.tm_year = strtol(s, &s, 10) - 1900;
    }
    if (tm.tm_sec  < 0  ||  tm.tm_sec  > 59  ||
        tm.tm_min  < 0  ||  tm.tm_min  > 59  ||
        tm.tm_hour < 0  ||  tm.tm_hour > 23  ||
        tm.tm_mday < 1  ||  tm.tm_mday > 31  ||
        tm.tm_mon  < 0  ||  tm.tm_mon  > 11  ||
        tm.tm_year < 70  ||  tm.tm_year > 120) {
        DBG(cerr << "ERROR....... Parsed illegal time" << endl);
        return 0;
    }

    /* Let mktime decide whether we have DST or not */
    tm.tm_isdst = -1;

#ifdef HAVE_TIMEGM

    t = timegm(&tm);

#else

#ifdef HAVE_MKTIME

    // Compute offset between localtime and GMT.
    time_t offset;
    time_t now = time(0);
#ifdef _REENTRANT
    struct tm gmt, local;
    offset = mktime(gmtime_r(&now, &gmt)) - mktime(localtime_r(&now, &local));
#else
    offset = mktime(gmtime(&now)) - mktime(localtime(&now));
#endif

    t = mktime(&tm) + offset;

#else

#error "Neither mktime nor timegm defined"

#endif /* HAVE_TIMEGM */
#endif /* HAVE_MKTIME */

    DBG(cerr << "Time string. " << str << " parsed to " << t
        << " calendar time or \"" << ctime(&t) << "\" in local time" << endl);

    return t;
}

/** Given a time in seconds since midnight 1 Jan 1970, return the RFC 1123
    date string. Example result string: Sun, 06 Nov 1994 08:49:37 GMT


    @param calendar Time in seconds
    @param local If true, return the local time, if false return GMT. The
    default value is false.
    @return A RFC 1123 date string. */

string date_time_str(time_t *calendar, bool local)
{
    char buf[40];

#ifdef HAVE_STRFTIME
    if (local) {
        /*
        ** Solaris 2.3 has a bug so we _must_ use reentrant version
        ** Thomas Maslen <tmaslen@verity.com>
        */
#if defined(_REENTRANT) || defined(SOLARIS)
        struct tm loctime;
        localtime_r(calendar, &loctime);
        strftime(buf, 40, "%a, %d %b %Y %H:%M:%S", &loctime);
#else
        struct tm *loctime = localtime(calendar);
        strftime(buf, 40, "%a, %d %b %Y %H:%M:%S", loctime);
#endif /* SOLARIS || _REENTRANT */
    }
    else {
#if defined(_REENTRANT) || defined(SOLARIS)
        struct tm gmt;
        gmtime_r(calendar, &gmt);
        strftime(buf, 40, "%a, %d %b %Y %H:%M:%S GMT", &gmt);
#else
        struct tm *gmt = gmtime(calendar);
        strftime(buf, 40, "%a, %d %b %Y %H:%M:%S GMT", gmt);
#endif /* SOLARIS || _REENTRANT */
    }

#else  /* !HAVE_STRFTIME */

    if (local) {
#if defined(_REENTRANT)
        struct tm loctime;
        localtime_r(calendar, &loctime);
        snprintf(buf, 40, "%s, %02d %s %04d %02d:%02d:%02d",
                wkdays[loctime.tm_wday],
                loctime.tm_mday,
                months[loctime.tm_mon],
                loctime.tm_year + 1900,
                loctime.tm_hour,
                loctime.tm_min,
                loctime.tm_sec);
#else
    struct tm *loctime = localtime(calendar);
    if (!loctime)
    	return "";
    snprintf(buf, 40, "%s, %02d %s %04d %02d:%02d:%02d",
            wkdays[loctime->tm_wday],
            loctime->tm_mday,
            months[loctime->tm_mon],
            loctime->tm_year + 1900,
            loctime->tm_hour,
            loctime->tm_min,
            loctime->tm_sec);
#endif /* _REENTRANT */
    }
    else {
#if defined(_REENTRANT) || defined(SOLARIS)
        struct tm gmt;
        gmtime_r(calendar, &gmt);
        snprintf(buf, 40, "%s, %02d %s %04d %02d:%02d:%02d GMT",
                wkdays[gmt.tm_wday],
                gmt.tm_mday,
                months[gmt.tm_mon],
                gmt.tm_year + 1900,
                gmt.tm_hour,
                gmt.tm_min,
                gmt.tm_sec);
#else
    struct tm *gmt = gmtime(calendar);
    if (!gmt)
    	return "";
    snprintf(buf, 40, "%s, %02d %s %04d %02d:%02d:%02d GMT",
            wkdays[gmt->tm_wday],
            gmt->tm_mday,
            months[gmt->tm_mon],
            gmt->tm_year + 1900,
            gmt->tm_hour,
            gmt->tm_min,
            gmt->tm_sec);
#endif
    }
#endif
    return string(buf);
}

} // namespace libdap
