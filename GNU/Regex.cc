/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* 
  Regex class implementation

  NB: I tried to replace the lib_error_handler() calls with STL exceptions,
  but I can't figure out where/if gcc 3.2.2 defines the children of
  exception. 06/06/03 jhrg
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>
#include <string.h>
#include <builtin.h>

#include <new>

extern "C" {
#if defined(WIN32) || defined(__POWERPC__)
#define __STDC__		/*  Needed for rx.h prototypes.               */
#include "regex-0.12/regex.h"   /*  Lack of case distinction on win32         */
#else				/*  Means regex.h and Regex.h are same file.  */
#include "regex.h"
#endif
}

#include <Regex.h>

using namespace std;

Regex::~Regex()
{
  if (buf->buffer) free(buf->buffer);
  if (buf->fastmap) free(buf->fastmap);
  if (buf->translate) free (buf->translate);

  if (reg->start)
    free (reg->start);
  if (reg->end)
    free (reg->end);

  delete(buf);
  delete(reg);
}

Regex::Regex(const char* t, int fast, int bufsize, 
               const char* transtable)
{
  int tlen = (t == 0)? 0 : strlen(t);
  buf = new re_pattern_buffer;
  memset (buf, 0, sizeof(re_pattern_buffer));
  reg = new re_registers;
  reg->start = 0;
  reg->end = 0;
  if (fast)
    buf->fastmap = (char*)malloc(256);
  else
    buf->fastmap = 0;
#if 0
  buf->translate = (unsigned char*)transtable;
#endif
  buf->translate = (char*)transtable;
  if (tlen > bufsize)
    bufsize = tlen;
  buf->allocated = bufsize;
  buf->buffer = (unsigned char *)malloc(buf->allocated);
#if 0
  buf->buffer = (char *)malloc(buf->allocated);
#endif
  const char* msg = re_compile_pattern((const char*)t, tlen, buf);
  if (msg != 0)
#if 0
    throw invalid_argument(string("Regex: ") + string(msg));
#endif
    (*lib_error_handler)("Regex", msg);
  else if (fast)
    re_compile_fastmap(buf);
}

int Regex::match_info(int& start, int& length, int nth) const
{
  if ((unsigned)(nth) >= RE_NREGS)
    return 0;
  else
  {
    start = reg->start[nth];
    length = reg->end[nth] - start;
    return start >= 0 && length >= 0;
  }
}

int Regex::search(const char* s, int len, int& matchlen, int startpos) const
{
  int matchpos, pos, range;
  if (startpos >= 0)
  {
    pos = startpos;
    range = len - startpos;
  }
  else
  {
    pos = len + startpos;
    range = -pos;
  }
  matchpos = re_search_2(buf, 0, 0, (char*)s, len, pos, range, reg, len);
  if (matchpos >= 0)
    matchlen = reg->end[0] - reg->start[0];
  else
    matchlen = 0;
  return matchpos;
}

int Regex::match(const char*s, int len, int p) const
{
  if (p < 0)
  {
    p += len;
    if (p > len)
      return -1;
    return re_match_2(buf, 0, 0, (char*)s, p, 0, reg, p);
  }
  else if (p > len)
    return -1;
  else
    return re_match_2(buf, 0, 0, (char*)s, len, p, reg, len);
}

int Regex::OK() const
{
// can't verify much, since we've lost the original string
  int v = buf != 0;             // have a regex buf
  v &= buf->buffer != 0;        // with a pat
  if (!v) (*lib_error_handler)("Regex", "invariant failure");
#if 0
  if (!v) throw domain_error(string("Regex") + string("invariant failure"));
#endif
  return v;
}

/*
 some built-in Regular expressions
*/

#if 0
// See my comment about these in Regex.h. 11/05/02 jhrg
const Regex RXwhite("[ \n\t\r\v\f]+", 1);
const Regex RXint("-?[0-9]+", 1);
#if 0
const Regex RXdouble("-?\\(\\([0-9]+\\.[0-9]*\\)\\|\\([0-9]+\\)\\|\\(\\.[0-9]+\\)\\)\\([eE][---+]?[0-9]+\\)?", 1, 200);
#endif
const Regex RXalpha("[A-Za-z]+", 1);
const Regex RXlowercase("[a-z]+", 1);
const Regex RXuppercase("[A-Z]+", 1);
const Regex RXalphanum("[0-9A-Za-z]+", 1);
const Regex RXidentifier("[A-Za-z_][A-Za-z0-9_]*", 1);
#endif

