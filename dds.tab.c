
/*  A Bison parser, made from dds.y with Bison version GNU Bison version 1.22
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

#line 95 "dds.y"

#ifdef NEVER
#define YYSTYPE char *
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define ID_MAX 256
#endif

static char rcsid[]={"$Id: dds.tab.c,v 1.4 1995/10/23 23:21:07 jimg Exp $"};

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

static BaseTypePtrXPStack ctor;	/* stack for ctor types */
static BaseType *current;
static Part part = nil;		/* Part is defined in BaseType */
static char id[ID_MAX];

int ddslex();
int ddserror(char *s);

void add_entry(DDS &table, BaseTypePtrXPStack &ctor, BaseType **current, 
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



#define	YYFINAL		82
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
     0,     2,     5,    12,    13,    15,    18,    21,    25,    26,
    34,    35,    43,    44,    45,    46,    61,    62,    63,    64,
    79,    81,    83,    85,    87,    89,    91,    93,    95,    97,
    99,   101,   104,   108,   109,   110,   118
};

static const short yyrhs[] = {    28,
     0,    27,    28,     0,     5,    20,    29,    21,    49,    22,
     0,     0,    30,     0,    29,    30,     0,    39,    30,     0,
    44,    45,    22,     0,     0,    40,    20,    29,    21,    31,
    45,    22,     0,     0,    41,    20,    29,    21,    32,    45,
    22,     0,     0,     0,     0,    42,    20,     6,    23,    33,
    29,     7,    23,    34,    29,    21,    35,    45,    22,     0,
     0,     0,     0,    43,    20,     8,    23,    36,    30,     9,
    23,    37,    29,    21,    38,    45,    22,     0,    10,     0,
    12,     0,    11,     0,    13,     0,    14,     0,    15,     0,
    16,     0,    17,     0,    18,     0,    19,     0,     3,     0,
    45,    46,     0,    24,     4,    25,     0,     0,     0,    24,
     3,    47,    26,     4,    48,    25,     0,     3,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   161,   162,   165,   168,   169,   170,   173,   177,   181,   183,
   187,   189,   193,   195,   197,   199,   206,   208,   210,   212,
   220,   223,   226,   229,   232,   235,   236,   237,   238,   239,
   242,   243,   246,   258,   262,   275,   277
};

static const char * const yytname[] = {   "$","error","$illegal.","ID","INTEGER",
"DATASET","INDEPENDENT","DEPENDENT","ARRAY","MAPS","LIST","SEQUENCE","STRUCTURE",
"FUNCTION","GRID","BYTE","INT32","FLOAT64","STRING","URL","'{'","'}'","';'",
"':'","'['","']'","'='","datasets","dataset","declarations","declaration","@1",
"@2","@3","@4","@5","@6","@7","@8","list","structure","sequence","function",
"grid","base_type","var","array_decl","@9","@10","name",""
};
#endif

static const short yyr1[] = {     0,
    27,    27,    28,    29,    29,    29,    30,    30,    31,    30,
    32,    30,    33,    34,    35,    30,    36,    37,    38,    30,
    39,    40,    41,    42,    43,    44,    44,    44,    44,    44,
    45,    45,    46,    47,    48,    46,    49
};

static const short yyr2[] = {     0,
     1,     2,     6,     0,     1,     2,     2,     3,     0,     7,
     0,     7,     0,     0,     0,    14,     0,     0,     0,    14,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     2,     3,     0,     0,     7,     1
};

static const short yydefact[] = {     0,
     0,     0,     1,     4,     2,    21,    23,    22,    24,    25,
    26,    27,    28,    29,    30,     0,     5,     0,     0,     0,
     0,     0,     0,     0,     6,     7,     4,     4,     0,     0,
    31,     0,    37,     0,     0,     0,     0,     0,     8,     0,
    32,     3,     9,    11,    13,    17,    34,     0,     0,     0,
     4,     0,     0,    33,     0,     0,     0,     0,     0,    10,
    12,     0,     0,    35,    14,    18,     0,     4,     4,    36,
     0,     0,    15,    19,     0,     0,     0,     0,    16,    20,
     0,     0
};

static const short yydefgoto[] = {     2,
     3,    16,    17,    49,    50,    51,    68,    75,    52,    69,
    76,    18,    19,    20,    21,    22,    23,    32,    41,    53,
    67,    34
};

static const short yypact[] = {    -4,
     5,    17,-32768,   105,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    -3,-32768,   105,     9,    14,
    18,    20,    43,    44,-32768,-32768,   105,   105,    42,    41,
-32768,    -1,-32768,    28,    57,    69,    29,    30,-32768,    39,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    26,    43,    43,
   105,   105,    51,-32768,     2,     6,    47,    80,    97,-32768,
-32768,    90,   102,-32768,-32768,-32768,   101,   105,   105,-32768,
    81,    93,-32768,-32768,    43,    43,    11,    15,-32768,-32768,
   127,-32768
};

static const short yypgoto[] = {-32768,
   126,   -24,   -16,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -44,-32768,-32768,
-32768,-32768
};


#define	YYLAST		128


static const short yytable[] = {    25,
     1,    26,    35,    36,    55,    56,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    81,    24,    25,    25,
    39,     1,    40,    60,     4,    40,    57,    61,    27,    40,
    77,    78,    79,    28,    40,    58,    80,    29,    40,    30,
    25,    47,    48,    71,    72,    31,    33,    37,    38,    42,
    54,    45,    46,    62,    25,    25,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    59,    43,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    63,    44,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    64,    73,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    65,    74,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    66,    70,    82,     5
};

static const short yycheck[] = {    16,
     5,    18,    27,    28,    49,    50,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,     0,    21,    35,    36,
    22,     5,    24,    22,    20,    24,    51,    22,    20,    24,
    75,    76,    22,    20,    24,    52,    22,    20,    24,    20,
    57,     3,     4,    68,    69,     3,     3,     6,     8,    22,
    25,    23,    23,     7,    71,    72,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    26,    21,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,     9,    21,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
     4,    21,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    23,    21,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    23,    25,     0,     2
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


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
#define YYLEX		yylex(&yylval, &yylloc)
#else
#define YYLEX		yylex(&yylval)
#endif
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
#define __yy_bcopy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (from, to, count)
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
__yy_bcopy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 184 "/usr/local/lib/bison.simple"
int
yyparse(DDS &table)
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
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
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

case 7:
#line 174 "dds.y"
{ if (current->check_semantics())
			add_entry(table, ctor, &current, part); ;
    break;}
case 8:
#line 178 "dds.y"
{ if (current->check_semantics())
			add_entry(table, ctor, &current, part); ;
    break;}
case 9:
#line 182 "dds.y"
{ current = ctor.pop(); ;
    break;}
case 10:
#line 184 "dds.y"
{ if (current->check_semantics())
			add_entry(table, ctor, &current, part); ;
    break;}
case 11:
#line 188 "dds.y"
{ current = ctor.pop(); ;
    break;}
case 12:
#line 190 "dds.y"
{ if (current->check_semantics())
			add_entry(table, ctor, &current, part); ;
    break;}
case 13:
#line 194 "dds.y"
{ part = independent; ;
    break;}
case 14:
#line 196 "dds.y"
{ part = dependent;;
    break;}
case 15:
#line 198 "dds.y"
{ current = ctor.pop(); ;
    break;}
case 16:
#line 200 "dds.y"
{ if (current->check_semantics()) {
			part = nil; 
			add_entry(table, ctor, &current, part); 
		      }
                    ;
    break;}
case 17:
#line 207 "dds.y"
{ part = array; ;
    break;}
case 18:
#line 209 "dds.y"
{ part = maps; ;
    break;}
case 19:
#line 211 "dds.y"
{ current = ctor.pop(); ;
    break;}
case 20:
#line 213 "dds.y"
{ if (current->check_semantics()) {
			part = nil; 
			add_entry(table, ctor, &current, part); 
		      }
                    ;
    break;}
case 21:
#line 220 "dds.y"
{ ctor.push(NewList()); ;
    break;}
case 22:
#line 223 "dds.y"
{ ctor.push(NewStructure()); ;
    break;}
case 23:
#line 226 "dds.y"
{ ctor.push(NewSequence()); ;
    break;}
case 24:
#line 229 "dds.y"
{ ctor.push(NewFunction()); ;
    break;}
case 25:
#line 232 "dds.y"
{ ctor.push(NewGrid()); ;
    break;}
case 26:
#line 235 "dds.y"
{ current = NewByte(); ;
    break;}
case 27:
#line 236 "dds.y"
{ current = NewInt32(); ;
    break;}
case 28:
#line 237 "dds.y"
{ current = NewFloat64(); ;
    break;}
case 29:
#line 238 "dds.y"
{ current = NewStr(); ;
    break;}
case 30:
#line 239 "dds.y"
{ current = NewUrl(); ;
    break;}
case 31:
#line 242 "dds.y"
{ current->set_name(yyvsp[0]); ;
    break;}
case 33:
#line 247 "dds.y"
{ 
		     if (current->type() == array_t) {
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
case 34:
#line 259 "dds.y"
{
		     save_str(id, yyvsp[0], dds_line_num);
		 ;
    break;}
case 35:
#line 263 "dds.y"
{ 
		     if (current->type() == array_t) {
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
case 37:
#line 277 "dds.y"
{ table.set_dataset_name(yyvsp[0]); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 465 "/usr/local/lib/bison.simple"

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
#line 280 "dds.y"


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
add_entry(DDS &table, BaseTypePtrXPStack &ctor, BaseType **current, Part part)
{ 
    if (!ctor.empty()) { /* must be parsing a ctor type */
	ctor.top()->add_var(*current, part);

 	const Type &ctor_type = ctor.top()->type();

	if (ctor_type == list_t || ctor_type == array_t)
	    *current = ctor.pop();
	else
	    return;
    }
    else
	table.add_var(*current);
}
