
/*  A Bison parser, made from das.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse dasparse
#define yylex daslex
#define yyerror daserror
#define yylval daslval
#define yychar daschar
#define yydebug dasdebug
#define yynerrs dasnerrs
#define	SCAN_ATTR	257
#define	SCAN_WORD	258
#define	SCAN_ALIAS	259
#define	SCAN_BYTE	260
#define	SCAN_INT16	261
#define	SCAN_UINT16	262
#define	SCAN_INT32	263
#define	SCAN_UINT32	264
#define	SCAN_FLOAT32	265
#define	SCAN_FLOAT64	266
#define	SCAN_STRING	267
#define	SCAN_URL	268

#line 40 "das.y"


#define YYSTYPE char *

#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <vector>

#include "DAS.h"
#include "Error.h"
#include "debug.h"
#include "parser.h"
#include "das.tab.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#define yylex daslex
#define yyerror daserror 

using namespace std;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 
// The parser now throws an exception when it encounters an error. 5/23/2002
// jhrg 

#define DAS_OBJ(arg) ((DAS *)((parser_arg *)(arg))->_object)

#define YYPARSE_PARAM arg

extern int das_line_num;	/* defined in das.lex */

// No global static objects. We go through this every so often, I guess I
// should learn... 1/24/2000 jhrg
static string *name;	/* holds name in attr_pair rule */
static string *type;	/* holds type in attr_pair rule */

static vector<AttrTable *> *attr_tab_stack;

// I use a vector of AttrTable pointers for a stack

#define TOP_OF_STACK (attr_tab_stack->back())
#define PUSH(x) (attr_tab_stack->push_back((x)))
#define POP (attr_tab_stack->pop_back())
#define STACK_LENGTH (attr_tab_stack->size())
#define OUTER_TABLE_ONLY (attr_tab_stack->size() == 1)
#define STACK_EMPTY (attr_tab_stack->empty())

#define TYPE_NAME_VALUE(x) *type << " " << *name << " " << (x)

static char *ATTR_TUPLE_MSG = 
"Expected an attribute type (Byte, Int16, UInt16, Int32, UInt32, Float32,\n\
Float64, String or Url) followed by a name and value.";
static char *NO_DAS_MSG =
"The attribute object returned from the dataset was null\n\
Check that the URL is correct.";

typedef int checker(const char *);

int daslex(void);
static void daserror(char *s);
static void add_attribute(const string &type, const string &name, 
			  const string &value, checker *chk) throw (Error);
static void add_alias(AttrTable *das, AttrTable *current, const string &name, 
		      const string &src) throw (Error);
static void add_bad_attribute(AttrTable *attr, const string &type,
			      const string &name, const string &value,
			      const string &msg);

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		126
#define	YYFLAG		-32768
#define	YYNTBASE	19

#define YYTRANSLATE(x) ((unsigned)(x) <= 268 ? yytranslate[x] : 62)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    18,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    17,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    15,     2,    16,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     9,    14,    16,    17,    19,    22,
    24,    25,    26,    33,    34,    35,    42,    43,    44,    51,
    52,    53,    60,    61,    62,    69,    70,    71,    78,    79,
    80,    87,    88,    89,    96,    97,    98,   105,   106,   107,
   114,   115,   119,   121,   125,   127,   131,   133,   137,   139,
   143,   145,   149,   151,   155,   157,   161,   163,   167,   169,
   173,   175,   177,   179,   181,   183,   185,   187,   189,   191,
   193,   195,   197,   199,   201,   203,   204,   205
};

static const short yyrhs[] = {    -1,
    20,    21,     0,    22,     0,    21,    22,     0,     3,    15,
    23,    16,     0,     1,     0,     0,    24,     0,    23,    24,
     0,    59,     0,     0,     0,     6,    25,    58,    26,    46,
    17,     0,     0,     0,     7,    27,    58,    28,    47,    17,
     0,     0,     0,     8,    29,    58,    30,    48,    17,     0,
     0,     0,     9,    31,    58,    32,    49,    17,     0,     0,
     0,    10,    33,    58,    34,    50,    17,     0,     0,     0,
    11,    35,    58,    36,    51,    17,     0,     0,     0,    12,
    37,    58,    38,    52,    17,     0,     0,     0,    13,    39,
    58,    40,    53,    17,     0,     0,     0,    14,    41,    58,
    42,    54,    17,     0,     0,     0,     4,    43,    15,    23,
    44,    16,     0,     0,     1,    45,    17,     0,     4,     0,
    46,    18,     4,     0,     4,     0,    47,    18,     4,     0,
     4,     0,    48,    18,     4,     0,     4,     0,    49,    18,
     4,     0,     4,     0,    50,    18,     4,     0,    57,     0,
    51,    18,    57,     0,    57,     0,    52,    18,    57,     0,
    56,     0,    53,    18,    56,     0,    55,     0,    54,    18,
    55,     0,     4,     0,     4,     0,     4,     0,     4,     0,
     3,     0,     5,     0,     6,     0,     7,     0,     8,     0,
     9,     0,    10,     0,    11,     0,    12,     0,    13,     0,
    14,     0,     0,     0,     5,     4,    60,     4,    61,    17,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   180,   188,   196,   197,   201,   202,   208,   209,   210,   213,
   215,   216,   217,   219,   220,   221,   223,   224,   225,   227,
   228,   229,   231,   232,   233,   235,   236,   237,   239,   240,
   241,   243,   244,   245,   247,   248,   249,   251,   271,   279,
   279,   283,   285,   289,   295,   299,   305,   309,   315,   319,
   325,   329,   335,   339,   345,   349,   355,   359,   365,   369,
   375,   378,   381,   384,   384,   384,   384,   384,   385,   385,
   385,   385,   386,   386,   386,   389,   394,   398
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SCAN_ATTR",
"SCAN_WORD","SCAN_ALIAS","SCAN_BYTE","SCAN_INT16","SCAN_UINT16","SCAN_INT32",
"SCAN_UINT32","SCAN_FLOAT32","SCAN_FLOAT64","SCAN_STRING","SCAN_URL","'{'","'}'",
"';'","','","attr_start","@1","attributes","attribute","attr_list","attr_tuple",
"@2","@3","@4","@5","@6","@7","@8","@9","@10","@11","@12","@13","@14","@15",
"@16","@17","@18","@19","@20","@21","@22","bytes","int16","uint16","int32","uint32",
"float32","float64","strs","urls","url","str_or_id","float_or_int","name","alias",
"@23","@24", NULL
};
#endif

static const short yyr1[] = {     0,
    20,    19,    21,    21,    22,    22,    23,    23,    23,    24,
    25,    26,    24,    27,    28,    24,    29,    30,    24,    31,
    32,    24,    33,    34,    24,    35,    36,    24,    37,    38,
    24,    39,    40,    24,    41,    42,    24,    43,    44,    24,
    45,    24,    46,    46,    47,    47,    48,    48,    49,    49,
    50,    50,    51,    51,    52,    52,    53,    53,    54,    54,
    55,    56,    57,    58,    58,    58,    58,    58,    58,    58,
    58,    58,    58,    58,    58,    60,    61,    59
};

static const short yyr2[] = {     0,
     0,     2,     1,     2,     4,     1,     0,     1,     2,     1,
     0,     0,     6,     0,     0,     6,     0,     0,     6,     0,
     0,     6,     0,     0,     6,     0,     0,     6,     0,     0,
     6,     0,     0,     6,     0,     0,     6,     0,     0,     6,
     0,     3,     1,     3,     1,     3,     1,     3,     1,     3,
     1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     0,     0,     6
};

static const short yydefact[] = {     1,
     0,     6,     0,     0,     3,     0,     4,    41,    38,     0,
    11,    14,    17,    20,    23,    26,    29,    32,    35,     0,
     8,    10,     0,     0,    76,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     5,     9,    42,     0,     0,    65,
    64,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    12,    15,    18,    21,    24,    27,    30,    33,    36,
     0,    77,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    43,     0,    45,     0,    47,     0,    49,
     0,    51,     0,    63,     0,    53,     0,    55,    62,     0,
    57,    61,     0,    59,    40,    78,    13,     0,    16,     0,
    19,     0,    22,     0,    25,     0,    28,     0,    31,     0,
    34,     0,    37,     0,    44,    46,    48,    50,    52,    54,
    56,    58,    60,     0,     0,     0
};

static const short yydefgoto[] = {   124,
     1,     4,     5,    20,    21,    26,    63,    27,    64,    28,
    65,    29,    66,    30,    67,    31,    68,    32,    69,    33,
    70,    34,    71,    24,    72,    23,    75,    77,    79,    81,
    83,    85,    87,    90,    93,    94,    91,    86,    52,    22,
    39,    73
};

static const short yypact[] = {-32768,
    33,-32768,     3,    32,-32768,     1,-32768,-32768,-32768,    26,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    15,
-32768,-32768,    37,    75,-32768,    53,    53,    53,    53,    53,
    53,    53,    53,    53,-32768,-32768,-32768,     1,    87,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    39,-32768,    88,    89,    90,    91,    92,    93,    93,    94,
    95,    84,    85,-32768,   -14,-32768,    20,-32768,    59,-32768,
    61,-32768,    63,-32768,    65,-32768,    67,-32768,-32768,    69,
-32768,-32768,    71,-32768,-32768,-32768,-32768,    97,-32768,    99,
-32768,   100,-32768,   101,-32768,   102,-32768,    93,-32768,    93,
-32768,    94,-32768,    95,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   107,   108,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   105,    72,   -19,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    -3,     0,   -69,    41,-32768,
-32768,-32768
};


#define	YYLAST		112


static const short yytable[] = {    88,
    36,     8,    97,    98,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,     8,    -7,     6,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    25,
    35,    -2,     2,     2,     3,     3,    99,   100,   120,     8,
   121,    36,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    37,   -39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    53,    54,    55,
    56,    57,    58,    59,    60,   101,   102,   103,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114,    38,
    62,    74,    76,    78,    80,    82,    84,    89,    92,    95,
   115,    96,   116,   117,   118,   119,   125,   126,     7,    61,
   123,   122
};

static const short yycheck[] = {    69,
    20,     1,    17,    18,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,     1,    16,    15,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,     4,
    16,     0,     1,     1,     3,     3,    17,    18,   108,     1,
   110,    61,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    17,    16,     3,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    27,    28,    29,
    30,    31,    32,    33,    34,    17,    18,    17,    18,    17,
    18,    17,    18,    17,    18,    17,    18,    17,    18,    15,
     4,     4,     4,     4,     4,     4,     4,     4,     4,    16,
     4,    17,     4,     4,     4,     4,     0,     0,     4,    38,
   114,   112
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 181 "das.y"
{
		    name = new string();
		    type = new string();
		    attr_tab_stack = new vector<AttrTable *>;
		    PUSH(DAS_OBJ(arg)); // push outermost AttrTable
		;
    break;}
case 2:
#line 188 "das.y"
{
		    POP;	// pop the DAS/AttrTable before stack's dtor
		    delete name;
		    delete type;
		    delete attr_tab_stack;
		;
    break;}
case 6:
#line 203 "das.y"
{
		    parse_error((parser_arg *)arg, NO_DAS_MSG, das_line_num);
		;
    break;}
case 11:
#line 215 "das.y"
{ save_str(*type, "Byte", das_line_num); ;
    break;}
case 12:
#line 216 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 14:
#line 219 "das.y"
{ save_str(*type, "Int16", das_line_num); ;
    break;}
case 15:
#line 220 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 17:
#line 223 "das.y"
{ save_str(*type, "UInt16", das_line_num); ;
    break;}
case 18:
#line 224 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 20:
#line 227 "das.y"
{ save_str(*type, "Int32", das_line_num); ;
    break;}
case 21:
#line 228 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 23:
#line 231 "das.y"
{ save_str(*type, "UInt32", das_line_num); ;
    break;}
case 24:
#line 232 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 26:
#line 235 "das.y"
{ save_str(*type, "Float32", das_line_num); ;
    break;}
case 27:
#line 236 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 29:
#line 239 "das.y"
{ save_str(*type, "Float64", das_line_num); ;
    break;}
case 30:
#line 240 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 32:
#line 243 "das.y"
{ *type = "String"; ;
    break;}
case 33:
#line 244 "das.y"
{ *name = yyvsp[0]; ;
    break;}
case 35:
#line 247 "das.y"
{ *type = "Url"; ;
    break;}
case 36:
#line 248 "das.y"
{ *name = yyvsp[0]; ;
    break;}
case 38:
#line 252 "das.y"
{
		    DBG(cerr << "Processing ID: " << yyvsp[0] << endl);
		    
		    AttrTable *at = TOP_OF_STACK->get_attr_table(yyvsp[0]);
		    if (!at) {
			try {
			    at = TOP_OF_STACK->append_container(yyvsp[0]);
			}
			catch (Error &e) {
			    // rethrow with line number info
			    parse_error(e.get_error_message().c_str(), 
					das_line_num);
			}
		    }
		    PUSH(at);

		    DBG(cerr << " Pushed attr_tab: " << at << endl);

		;
    break;}
case 39:
#line 272 "das.y"
{
		    /* pop top of stack; store in attr_tab */
		    DBG(cerr << " Poped attr_tab: " << TOP_OF_STACK << endl);
		    POP;
		;
    break;}
case 41:
#line 280 "das.y"
{ 
		    parse_error(ATTR_TUPLE_MSG, das_line_num, yyvsp[0]);
		;
    break;}
case 43:
#line 286 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_byte);
		;
    break;}
case 44:
#line 290 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_byte);
		;
    break;}
case 45:
#line 296 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_int16);
		;
    break;}
case 46:
#line 300 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_int16);
		;
    break;}
case 47:
#line 306 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_uint16);
		;
    break;}
case 48:
#line 310 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_uint16);
		;
    break;}
case 49:
#line 316 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_int32);
		;
    break;}
case 50:
#line 320 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_int32);
		;
    break;}
case 51:
#line 326 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_uint32);
		;
    break;}
case 52:
#line 330 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_uint32);
		;
    break;}
case 53:
#line 336 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_float32);
		;
    break;}
case 54:
#line 340 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_float32);
		;
    break;}
case 55:
#line 346 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_float64);
		;
    break;}
case 56:
#line 350 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_float64);
		;
    break;}
case 57:
#line 356 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], 0);
		;
    break;}
case 58:
#line 360 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], 0);
		;
    break;}
case 59:
#line 366 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_url);
		;
    break;}
case 60:
#line 370 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_url);
		;
    break;}
case 76:
#line 390 "das.y"
{ 
		    *name = yyvsp[0];
		;
    break;}
case 77:
#line 394 "das.y"
{
		    add_alias(DAS_OBJ(arg), TOP_OF_STACK, *name, string(yyvsp[0]))
                ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 400 "das.y"


// This function is required for linking, but DODS uses its own error
// reporting mechanism.

static void
daserror(char *)
{
}

static string
a_or_an(const string &subject)
{
    string first_char(1, subject[0]);
    string::size_type pos = first_char.find_first_of("aeiouAEIOUyY");
    
    if (pos == string::npos)
	return "a";
    else
	return "an";
}

// This code used to throw an exception when a bad attribute value came
// along; now it dumps the errant value(s) into a sub container called *_DODS
// and stores the parser's error message in a string attribute named
// `explanation.' 
static void
add_attribute(const string &type, const string &name, const string &value,
	      checker *chk) throw (Error)
{
    DBG(cerr << "Adding: " << type << " " << name << " " << value \
	<< " to Attrtable: " << TOP_OF_STACK << endl);

    if (chk && !(*chk)(value.c_str())) {
	string msg = "`";
	msg += value + "' is not " + a_or_an(type) + " " + type + " value.";
	add_bad_attribute(TOP_OF_STACK, type, name, value, msg);
	return;
    }
    
    if (STACK_EMPTY) {
	string msg = "Whoa! Attribute table stack empty when adding `" ;
	msg += name + ".' ";
	parse_error(msg.c_str(), das_line_num);
    }
    
    try {
	TOP_OF_STACK->append_attr(name, type, value);
    }
    catch (Error &e) {
	// rethrow with line number
	parse_error(e.get_error_message().c_str(), das_line_num);
    }
}

static void
add_alias(AttrTable *das, AttrTable *current, const string &name, 
	  const string &src) throw (Error)
{
    DBG(cerr << "Adding an alias: " << name << ": " << src << endl);

    AttrTable *table = das->get_attr_table(src);
    if (table) {
	try {
	    current->add_container_alias(name, table);
	}
	catch (Error &e) {
	    parse_error(e.get_error_message().c_str(), das_line_num);
	}
    }
    else {
	try {
	    current->add_value_alias(das, name, src);
	}
	catch (Error &e) {
	    parse_error(e.get_error_message().c_str(), das_line_num);
	}
    }
}

static void
add_bad_attribute(AttrTable *attr, const string &type, const string &name,
		  const string &value, const string &msg)
{
    // First, if this bad value is already in a *_dods_errors container,
    // then just add it. This can happen when the server side processes a DAS
    // and then hands it off to a client which does the same.
    // Make a new container. Call it <attr's name>_errors. If that container
    // already exists, use it.
    // Add the attribute.
    // Add the error string to an attribute in the container called
    // `<name_explanation.'. 
    
    if (attr->get_name().find("_dods_errors") != string::npos) {
	attr->append_attr(name, type, value);
    }
    else {
	string error_cont_name = attr->get_name() + "_dods_errors";
	AttrTable *error_cont = attr->get_attr_table(error_cont_name);
	if (!error_cont)
	    error_cont = attr->append_container(error_cont_name);

	error_cont->append_attr(name, type, value);
	error_cont->append_attr(name + "_explanation", "String",
				"\"" + msg + "\"");
    }
}
