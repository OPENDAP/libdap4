
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
#define	SCAN_ID	258
#define	SCAN_INT	259
#define	SCAN_FLOAT	260
#define	SCAN_STR	261
#define	SCAN_ALIAS	262
#define	SCAN_BYTE	263
#define	SCAN_INT16	264
#define	SCAN_UINT16	265
#define	SCAN_INT32	266
#define	SCAN_UINT32	267
#define	SCAN_FLOAT32	268
#define	SCAN_FLOAT64	269
#define	SCAN_STRING	270
#define	SCAN_URL	271

#line 18 "das.y"


#define YYSTYPE char *

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: das.tab.c,v 1.20 2001/01/26 19:48:09 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>

#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif
#include <vector>

#include "DAS.h"
#include "Error.h"
#include "debug.h"
#include "parser.h"
#include "das.tab.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
using std::ends;
using std::ostrstream;
#endif

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

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

#if 0
void mem_list_report();
#endif
int daslex(void);
static void daserror(char *s);
#if 0
static string attr_name(string name);
#endif
static void add_attribute(const string &type, const string &name, 
			  const string &value, checker *chk) throw (Error);
static void add_alias(AttrTable *das, AttrTable *current, const string &name, 
		      const string &src) throw (Error);

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		119
#define	YYFLAG		-32768
#define	YYNTBASE	22

#define YYTRANSLATE(x) ((unsigned)(x) <= 271 ? yytranslate[x] : 64)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    21,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    20,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    18,     2,    19,     2,     2,     2,     2,     2,
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
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     9,    14,    16,    17,    19,    22,
    24,    25,    26,    33,    34,    35,    42,    43,    44,    51,
    52,    53,    60,    61,    62,    69,    70,    71,    78,    79,
    80,    87,    88,    89,    96,    97,    98,   105,   106,   107,
   114,   115,   119,   121,   125,   127,   131,   133,   137,   139,
   143,   145,   149,   151,   155,   157,   161,   163,   167,   169,
   173,   175,   177,   179,   181,   183,   185,   187,   189,   190,
   191
};

static const short yyrhs[] = {    -1,
    23,    24,     0,    25,     0,    24,    25,     0,     3,    18,
    26,    19,     0,     1,     0,     0,    27,     0,    26,    27,
     0,    61,     0,     0,     0,     9,    28,     4,    29,    49,
    20,     0,     0,     0,    10,    30,     4,    31,    50,    20,
     0,     0,     0,    11,    32,     4,    33,    51,    20,     0,
     0,     0,    12,    34,     4,    35,    52,    20,     0,     0,
     0,    13,    36,     4,    37,    53,    20,     0,     0,     0,
    14,    38,     4,    39,    54,    20,     0,     0,     0,    15,
    40,     4,    41,    55,    20,     0,     0,     0,    16,    42,
     4,    43,    56,    20,     0,     0,     0,    17,    44,     4,
    45,    57,    20,     0,     0,     0,     4,    46,    18,    26,
    47,    19,     0,     0,     1,    48,    20,     0,     5,     0,
    49,    21,     5,     0,     5,     0,    50,    21,     5,     0,
     5,     0,    51,    21,     5,     0,     5,     0,    52,    21,
     5,     0,     5,     0,    53,    21,     5,     0,    60,     0,
    54,    21,    60,     0,    60,     0,    55,    21,    60,     0,
    59,     0,    56,    21,    59,     0,    58,     0,    57,    21,
    58,     0,     4,     0,     7,     0,     7,     0,     4,     0,
     5,     0,     6,     0,     6,     0,     5,     0,     0,     0,
     8,     4,    62,     4,    63,    20,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   167,   175,   183,   184,   188,   189,   195,   196,   197,   200,
   202,   203,   204,   206,   207,   208,   210,   211,   212,   214,
   215,   216,   218,   219,   220,   222,   223,   224,   226,   227,
   228,   230,   231,   232,   234,   235,   236,   238,   258,   266,
   266,   270,   272,   276,   282,   286,   292,   296,   302,   306,
   312,   316,   322,   326,   332,   336,   342,   346,   352,   356,
   362,   362,   365,   365,   365,   365,   368,   368,   371,   376,
   380
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SCAN_ATTR",
"SCAN_ID","SCAN_INT","SCAN_FLOAT","SCAN_STR","SCAN_ALIAS","SCAN_BYTE","SCAN_INT16",
"SCAN_UINT16","SCAN_INT32","SCAN_UINT32","SCAN_FLOAT32","SCAN_FLOAT64","SCAN_STRING",
"SCAN_URL","'{'","'}'","';'","','","attr_start","@1","attributes","attribute",
"attr_list","attr_tuple","@2","@3","@4","@5","@6","@7","@8","@9","@10","@11",
"@12","@13","@14","@15","@16","@17","@18","@19","@20","@21","@22","bytes","int16",
"uint16","int32","uint32","float32","float64","strs","urls","url","str_or_id",
"float_or_int","alias","@23","@24", NULL
};
#endif

static const short yyr1[] = {     0,
    23,    22,    24,    24,    25,    25,    26,    26,    26,    27,
    28,    29,    27,    30,    31,    27,    32,    33,    27,    34,
    35,    27,    36,    37,    27,    38,    39,    27,    40,    41,
    27,    42,    43,    27,    44,    45,    27,    46,    47,    27,
    48,    27,    49,    49,    50,    50,    51,    51,    52,    52,
    53,    53,    54,    54,    55,    55,    56,    56,    57,    57,
    58,    58,    59,    59,    59,    59,    60,    60,    62,    63,
    61
};

static const short yyr2[] = {     0,
     0,     2,     1,     2,     4,     1,     0,     1,     2,     1,
     0,     0,     6,     0,     0,     6,     0,     0,     6,     0,
     0,     6,     0,     0,     6,     0,     0,     6,     0,     0,
     6,     0,     0,     6,     0,     0,     6,     0,     0,     6,
     0,     3,     1,     3,     1,     3,     1,     3,     1,     3,
     1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
     1,     1,     1,     1,     1,     1,     1,     1,     0,     0,
     6
};

static const short yydefact[] = {     1,
     0,     6,     0,     0,     3,     0,     4,    41,    38,     0,
    11,    14,    17,    20,    23,    26,    29,    32,    35,     0,
     8,    10,     0,     0,    69,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     5,     9,    42,     0,     0,    12,
    15,    18,    21,    24,    27,    30,    33,    36,     0,    70,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    43,     0,    45,     0,    47,     0,    49,     0,    51,
     0,    68,    67,     0,    53,     0,    55,    64,    65,    66,
    63,     0,    57,    61,    62,     0,    59,    40,    71,    13,
     0,    16,     0,    19,     0,    22,     0,    25,     0,    28,
     0,    31,     0,    34,     0,    37,     0,    44,    46,    48,
    50,    52,    54,    56,    58,    60,     0,     0,     0
};

static const short yydefgoto[] = {   117,
     1,     4,     5,    20,    21,    26,    51,    27,    52,    28,
    53,    29,    54,    30,    55,    31,    56,    32,    57,    33,
    58,    34,    59,    24,    60,    23,    63,    65,    67,    69,
    71,    74,    76,    82,    86,    87,    83,    75,    22,    39,
    61
};

static const short yypact[] = {-32768,
    20,-32768,    25,     3,-32768,     1,-32768,-32768,-32768,    37,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    23,
-32768,-32768,    27,    33,-32768,    76,    77,    78,    79,    80,
    81,    82,    83,    84,-32768,-32768,-32768,     1,    85,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    44,-32768,
    57,    86,    87,    88,    89,     2,     2,    60,    15,    71,
    75,-32768,     5,-32768,     8,-32768,    29,-32768,    48,-32768,
    50,-32768,-32768,    52,-32768,    54,-32768,-32768,-32768,-32768,
-32768,    56,-32768,-32768,-32768,    58,-32768,-32768,-32768,-32768,
    91,-32768,    92,-32768,    93,-32768,    94,-32768,    95,-32768,
     2,-32768,     2,-32768,    60,-32768,    15,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   101,   102,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    99,    66,   -19,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    -2,     4,   -57,-32768,-32768,
-32768
};


#define	YYLAST		109


static const short yytable[] = {    77,
    36,     8,    -2,     2,     9,     3,    72,    73,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    84,    -7,
     2,    85,     3,     8,    90,    91,     9,    92,    93,    36,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    25,    35,     6,   113,     8,   114,    37,     9,    94,    95,
    38,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    62,   -39,    78,    79,    80,    81,    96,    97,    98,
    99,   100,   101,   102,   103,   104,   105,   106,   107,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    50,    88,
    64,    66,    68,    70,    89,   108,   109,   110,   111,   112,
   118,   119,     7,    49,   116,     0,     0,     0,   115
};

static const short yycheck[] = {    57,
    20,     1,     0,     1,     4,     3,     5,     6,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,     4,    19,
     1,     7,     3,     1,    20,    21,     4,    20,    21,    49,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
     4,    19,    18,   101,     1,   103,    20,     4,    20,    21,
    18,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,     5,    19,     4,     5,     6,     7,    20,    21,    20,
    21,    20,    21,    20,    21,    20,    21,    20,    21,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,    19,
     5,     5,     5,     5,    20,     5,     5,     5,     5,     5,
     0,     0,     4,    38,   107,    -1,    -1,    -1,   105
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
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

#line 217 "/usr/lib/bison.simple"

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
#line 168 "das.y"
{
		    name = new string();
		    type = new string();
		    attr_tab_stack = new vector<AttrTable *>;
		    PUSH(DAS_OBJ(arg)); // push outermost AttrTable
		;
    break;}
case 2:
#line 175 "das.y"
{
		    POP;	// pop the DAS/AttrTable before stack's dtor
		    delete name;
		    delete type;
		    delete attr_tab_stack;
		;
    break;}
case 6:
#line 190 "das.y"
{
		    parse_error((parser_arg *)arg, NO_DAS_MSG, das_line_num);
		;
    break;}
case 11:
#line 202 "das.y"
{ *type = "Byte"; ;
    break;}
case 12:
#line 203 "das.y"
{ *name = yyvsp[0]; ;
    break;}
case 14:
#line 206 "das.y"
{ save_str(*type, "Int16", das_line_num); ;
    break;}
case 15:
#line 207 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 17:
#line 210 "das.y"
{ save_str(*type, "UInt16", das_line_num); ;
    break;}
case 18:
#line 211 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 20:
#line 214 "das.y"
{ save_str(*type, "Int32", das_line_num); ;
    break;}
case 21:
#line 215 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 23:
#line 218 "das.y"
{ save_str(*type, "UInt32", das_line_num); ;
    break;}
case 24:
#line 219 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 26:
#line 222 "das.y"
{ save_str(*type, "Float32", das_line_num); ;
    break;}
case 27:
#line 223 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 29:
#line 226 "das.y"
{ save_str(*type, "Float64", das_line_num); ;
    break;}
case 30:
#line 227 "das.y"
{ save_str(*name, yyvsp[0], das_line_num); ;
    break;}
case 32:
#line 230 "das.y"
{ *type = "String"; ;
    break;}
case 33:
#line 231 "das.y"
{ *name = yyvsp[0]; ;
    break;}
case 35:
#line 234 "das.y"
{ *type = "Url"; ;
    break;}
case 36:
#line 235 "das.y"
{ *name = yyvsp[0]; ;
    break;}
case 38:
#line 239 "das.y"
{
		    DBG(cerr << "Processing ID: " << yyvsp[0] << endl);
		    
		    AttrTable *at = TOP_OF_STACK->get_attr_table(yyvsp[0]);
		    if (!at) {
			try {
			    at = TOP_OF_STACK->append_container(yyvsp[0]);
			}
			catch (Error &e) {
			    // rethrow with line number info
			    parse_error(e.error_message().c_str(), 
					das_line_num);
			}
		    }
		    PUSH(at);

		    DBG(cerr << " Pushed attr_tab: " << at << endl);

		;
    break;}
case 39:
#line 259 "das.y"
{
		    /* pop top of stack; store in attr_tab */
		    DBG(cerr << " Poped attr_tab: " << TOP_OF_STACK << endl);
		    POP;
		;
    break;}
case 41:
#line 267 "das.y"
{ 
		    parse_error(ATTR_TUPLE_MSG, das_line_num, yyvsp[0]);
		;
    break;}
case 43:
#line 273 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_byte);
		;
    break;}
case 44:
#line 277 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_byte);
		;
    break;}
case 45:
#line 283 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_int16);
		;
    break;}
case 46:
#line 287 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_int16);
		;
    break;}
case 47:
#line 293 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_uint16);
		;
    break;}
case 48:
#line 297 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_uint16);
		;
    break;}
case 49:
#line 303 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_int32);
		;
    break;}
case 50:
#line 307 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_int32);
		;
    break;}
case 51:
#line 313 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_uint32);
		;
    break;}
case 52:
#line 317 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_uint32);
		;
    break;}
case 53:
#line 323 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_float32);
		;
    break;}
case 54:
#line 327 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_float32);
		;
    break;}
case 55:
#line 333 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_float64);
		;
    break;}
case 56:
#line 337 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_float64);
		;
    break;}
case 57:
#line 343 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], 0);
		;
    break;}
case 58:
#line 347 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], 0);
		;
    break;}
case 59:
#line 353 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_url);
		;
    break;}
case 60:
#line 357 "das.y"
{
		    add_attribute(*type, *name, yyvsp[0], &check_url);
		;
    break;}
case 69:
#line 372 "das.y"
{ 
		    *name = yyvsp[0];
		;
    break;}
case 70:
#line 376 "das.y"
{
		    add_alias(DAS_OBJ(arg), TOP_OF_STACK, *name, string(yyvsp[0]))
                ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

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
#line 381 "das.y"


// This function is required for linking, but DODS uses its own error
// reporting mechanism.

static void
daserror(char *)
{
}

// Return the rightmost component of name (where each component is separated
// by `.'.

#if 0
static string
attr_name(string name)
{
    string::size_type i = name.rfind('.');
    if(i == string::npos)
      return name;
    else
      return name.substr(i+1);
}
#endif

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

static void
add_attribute(const string &type, const string &name, const string &value,
	      checker *chk) throw (Error)
{
    DBG(cerr << "Adding: " << type << " " << name << " " << value \
	<< " to Attrtable: " << TOP_OF_STACK << endl);

    if (chk && !(*chk)(value.c_str())) {
	string msg = "`";
	msg += value + "' is not " + a_or_an(type) + " " + type + " value.";
	parse_error(msg.c_str(), das_line_num);	// throws Error.
    }
    
    if (STACK_EMPTY) {
	string msg = "Whoa! Attribute table stack empty when adding `" ;
	msg += name + "' .";
	parse_error(msg.c_str(), das_line_num);
    }
    
    try {
	TOP_OF_STACK->append_attr(name, type, value);
    }
    catch (Error &e) {
	// rethrow with line number
	parse_error(e.error_message().c_str(), das_line_num);
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
	    parse_error(e.error_message().c_str(), das_line_num);
	}
    }
    else {
	try {
	    current->add_value_alias(das, name, src);
	}
	catch (Error &e) {
	    parse_error(e.error_message().c_str(), das_line_num);
	}
    }
}

/* 
 * $Log: das.tab.c,v $
 * Revision 1.20  2001/01/26 19:48:09  jimg
 * Merged with release-3-2-3.
 *
 * Revision 1.19.4.5  2000/12/13 03:27:15  jimg
 * *** empty log message ***
 *
 * Revision 1.40.4.2  2000/11/30 05:24:46  jimg
 * Significant changes and improvements to the AttrTable and DAS classes. DAS
 * now is a child of AttrTable, which makes attributes behave uniformly at
 * all levels of the DAS object. Alias now work. I've added unit tests for
 * several methods in AttrTable and some of the functions in parser-util.cc.
 * In addition, all of the DAS tests now work.
 *
 * Revision 1.40.4.1  2000/11/22 21:47:42  jimg
 * Changed the implementation of DAS; it now inherits from AttrTable
 *
 * Revision 1.40  2000/09/22 02:17:22  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.39  2000/07/09 21:43:29  rmorris
 * Mods to increase portability, minimize ifdef's for win32
 *
 * Revision 1.38  2000/06/07 19:33:21  jimg
 * Merged with verson 3.1.6
 *
 * Revision 1.37  2000/06/07 18:07:00  jimg
 * Merged the pc port branch
 *
 * Revision 1.36.6.1  2000/06/02 18:36:38  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.35.6.3  2000/05/18 17:47:21  jimg
 * Fixed a bug in the AttrTable. Container attributes below the top level were
 * broken in the latest changes to the DAS code.
 *
 * Revision 1.36  2000/01/27 06:30:00  jimg
 * Resolved conflicts from merge with release-3-1-4
 *
 * Revision 1.35.6.2  2000/01/24 22:25:10  jimg
 * Removed static global objects
 *
 * Revision 1.35.6.1  1999/10/19 16:45:14  jimg
 * Fixed a minor bug in the check of int16 attributes. the check_int16 was
 * called when check_uint16 should have been called.
 *
 * Revision 1.35  1999/05/04 19:47:23  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.34  1999/04/29 02:29:35  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.33  1999/03/24 23:33:44  jimg
 * Added support for the new Int16, UInt16 and Float32 types.
 *
 * Revision 1.32.12.2  1999/02/05 09:32:35  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.32.12.1  1999/02/02 21:57:06  jimg
 * String to string version
 *
 * Revision 1.32  1997/07/01 00:13:23  jimg
 * Fixed a bug when vectors of UInt32 were used. I changed the way the type
 * name was passed to AttrTable::append_attr() so that the names were always
 * the same regardless of form of the name used in the DAS.
 * Fixed a bug when Urls are used as an attribute type.
 *
 * Revision 1.31  1997/05/21 00:10:35  jimg
 * Added a fix for aliases between top level groups of attributes.
 *
 * Revision 1.30  1997/05/13 23:32:19  jimg
 * Added changes to handle the new Alias and lexical scoping rules.
 *
 * Revision 1.29  1997/05/06 22:09:57  jimg
 * Added aliases to the grammar. An alias can appear in place of an attribute
 * and uses the syntax `alias <var1> <var2>'. If var1 exists, var2 becomes an
 * alias to it and vice versa. If neither var1 nor var2 exists or if they both
 * exist, and error is reported and parsing stops.
 *
 * Revision 1.28  1997/02/28 01:01:07  jimg
 * Tweaked error messages so that they no longer mumble about parse errors.
 *
 * Revision 1.27  1997/02/10 02:36:57  jimg
 * Fixed bug where attribute type of int32 was broken on 64bit machines.
 *
 * Revision 1.26  1996/10/28 23:04:46  jimg
 * Added unsigned int to set of possible attribute value types.
 *
 * Revision 1.25  1996/10/11 00:11:03  jimg
 * Fixed DODS_BISON_VER preprocessor statement. >= apparently is not recognized
 * by g++'s preprocessor.
 *
 * Revision 1.24  1996/10/08 17:04:40  jimg
 * Added a fix for Bison 1.25 so that PARSE_PARAM will still work
 *
 * Revision 1.23  1996/08/13 18:46:38  jimg
 * Added parser_arg object macros.
 * `Fixed' error messages.
 * Changed return typw of daserror() from int to void.
 *
 * Revision 1.22  1996/06/07 15:05:16  jimg
 * Removed old type checking code - use the type checkers in parser-util.cc.
 *
 * Revision 1.21  1996/05/31 23:30:52  jimg
 * Updated copyright notice.
 *
 * Revision 1.20  1996/04/05 00:22:13  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.19  1995/12/06  19:46:29  jimg
 * Added definition of YYSTYPE.
 *
 * Revision 1.18  1995/10/23  22:54:39  jimg
 * Removed code that was NEVER'd.
 * Changed some rules so that they call functions in parser_util.cc .
 *
 * Revision 1.17  1995/09/05  23:19:45  jimg
 * Fixed a bug in check_float where `=' was used where `==' should have been.
 *
 * Revision 1.16  1995/08/23  00:25:54  jimg
 * Added copyright notice.
 * Fixed some bogus comments.
 *
 * Revision 1.15  1995/07/08  18:32:10  jimg
 * Edited comments.
 * Removed unnecessary declarations.
 *
 * Revision 1.14  1995/05/10  13:45:43  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.13  1995/02/16  15:30:46  jimg
 * Fixed bug which caused Byte, ... values which were out of range to be
 * added to the attribute table anyway.
 * Corrected the number of expected shift-reduce conflicts.
 *
 * Revision 1.12  1995/02/10  02:56:21  jimg
 * Added type checking.
 *
 * Revision 1.11  1994/12/22  04:30:56  reza
 * Made save_str static to avoid linking conflict.
 *
 * Revision 1.10  1994/12/16  22:06:23  jimg
 * Fixed a bug in save_str() where the global NAME was used instead of the
 * parameter DST.
 *
 * Revision 1.9  1994/12/07  21:19:45  jimg
 * Added a new rule (var) and modified attr_val to handle attribute vectors.
 * Each element in the vector is seaprated by a comma.
 * Replaces some old instrumentation code with newer code using the DGB
 * macros.
 *
 * Revision 1.8  1994/11/10  19:50:55  jimg
 * In the past it was possible to have a null file correctly parse as a
 * DAS or DDS. However, now that is not possible. It is possible to have
 * a file that contains no variables parse, but the keyword `Attribute'
 * or `Dataset' *must* be present. This was changed so that errors from
 * the CGIs could be detected (since they return nothing in the case of
 * a error).
 *
 * Revision 1.7  1994/10/18  00:23:18  jimg
 * Added debugging statements.
 *
 * Revision 1.6  1994/10/05  16:46:51  jimg
 * Modified the DAS grammar so that TYPE tokens (from the scanner) were
 * parsed correcly and added to the new AttrTable class.
 * Changed the code used to add entries based on changes to AttrTable.
 * Consoladated error reporting code.
 *
 * Revision 1.5  1994/09/27  23:00:39  jimg
 * Modified to use the new DAS class and new AttrTable class.
 *
 * Revision 1.4  1994/09/15  21:10:56  jimg
 * Added commentary to das.y -- how does it work.
 *
 * Revision 1.3  1994/09/09  16:16:38  jimg
 * Changed the include name to correspond with the class name changes (Var*
 * to DAS*).
 *
 * Revision 1.2  1994/08/02  18:54:15  jimg
 * Added C++ statements to grammar to generate a table of parsed attributes.
 * Added a single parameter to dasparse - an object of class DAS.
 * Solved strange `string accumulation' bug with $1 %2 ... by copying
 * token's semantic values to temps using mid rule actions.
 * Added code to create new attribute tables as each variable is parsed (unless
 * a table has already been allocated, in which case that one is used).
 *
 * Revision 1.2  1994/07/25  19:01:21  jimg
 * Modified scanner and parser so that they can be compiled with g++ and
 * so that they can be linked using g++. They will be combined with a C++
 * method using a global instance variable.
 * Changed the name of line_num in the scanner to das_line_num so that
 * global symbol won't conflict in executables/libraries with multiple
 * scanners.
 *
 * Revision 1.1  1994/07/25  14:26:45  jimg
 * Test files for the DAS/DDS parsers and symbol table software.
 */

