
/*  A Bison parser, made from dds.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse ddsparse
#define yylex ddslex
#define yyerror ddserror
#define yylval ddslval
#define yychar ddschar
#define yydebug ddsdebug
#define yynerrs ddsnerrs
#define	SCAN_ID	258
#define	SCAN_NAME	259
#define	SCAN_INTEGER	260
#define	SCAN_DATASET	261
#define	SCAN_INDEPENDENT	262
#define	SCAN_DEPENDENT	263
#define	SCAN_ARRAY	264
#define	SCAN_MAPS	265
#define	SCAN_LIST	266
#define	SCAN_SEQUENCE	267
#define	SCAN_STRUCTURE	268
#define	SCAN_FUNCTION	269
#define	SCAN_GRID	270
#define	SCAN_BYTE	271
#define	SCAN_INT16	272
#define	SCAN_UINT16	273
#define	SCAN_INT32	274
#define	SCAN_UINT32	275
#define	SCAN_FLOAT32	276
#define	SCAN_FLOAT64	277
#define	SCAN_STRING	278
#define	SCAN_URL	279

#line 153 "dds.y"


#define YYSTYPE char *

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: dds.tab.c,v 1.18 2000/06/07 18:07:00 jimg Exp $"};

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
using namespace std;
#endif

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)
#define ERROR_OBJ(arg) ((parser_arg *)(arg))->_error
#define STATUS(arg) ((parser_arg *)(arg))->_status
#if DODS_BISON_VER > 124
#define YYPARSE_PARAM arg
#else
#define YYPARSE_PARAM void *arg
#endif

extern int dds_line_num;	/* defined in dds.lex */

// No global static objects in the dap library! 1/24/2000 jhrg
static stack<BaseType *> *ctor;
static BaseType *current;
static string *id;
static Part part = nil;		/* Part is defined in BaseType */

static char *NO_DDS_MSG =
"The descriptor object returned from the dataset was null.\n\
Check that the URL is correct.";

int ddslex();
void ddserror(char *s);

void add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, 
	       Part p);

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		77
#define	YYFLAG		-32768
#define	YYNTBASE	32

#define YYTRANSLATE(x) ((unsigned)(x) <= 279 ? yytranslate[x] : 52)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    28,    27,     2,
    31,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    29,     2,    30,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    25,     2,    26,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,    12,    14,    15,    17,    20,    23,    25,
    29,    30,    38,    39,    47,    48,    49,    50,    65,    67,
    69,    71,    73,    75,    77,    79,    81,    83,    85,    87,
    89,    91,    93,    95,    98,   102,   103,   104,   112,   114,
   116,   118
};

static const short yyrhs[] = {    33,
     0,    32,    33,     0,     6,    25,    34,    26,    51,    27,
     0,     1,     0,     0,    35,     0,    34,    35,     0,    42,
    36,     0,    36,     0,    46,    47,    27,     0,     0,    43,
    25,    34,    26,    37,    47,    27,     0,     0,    44,    25,
    34,    26,    38,    47,    27,     0,     0,     0,     0,    45,
    25,     9,    28,    39,    35,    10,    28,    40,    34,    26,
    41,    47,    27,     0,     1,     0,    11,     0,    13,     0,
    12,     0,    15,     0,    16,     0,    17,     0,    18,     0,
    19,     0,    20,     0,    21,     0,    22,     0,    23,     0,
    24,     0,     3,     0,    47,    48,     0,    29,     5,    30,
     0,     0,     0,    29,     3,    49,    31,     5,    50,    30,
     0,     1,     0,     4,     0,     3,     0,     1,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   245,   246,   249,   250,   257,   258,   259,   262,   282,   289,
   310,   315,   336,   341,   362,   364,   366,   371,   394,   416,
   424,   432,   440,   448,   449,   450,   451,   452,   453,   454,
   455,   456,   459,   460,   463,   476,   480,   496,   496,   511,
   512,   513
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SCAN_ID",
"SCAN_NAME","SCAN_INTEGER","SCAN_DATASET","SCAN_INDEPENDENT","SCAN_DEPENDENT",
"SCAN_ARRAY","SCAN_MAPS","SCAN_LIST","SCAN_SEQUENCE","SCAN_STRUCTURE","SCAN_FUNCTION",
"SCAN_GRID","SCAN_BYTE","SCAN_INT16","SCAN_UINT16","SCAN_INT32","SCAN_UINT32",
"SCAN_FLOAT32","SCAN_FLOAT64","SCAN_STRING","SCAN_URL","'{'","'}'","';'","':'",
"'['","']'","'='","datasets","dataset","declarations","declaration","non_list_decl",
"@1","@2","@3","@4","@5","list","structure","sequence","grid","base_type","var",
"array_decl","@6","@7","name", NULL
};
#endif

static const short yyr1[] = {     0,
    32,    32,    33,    33,    34,    34,    34,    35,    35,    36,
    37,    36,    38,    36,    39,    40,    41,    36,    36,    42,
    43,    44,    45,    46,    46,    46,    46,    46,    46,    46,
    46,    46,    47,    47,    48,    49,    50,    48,    48,    51,
    51,    51
};

static const short yyr2[] = {     0,
     1,     2,     6,     1,     0,     1,     2,     2,     1,     3,
     0,     7,     0,     7,     0,     0,     0,    14,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     2,     3,     0,     0,     7,     1,     1,
     1,     1
};

static const short yydefact[] = {     0,
     4,     0,     0,     1,     0,     2,    19,    20,    22,    21,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
     0,     6,     9,     0,     0,     0,     0,     0,     0,     7,
     8,     0,     0,     0,    33,     0,    42,    41,    40,     0,
     0,     0,     0,    39,    10,     0,    34,     3,    11,    13,
    15,    36,     0,     0,     0,     0,     0,    35,     0,     0,
     0,     0,    12,    14,     0,    37,    16,     0,     0,    38,
     0,    17,     0,     0,    18,     0,     0
};

static const short yydefgoto[] = {     3,
     4,    21,    22,    23,    54,    55,    56,    69,    73,    24,
    25,    26,    27,    28,    36,    47,    57,    68,    40
};

static const short yypact[] = {     6,
-32768,   -12,     9,-32768,    40,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    64,-32768,-32768,   173,    -7,    -6,    -3,    23,    13,-32768,
-32768,    40,    40,    18,-32768,     2,-32768,-32768,-32768,     1,
    88,   112,     5,-32768,-32768,    20,-32768,-32768,-32768,-32768,
-32768,-32768,    12,    23,    23,   160,    14,-32768,     3,     7,
    30,    38,-32768,-32768,    16,-32768,-32768,    17,    40,-32768,
   136,-32768,    23,    10,-32768,    46,-32768
};

static const short yypgoto[] = {-32768,
    45,   -31,   -21,    25,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   -49,-32768,-32768,-32768,-32768
};


#define	YYLAST		197


static const short yytable[] = {    30,
    41,    42,    44,    44,    59,    60,     1,    44,    76,     1,
    44,     2,     5,    37,     2,    38,    39,    32,    33,    30,
    30,    34,    52,    74,    53,    35,    43,    48,    45,    63,
    46,    46,    51,    64,    61,    46,    75,    71,    46,    65,
     7,    58,    66,    67,    62,    77,    70,     6,    31,    30,
     8,     9,    10,     0,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,     7,    -5,     0,     0,     0,     0,
     0,     0,     0,     0,     8,     9,    10,     0,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,     7,    29,
     0,     0,     0,     0,     0,     0,     0,     0,     8,     9,
    10,     0,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     7,    49,     0,     0,     0,     0,     0,     0,
     0,     0,     8,     9,    10,     0,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,     7,    50,     0,     0,
     0,     0,     0,     0,     0,     0,     8,     9,    10,     0,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
     7,    72,     0,     0,     0,     0,     0,     0,     0,     0,
     8,     9,    10,     7,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,     9,    10,     0,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20
};

static const short yycheck[] = {    21,
    32,    33,     1,     1,    54,    55,     1,     1,     0,     1,
     1,     6,    25,     1,     6,     3,     4,    25,    25,    41,
    42,    25,     3,    73,     5,     3,     9,    27,    27,    27,
    29,    29,    28,    27,    56,    29,    27,    69,    29,    10,
     1,    30,     5,    28,    31,     0,    30,     3,    24,    71,
    11,    12,    13,    -1,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,     1,    26,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    11,    12,    13,    -1,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,     1,    26,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,    12,
    13,    -1,    15,    16,    17,    18,    19,    20,    21,    22,
    23,    24,     1,    26,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    11,    12,    13,    -1,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    24,     1,    26,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    11,    12,    13,    -1,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
     1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    11,    12,    13,     1,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    12,    13,    -1,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
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

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

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
     int count;
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
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "bison.simple"

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

int
yyparse(YYPARSE_PARAM_ARG)
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
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
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
#line 251 "dds.y"
{
		    parse_error((parser_arg *)arg, NO_DDS_MSG);
		    YYABORT;
		;
    break;}
case 8:
#line 263 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
#ifdef WIN32
			std::ostrstream msg;
#else
			ostrstream msg;
#endif
			msg << "In the dataset descriptor object:" << endl
			    << "`" << yyvsp[-1] << " " << yyvsp[0] 
			    << "' is not a valid declaration" << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		;
    break;}
case 10:
#line 290 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
#ifdef WIN32
			std::ostrstream msg;
#else
			ostrstream msg;
#endif
			msg << "In the dataset descriptor object:" << endl
			    << "`" << yyvsp[-2] << " " << yyvsp[-1] 
			    << "' is not a valid declaration" << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		;
    break;}
case 11:
#line 311 "dds.y"
{ 
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 12:
#line 316 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
#ifdef WIN32
			std::ostrstream msg;
#else
			ostrstream msg;
#endif
			msg << "In the dataset descriptor object:" << endl
			    << "`" << yyvsp[-6] << "'" << endl
			    << "is not a valid declaration." << endl
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		;
    break;}
case 13:
#line 337 "dds.y"
{ 
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 14:
#line 342 "dds.y"
{ 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
#ifdef WIN32
			std::ostrstream msg;
#else
			ostrstream msg;
#endif
			msg << "In the dataset descriptor object:" << endl
			    << "`" << yyvsp[-6] << "'" << endl
			    << "is not a valid declaration." << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		;
    break;}
case 15:
#line 363 "dds.y"
{ part = array; ;
    break;}
case 16:
#line 365 "dds.y"
{ part = maps; ;
    break;}
case 17:
#line 367 "dds.y"
{
		    current = ctor->top(); 
		    ctor->pop();
		;
    break;}
case 18:
#line 372 "dds.y"
{
		    string smsg;
		    if (current->check_semantics(smsg)) {
			part = nil; 
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    }
		    else {
#ifdef WIN32
			std::ostrstream msg;
#else
			ostrstream msg;
#endif
			msg << "In the dataset descriptor object:" << endl
			    << "`" << yyvsp[-13] << "'" << endl
			    << "is not a valid declaration." << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		;
    break;}
case 19:
#line 395 "dds.y"
{
#ifdef WIN32
			std::ostrstream msg;
#else
		    ostrstream msg;
#endif
		    msg << "In the dataset descriptor object:" << endl
			<< "Expected a varaible declaration" << endl 
			<< "(e.g., Int32 i;). Make sure that the" << endl
			<< "variable name is not a reserved word" << endl
			<< "(Byte, Int32, Float64, String, Url" <<endl
			<< "Structure, Sequence or Grid - all" << endl
			<< "forms, byte, Byte and BYTE, are the same)" << endl
			<< ends;
		    parse_error((parser_arg *)arg, msg.str());
		    msg.freeze(0);
		    YYABORT;
		;
    break;}
case 20:
#line 417 "dds.y"
{ 
		    if (!ctor) 
			ctor = new stack<BaseType *>;
		    ctor->push(NewList()); 
		;
    break;}
case 21:
#line 425 "dds.y"
{ 
		    if (!ctor)
	                ctor = new stack<BaseType *>;
		    ctor->push(NewStructure()); 
		;
    break;}
case 22:
#line 433 "dds.y"
{ 
		    if (!ctor)
			ctor = new stack<BaseType *>;
		    ctor->push(NewSequence()); 
		;
    break;}
case 23:
#line 441 "dds.y"
{ 
		    if (!ctor)
			ctor = new stack<BaseType *>;
		    ctor->push(NewGrid()); 
		;
    break;}
case 24:
#line 448 "dds.y"
{ current = NewByte(); ;
    break;}
case 25:
#line 449 "dds.y"
{ current = NewInt16(); ;
    break;}
case 26:
#line 450 "dds.y"
{ current = NewUInt16(); ;
    break;}
case 27:
#line 451 "dds.y"
{ current = NewInt32(); ;
    break;}
case 28:
#line 452 "dds.y"
{ current = NewUInt32(); ;
    break;}
case 29:
#line 453 "dds.y"
{ current = NewFloat32(); ;
    break;}
case 30:
#line 454 "dds.y"
{ current = NewFloat64(); ;
    break;}
case 31:
#line 455 "dds.y"
{ current = NewStr(); ;
    break;}
case 32:
#line 456 "dds.y"
{ current = NewUrl(); ;
    break;}
case 33:
#line 459 "dds.y"
{ current->set_name(yyvsp[0]); ;
    break;}
case 35:
#line 464 "dds.y"
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
case 36:
#line 477 "dds.y"
{
		     id = new string(yyvsp[0]);
		 ;
    break;}
case 37:
#line 481 "dds.y"
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
case 39:
#line 497 "dds.y"
{
#ifdef WIN32
			std::ostrstream msg;
#else
		     ostrstream msg;
#endif
		     msg << "In the dataset descriptor object:" << endl
			 << "Expected an array subscript." << endl << ends;
		     parse_error((parser_arg *)arg, msg.str());
		     msg.rdbuf()->freeze(0);
		     YYABORT;
		 ;
    break;}
case 40:
#line 511 "dds.y"
{ (*DDS_OBJ(arg)).set_dataset_name(yyvsp[0]); ;
    break;}
case 41:
#line 512 "dds.y"
{ (*DDS_OBJ(arg)).set_dataset_name(yyvsp[0]); ;
    break;}
case 42:
#line 514 "dds.y"
{
#ifdef WIN32
			std::ostrstream msg;
#else
			ostrstream msg;
#endif
		     msg << "Error parsing the dataset name." << endl
			 << "The name may be missing or may contain an illegal character." << endl << ends;
		     parse_error((parser_arg *)arg, msg.str());
		     msg.rdbuf()->freeze(0);
		     YYABORT;
		;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "bison.simple"

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
}
#line 528 "dds.y"


/* 
   This function must be defined. However, use the error reporting code in
   parser-utils.cc.
*/

void 
ddserror(char *)
{
}

/*
  Add the variable pointed to by CURRENT to either the topmost ctor object on
  the stack CTOR or to the dataset variable table TABLE if CTOR is empty.  If
  it exists, the current ctor object is poped off the stack and assigned to
  CURRENT.

  NB: the ctor stack is poped for lists and arrays because they are ctors
  which contain only a single variable. For other ctor types, several
  varaiables may be members and the parse rule (see `declaration' above)
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
