
/*  A Bison parser, made from dds.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse ddsparse
#define yylex ddslex
#define yyerror ddserror
#define yylval ddslval
#define yychar ddschar
#define yydebug ddsdebug
#define yynerrs ddsnerrs
#define	ID	258
#define	INTEGER	259
#define	DATASET	260
#define	INDEPENDENT	261
#define	DEPENDENT	262
#define	ARRAY	263
#define	MAPS	264
#define	LIST	265
#define	SEQUENCE	266
#define	STRUCTURE	267
#define	FUNCTION	268
#define	GRID	269
#define	BYTE	270
#define	INT32	271
#define	FLOAT64	272
#define	STRING	273
#define	URL	274

#line 120 "dds.y"


#define YYSTYPE char *

static char rcsid[]={"$Id: dds.tab.c,v 1.10 1996/05/29 22:06:28 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream.h>

#include "parser.h"
#include "dds.tab.h"
#include "util.h"
#include "DDS.h"
#include "Array.h"
#include "BTXPStack.h"

extern int dds_line_num;	/* defined in dds.lex */

static BaseTypePtrXPStack *ctor; /* stack for ctor types */
static BaseType *current;
static Part part = nil;		/* Part is defined in BaseType */
static char id[ID_MAX];

int ddslex();
int ddserror(char *s);

void add_entry(DDS &table, BaseTypePtrXPStack **ctor, BaseType **current, 
	       Part p);


#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		81
#define	YYFLAG		-32768
#define	YYNTBASE	27

#define YYTRANSLATE(x) ((unsigned)(x) <= 274 ? yytranslate[x] : 50)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    23,    22,     2,
    26,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    24,     2,    25,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    20,     2,    21,     2,     2,     2,     2,     2,
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
    16,    17,    18,    19
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     7,     8,    10,    13,    16,    18,    22,    23,    31,
    32,    40,    41,    42,    43,    58,    59,    60,    61,    76,
    78,    80,    82,    84,    86,    88,    90,    92,    94,    96,
    98,   101,   105,   106,   107,   115
};

static const short yyrhs[] = {     5,
    20,    28,    21,    49,    22,     0,     0,    29,     0,    28,
    29,     0,    39,    30,     0,    30,     0,    44,    45,    22,
     0,     0,    40,    20,    28,    21,    31,    45,    22,     0,
     0,    41,    20,    28,    21,    32,    45,    22,     0,     0,
     0,     0,    42,    20,     6,    23,    33,    28,     7,    23,
    34,    28,    21,    35,    45,    22,     0,     0,     0,     0,
    43,    20,     8,    23,    36,    29,     9,    23,    37,    28,
    21,    38,    45,    22,     0,    10,     0,    12,     0,    11,
     0,    13,     0,    14,     0,    15,     0,    16,     0,    17,
     0,    18,     0,    19,     0,     3,     0,    45,    46,     0,
    24,     4,    25,     0,     0,     0,    24,     3,    47,    26,
     4,    48,    25,     0,     3,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   186,   189,   190,   191,   194,   199,   206,   212,   216,   222,
   226,   232,   234,   236,   240,   248,   250,   252,   256,   265,
   273,   281,   289,   297,   305,   306,   307,   308,   309,   312,
   313,   316,   328,   332,   345,   347
};

static const char * const yytname[] = {   "$","error","$undefined.","ID","INTEGER",
"DATASET","INDEPENDENT","DEPENDENT","ARRAY","MAPS","LIST","SEQUENCE","STRUCTURE",
"FUNCTION","GRID","BYTE","INT32","FLOAT64","STRING","URL","'{'","'}'","';'",
"':'","'['","']'","'='","dataset","declarations","declaration","non_list_decl",
"@1","@2","@3","@4","@5","@6","@7","@8","list","structure","sequence","function",
"grid","base_type","var","array_decl","@9","@10","name",""
};
#endif

static const short yyr1[] = {     0,
    27,    28,    28,    28,    29,    29,    30,    31,    30,    32,
    30,    33,    34,    35,    30,    36,    37,    38,    30,    39,
    40,    41,    42,    43,    44,    44,    44,    44,    44,    45,
    45,    46,    47,    48,    46,    49
};

static const short yyr2[] = {     0,
     6,     0,     1,     2,     2,     1,     3,     0,     7,     0,
     7,     0,     0,     0,    14,     0,     0,     0,    14,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     2,     3,     0,     0,     7,     1
};

static const short yydefact[] = {     0,
     0,     2,    20,    22,    21,    23,    24,    25,    26,    27,
    28,    29,     0,     3,     6,     0,     0,     0,     0,     0,
     0,     0,     4,     5,     2,     2,     0,     0,    30,     0,
    36,     0,     0,     0,     0,     0,     7,     0,    31,     1,
     8,    10,    12,    16,    33,     0,     0,     0,     2,     0,
     0,    32,     0,     0,     0,     0,     0,     9,    11,     0,
     0,    34,    13,    17,     0,     2,     2,    35,     0,     0,
    14,    18,     0,     0,     0,     0,    15,    19,     0,     0,
     0
};

static const short yydefgoto[] = {    79,
    13,    14,    15,    47,    48,    49,    66,    73,    50,    67,
    74,    16,    17,    18,    19,    20,    21,    30,    39,    51,
    65,    32
};

static const short yypact[] = {    -4,
     8,    35,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    48,-32768,-32768,   107,    16,    18,    19,    20,
    38,    52,-32768,-32768,    35,    35,    13,   108,-32768,     0,
-32768,    46,    60,    72,    57,    69,-32768,     4,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    79,    38,    38,    35,    35,
   101,-32768,     1,     5,    -1,   119,   125,-32768,-32768,   109,
   110,-32768,-32768,-32768,   105,    35,    35,-32768,    84,    96,
-32768,-32768,    38,    38,    10,    11,-32768,-32768,   131,   134,
-32768
};

static const short yypgoto[] = {-32768,
   -23,   -13,   120,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -43,-32768,-32768,
-32768,-32768
};


#define	YYLAST		136


static const short yytable[] = {    23,
     1,    33,    34,    53,    54,    60,    45,    46,     3,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    35,    23,
    23,    37,    58,    38,    38,    55,    59,     2,    38,    75,
    76,    77,    78,    38,    38,    25,    56,    26,    27,    28,
    29,    23,    69,    70,     3,     4,     5,     6,     7,     8,
     9,    10,    11,    12,    31,    23,    23,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    40,    22,     3,
     4,     5,     6,     7,     8,     9,    10,    11,    12,    43,
    41,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    44,    42,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    52,    71,     3,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    36,    72,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    57,    61,    62,    68,
    80,    63,    64,    81,     0,    24
};

static const short yycheck[] = {    13,
     5,    25,    26,    47,    48,     7,     3,     4,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,     6,    33,
    34,    22,    22,    24,    24,    49,    22,    20,    24,    73,
    74,    22,    22,    24,    24,    20,    50,    20,    20,    20,
     3,    55,    66,    67,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,     3,    69,    70,    10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    22,    21,    10,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    23,
    21,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    23,    21,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    25,    21,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,     8,    21,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    26,     9,     4,    25,
     0,    23,    23,     0,    -1,    16
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"

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
int yyparse (DDS &table);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
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
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(DDS &table)
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
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
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
#line 186 "dds.y"
{ YYACCEPT; ;
    break;}
case 5:
#line 195 "dds.y"
{ 
		    if (current->check_semantics())
			add_entry(table, &ctor, &current, part); 
		;
    break;}
case 7:
#line 207 "dds.y"
{ 
		    if (current->check_semantics())
			add_entry(table, &ctor, &current, part); 
		;
    break;}
case 8:
#line 213 "dds.y"
{ 
		    current = ctor->pop(); 
		;
    break;}
case 9:
#line 217 "dds.y"
{ 
		    if (current->check_semantics())
			add_entry(table, &ctor, &current, part); 
		;
    break;}
case 10:
#line 223 "dds.y"
{ 
		    current = ctor->pop(); 
		;
    break;}
case 11:
#line 227 "dds.y"
{ 
		    if (current->check_semantics())
			add_entry(table, &ctor, &current, part); 
		;
    break;}
case 12:
#line 233 "dds.y"
{ part = independent; ;
    break;}
case 13:
#line 235 "dds.y"
{ part = dependent;;
    break;}
case 14:
#line 237 "dds.y"
{ 
		    current = ctor->pop(); 
		;
    break;}
case 15:
#line 241 "dds.y"
{ 
		    if (current->check_semantics()) {
			part = nil; 
			add_entry(table, &ctor, &current, part); 
		    }
		;
    break;}
case 16:
#line 249 "dds.y"
{ part = array; ;
    break;}
case 17:
#line 251 "dds.y"
{ part = maps; ;
    break;}
case 18:
#line 253 "dds.y"
{
		    current = ctor->pop(); 
		;
    break;}
case 19:
#line 257 "dds.y"
{
		    if (current->check_semantics()) {
			part = nil; 
			add_entry(table, &ctor, &current, part); 
		    }
		;
    break;}
case 20:
#line 266 "dds.y"
{ 
		    if (!ctor) 
			ctor = new BaseTypePtrXPStack;
		    ctor->push(NewList()); 
		;
    break;}
case 21:
#line 274 "dds.y"
{ 
		    if (!ctor)
	                ctor = new BaseTypePtrXPStack;
		    ctor->push(NewStructure()); 
		;
    break;}
case 22:
#line 282 "dds.y"
{ 
		    if (!ctor)
			ctor = new BaseTypePtrXPStack;
		    ctor->push(NewSequence()); 
		;
    break;}
case 23:
#line 290 "dds.y"
{ 
		    if (!ctor)
			ctor = new BaseTypePtrXPStack;
		    ctor->push(NewFunction()); 
		;
    break;}
case 24:
#line 298 "dds.y"
{ 
		    if (!ctor)
			ctor = new BaseTypePtrXPStack;
		    ctor->push(NewGrid()); 
		;
    break;}
case 25:
#line 305 "dds.y"
{ current = NewByte(); ;
    break;}
case 26:
#line 306 "dds.y"
{ current = NewInt32(); ;
    break;}
case 27:
#line 307 "dds.y"
{ current = NewFloat64(); ;
    break;}
case 28:
#line 308 "dds.y"
{ current = NewStr(); ;
    break;}
case 29:
#line 309 "dds.y"
{ current = NewUrl(); ;
    break;}
case 30:
#line 312 "dds.y"
{ current->set_name(yyvsp[0]); ;
    break;}
case 32:
#line 317 "dds.y"
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
case 33:
#line 329 "dds.y"
{
		     save_str(id, yyvsp[0], dds_line_num);
		 ;
    break;}
case 34:
#line 333 "dds.y"
{ 
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi(yyvsp[0]), id);
		     }
		     else {
			 Array *a = NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi(yyvsp[0]), id);
			 current = a;
		     }
		 ;
    break;}
case 36:
#line 347 "dds.y"
{ table.set_dataset_name(yyvsp[0]); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "/usr/local/share/bison.simple"

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
#line 350 "dds.y"


int 
ddserror(char *s)
{
    fprintf(stderr, "%s line: %d\n", s, dds_line_num);
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
add_entry(DDS &table, BaseTypePtrXPStack **ctor, BaseType **current, Part part)
{ 
    if (!*ctor)
	*ctor = new BaseTypePtrXPStack;

    if (!(*ctor)->empty()) { /* must be parsing a ctor type */
	(*ctor)->top()->add_var(*current, part);

 	const Type &ctor_type = (*ctor)->top()->type();

	if (ctor_type == dods_list_c || ctor_type == dods_array_c)
	    *current = (*ctor)->pop();
	else
	    return;
    }
    else
	table.add_var(*current);
}
