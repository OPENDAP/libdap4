
/*  A Bison parser, made from das.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse dasparse
#define yylex daslex
#define yyerror daserror
#define yylval daslval
#define yychar daschar
#define yydebug dasdebug
#define yynerrs dasnerrs
#define	ATTR	258
#define	ID	259
#define	INT	260
#define	FLOAT	261
#define	STR	262
#define	BYTE	263
#define	INT32	264
#define	FLOAT64	265
#define	STRING	266
#define	URL	267

#line 132 "das.y"


#define YYSTYPE char *

static char rcsid[]={"$Id: das.tab.c,v 1.2 1996/06/04 21:33:56 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config_dap.h"
#include "debug.h"
#include "parser.h"
#include "das.tab.h"
#include "DAS.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

extern int das_line_num;	/* defined in das.lex */

static char name[ID_MAX];	/* holds name in attr_pair rule */
static char type[ID_MAX];	/* holds type in attr_pair rule */
static AttrTablePtr attr_tab_ptr;

void mem_list_report();
int daslex(void);
int daserror(char *s);
#ifdef NEVER
static void not_a_datatype(char *s);
static void save_str(char *dst, char *src);
static int check_byte(char *val);
static int check_int(char *val);
static int check_float(char *val);
static int check_url(char *val);
#endif


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



#define	YYFINAL		75
#define	YYFLAG		-32768
#define	YYNTBASE	17

#define YYTRANSLATE(x) ((unsigned)(x) <= 267 ? yytranslate[x] : 43)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    16,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    15,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    13,     2,    14,     2,     2,     2,     2,     2,
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
     6,     7,     8,     9,    10,    11,    12
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     6,    12,    13,    15,    18,    19,    25,
    27,    28,    30,    33,    34,    35,    42,    43,    44,    51,
    52,    53,    60,    61,    62,    69,    70,    71,    78,    79,
    83,    85,    89,    91,    95,    97,   101,   103,   107,   109,
   113,   115,   117,   119,   121,   123
};

static const short yyrhs[] = {    18,
     0,    17,    18,     0,     0,     3,    19,    13,    20,    14,
     0,     0,    21,     0,    20,    21,     0,     0,     4,    22,
    13,    23,    14,     0,     1,     0,     0,    24,     0,    23,
    24,     0,     0,     0,     8,    25,     4,    26,    36,    15,
     0,     0,     0,     9,    27,     4,    28,    37,    15,     0,
     0,     0,    10,    29,     4,    30,    38,    15,     0,     0,
     0,    11,    31,     4,    32,    39,    15,     0,     0,     0,
    12,    33,     4,    34,    40,    15,     0,     0,     1,    35,
    15,     0,     5,     0,    36,    16,     5,     0,     5,     0,
    37,    16,     5,     0,    42,     0,    38,    16,    42,     0,
    41,     0,    39,    16,    41,     0,     7,     0,    39,    16,
     7,     0,     7,     0,     4,     0,     5,     0,     6,     0,
     6,     0,     5,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   222,   223,   226,   226,   229,   230,   231,   234,   246,   247,
   250,   251,   252,   255,   256,   257,   259,   260,   261,   263,
   264,   265,   267,   268,   269,   271,   272,   273,   275,   276,
   278,   290,   304,   316,   330,   342,   356,   366,   377,   389,
   403,   403,   403,   403,   406,   406
};

static const char * const yytname[] = {   "$","error","$undefined.","ATTR","ID",
"INT","FLOAT","STR","BYTE","INT32","FLOAT64","STRING","URL","'{'","'}'","';'",
"','","attributes","attribute","@1","var_attr_list","var_attr","@2","attr_list",
"attr_tuple","@3","@4","@5","@6","@7","@8","@9","@10","@11","@12","@13","bytes",
"ints","floats","strs","urls","str_or_id","float_or_int",""
};
#endif

static const short yyr1[] = {     0,
    17,    17,    19,    18,    20,    20,    20,    22,    21,    21,
    23,    23,    23,    25,    26,    24,    27,    28,    24,    29,
    30,    24,    31,    32,    24,    33,    34,    24,    35,    24,
    36,    36,    37,    37,    38,    38,    39,    39,    40,    40,
    41,    41,    41,    41,    42,    42
};

static const short yyr2[] = {     0,
     1,     2,     0,     5,     0,     1,     2,     0,     5,     1,
     0,     1,     2,     0,     0,     6,     0,     0,     6,     0,
     0,     6,     0,     0,     6,     0,     0,     6,     0,     3,
     1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     0,
     3,     0,     1,     0,     2,     0,    10,     8,     0,     6,
     0,     4,     7,     0,    29,    14,    17,    20,    23,    26,
     0,    12,     0,     0,     0,     0,     0,     0,     9,    13,
    30,    15,    18,    21,    24,    27,     0,     0,     0,     0,
     0,    31,     0,    33,     0,    46,    45,     0,    35,    42,
    43,    44,    41,     0,    37,    39,     0,     0,    16,     0,
    19,     0,    22,     0,    25,     0,     0,    28,    32,    34,
    36,    38,    40,     0,     0
};

static const short yydefgoto[] = {     2,
     3,     4,     9,    10,    11,    21,    22,    24,    37,    25,
    38,    26,    39,    27,    40,    28,    41,    23,    43,    45,
    48,    54,    58,    55,    49
};

static const short yypact[] = {    13,
-32768,     3,-32768,    35,-32768,     0,-32768,-32768,     1,-32768,
    36,-32768,-32768,    -1,-32768,-32768,-32768,-32768,-32768,-32768,
    11,-32768,     9,    46,    47,    48,    49,    50,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    51,    52,    12,    22,
    26,-32768,    23,-32768,    25,-32768,-32768,    27,-32768,-32768,
-32768,-32768,-32768,    29,-32768,    39,    42,    44,-32768,    55,
-32768,    56,-32768,    12,-32768,    22,    30,-32768,-32768,-32768,
-32768,-32768,    39,    62,-32768
};

static const short yypgoto[] = {-32768,
    61,-32768,-32768,    57,-32768,-32768,    43,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    24,-32768,   -20,     4
};


#define	YYLAST		68


static const short yytable[] = {    15,
     7,     7,    74,     8,     8,     1,    16,    17,    18,    19,
    20,    15,   -11,    -5,    12,     1,    46,    47,    16,    17,
    18,    19,    20,    31,    29,    50,    51,    52,    53,    50,
    51,    52,    56,    50,    51,    52,    73,    59,    60,    61,
    62,    63,    64,    65,    66,    72,    72,     6,    14,    32,
    33,    34,    35,    36,   -41,    42,    44,    67,    68,    69,
    70,    75,     5,    30,    57,    13,     0,    71
};

static const short yycheck[] = {     1,
     1,     1,     0,     4,     4,     3,     8,     9,    10,    11,
    12,     1,    14,    14,    14,     3,     5,     6,     8,     9,
    10,    11,    12,    15,    14,     4,     5,     6,     7,     4,
     5,     6,     7,     4,     5,     6,     7,    15,    16,    15,
    16,    15,    16,    15,    16,    66,    67,    13,    13,     4,
     4,     4,     4,     4,    16,     5,     5,    16,    15,     5,
     5,     0,     2,    21,    41,     9,    -1,    64
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
int yyparse (DAS &table, int &parse_ok);
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
yyparse(DAS &table, int &parse_ok)
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

case 3:
#line 226 "das.y"
{ parse_ok = TRUE; ;
    break;}
case 8:
#line 235 "das.y"
{ 
		    DBG2(mem_list_report()); /* mem_list_report is in */
					     /* libdbnew.a  */
		    attr_tab_ptr = table.get_table(yyvsp[0]);
		    DBG2(mem_list_report());
		    if (!attr_tab_ptr) { /* is this a new var? */
			attr_tab_ptr = table.add_table(yyvsp[0], new AttrTable);
			DBG(cerr << "attr_tab_ptr: " << attr_tab_ptr << endl);
		    }
		    DBG2(mem_list_report());
		;
    break;}
case 10:
#line 247 "das.y"
{ parse_ok = FALSE; ;
    break;}
case 14:
#line 255 "das.y"
{ save_str(type, yyvsp[0], das_line_num); ;
    break;}
case 15:
#line 256 "das.y"
{ save_str(name, yyvsp[0], das_line_num); ;
    break;}
case 17:
#line 259 "das.y"
{ save_str(type, yyvsp[0], das_line_num); ;
    break;}
case 18:
#line 260 "das.y"
{ save_str(name, yyvsp[0], das_line_num); ;
    break;}
case 20:
#line 263 "das.y"
{ save_str(type, yyvsp[0], das_line_num); ;
    break;}
case 21:
#line 264 "das.y"
{ save_str(name, yyvsp[0], das_line_num); ;
    break;}
case 23:
#line 267 "das.y"
{ save_str(type, yyvsp[0], das_line_num); ;
    break;}
case 24:
#line 268 "das.y"
{ save_str(name, yyvsp[0], das_line_num); ;
    break;}
case 26:
#line 271 "das.y"
{ save_str(type, yyvsp[0], das_line_num); ;
    break;}
case 27:
#line 272 "das.y"
{ save_str(name, yyvsp[0], das_line_num); ;
    break;}
case 29:
#line 275 "das.y"
{ parse_ok = FALSE; ;
    break;}
case 31:
#line 279 "das.y"
{
		    DBG(cerr << "Adding byte: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (!check_byte(yyvsp[0], das_line_num)) {
			parse_ok = 0;
		    }
		    else if (!attr_tab_ptr->append_attr(name, type, yyvsp[0])) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 32:
#line 291 "das.y"
{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (!check_byte(yyvsp[0], das_line_num)) {
			parse_ok = 0;
		    }
		    else if (!attr_tab_ptr->append_attr(name, type, yyvsp[0])) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 33:
#line 305 "das.y"
{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (!check_int(yyvsp[0], das_line_num)) {
			parse_ok = 0;
		    }
		    else if (!attr_tab_ptr->append_attr(name, type, yyvsp[0])) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 34:
#line 317 "das.y"
{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (!check_int(yyvsp[0], das_line_num)) {
			parse_ok = 0;
		    }
		    else if (!attr_tab_ptr->append_attr(name, type, yyvsp[0])) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 35:
#line 331 "das.y"
{
		    DBG(cerr << "Adding FLOAT: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (!check_float(yyvsp[0], das_line_num)) {
			parse_ok = 0;
		    }
		    else if (!attr_tab_ptr->append_attr(name, type, yyvsp[0])) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 36:
#line 343 "das.y"
{
		    DBG(cerr << "Adding FLOAT: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (!check_float(yyvsp[0], das_line_num)) {
			parse_ok = 0;
		    }
		    else if (!attr_tab_ptr->append_attr(name, type, yyvsp[0])) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 37:
#line 357 "das.y"
{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    /* assume that a string that parsers is a vaild string */
		    if (attr_tab_ptr->append_attr(name, type, yyvsp[0]) == 0) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 38:
#line 367 "das.y"
{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (attr_tab_ptr->append_attr(name, type, yyvsp[0]) == 0) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 39:
#line 378 "das.y"
{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (!check_url(yyvsp[0], das_line_num)) {
			parse_ok = 0;
		    }
		    else if (!attr_tab_ptr->append_attr(name, type, yyvsp[0])) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
    break;}
case 40:
#line 390 "das.y"
{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< yyvsp[0] << endl);
		    if (!check_url(yyvsp[0], das_line_num)) {
			parse_ok = 0;
		    }
		    else if (!attr_tab_ptr->append_attr(name, type, yyvsp[0])) {
			parse_error("Variable redefinition", das_line_num);
			parse_ok = 0;
		    }
		;
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
#line 409 "das.y"


int 
daserror(char *s)
{
    fprintf(stderr, "%s line: %d\n", s, das_line_num);

    return 1;
}

#ifdef NEVER
static void
save_str(char *dst, char *src)
{
    strncpy(dst, src, ID_MAX);
    dst[ID_MAX-1] = '\0';		/* in case ... */
    if (strlen(src) >= ID_MAX) 
	cerr << "line: " << das_line_num << "`" << src << "' truncated to `"
             << dst << "'" << endl;
}

static void
not_a_datatype(char *s)
{
    fprintf(stderr, "`%s' is not a datatype; line %d\n", s, das_line_num);
}

static int
check_byte(char *val)
{
    int v = atoi(val);

    if (abs(v) > 255) {
	daserror("Not a byte value");
	return FALSE;
    }

    return TRUE;
}

static int
check_int(char *val)
{
    int v = atoi(val);

    if (abs(v) > 2147483647) {	/* don't use the constant from limits.h */
	daserror("Not a 32-bit integer value");
	return FALSE;
    }

    return TRUE;
}

static int
check_float(char *val)
{
    double v = atof(val);

    if (v == 0.0) {
	daserror("Not decodable to a 64-bit float value");
	return FALSE;
    }

    return TRUE;
}

/*
  Maybe someday we will really check the Urls to see if they are valid...
*/

static int
check_url(char *val)
{
    return TRUE;
}
#endif

