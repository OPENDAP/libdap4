
/*  A Bison parser, made from dds.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse ddsparse
#define yylex ddslex
#define yyerror ddserror
#define yylval ddslval
#define yychar ddschar
#define yydebug ddsdebug
#define yynerrs ddsnerrs
#define	SCAN_ID	257
#define	SCAN_NAME	258
#define	SCAN_INTEGER	259
#define	SCAN_DATASET	260
#define	SCAN_LIST	261
#define	SCAN_SEQUENCE	262
#define	SCAN_STRUCTURE	263
#define	SCAN_FUNCTION	264
#define	SCAN_GRID	265
#define	SCAN_BYTE	266
#define	SCAN_INT16	267
#define	SCAN_UINT16	268
#define	SCAN_INT32	269
#define	SCAN_UINT32	270
#define	SCAN_FLOAT32	271
#define	SCAN_FLOAT64	272
#define	SCAN_STRING	273
#define	SCAN_URL	274

#line 25 "dds.y"


#define YYSTYPE char *

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: dds.tab.c,v 1.34 2001/09/28 17:50:07 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <stack>
#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif

#include "DDS.h"
#include "Array.h"
#include "Error.h"
#include "parser.h"
#include "dds.tab.h"
#include "util.h"

#ifdef WIN32
using std::endl;
using std::ends;
using std::ostrstream;
#endif

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a structure (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)

#define YYPARSE_PARAM arg

extern int dds_line_num;	/* defined in dds.lex */

// No global static objects in the dap library! 1/24/2000 jhrg
static stack<BaseType *> *ctor;
static BaseType *current;
static string *id;
static Part part = nil;		/* Part is defined in BaseType */

static char *NO_DDS_MSG =
"The descriptor object returned from the dataset was null.\n\
Check that the URL is correct.";

static char *BAD_DECLARATION =
"In the dataset descriptor object: Expected a variable declaration\n\
(e.g., Int32 i;). Make sure that the variable name is not the name\n\
of a datatype and that the Array: and Maps: sections of a Grid are\n\
labeled properly.";
 
int ddslex();
void ddserror(char *s);

void add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, 
	       Part p);
void invalid_declaration(parser_arg *arg, string semantic_err_msg, 
			 char *type, char *name);

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		91
#define	YYFLAG		-32768
#define	YYNTBASE	27

#define YYTRANSLATE(x) ((unsigned)(x) <= 274 ? yytranslate[x] : 48)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    23,     2,
    26,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    24,     2,    25,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    21,     2,    22,     2,     2,     2,     2,     2,
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
    17,    18,    19,    20
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,    12,    14,    15,    17,    20,    23,    25,
    29,    30,    38,    39,    47,    48,    49,    50,    63,    65,
    67,    69,    71,    73,    75,    77,    79,    81,    83,    85,
    87,    89,    91,    93,    96,    98,   100,   102,   104,   106,
   108,   110,   112,   114,   116,   118,   120,   122,   124,   128,
   129,   130,   138,   140,   142,   144,   146,   148
};

static const short yyrhs[] = {    28,
     0,    27,    28,     0,     6,    21,    29,    22,    47,    23,
     0,     1,     0,     0,    30,     0,    29,    30,     0,    37,
    31,     0,    31,     0,    41,    42,    23,     0,     0,    38,
    21,    29,    22,    32,    42,    23,     0,     0,    39,    21,
    29,    22,    33,    42,    23,     0,     0,     0,     0,    40,
    21,     3,    34,    30,     3,    35,    29,    22,    36,    42,
    23,     0,     1,     0,     7,     0,     9,     0,     8,     0,
    11,     0,    12,     0,    13,     0,    14,     0,    15,     0,
    16,     0,    17,     0,    18,     0,    19,     0,    20,     0,
    43,     0,    42,    44,     0,     3,     0,    12,     0,    13,
     0,    15,     0,    14,     0,    16,     0,    17,     0,    18,
     0,    19,     0,    20,     0,     9,     0,     8,     0,    11,
     0,     7,     0,    24,     5,    25,     0,     0,     0,    24,
     3,    45,    26,     5,    46,    25,     0,     1,     0,     4,
     0,     5,     0,     6,     0,    43,     0,     1,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   118,   119,   122,   123,   131,   132,   133,   136,   146,   153,
   164,   169,   180,   185,   196,   209,   222,   227,   240,   252,
   260,   268,   276,   284,   285,   286,   287,   288,   289,   290,
   291,   292,   295,   296,   299,   299,   299,   299,   299,   300,
   300,   300,   300,   301,   301,   301,   301,   302,   305,   318,
   322,   338,   338,   350,   351,   352,   353,   354
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SCAN_ID",
"SCAN_NAME","SCAN_INTEGER","SCAN_DATASET","SCAN_LIST","SCAN_SEQUENCE","SCAN_STRUCTURE",
"SCAN_FUNCTION","SCAN_GRID","SCAN_BYTE","SCAN_INT16","SCAN_UINT16","SCAN_INT32",
"SCAN_UINT32","SCAN_FLOAT32","SCAN_FLOAT64","SCAN_STRING","SCAN_URL","'{'","'}'",
"';'","'['","']'","'='","datasets","dataset","declarations","declaration","non_list_decl",
"@1","@2","@3","@4","@5","list","structure","sequence","grid","base_type","var",
"var_name","array_decl","@6","@7","name", NULL
};
#endif

static const short yyr1[] = {     0,
    27,    27,    28,    28,    29,    29,    29,    30,    30,    31,
    32,    31,    33,    31,    34,    35,    36,    31,    31,    37,
    38,    39,    40,    41,    41,    41,    41,    41,    41,    41,
    41,    41,    42,    42,    43,    43,    43,    43,    43,    43,
    43,    43,    43,    43,    43,    43,    43,    43,    44,    45,
    46,    44,    44,    47,    47,    47,    47,    47
};

static const short yyr2[] = {     0,
     1,     2,     6,     1,     0,     1,     2,     2,     1,     3,
     0,     7,     0,     7,     0,     0,     0,    12,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     3,     0,
     0,     7,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     0,
     4,     0,     0,     1,     0,     2,    19,    20,    22,    21,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
     0,     6,     9,     0,     0,     0,     0,     0,     0,     7,
     8,     0,     0,     0,    35,    48,    46,    45,    47,    36,
    37,    39,    38,    40,    41,    42,    43,    44,     0,    33,
    58,    54,    55,    56,    57,     0,     0,     0,    15,    53,
    10,     0,    34,     3,    11,    13,     0,    50,     0,     0,
     0,     0,     0,    49,     0,     0,    16,     0,    12,    14,
     0,    51,     0,     0,    17,    52,     0,     0,    18,     0,
     0
};

static const short yydefgoto[] = {     3,
     4,    21,    22,    23,    70,    71,    67,    81,    87,    24,
    25,    26,    27,    28,    49,    50,    63,    73,    84,    56
};

static const short yypact[] = {     7,
-32768,   -15,    38,-32768,    52,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    72,-32768,-32768,   172,    12,    20,    22,   190,   152,-32768,
-32768,    52,    52,    21,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,     2,-32768,
-32768,-32768,-32768,-32768,-32768,    24,    92,   112,-32768,-32768,
-32768,    37,-32768,-32768,-32768,-32768,     3,-32768,    23,   190,
   190,    42,    26,-32768,     4,     6,-32768,    44,-32768,-32768,
    52,-32768,   132,    29,-32768,-32768,   190,     8,-32768,    55,
-32768
};

static const short yypgoto[] = {-32768,
    53,   -31,   -21,    33,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   -36,    46,-32768,-32768,-32768,-32768
};


#define	YYLAST		210


static const short yytable[] = {    30,
    57,    58,    60,     7,    60,     5,    60,     1,    60,     8,
     9,    10,     2,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    59,    61,    62,    79,    62,    80,    62,
    89,    62,    32,    75,    76,    30,    30,    90,     1,    68,
    33,    69,    34,     2,    77,    72,    64,    74,    82,    83,
    88,    78,     7,    86,    91,     6,    31,     0,     8,     9,
    10,    30,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     7,    -5,    55,     0,     0,     0,     8,     9,
    10,     0,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     7,    29,     0,     0,     0,     0,     8,     9,
    10,     0,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     7,    65,     0,     0,     0,     0,     8,     9,
    10,     0,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     7,    66,     0,     0,     0,     0,     8,     9,
    10,     0,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    51,    85,    35,    52,    53,    54,    36,    37,
    38,     0,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,     7,     0,     0,     0,     0,     0,     0,     9,
    10,     0,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    35,     0,     0,     0,    36,    37,    38,     0,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48
};

static const short yycheck[] = {    21,
    32,    33,     1,     1,     1,    21,     1,     1,     1,     7,
     8,     9,     6,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,     3,    23,    24,    23,    24,    23,    24,
    23,    24,    21,    70,    71,    57,    58,     0,     1,     3,
    21,     5,    21,     6,     3,    67,    23,    25,     5,    81,
    87,    26,     1,    25,     0,     3,    24,    -1,     7,     8,
     9,    83,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     1,    22,    29,    -1,    -1,    -1,     7,     8,
     9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     1,    22,    -1,    -1,    -1,    -1,     7,     8,
     9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     1,    22,    -1,    -1,    -1,    -1,     7,     8,
     9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     1,    22,    -1,    -1,    -1,    -1,     7,     8,
     9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     1,    22,     3,     4,     5,     6,     7,     8,
     9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     1,    -1,    -1,    -1,    -1,    -1,    -1,     8,
     9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     3,    -1,    -1,    -1,     7,     8,     9,    -1,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    20
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

case 4:
#line 124 "dds.y"
{
		    parse_error((parser_arg *)arg, NO_DDS_MSG,
				dds_line_num, yyvsp[0]);
		    YYABORT;
		;
    break;}
case 8:
#line 137 "dds.y"
{ 
		  string smsg;
		  if (current->check_semantics(smsg))
		    add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		  else {
		    invalid_declaration((parser_arg *)arg, smsg, yyvsp[-1], yyvsp[0]);
		    YYABORT;
		  }
		;
    break;}
case 10:
#line 154 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, yyvsp[-2], yyvsp[-1]);
		      YYABORT;
		    }
		;
    break;}
case 11:
#line 165 "dds.y"
{ 
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 12:
#line 170 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, yyvsp[-6], yyvsp[-5]);
		      YYABORT;
		    }
		;
    break;}
case 13:
#line 181 "dds.y"
{ 
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 14:
#line 186 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, yyvsp[-6], yyvsp[-5]);
		      YYABORT;
		    }
		;
    break;}
case 15:
#line 197 "dds.y"
{ 
		    if (is_keyword(string(yyvsp[-2]), "array:"))
			part = array; 
		    else {
			ostrstream msg;
			msg << BAD_DECLARATION << ends;
			parse_error((parser_arg *)arg, msg.str(),
				    dds_line_num, yyvsp[-2]);
			msg.freeze(0);
			YYABORT;
		    }
                ;
    break;}
case 16:
#line 210 "dds.y"
{ 
		    if (is_keyword(string(yyvsp[-4]), "maps:"))
			part = maps; 
		    else {
			ostrstream msg;
			msg << BAD_DECLARATION << ends;
			parse_error((parser_arg *)arg, msg.str(),
				    dds_line_num, yyvsp[-4]);
			msg.freeze(0);
			YYABORT;
		    }
                ;
    break;}
case 17:
#line 223 "dds.y"
{
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 18:
#line 228 "dds.y"
{
		    string smsg;
		    if (current->check_semantics(smsg)) {
			part = nil; 
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    }
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, yyvsp[-11], yyvsp[-10]);
		      YYABORT;
		    }
		;
    break;}
case 19:
#line 241 "dds.y"
{
		    ostrstream msg;
		    msg << BAD_DECLARATION << ends;
		    parse_error((parser_arg *)arg, msg.str(),
				dds_line_num, yyvsp[0]);
		    msg.freeze(0);
		    YYABORT;
		;
    break;}
case 20:
#line 253 "dds.y"
{ 
		    if (!ctor) 
			ctor = new stack<BaseType *>;
		    ctor->push(NewList()); 
		;
    break;}
case 21:
#line 261 "dds.y"
{ 
		    if (!ctor)
	                ctor = new stack<BaseType *>;
		    ctor->push(NewStructure()); 
		;
    break;}
case 22:
#line 269 "dds.y"
{ 
		    if (!ctor)
			ctor = new stack<BaseType *>;
		    ctor->push(NewSequence()); 
		;
    break;}
case 23:
#line 277 "dds.y"
{ 
		    if (!ctor)
			ctor = new stack<BaseType *>;
		    ctor->push(NewGrid()); 
		;
    break;}
case 24:
#line 284 "dds.y"
{ current = NewByte(); ;
    break;}
case 25:
#line 285 "dds.y"
{ current = NewInt16(); ;
    break;}
case 26:
#line 286 "dds.y"
{ current = NewUInt16(); ;
    break;}
case 27:
#line 287 "dds.y"
{ current = NewInt32(); ;
    break;}
case 28:
#line 288 "dds.y"
{ current = NewUInt32(); ;
    break;}
case 29:
#line 289 "dds.y"
{ current = NewFloat32(); ;
    break;}
case 30:
#line 290 "dds.y"
{ current = NewFloat64(); ;
    break;}
case 31:
#line 291 "dds.y"
{ current = NewStr(); ;
    break;}
case 32:
#line 292 "dds.y"
{ current = NewUrl(); ;
    break;}
case 33:
#line 295 "dds.y"
{ current->set_name(yyvsp[0]); ;
    break;}
case 49:
#line 306 "dds.y"
{ 
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi(yyvsp[-1]));
		     }
		     else {
			 Array *a = NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi(yyvsp[-1]));
			 current = a;
		     }
		 ;
    break;}
case 50:
#line 319 "dds.y"
{
		     id = new string(yyvsp[0]);
		 ;
    break;}
case 51:
#line 323 "dds.y"
{ 
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi(yyvsp[0]), *id);
		     }
		     else {
			 Array *a = NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi(yyvsp[0]), *id);
			 current = a;
		     }

		     delete id;
		 ;
    break;}
case 53:
#line 339 "dds.y"
{
		     ostrstream msg;
		     msg << "In the dataset descriptor object:" << endl
			 << "Expected an array subscript." << endl << ends;
		     parse_error((parser_arg *)arg, msg.str(), 
				 dds_line_num, yyvsp[0]);
		     msg.rdbuf()->freeze(0);
		     YYABORT;
		 ;
    break;}
case 54:
#line 350 "dds.y"
{ (*DDS_OBJ(arg)).set_dataset_name(yyvsp[0]); ;
    break;}
case 55:
#line 351 "dds.y"
{ (*DDS_OBJ(arg)).set_dataset_name(yyvsp[0]); ;
    break;}
case 56:
#line 352 "dds.y"
{ (*DDS_OBJ(arg)).set_dataset_name(yyvsp[0]); ;
    break;}
case 57:
#line 353 "dds.y"
{ (*DDS_OBJ(arg)).set_dataset_name(yyvsp[0]); ;
    break;}
case 58:
#line 355 "dds.y"
{
		  ostrstream msg;
		  msg << "Error parsing the dataset name." << endl
		      << "The name may be missing or may contain an illegal character." << endl << ends;
		     parse_error((parser_arg *)arg, msg.str(),
				 dds_line_num);
		     msg.rdbuf()->freeze(0);
		     YYABORT;
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
#line 366 "dds.y"


/* 
   This function must be defined. However, use the error reporting code in
   parser-utils.cc.
*/

void 
ddserror(char *)
{
}

/*
  Invalid declaration message.
*/

void
invalid_declaration(parser_arg *arg, string semantic_err_msg, char *type, 
		    char *name)
{
  ostrstream msg;
  msg << "In the dataset descriptor object: `" << type << " " << name 
      << "'" << endl << "is not a valid declaration." << endl 
      << semantic_err_msg << ends;
  parse_error((parser_arg *)arg, msg.str(), dds_line_num);
  msg.rdbuf()->freeze(0);
}

/*
  Add the variable pointed to by CURRENT to either the topmost ctor object on
  the stack CTOR or to the dataset variable table TABLE if CTOR is empty.  If
  it exists, the current ctor object is popped off the stack and assigned to
  CURRENT.

  NB: the ctor stack is popped for lists and arrays because they are ctors
  which contain only a single variable. For other ctor types, several
  variables may be members and the parse rule (see `declaration' above)
  determines when to pop the stack. 

  Returns: void 
*/

void	
add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, Part part)
{ 
    if (!*ctor)
	*ctor = new stack<BaseType *>;

    if (!(*ctor)->empty()) { /* must be parsing a ctor type */
	(*ctor)->top()->add_var(*current, part);

 	const Type &ctor_type = (*ctor)->top()->type();

	if (ctor_type == dods_list_c || ctor_type == dods_array_c) {
	    *current = (*ctor)->top();
	    (*ctor)->pop();
	}
	else
	    return;
    }
    else
	table.add_var(*current);
}

/* 
 * $Log: dds.tab.c,v $
 * Revision 1.34  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 *
 * Revision 1.36  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.33.4.3  2001/06/23 00:52:08  jimg
 * Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
 * that they are (more or less) the same in all the scanners. There are
 * one or two characters that differ (for example das.lex allows ( and )
 * in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
 * are essentially the same across the board.
 * Added `#' to the set of characeters allowed in an ID (bug 179).
 *
 * Revision 1.35  2001/06/15 23:49:03  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.33.4.2  2001/05/08 19:10:47  jimg
 * Expanded the set of names that the dds.y parser will recognize to
 * include integers (for files named like 990412.nc). Also removed the
 * unused keywords Dependent and Independent from both the DDS scanner
 * and parser.
 * Added other reserved words to the set of possible Dataset names.
 *
 * Revision 1.33.4.1  2001/05/04 00:12:10  jimg
 * Added a rule that allows variable names to be the names of datatypes.
 *
 * Revision 1.33  2000/09/22 02:17:22  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.32  2000/09/21 16:22:10  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.31  2000/08/16 18:29:02  jimg
 * Added dot (.) to the set of characters allowed in a variable name
 *
 * Revision 1.30  2000/07/09 21:43:29  rmorris
 * Mods to increase portability, minimize ifdef's for win32
 *
 * Revision 1.29  2000/06/07 18:07:00  jimg
 * Merged the pc port branch
 *
 * Revision 1.28.6.1  2000/06/02 18:36:38  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.27.8.1  2000/02/17 05:03:17  jimg
 * Added file and line number information to calls to InternalErr.
 * Resolved compile-time problems with read due to a change in its
 * parameter list given that errors are now reported using exceptions.
 *
 * Revision 1.28  2000/01/27 06:30:00  jimg
 * Resolved conflicts from merge with release-3-1-4
 *
 * Revision 1.27.2.1  2000/01/24 22:25:10  jimg
 * Removed static global objects
 *
 * Revision 1.27  1999/07/22 17:07:47  jimg
 * Fixed a bug found by Peter Fox. Array index names were not handled properly
 * after the String to string conversion.
 *
 * Revision 1.26  1999/05/04 19:47:23  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.25  1999/04/29 02:29:36  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.24  1999/03/24 23:32:33  jimg
 * Added support for the new Int16, UInt16 and Float32 types.
 *
 * Revision 1.23  1998/08/13 22:12:44  jimg
 * Fixed error messages.
 *
 * Revision 1.22.6.2  1999/02/05 09:32:36  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.22.6.1  1999/02/02 21:57:06  jimg
 * String to string version
 *
 * Revision 1.22  1997/11/20 20:14:10  jimg
 * Added to the name rule so that it recognizes both the ID and NAME lexeme
 * as valid when parsing the dataset name. NAME (see dds.lex) is just like ID
 * except that it includes `.'. Thus datasets with names like sst.reynolds.nc
 * now parse correctly.
 *
 * Revision 1.21  1997/02/28 01:31:22  jimg
 * Added error messages.
 *
 * Revision 1.20  1996/10/28 23:44:16  jimg
 * Added unsigned int to set of possible datatypes.
 *
 * Revision 1.19  1996/10/16 22:35:31  jimg
 * Fixed bad operator in DODS_BISON_VER preprocessor statement.
 *
 * Revision 1.18  1996/10/08 17:04:42  jimg
 * Added a fix for Bison 1.25 so that PARSE_PARAM will still work
 *
 * Revision 1.17  1996/08/13 20:54:45  jimg
 * Generated files.
 *
 * Revision 1.16  1996/05/31 23:27:17  jimg
 * Removed {YYACCEPT;} from rule 2 (dataset: DATASET ...).
 *
 * Revision 1.15  1996/05/29 21:59:51  jimg
 * *** empty log message ***
 *
 * Revision 1.14  1996/05/14 15:38:54  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.13  1996/04/05 21:59:38  jimg
 * Misc Changes for release 2.0.1 of the core software - for developers.
 *
 * Revision 1.12  1996/04/05 00:06:45  jimg
 * Merged changes from version 1.1.1.
 * Eliminated the static global CTOR.
 *
 * Revision 1.11  1995/12/06 19:45:08  jimg
 * Changed grammar so that List List ... <type> is no longer possible. This
 * fixed some hard problems in the serailize/deserailize mfuncs.
 *
 * Revision 1.10  1995/10/23  22:59:41  jimg
 * Modified some rules so that they use the functions defined in
 * parser_util.cc instead of local definitions.
 *
 * Revision 1.9  1995/08/23  00:27:47  jimg
 * Uses new member functions.
 * Added copyright notice.
 * Switched from String to enum type representation.
 *
 * Revision 1.8.2.1  1996/04/04 23:24:44  jimg
 * Removed static global CTOR from the dds parser. The stack for constructor
 * variable is now managed via a pointer. The stack is allocated when first
 * used by add_entry().
 *
 * Revision 1.8  1995/01/19  20:13:04  jimg
 * The parser now uses the new utility functions to create new instances
 * of the variable objects (Byte, ..., Grid).
 * Fixed the number of shift/reduce conflicts expected (now at 60).
 *
 * Revision 1.7  1994/12/22  04:30:57  reza
 * Made save_str static to avoid linking conflict.
 *
 * Revision 1.6  1994/12/16  22:24:23  jimg
 * Switched from a CtorType stack to BaseType stack.
 * Fixed an error in save_str() (see das.y).
 * Fixed a bug in the use of append_dim - it was called with $4 when it
 * should have been called with $5.
 *
 * Revision 1.5  1994/12/09  21:42:41  jimg
 * Added to array: so that an array decl can contain: an int or an id=int.
 * This is for the named dimensions (see Array.{cc,h}).
 *
 * Revision 1.4  1994/11/10  19:50:54  jimg
 * In the past it was possible to have a null file correctly parse as a
 * DAS or DDS. However, now that is not possible. It is possible to have
 * a file that contains no variables parse, but the keyword `Attribute'
 * or `Dataset' *must* be present. This was changed so that errors from
 * the CGIs could be detected (since they return nothing in the case of
 * a error).
 *
 * Revision 1.3  1994/09/23  14:56:19  jimg
 * Added code to build in-memory DDS during parse.
 *
 * Revision 1.2  1994/09/15  21:11:56  jimg
 * Modified dds.y so that it can parse all the DDS types.
 * Still no error checking beyond what bison gives you.
 *
 * Revision 1.1  1994/09/08  21:10:45  jimg
 * DDS Class test driver and parser and scanner.
 */

