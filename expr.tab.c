
/*  A Bison parser, made from expr.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse exprparse
#define yylex exprlex
#define yyerror exprerror
#define yylval exprlval
#define yychar exprchar
#define yydebug exprdebug
#define yynerrs exprnerrs
#define	INT	258
#define	FLOAT	259
#define	STR	260
#define	ID	261
#define	FIELD	262
#define	EQUAL	263
#define	NOT_EQUAL	264
#define	GREATER	265
#define	GREATER_EQL	266
#define	LESS	267
#define	LESS_EQL	268
#define	REGEXP	269

#line 77 "expr.y"


static char rcsid[]={"$Id: expr.tab.c,v 1.3 1996/06/18 23:56:06 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <String.h>
#include <SLList.h>

#include "debug.h"

#include "DDS.h"

#include "Connect.h"

#include "BaseType.h"
#include "Array.h"
#include "List.h"
#include "Structure.h"
#include "Sequence.h"
#include "Function.h"
#include "Grid.h"

#include "config_dap.h"
#include "util.h"
#include "parser.h"
#include "expr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

int exprlex(void);		/* the scanner; see expr.lex */

int exprerror(const char *s);	/* easier to overload than use stdarg... */
int exprerror(const char *s, const char *s2);

int_list *make_array_index(value &i1, value &i2, value &i3);
int_list_list *make_array_indices(int_list *index);
int_list_list *append_array_index(int_list_list *indices, int_list *index);
void delete_array_indices(int_list_list *indices);
bool process_array_indices(BaseType *variable, int_list_list *indices); 
bool process_grid_indices(BaseType *variable, int_list_list *indices); 

bool is_array_t(BaseType *variable);
bool is_grid_t(BaseType *variable);

rvalue_list *make_rvalue_list(DDS &table, rvalue *rv);
rvalue_list *append_rvalue_list(DDS &table, rvalue_list *rvals, rvalue *rv);

BaseType *make_variable(DDS &table, const value &val);

rvalue *dereference_variable(DDS &table, rvalue *rv);
rvalue *dereference_url(DDS &table, value &val);

bool_func get_function(const DDS &table, const char *name);
btp_func get_btp_function(const DDS &table, const char *name);

/* 
  The parser receives a DDS &table as a formal argument. TABLE is the DDS
  of the entire dataset; each variable in the constraint expression must be
  in this DDS and their data types must match the use in the constraint
  expression.
*/


#line 149 "expr.y"
typedef union {
    bool boolean;
    int op;
    char id[ID_MAX];

    value val;

    bool_func b_func;
    btp_func bt_func;

    int_list *int_l_ptr;
    int_list_list *int_ll_ptr;
    
    rvalue *rval_ptr;
    rvalue_list *r_val_l_ptr;
} YYSTYPE;

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

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		68
#define	YYFLAG		-32768
#define	YYNTBASE	25

#define YYTRANSLATE(x) ((unsigned)(x) <= 269 ? yytranslate[x] : 38)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    15,     2,    16,
    17,    21,     2,    18,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    23,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    22,     2,    24,     2,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     3,     4,     8,    12,    17,    19,    21,    23,
    27,    31,    35,    37,    41,    47,    51,    56,    58,    60,
    63,    66,    71,    73,    77,    79,    81,    83,    85,    87,
    90,    93,    95,    98,   104,   112,   114,   116,   118,   120,
   122,   124
};

static const short yyrhs[] = {    -1,
    27,     0,     0,    15,    26,    28,     0,    27,    15,    28,
     0,     6,    16,    31,    17,     0,     6,     0,     7,     0,
    34,     0,    27,    18,     6,     0,    27,    18,     7,     0,
    27,    18,    34,     0,    29,     0,    28,    15,    29,     0,
    30,    37,    19,    31,    20,     0,    30,    37,    30,     0,
     6,    16,    31,    17,     0,    32,     0,    33,     0,    21,
    32,     0,    21,     5,     0,     6,    16,    31,    17,     0,
    30,     0,    31,    18,    30,     0,     6,     0,     7,     0,
     3,     0,     4,     0,     5,     0,     6,    35,     0,     7,
    35,     0,    36,     0,    35,    36,     0,    22,     3,    23,
     3,    24,     0,    22,     3,    23,     3,    23,     3,    24,
     0,     8,     0,     9,     0,    10,     0,    11,     0,    12,
     0,    13,     0,    14,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   190,   196,   198,   199,   202,   206,   220,   232,   242,   246,
   259,   269,   275,   276,   282,   288,   297,   311,   312,   313,
   320,   326,   337,   341,   347,   354,   363,   368,   373,   381,
   397,   415,   419,   425,   432,   438,   439,   440,   441,   442,
   443,   444
};

static const char * const yytname[] = {   "$","error","$undefined.","INT","FLOAT",
"STR","ID","FIELD","EQUAL","NOT_EQUAL","GREATER","GREATER_EQL","LESS","LESS_EQL",
"REGEXP","'&'","'('","')'","','","'{'","'}'","'*'","'['","':'","']'","constraint_expr",
"@1","projection","selection","clause","r_value","r_value_list","identifier",
"constant","array_sel","array_indices","array_index","rel_op",""
};
#endif

static const short yyr1[] = {     0,
    25,    25,    26,    25,    25,    25,    27,    27,    27,    27,
    27,    27,    28,    28,    29,    29,    29,    30,    30,    30,
    30,    30,    31,    31,    32,    32,    33,    33,    33,    34,
    34,    35,    35,    36,    36,    37,    37,    37,    37,    37,
    37,    37
};

static const short yyr2[] = {     0,
     0,     1,     0,     3,     3,     4,     1,     1,     1,     3,
     3,     3,     1,     3,     5,     3,     4,     1,     1,     2,
     2,     4,     1,     3,     1,     1,     1,     1,     1,     2,
     2,     1,     2,     5,     7,     1,     1,     1,     1,     1,
     1,     1
};

static const short yydefact[] = {     1,
     7,     8,     3,     2,     9,     0,     0,    30,    32,    31,
     0,     0,     0,    27,    28,    29,    25,    26,     0,    23,
     0,    18,    19,     0,    33,    25,     4,    13,     0,     5,
    10,    11,    12,     0,    21,    25,    20,     6,     0,     0,
     0,     0,    36,    37,    38,    39,    40,    41,    42,     0,
     0,    24,     0,     0,    14,     0,    16,    22,     0,    34,
    22,     0,     0,    15,    35,     0,     0,     0
};

static const short yydefgoto[] = {    66,
    11,     4,    27,    28,    20,    21,    22,    23,     5,     8,
     9,    50
};

static const short yypact[] = {    11,
     5,    -6,-32768,    34,-32768,     8,    16,    -6,-32768,    -6,
    30,    30,    53,-32768,-32768,-32768,     7,-32768,    48,-32768,
    44,-32768,-32768,    15,-32768,    14,    54,-32768,    32,    54,
    -6,    -6,-32768,     8,-32768,-32768,-32768,-32768,     8,    67,
     8,    30,-32768,-32768,-32768,-32768,-32768,-32768,-32768,     1,
    46,-32768,    42,    50,-32768,     8,-32768,-32768,    68,-32768,
     9,    38,    23,-32768,-32768,    57,    72,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    61,    33,   -11,   -31,    55,-32768,    63,     0,
    40,-32768
};


#define	YYLAST		76


static const short yytable[] = {    29,
    29,    10,    51,    14,    15,    16,    17,    18,   -17,    54,
    14,    15,    16,    17,    18,     7,     1,     2,    24,    56,
     6,    19,    34,   -17,    62,     3,     7,    52,    19,    41,
    29,    10,    14,    15,    16,    26,    18,    40,    57,    43,
    44,    45,    46,    47,    48,    49,    65,    25,    12,    25,
    19,    13,    35,    36,    18,    39,    67,    64,    31,    32,
    38,    39,    58,    39,    59,    60,    61,    39,    42,    53,
    63,    68,    30,    37,    55,    33
};

static const short yycheck[] = {    11,
    12,     2,    34,     3,     4,     5,     6,     7,     0,    41,
     3,     4,     5,     6,     7,    22,     6,     7,     3,    19,
    16,    21,    16,    15,    56,    15,    22,    39,    21,    16,
    42,    32,     3,     4,     5,     6,     7,    23,    50,     8,
     9,    10,    11,    12,    13,    14,    24,     8,    15,    10,
    21,    18,     5,     6,     7,    18,     0,    20,     6,     7,
    17,    18,    17,    18,    23,    24,    17,    18,    15,     3,
     3,     0,    12,    19,    42,    13
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
#line 191 "expr.y"
{
		     table.mark_all(true);
		     yyval.boolean = true;
		 ;
    break;}
case 3:
#line 198 "expr.y"
{ table.mark_all(true); ;
    break;}
case 4:
#line 199 "expr.y"
{ 
		       yyval.boolean = yyvsp[0].boolean;
		   ;
    break;}
case 5:
#line 203 "expr.y"
{
		       yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		   ;
    break;}
case 6:
#line 207 "expr.y"
{
		       btp_func func = get_btp_function(table, yyvsp[-3].id);
		       if (!func) {
			   exprerror("Not a BaseType pointer function", yyvsp[-3].id);
			   yyval.boolean = false;
		       }
		       else {
			   table.append_clause(func, yyvsp[-1].r_val_l_ptr);
			   yyval.boolean = true;
		       }
		   ;
    break;}
case 7:
#line 221 "expr.y"
{ 
		      BaseType *var = table.var(yyvsp[0].id);
		      if (var) {
			  var->set_send_p(true); // add to projection
			  yyval.boolean = true;
		      }
		      else {
			  yyval.boolean = false;
			  exprerror("No such identifier in dataset", yyvsp[0].id);
		      }
		  ;
    break;}
case 8:
#line 233 "expr.y"
{ 
		      BaseType *var = table.var(yyvsp[0].id);
		      if (var)
			  yyval.boolean = table.mark(yyvsp[0].id, true); // must add parents, too
		      else {
			  yyval.boolean = false;
			  exprerror("No such field in dataset", yyvsp[0].id);
		      }
		  ;
    break;}
case 9:
#line 243 "expr.y"
{
		      yyval.boolean = yyvsp[0].boolean;
		  ;
    break;}
case 10:
#line 247 "expr.y"
{ 
		      BaseType *var = table.var(yyvsp[0].id);
		      if (var) {
			  var->set_send_p(true);
			  yyval.boolean = true;
		      }
		      else {
			  yyval.boolean = false;
			  exprerror("No such identifier in dataset", yyvsp[0].id);
		      }

		  ;
    break;}
case 11:
#line 260 "expr.y"
{ 
		      BaseType *var = table.var(yyvsp[0].id);
		      if (var)
			  yyval.boolean = table.mark(yyvsp[0].id, true);
		      else {
			  yyval.boolean = false;
			  exprerror("No such field in dataset", yyvsp[0].id);
		      }
		  ;
    break;}
case 12:
#line 270 "expr.y"
{
		      yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		  ;
    break;}
case 14:
#line 277 "expr.y"
{
		      yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		  ;
    break;}
case 15:
#line 283 "expr.y"
{
		      assert((yyvsp[-4].rval_ptr));
		      table.append_clause(yyvsp[-3].op, yyvsp[-4].rval_ptr, yyvsp[-1].r_val_l_ptr);
		      yyval.boolean = true;
		  ;
    break;}
case 16:
#line 289 "expr.y"
{
		      assert((yyvsp[-2].rval_ptr));

		      rvalue_list *rv = new rvalue_list;
		      rv->append(yyvsp[0].rval_ptr);
		      table.append_clause(yyvsp[-1].op, yyvsp[-2].rval_ptr, rv);
		      yyval.boolean = true;
		  ;
    break;}
case 17:
#line 298 "expr.y"
{
		      bool_func b_func = get_function(table, yyvsp[-3].id);
		      if (!b_func) {
  			  exprerror("Not a boolean function", yyvsp[-3].id);
			  yyval.boolean = false;
		      }
		      else {
			  table.append_clause(b_func, yyvsp[-1].r_val_l_ptr);
			  yyval.boolean = true;
		      }
		  ;
    break;}
case 20:
#line 314 "expr.y"
{
		      yyval.rval_ptr = dereference_variable(table, yyvsp[0].rval_ptr);
		      if (!yyval.rval_ptr)
			  exprerror("Could not dereference variable", 
				    (yyvsp[0].rval_ptr)->value->name());
		  ;
    break;}
case 21:
#line 321 "expr.y"
{
		      yyval.rval_ptr = dereference_url(table, yyvsp[0].val);
		      if (!yyval.rval_ptr)
			  exprerror("Could not dereference URL", *(yyvsp[0].val).v.s);
		  ;
    break;}
case 22:
#line 327 "expr.y"
{
		      btp_func bt_func = get_btp_function(table, yyvsp[-3].id);
		      if (!bt_func) {
  			  exprerror("Not a BaseType * function", yyvsp[-3].id);
			  yyval.rval_ptr = 0;
		      }
		      yyval.rval_ptr = new rvalue(new func_rvalue(bt_func, yyvsp[-1].r_val_l_ptr));
		  ;
    break;}
case 23:
#line 338 "expr.y"
{
		    yyval.r_val_l_ptr = make_rvalue_list(table, yyvsp[0].rval_ptr);
		;
    break;}
case 24:
#line 342 "expr.y"
{
		    yyval.r_val_l_ptr = append_rvalue_list(table, yyvsp[-2].r_val_l_ptr, yyvsp[0].rval_ptr);
		;
    break;}
case 25:
#line 348 "expr.y"
{ 
		      BaseType *btp = table.var(yyvsp[0].id);
		      if (!btp)
			  exprerror("No such identifier in dataset", yyvsp[0].id);
		      yyval.rval_ptr = new rvalue(btp);
		  ;
    break;}
case 26:
#line 355 "expr.y"
{ 
		      BaseType *btp = table.var(yyvsp[0].id);
		      if (!btp)
			  exprerror("No such field in dataset", yyvsp[0].id);
		      yyval.rval_ptr = new rvalue(btp);
		  ;
    break;}
case 27:
#line 364 "expr.y"
{
		      BaseType *btp = make_variable(table, yyvsp[0].val);
		      yyval.rval_ptr = new rvalue(btp);
		  ;
    break;}
case 28:
#line 369 "expr.y"
{
		      BaseType *btp = make_variable(table, yyvsp[0].val);
		      yyval.rval_ptr = new rvalue(btp);
		  ;
    break;}
case 29:
#line 374 "expr.y"
{ 
		      BaseType *btp = make_variable(table, yyvsp[0].val); 
		      yyval.rval_ptr = new rvalue(btp);
		  ;
    break;}
case 30:
#line 382 "expr.y"
{
		      BaseType *var = table.var(yyvsp[-1].id);
		      if (var && is_array_t(var)) {
			  var->set_send_p(true);
			  yyval.boolean = process_array_indices(var, yyvsp[0].int_ll_ptr);
			  delete_array_indices(yyvsp[0].int_ll_ptr);
		      }
		      else if (var && is_grid_t(var)) {
			  var->set_send_p(true);
			  yyval.boolean = process_grid_indices(var, yyvsp[0].int_ll_ptr);
			  delete_array_indices(yyvsp[0].int_ll_ptr);
		      }
		      else
			  yyval.boolean = false;
		  ;
    break;}
case 31:
#line 398 "expr.y"
{
		      BaseType *var = table.var(yyvsp[-1].id);
		      if (var && is_array_t(var)) {
			  yyval.boolean = table.mark(yyvsp[-1].id, true) // set all the parents, too
			      && process_array_indices(var, yyvsp[0].int_ll_ptr);
			  delete_array_indices(yyvsp[0].int_ll_ptr);
		      }
		      else if (var && is_grid_t(var)) {
			  yyval.boolean = table.mark(yyvsp[-1].id, true) // set all the parents, too
			       && process_grid_indices(var, yyvsp[0].int_ll_ptr);
			  delete_array_indices(yyvsp[0].int_ll_ptr);
		      }
		      else
			  yyval.boolean = false;
		  ;
    break;}
case 32:
#line 416 "expr.y"
{
		      yyval.int_ll_ptr = make_array_indices(yyvsp[0].int_l_ptr);
		  ;
    break;}
case 33:
#line 420 "expr.y"
{
		      yyval.int_ll_ptr = append_array_index(yyvsp[-1].int_ll_ptr, yyvsp[0].int_l_ptr);
		  ;
    break;}
case 34:
#line 426 "expr.y"
{
		      value val;
		      val.type = dods_int32_c;
		      val.v.i = 1;
		      yyval.int_l_ptr = make_array_index(yyvsp[-3].val, val, yyvsp[-1].val);
		  ;
    break;}
case 35:
#line 433 "expr.y"
{
		      yyval.int_l_ptr = make_array_index(yyvsp[-5].val, yyvsp[-3].val, yyvsp[-1].val);
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
#line 447 "expr.y"


int 
exprerror(const char *s)
{
    cerr << "Parse error: " << s << endl;
}

int 
exprerror(const char *s, const char *s2)
{
    cerr << "Parse error: " << s << ": " << s2 << endl;
}

// Given three values (I1, I2, I3), all of which must be integers, build an
// int_list which contains those values.
//
// Returns: A pointer to an int_list of three integers or NULL if any of the
// values are not integers.

int_list *
make_array_index(value &i1, value &i2, value &i3)
{
    int_list *index = new int_list;

    if (i1.type != dods_int32_c
	|| i2.type != dods_int32_c
	|| i3.type != dods_int32_c)
	return (void *)0;

    index->append((int)i1.v.i);
    index->append((int)i2.v.i);
    index->append((int)i3.v.i);

    DBG(Pix dp;\
	cout << "index: ";\
	for (dp = index->first(); dp; index->next(dp))\
	cout << (*index)(dp) << " ";\
	cout << endl);

    return index;
}

int_list_list *
make_array_indices(int_list *index)
{
    int_list_list *indices = new int_list_list;

    DBG(Pix dp;\
	cout << "index: ";\
	for (dp = index->first(); dp; index->next(dp))\
	cout << (*index)(dp) << " ";\
	cout << endl);

    indices->append(index);

    return indices;
}

int_list_list *
append_array_index(int_list_list *indices, int_list *index)
{
    indices->append(index);

    return indices;
}

// Delete an array indices list. 

void
delete_array_indices(int_list_list *indices)
{
    for (Pix p = indices->first(); p; indices->next(p))
	delete (*indices)(p);

    delete indices;
}

bool
is_array_t(BaseType *variable)
{
    if (variable->type() != dods_array_c) {
#if 0
	cerr << "Variable " << variable->name() << " is not an array." << endl;
#endif
	return false;
    }
    else
	return true;
}

bool
is_grid_t(BaseType *variable)
{
    if (variable->type() != dods_grid_c) {
#if 0
	cerr << "Variable " << variable->name() << " is not an grid." << endl;
#endif
	return false;
    }
    else
	return true;
}

bool
process_array_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    Array *a = (Array *)variable; // replace with dynamic cast

    DBG(cerr << "Before clear_costraint:" << endl);
    DBG(a->print_decl(cerr, "", true, false, true));

    a->clear_constraint();	// each projection erases the previous one
    
    DBG(cerr << "After clear_costraint:" << endl);
    DBG(a->print_decl(cerr, "", true, false, true));

    Pix p, r;
    for (p = indices->first(), r = a->first_dim(); 
	 p && r; 
	 indices->next(p), a->next_dim(r)) {

	int_list *index = (*indices)(p);

	Pix q = index->first(); 
	int start = (*index)(q);

	index->next(q);
	int stride = (*index)(q);
	
	index->next(q);

	int stop = (*index)(q);

	index->next(q);
	if (q) {
	    cerr << "Too many values in index list for " << a->name() << "." 
		 << endl;
	    status = false;
	    goto exit;
	}
	
	a->add_constraint(r, start, stride, stop);
	DBG(cerr << "Set Constraint: " << a->dimension_size(r, true) << endl);
    }

    DBG(cerr << "After processing loop:" << endl);
    DBG(a->print_decl(cerr, "", true, false, true));

    DBG(Pix dp;\
	cout << "Array Constraint: ";\
	for (dp = a->first_dim(); dp; a->next_dim(dp))\
	    cout << a->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p && !r) {
	cerr << "Too many indices in constraint for " << a->name() << "." 
	     << endl;
	status= false;
    }

exit:
#if 0
    delete_array_indices(indices);
#endif
    return status;
}

bool
process_grid_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    Grid *g = (Grid *)variable; // Replace with dynamic cast.

    // First do the constraints on the ARRAY in the grid.
    status = process_array_indices(g->array_var(), indices);
    if (!status)
	goto exit;

    // Now process the maps.
    Pix p, r;

    // Supress all maps by default.
    for (r = g->first_map_var(); r; g->next_map_var(r))
	g->map_var(r)->set_send_p(false);

    // Add specified maps to the current projection.
    for (p = indices->first(), r = g->first_map_var(); 
	 p && r; 
	 indices->next(p), g->next_map_var(r)) {

	int_list *index = (*indices)(p);

	Pix q = index->first(); 
	int start = (*index)(q);

	index->next(q);
	int stride = (*index)(q);
	
	index->next(q);

	int stop = (*index)(q);

	Array *a = (Array *)g->map_var(r);
	a->set_send_p(true);
	a->clear_constraint();

	index->next(q);
	if (q) {
	    cerr << "Too many values in index list for " << a->name() << "." 
		 << endl;
	    status = false;
	    goto exit;
	}

	a->add_constraint(a->first_dim(), start, stride, stop);
	DBG(cerr << "Set Constraint: " \
	    << a->dimension_size(a->first_dim(), true) << endl);
    }

    DBG(Pix dp;\
	cout << "Grid Constraint: ";\
	for (dp = ((Array *)g->array_var())->first_dim(); dp; \
		 ((Array *)g->array_var())->next_dim(dp))\
	   cout << ((Array *)g->array_var())->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p && !r) {
	cerr << "Too many indices in constraint for " 
	     << g->map_var(r)->name() << "." << endl;
	status= false;
    }

exit:
#if 0
    delete_array_indices(indices);
#endif
    return status;
}

// Create a list of r values and add VAL to the list.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
make_rvalue_list(DDS &table, rvalue *rv)
{
    rvalue_list *rvals = new rvalue_list;

    return append_rvalue_list(table, rvals, rv);
}

// Given a rvalue_list pointer RVALS and a value pointer VAL, make a variable
// to hold VAL and append that variable to the list RVALS.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
append_rvalue_list(DDS &table, rvalue_list *rvals, rvalue *rv)
{
    rvals->append(rv);

    return rvals;
}

// Given a string which is a URL, dereference it and return the data it
// points to.

static rvalue *
dereference_string(DDS &table, String &s)
{
    String url = s.before("?");	// strip off CE
    String ce = s.after("?");	// yes, get the CE

    Connect c = Connect(url);	// make the virtual connection

    // the initial URL must be a complete reference to data; thus no
    // additional CE is needed. 
    DDS d = c.request_data(ce, false, false); 

    // By definition, the DDS `D' can have only one variable, so make sure
    // that is true.
    if (d.num_var() != 1) {
	cerr << 
	    "Too many variables in URL; use only single variable projections"
	     << endl;
	return 0;
    }

    // OK, we're here. The first_var() must be the only var, return it bound
    // up in an rvalue struct. NB: the *object* must be copied since the one
    // within DDS `D' will be deleted by D's dtor.
    BaseType *btp = d.var(d.first_var())->ptr_duplicate();
    rvalue *rv = new rvalue(btp);

    return rv;
}

rvalue *
dereference_url(DDS &table, value &val)
{
    if (val.type != dods_str_c)
	return 0;

    return dereference_string(table, *val.v.s);
}

// Given a rvalue, get the BaseType that encapsulates its value, make sure it
// is a string and, if all that works, dereference it.

rvalue *
dereference_variable(DDS &table, rvalue *rv)
{
    BaseType *btp = rv->bvalue("dummy"); // the value will be read over the net
    if (btp->type() != dods_str_c && btp->type() != dods_url_c) {
	cerr << "Variable: " << btp->name() 
	    << " must be either a string or a url" 
	    << endl;
	return 0;
    }

    String s;
    String  *sp = &s;
    btp->buf2val((void **)&sp);
    
    return dereference_string(table, s);
}

// Given a value, wrap it up in a BaseType and return a pointer to the same.

BaseType *
make_variable(DDS &table, const value &val)
{
    BaseType *var;
    switch (val.type) {
      case dods_int32_c: {
	var = (BaseType *)NewInt32("dummy");
	var->val2buf((void *)&val.v.i);
	break;
      }

      case dods_float64_c: {
	var = (BaseType *)NewFloat64("dummy");
	var->val2buf((void *)&val.v.f);
	break;
      }

      case dods_str_c: {
	var = (BaseType *)NewStr("dummy");
	var->val2buf((void *)val.v.s);
	break;
      }

      default:
	cerr << "Unknow type constant value" << endl;
	var = (BaseType *)0;
	return var;
    }

    var->set_read_p(true);	// ...so the evaluator will know it has data
    table.append_constant(var);

    return var;
}

// Given a string (passed in VAL), consult the DDS CE function lookup table
// to see if a function by that name exists. 
// NB: function arguments are type-checked at run-time.
//
// Returns: A poitner to the function or NULL if not such function exists.

bool_func
get_function(const DDS &table, const char *name)
{
    bool_func f;

    if (table.find_function(name, &f))
	return f;
    else
	return 0;
}

btp_func
get_btp_function(const DDS &table, const char *name)
{
    btp_func f;

    if (table.find_function(name, &f))
	return f;
    else
	return 0;
}
