
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
#define	SCAN_WORD	257
#define	SCAN_DATASET	258
#define	SCAN_LIST	259
#define	SCAN_SEQUENCE	260
#define	SCAN_STRUCTURE	261
#define	SCAN_FUNCTION	262
#define	SCAN_GRID	263
#define	SCAN_BYTE	264
#define	SCAN_INT16	265
#define	SCAN_UINT16	266
#define	SCAN_INT32	267
#define	SCAN_UINT32	268
#define	SCAN_FLOAT32	269
#define	SCAN_FLOAT64	270
#define	SCAN_STRING	271
#define	SCAN_URL	272

#line 47 "dds.y"


#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <stack>
#include <sstream>

#include "DDS.h"
#include "Array.h"
#include "Error.h"
#include "parser.h"
#include "util.h"

 using namespace std;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a structure (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 
// ERROR is no longer used. These parsers now signal problems by throwing
// exceptions. 5/22/2002 jhrg
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
void error_exit_cleanup();
void add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, 
	       Part p);
void invalid_declaration(parser_arg *arg, string semantic_err_msg, 
			 char *type, char *name);


#line 109 "dds.y"
typedef union {
    bool boolean;
    char word[ID_MAX];
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		88
#define	YYFLAG		-32768
#define	YYNTBASE	26

#define YYTRANSLATE(x) ((unsigned)(x) <= 272 ? yytranslate[x] : 47)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    22,    21,     2,
    25,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    23,     2,    24,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    19,     2,    20,     2,     2,     2,     2,     2,
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
    17,    18
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     9,    16,    18,    19,    21,    24,
    28,    29,    37,    38,    46,    47,    48,    49,    64,    66,
    68,    70,    72,    74,    76,    78,    80,    82,    84,    86,
    88,    90,    92,    95,    97,    99,   101,   103,   105,   107,
   109,   111,   113,   115,   117,   119,   121,   123,   127,   128,
   129,   137,   139,   141,   143
};

static const short yyrhs[] = {    -1,
    27,    28,     0,    29,     0,    28,    29,     0,     4,    19,
    30,    20,    46,    21,     0,     1,     0,     0,    31,     0,
    30,    31,     0,    40,    41,    21,     0,     0,    37,    19,
    30,    20,    32,    41,    21,     0,     0,    38,    19,    30,
    20,    33,    41,    21,     0,     0,     0,     0,    39,    19,
     3,    22,    34,    31,     3,    22,    35,    30,    20,    36,
    41,    21,     0,     1,     0,     7,     0,     6,     0,     9,
     0,    10,     0,    11,     0,    12,     0,    13,     0,    14,
     0,    15,     0,    16,     0,    17,     0,    18,     0,    42,
     0,    41,    43,     0,     3,     0,    10,     0,    11,     0,
    13,     0,    12,     0,    14,     0,    15,     0,    16,     0,
    17,     0,    18,     0,     7,     0,     6,     0,     9,     0,
     5,     0,    23,     3,    24,     0,     0,     0,    23,     3,
    44,    25,     3,    45,    24,     0,     1,     0,    42,     0,
     4,     0,     1,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   137,   143,   148,   149,   152,   156,   165,   170,   171,   178,
   200,   206,   220,   226,   240,   252,   264,   270,   286,   297,
   303,   309,   315,   316,   317,   318,   319,   320,   321,   322,
   323,   326,   327,   330,   330,   330,   330,   330,   331,   331,
   331,   331,   332,   332,   332,   332,   333,   336,   357,   361,
   386,   386,   397,   398,   399
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SCAN_WORD",
"SCAN_DATASET","SCAN_LIST","SCAN_SEQUENCE","SCAN_STRUCTURE","SCAN_FUNCTION",
"SCAN_GRID","SCAN_BYTE","SCAN_INT16","SCAN_UINT16","SCAN_INT32","SCAN_UINT32",
"SCAN_FLOAT32","SCAN_FLOAT64","SCAN_STRING","SCAN_URL","'{'","'}'","';'","':'",
"'['","']'","'='","start","@1","datasets","dataset","declarations","declaration",
"@2","@3","@4","@5","@6","structure","sequence","grid","base_type","var","var_name",
"array_decl","@7","@8","name", NULL
};
#endif

static const short yyr1[] = {     0,
    27,    26,    28,    28,    29,    29,    30,    30,    30,    31,
    32,    31,    33,    31,    34,    35,    36,    31,    31,    37,
    38,    39,    40,    40,    40,    40,    40,    40,    40,    40,
    40,    41,    41,    42,    42,    42,    42,    42,    42,    42,
    42,    42,    42,    42,    42,    42,    42,    43,    44,    45,
    43,    43,    46,    46,    46
};

static const short yyr2[] = {     0,
     0,     2,     1,     2,     6,     1,     0,     1,     2,     3,
     0,     7,     0,     7,     0,     0,     0,    14,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     3,     0,     0,
     7,     1,     1,     1,     1
};

static const short yydefact[] = {     1,
     0,     6,     0,     0,     3,     0,     4,    19,    21,    20,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
     0,     8,     0,     0,     0,     0,     0,     9,     0,     0,
     0,    34,    47,    45,    44,    46,    35,    36,    38,    37,
    39,    40,    41,    42,    43,     0,    32,    55,    54,    53,
     0,     0,     0,     0,    52,    10,     0,    33,     5,    11,
    13,    15,    49,     0,     0,     0,    48,     0,     0,     0,
     0,     0,    12,    14,     0,    50,    16,     0,     0,    51,
     0,    17,     0,     0,    18,     0,     0,     0
};

static const short yydefgoto[] = {    86,
     1,     4,     5,    21,    22,    64,    65,    66,    79,    83,
    23,    24,    25,    26,    46,    47,    58,    68,    78,    51
};

static const short yypact[] = {-32768,
     9,-32768,    -8,     5,-32768,    52,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    70,-32768,    -7,    -3,    -2,   176,   142,-32768,    52,    52,
    15,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,     2,-32768,-32768,-32768,-32768,
    -1,    88,   106,     0,-32768,-32768,    16,-32768,-32768,-32768,
-32768,-32768,    10,   176,   176,   160,-32768,    -4,     3,     6,
    32,    33,-32768,-32768,    17,-32768,-32768,    13,    52,-32768,
   124,-32768,   176,     7,-32768,    38,    40,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    37,   -28,   -21,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   -50,    19,-32768,-32768,-32768,-32768
};


#define	YYLAST		194


static const short yytable[] = {    28,
    52,    53,    55,    55,    -2,     2,    55,    55,     3,     2,
     6,    29,     3,    69,    70,    30,    31,    54,    63,    59,
    72,    62,    56,    73,    57,    57,    74,    85,    57,    57,
    28,    28,    84,    67,    75,    76,    80,    87,    77,    88,
     7,     0,     0,     0,    71,    50,     0,     0,     0,     0,
    81,     0,     8,     0,     0,     0,     0,     9,    10,    28,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
     8,    -7,     0,     0,     0,     9,    10,     0,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,     8,    27,
     0,     0,     0,     9,    10,     0,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,     8,    60,     0,     0,
     0,     9,    10,     0,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,     8,    61,     0,     0,     0,     9,
    10,     0,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    48,    82,    32,    49,    33,    34,    35,     0,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
     8,     0,     0,     0,     0,     9,    10,     0,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    32,     0,
    33,    34,    35,     0,    36,    37,    38,    39,    40,    41,
    42,    43,    44,    45
};

static const short yycheck[] = {    21,
    29,    30,     1,     1,     0,     1,     1,     1,     4,     1,
    19,    19,     4,    64,    65,    19,    19,     3,     3,    21,
    25,    22,    21,    21,    23,    23,    21,    21,    23,    23,
    52,    53,    83,    24,     3,     3,    24,     0,    22,     0,
     4,    -1,    -1,    -1,    66,    27,    -1,    -1,    -1,    -1,
    79,    -1,     1,    -1,    -1,    -1,    -1,     6,     7,    81,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
     1,    20,    -1,    -1,    -1,     6,     7,    -1,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,     1,    20,
    -1,    -1,    -1,     6,     7,    -1,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,     1,    20,    -1,    -1,
    -1,     6,     7,    -1,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,     1,    20,    -1,    -1,    -1,     6,
     7,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,     1,    20,     3,     4,     5,     6,     7,    -1,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
     1,    -1,    -1,    -1,    -1,     6,     7,    -1,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,     3,    -1,
     5,     6,     7,    -1,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18
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
#line 138 "dds.y"
{
		    /* On entry to the parser, make the BaseType stack. */
		    ctor = new stack<BaseType *>;
                ;
    break;}
case 2:
#line 143 "dds.y"
{
		    delete ctor; ctor = 0;
		;
    break;}
case 5:
#line 153 "dds.y"
{
		    yyval.boolean = yyvsp[-3].boolean && yyvsp[-1].word;
		;
    break;}
case 6:
#line 157 "dds.y"
{
		    parse_error((parser_arg *)arg, NO_DDS_MSG,
 				dds_line_num, yyvsp[0].word);
		    error_exit_cleanup();
		    YYABORT;
		;
    break;}
case 7:
#line 166 "dds.y"
{
		    yyval.boolean = true;
		;
    break;}
case 8:
#line 170 "dds.y"
{ yyval.boolean = true; ;
    break;}
case 9:
#line 171 "dds.y"
{ yyval.boolean = true; ;
    break;}
case 10:
#line 179 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg)) {
			/* BaseType *current_save = current ; */
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
			/* FIX
			if( current_save == current )
			{
			    delete current ;
			    current = 0 ;
			}
			*/
		    } else {
		      invalid_declaration((parser_arg *)arg, smsg, yyvsp[-2].word, yyvsp[-1].word);
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strncpy(yyval.word,yyvsp[-1].word,ID_MAX);
                    yyval.word[ID_MAX-1] = '\0';
		;
    break;}
case 11:
#line 201 "dds.y"
{ 
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 12:
#line 207 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, yyvsp[-6].word, yyvsp[-1].word);
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strncpy(yyval.word,yyvsp[-1].word,ID_MAX);
                    yyval.word[ID_MAX-1] = '\0';
		;
    break;}
case 13:
#line 221 "dds.y"
{ 
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 14:
#line 227 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, yyvsp[-6].word, yyvsp[-1].word);
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strncpy(yyval.word,yyvsp[-1].word,ID_MAX);
                    yyval.word[ID_MAX-1] = '\0';
		;
    break;}
case 15:
#line 241 "dds.y"
{ 
		    if (is_keyword(string(yyvsp[-1].word), "array"))
			part = array; 
		    else {
			ostringstream msg;
			msg << BAD_DECLARATION;
			parse_error((parser_arg *)arg, msg.str().c_str(),
				    dds_line_num, yyvsp[-1].word);
			YYABORT;
		    }
                ;
    break;}
case 16:
#line 253 "dds.y"
{ 
		    if (is_keyword(string(yyvsp[-1].word), "maps"))
			part = maps; 
		    else {
			ostringstream msg;
			msg << BAD_DECLARATION;
			parse_error((parser_arg *)arg, msg.str().c_str(),
				    dds_line_num, yyvsp[-1].word);
			YYABORT;
		    }
                ;
    break;}
case 17:
#line 265 "dds.y"
{
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 18:
#line 271 "dds.y"
{
		    string smsg;
		    if (current->check_semantics(smsg)) {
			part = nil; 
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    }
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, yyvsp[-13].word, yyvsp[-1].word);
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strncpy(yyval.word,yyvsp[-1].word,ID_MAX);
                    yyval.word[ID_MAX-1] = '\0';
		;
    break;}
case 19:
#line 287 "dds.y"
{
		    ostringstream msg;
		    msg << BAD_DECLARATION;
		    parse_error((parser_arg *)arg, msg.str().c_str(),
				dds_line_num, yyvsp[0].word);
		    YYABORT;
		;
    break;}
case 20:
#line 298 "dds.y"
{ 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewStructure()); 
		;
    break;}
case 21:
#line 304 "dds.y"
{ 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewSequence()); 
		;
    break;}
case 22:
#line 310 "dds.y"
{ 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewGrid()); 
		;
    break;}
case 23:
#line 315 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewByte(); ;
    break;}
case 24:
#line 316 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewInt16(); ;
    break;}
case 25:
#line 317 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUInt16(); ;
    break;}
case 26:
#line 318 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewInt32(); ;
    break;}
case 27:
#line 319 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUInt32(); ;
    break;}
case 28:
#line 320 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewFloat32(); ;
    break;}
case 29:
#line 321 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewFloat64(); ;
    break;}
case 30:
#line 322 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewStr(); ;
    break;}
case 31:
#line 323 "dds.y"
{ if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUrl(); ;
    break;}
case 32:
#line 326 "dds.y"
{ current->set_name(yyvsp[0].word); ;
    break;}
case 48:
#line 337 "dds.y"
{ 
		     if (!check_int32(yyvsp[-1].word)) {
			 string msg = "In the dataset descriptor object:\n";
			 msg += "Expected an array subscript.\n";
			 parse_error((parser_arg *)arg, msg.c_str(), 
				 dds_line_num, yyvsp[-1].word);
		     }
		     if (current->type() == dods_array_c
			 && check_int32(yyvsp[-1].word)) {
			 ((Array *)current)->append_dim(atoi(yyvsp[-1].word));
		     }
		     else {
			 Array *a = DDS_OBJ(arg)->get_factory()->NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi(yyvsp[-1].word));
			 if( current ) delete current ;
			 current = a;
		     }
		 ;
    break;}
case 49:
#line 358 "dds.y"
{
		     id = new string(yyvsp[0].word);
		 ;
    break;}
case 50:
#line 362 "dds.y"
{ 
		     if (!check_int32(yyvsp[0].word)) {
			 string msg = "In the dataset descriptor object:\n";
			 msg += "Expected an array subscript.\n";
			 parse_error((parser_arg *)arg, msg.c_str(), 
				 dds_line_num, yyvsp[0].word);
			 error_exit_cleanup();
			 YYABORT;
		     }
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi(yyvsp[0].word), *id);
		     }
		     else {
			 Array *a = DDS_OBJ(arg)->get_factory()->NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi(yyvsp[0].word), *id);
			 if( current ) delete current ;
			 current = a;
		     }

		     delete id; id = 0;
		 ;
    break;}
case 52:
#line 387 "dds.y"
{
		     ostringstream msg;
		     msg << "In the dataset descriptor object:" << endl
			 << "Expected an array subscript." << endl;
		     parse_error((parser_arg *)arg, msg.str().c_str(), 
				 dds_line_num, yyvsp[0].word);
		     YYABORT;
		 ;
    break;}
case 53:
#line 397 "dds.y"
{ (*DDS_OBJ(arg)).set_dataset_name(yyvsp[0].word); ;
    break;}
case 54:
#line 398 "dds.y"
{ (*DDS_OBJ(arg)).set_dataset_name(yyvsp[0].word); ;
    break;}
case 55:
#line 400 "dds.y"
{
		  ostringstream msg;
		  msg << "Error parsing the dataset name." << endl
		      << "The name may be missing or may contain an illegal character." << endl;
		     parse_error((parser_arg *)arg, msg.str().c_str(),
				 dds_line_num, yyvsp[0].word);
		     YYABORT;
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
#line 410 "dds.y"


/* 
   This function must be defined. However, use the error reporting code in
   parser-utils.cc.
*/

void 
ddserror(char *)
{
}

/*
  Error clean up. Call this before calling YYBORT. Don't call this on a
  normal exit.
*/

void
error_exit_cleanup()
{
    delete id; id = 0;
    delete current; current = 0;
    delete ctor; ctor = 0;
}

/*
  Invalid declaration message.
*/

void
invalid_declaration(parser_arg *arg, string semantic_err_msg, char *type, 
		    char *name)
{
  ostringstream msg;
  msg << "In the dataset descriptor object: `" << type << " " << name 
      << "'" << endl << "is not a valid declaration." << endl 
      << semantic_err_msg;
  parse_error((parser_arg *)arg, msg.str().c_str(), dds_line_num);
}

/*
  Add the variable pointed to by CURRENT to either the topmost ctor object on
  the stack CTOR or to the dataset variable table TABLE if CTOR is empty.  If
  it exists, the current ctor object is popped off the stack and assigned to
  CURRENT.

  NB: the ctor stack is popped for arrays because they are ctors which
  contain only a single variable. For other ctor types, several variables may
  be members and the parse rule (see `declaration' above) determines when to
  pop the stack.

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

	if (ctor_type == dods_array_c) {
	    if( *current ) delete *current ;
	    *current = (*ctor)->top();
	    (*ctor)->pop();

	    // Return here to avoid deleting the new value of 'current.'
	    return;
	}
    }
    else {
	table.add_var(*current);
    }

    if (*current) 
	delete *current; 
    *current = 0;
}

