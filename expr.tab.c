
/*  A Bison parser, made from expr.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse exprparse
#define yylex exprlex
#define yyerror exprerror
#define yylval exprlval
#define yychar exprchar
#define yydebug exprdebug
#define yynerrs exprnerrs
#define	SCAN_INT	257
#define	SCAN_FLOAT	258
#define	SCAN_STR	259
#define	SCAN_ID	260
#define	SCAN_FIELD	261
#define	SCAN_EQUAL	262
#define	SCAN_NOT_EQUAL	263
#define	SCAN_GREATER	264
#define	SCAN_GREATER_EQL	265
#define	SCAN_LESS	266
#define	SCAN_LESS_EQL	267
#define	SCAN_REGEXP	268

#line 187 "expr.y"


#include "config_dap.h"

static char rcsid[] not_used = {"$Id: expr.tab.c,v 1.13 2000/08/02 22:46:50 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <SLList.h>

#include "debug.h"

#include "DDS.h"

#include "Connect.h"

#include "BaseType.h"
#include "Array.h"
#include "List.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"
#include "Error.h"

#include "util.h"
#include "parser.h"
#include "expr.h"
#include "RValue.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed into the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro does not explicitly casts OBJ to an
// ERROR *.

#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)
#define ERROR_OBJ(arg) ((parser_arg *)(arg))->_error
#define STATUS(arg) ((parser_arg *)(arg))->_status

#if DODS_BISON_VER > 124
#define YYPARSE_PARAM arg
#else
#define YYPARSE_PARAM void *arg
#endif

int exprlex(void);		/* the scanner; see expr.lex */

void exprerror(const char *s);	/* easier to overload than to use stdarg... */
void exprerror(const char *s, const char *s2);
int no_such_func(void *arg, char *name);
int no_such_ident(void *arg, char *name, char *word);

void exprerror(const string &s); 
void exprerror(const string &s, const string &s2);
int no_such_func(void *arg, const string &name);
int no_such_ident(void *arg, const string &name, const string &word);

int_list *make_array_index(value &i1, value &i2, value &i3);
int_list *make_array_index(value &i1, value &i2);
int_list *make_array_index(value &i1);
int_list_list *make_array_indices(int_list *index);
int_list_list *append_array_index(int_list_list *indices, int_list *index);
void delete_array_indices(int_list_list *indices);
bool process_array_indices(BaseType *variable, int_list_list *indices); 
bool process_grid_indices(BaseType *variable, int_list_list *indices); 

bool is_array_t(BaseType *variable);
bool is_grid_t(BaseType *variable);

rvalue_list *make_rvalue_list(rvalue *rv);
rvalue_list *append_rvalue_list(rvalue_list *rvals, rvalue *rv);

BaseType *make_variable(DDS &table, const value &val);

rvalue *dereference_variable(rvalue *rv, DDS &dds);
rvalue *dereference_url(value &val);

bool_func get_function(const DDS &table, const char *name);
btp_func get_btp_function(const DDS &table, const char *name);
proj_func get_proj_function(const DDS &table, const char *name);


#line 283 "expr.y"
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
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		71
#define	YYFLAG		-32768
#define	YYNTBASE	25

#define YYTRANSLATE(x) ((unsigned)(x) <= 268 ? yytranslate[x] : 42)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    15,     2,    17,
    18,    21,     2,    16,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    24,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    22,     2,    23,     2,     2,     2,     2,     2,     2,     2,
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
     7,     8,     9,    10,    11,    12,    13,    14
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     3,     4,     8,    12,    14,    18,    20,    22,
    24,    26,    31,    33,    37,    43,    47,    49,    54,    56,
    58,    61,    64,    69,    71,    75,    77,    78,    80,    82,
    84,    86,    88,    91,    94,    96,    99,   103,   109,   117,
   119,   121,   123,   125,   127,   129
};

static const short yyrhs[] = {    -1,
    27,     0,     0,    15,    26,    30,     0,    27,    15,    30,
     0,    28,     0,    28,    16,    27,     0,     6,     0,     7,
     0,    29,     0,    38,     0,     6,    17,    35,    18,     0,
    31,     0,    30,    15,    31,     0,    33,    41,    19,    34,
    20,     0,    33,    41,    33,     0,    32,     0,     6,    17,
    35,    18,     0,    36,     0,    37,     0,    21,    36,     0,
    21,     5,     0,     6,    17,    35,    18,     0,    33,     0,
    34,    16,    33,     0,    34,     0,     0,     6,     0,     7,
     0,     3,     0,     4,     0,     5,     0,     6,    39,     0,
     7,    39,     0,    40,     0,    39,    40,     0,    22,     3,
    23,     0,    22,     3,    24,     3,    23,     0,    22,     3,
    24,     3,    24,     3,    23,     0,     8,     0,     9,     0,
    10,     0,    11,     0,    12,     0,    13,     0,    14,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   325,   331,   333,   334,   337,   343,   344,   350,   360,   369,
   373,   379,   399,   400,   406,   415,   426,   432,   445,   446,
   447,   459,   465,   477,   484,   493,   497,   503,   512,   523,
   528,   533,   540,   576,   607,   611,   617,   621,   625,   631,
   632,   633,   634,   635,   636,   637
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SCAN_INT",
"SCAN_FLOAT","SCAN_STR","SCAN_ID","SCAN_FIELD","SCAN_EQUAL","SCAN_NOT_EQUAL",
"SCAN_GREATER","SCAN_GREATER_EQL","SCAN_LESS","SCAN_LESS_EQL","SCAN_REGEXP",
"'&'","','","'('","')'","'{'","'}'","'*'","'['","']'","':'","constraint_expr",
"@1","projection","proj_clause","proj_function","selection","clause","bool_function",
"r_value","r_value_list","arg_list","identifier","constant","array_proj","array_indices",
"array_index","rel_op", NULL
};
#endif

static const short yyr1[] = {     0,
    25,    25,    26,    25,    25,    27,    27,    28,    28,    28,
    28,    29,    30,    30,    31,    31,    31,    32,    33,    33,
    33,    33,    33,    34,    34,    35,    35,    36,    36,    37,
    37,    37,    38,    38,    39,    39,    40,    40,    40,    41,
    41,    41,    41,    41,    41,    41
};

static const short yyr2[] = {     0,
     0,     1,     0,     3,     3,     1,     3,     1,     1,     1,
     1,     4,     1,     3,     5,     3,     1,     4,     1,     1,
     2,     2,     4,     1,     3,     1,     0,     1,     1,     1,
     1,     1,     2,     2,     1,     2,     3,     5,     7,     1,
     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     1,
     8,     9,     3,     2,     6,    10,    11,    27,     0,    33,
    35,    34,     0,     0,     0,    30,    31,    32,    28,    29,
     0,    24,    26,     0,    19,    20,     0,    36,    28,     4,
    13,    17,     0,     5,     7,    27,    22,    28,    21,     0,
    12,    37,     0,    27,     0,    40,    41,    42,    43,    44,
    45,    46,     0,     0,    25,     0,     0,    14,     0,    16,
    23,    38,     0,    23,     0,     0,    15,    39,     0,     0,
     0
};

static const short yydefgoto[] = {    69,
    13,     4,     5,     6,    30,    31,    32,    22,    23,    24,
    25,    26,     7,    10,    11,    53
};

static const short yypact[] = {    16,
    11,    -5,-32768,    36,    37,-32768,-32768,     4,    53,    -5,
-32768,    -5,     9,     9,    43,-32768,-32768,-32768,    40,-32768,
    30,-32768,    42,    41,-32768,-32768,    15,-32768,    44,    45,
-32768,-32768,    33,    45,-32768,     4,-32768,-32768,-32768,     4,
-32768,-32768,    59,     4,     9,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    -1,    46,-32768,    31,    47,-32768,     4,-32768,
-32768,-32768,    60,    19,    32,    48,-32768,-32768,    66,    67,
-32768
};

static const short yypgoto[] = {-32768,
-32768,    54,-32768,-32768,    56,    23,-32768,   -13,    13,   -15,
    52,-32768,-32768,    72,    14,-32768
};


#define	YYLAST		74


static const short yytable[] = {    33,
    33,    16,    17,    18,    19,    20,    16,    17,    18,    19,
    20,    16,    17,    18,    29,    20,     9,    59,   -18,    21,
    54,     1,     2,    28,    21,    28,    55,     8,    57,    21,
     3,    33,     9,   -18,    37,    38,    20,    42,    43,    60,
    46,    47,    48,    49,    50,    51,    52,    40,     1,     2,
    14,    67,    15,    62,    63,    27,    36,    40,    41,    45,
    44,    56,    66,    61,    64,    70,    71,    58,    35,    34,
    68,    65,    39,    12
};

static const short yycheck[] = {    13,
    14,     3,     4,     5,     6,     7,     3,     4,     5,     6,
     7,     3,     4,     5,     6,     7,    22,    19,     0,    21,
    36,     6,     7,    10,    21,    12,    40,    17,    44,    21,
    15,    45,    22,    15,     5,     6,     7,    23,    24,    53,
     8,     9,    10,    11,    12,    13,    14,    16,     6,     7,
    15,    20,    16,    23,    24,     3,    17,    16,    18,    15,
    17,     3,     3,    18,    18,     0,     0,    45,    15,    14,
    23,    59,    21,     2
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
#line 326 "expr.y"
{
		     (*DDS_OBJ(arg)).mark_all(true);
		     yyval.boolean = true;
		 ;
    break;}
case 3:
#line 333 "expr.y"
{ (*DDS_OBJ(arg)).mark_all(true); ;
    break;}
case 4:
#line 334 "expr.y"
{ 
		     yyval.boolean = yyvsp[0].boolean;
		 ;
    break;}
case 5:
#line 338 "expr.y"
{
		     yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		 ;
    break;}
case 7:
#line 345 "expr.y"
{
		    yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		;
    break;}
case 8:
#line 351 "expr.y"
{ 
		    BaseType *var = (*DDS_OBJ(arg)).var(yyvsp[0].id);
		    if (var) {
			yyval.boolean = (*DDS_OBJ(arg)).mark(yyvsp[0].id, true);
		    }
		    else {
			yyval.boolean = no_such_ident(arg, yyvsp[0].id, "identifier");
		    }
		;
    break;}
case 9:
#line 361 "expr.y"
{ 
		    BaseType *var = (*DDS_OBJ(arg)).var(yyvsp[0].id);
		    if (var)
			yyval.boolean = (*DDS_OBJ(arg)).mark(yyvsp[0].id, true);
		    else {
			yyval.boolean = no_such_ident(arg, yyvsp[0].id, "field");
		    }
		;
    break;}
case 10:
#line 370 "expr.y"
{
		    yyval.boolean = yyvsp[0].boolean;
		;
    break;}
case 11:
#line 374 "expr.y"
{
		    yyval.boolean = yyvsp[0].boolean;
		;
    break;}
case 12:
#line 380 "expr.y"
{
		    proj_func p_f = 0;
		    btp_func f = 0;

		    if ((f = get_btp_function(*(DDS_OBJ(arg)), yyvsp[-3].id))) {
			(*DDS_OBJ(arg)).append_clause(f, yyvsp[-1].r_val_l_ptr);
			yyval.boolean = true;
		    }
		    else if ((p_f = get_proj_function(*(DDS_OBJ(arg)), yyvsp[-3].id))) {
			BaseType **args = build_btp_args(yyvsp[-1].r_val_l_ptr, *(DDS_OBJ(arg)));
			(*p_f)((yyvsp[-1].r_val_l_ptr) ? yyvsp[-1].r_val_l_ptr->length():0, args, *(DDS_OBJ(arg)));
			yyval.boolean = true;
		    }
		    else {
			yyval.boolean = no_such_func(arg, yyvsp[-3].id);
		    }
		;
    break;}
case 14:
#line 401 "expr.y"
{
		    yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		;
    break;}
case 15:
#line 407 "expr.y"
{
		    if (yyvsp[-4].rval_ptr) {
			(*DDS_OBJ(arg)).append_clause(yyvsp[-3].op, yyvsp[-4].rval_ptr, yyvsp[-1].r_val_l_ptr);
			yyval.boolean = true;
		    }
		    else
			yyval.boolean = false;
		;
    break;}
case 16:
#line 416 "expr.y"
{
		    if (yyvsp[-2].rval_ptr) {
			rvalue_list *rv = new rvalue_list;
			rv->append(yyvsp[0].rval_ptr);
			(*DDS_OBJ(arg)).append_clause(yyvsp[-1].op, yyvsp[-2].rval_ptr, rv);
			yyval.boolean = true;
		    }
		    else
			yyval.boolean = false;
		;
    break;}
case 17:
#line 427 "expr.y"
{
		    yyval.boolean = yyvsp[0].boolean;
		;
    break;}
case 18:
#line 433 "expr.y"
{
		   bool_func b_func = get_function((*DDS_OBJ(arg)), yyvsp[-3].id);
		   if (!b_func) {
		       yyval.boolean = no_such_func(arg, yyvsp[-3].id);
		   }
		   else {
		       (*DDS_OBJ(arg)).append_clause(b_func, yyvsp[-1].r_val_l_ptr);
		       yyval.boolean = true;
		   }
	       ;
    break;}
case 21:
#line 448 "expr.y"
{
		    yyval.rval_ptr = dereference_variable(yyvsp[0].rval_ptr, *DDS_OBJ(arg));
		    if (!yyval.rval_ptr) {
			exprerror("Could not dereference variable", 
				  (yyvsp[0].rval_ptr)->value_name());
			string msg = "Could not dereference the URL: ";
			msg += (yyvsp[0].rval_ptr)->value_name();
			ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			STATUS(arg) = false;
		    }
		;
    break;}
case 22:
#line 460 "expr.y"
{
		    yyval.rval_ptr = dereference_url(yyvsp[0].val);
		    if (!yyval.rval_ptr)
			exprerror("Could not dereference URL", *(yyvsp[0].val).v.s);
		;
    break;}
case 23:
#line 466 "expr.y"
{
		    btp_func func = get_btp_function((*DDS_OBJ(arg)), yyvsp[-3].id);
		    if (func) {
			yyval.rval_ptr = new rvalue(func, yyvsp[-1].r_val_l_ptr);
		    } 
		    else {  		
			yyval.rval_ptr = (rvalue *)no_such_func(arg, yyvsp[-3].id);
		    }
		;
    break;}
case 24:
#line 478 "expr.y"
{
		    if (yyvsp[0].rval_ptr)
			yyval.r_val_l_ptr = make_rvalue_list(yyvsp[0].rval_ptr);
		    else
			yyval.r_val_l_ptr = 0;
		;
    break;}
case 25:
#line 485 "expr.y"
{
		    if (yyvsp[-2].r_val_l_ptr && yyvsp[0].rval_ptr)
			yyval.r_val_l_ptr = append_rvalue_list(yyvsp[-2].r_val_l_ptr, yyvsp[0].rval_ptr);
		    else
			yyval.r_val_l_ptr = 0;
		;
    break;}
case 26:
#line 494 "expr.y"
{  
		  yyval.r_val_l_ptr = yyvsp[0].r_val_l_ptr;
	      ;
    break;}
case 27:
#line 498 "expr.y"
{ 
		  yyval.r_val_l_ptr = 0; 
	      ;
    break;}
case 28:
#line 504 "expr.y"
{ 
		    BaseType *btp = (*DDS_OBJ(arg)).var(yyvsp[0].id);
		    if (!btp) {
			yyval.rval_ptr = (rvalue *)no_such_ident(arg, yyvsp[0].id, "identifier");
		    }
		    else
			yyval.rval_ptr = new rvalue(btp);
		;
    break;}
case 29:
#line 513 "expr.y"
{ 
		    BaseType *btp = (*DDS_OBJ(arg)).var(yyvsp[0].id);
		    if (!btp) {
			yyval.rval_ptr = (rvalue *)no_such_ident(arg, yyvsp[0].id, "field");
		    }
		    else
			yyval.rval_ptr = new rvalue(btp);
		;
    break;}
case 30:
#line 524 "expr.y"
{
		    BaseType *btp = make_variable((*DDS_OBJ(arg)), yyvsp[0].val);
		    yyval.rval_ptr = new rvalue(btp);
		;
    break;}
case 31:
#line 529 "expr.y"
{
		    BaseType *btp = make_variable((*DDS_OBJ(arg)), yyvsp[0].val);
		    yyval.rval_ptr = new rvalue(btp);
		;
    break;}
case 32:
#line 534 "expr.y"
{ 
		    BaseType *btp = make_variable((*DDS_OBJ(arg)), yyvsp[0].val); 
		    yyval.rval_ptr = new rvalue(btp);
		;
    break;}
case 33:
#line 541 "expr.y"
{
		    BaseType *var = (*DDS_OBJ(arg)).var(yyvsp[-1].id);
		    if (var && is_array_t(var)) {
			/* calls to set_send_p should be replaced with
			   calls to DDS::mark so that arrays of Structures,
			   etc. will be processed correctly when individual
			   elements are projected using short names (Whew!)
			   9/1/98 jhrg */
			/* var->set_send_p(true); */
			(*DDS_OBJ(arg)).mark(yyvsp[-1].id, true);
			yyval.boolean = process_array_indices(var, yyvsp[0].int_ll_ptr);
			if (!yyval.boolean) {
			    string msg = "The indices given for `";
			    msg += (string)yyvsp[-1].id + (string)"' are out of range.";
			    ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			    STATUS(arg) = false;
			}
			delete_array_indices(yyvsp[0].int_ll_ptr);
		    }
		    else if (var && is_grid_t(var)) {
			(*DDS_OBJ(arg)).mark(yyvsp[-1].id, true);
			/* var->set_send_p(true); */
			yyval.boolean = process_grid_indices(var, yyvsp[0].int_ll_ptr);
			if (!yyval.boolean) {
			    string msg = "The indices given for `";
			    msg += (string)yyvsp[-1].id + (string)"' are out of range.";
			    ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			    STATUS(arg) = false;
			}
			delete_array_indices(yyvsp[0].int_ll_ptr);
		    }
		    else {
			yyval.boolean = no_such_ident(arg, yyvsp[-1].id, "array or grid");
		    }
		;
    break;}
case 34:
#line 577 "expr.y"
{
		    BaseType *var = (*DDS_OBJ(arg)).var(yyvsp[-1].id);
		    if (var && is_array_t(var)) {
			yyval.boolean = (*DDS_OBJ(arg)).mark(yyvsp[-1].id, true) 
			    && process_array_indices(var, yyvsp[0].int_ll_ptr);
			if (!yyval.boolean) {
			    string msg = "The indices given for `";
			    msg += (string)yyvsp[-1].id + (string)"' are out of range.";
			    ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			    STATUS(arg) = false;
			}
			delete_array_indices(yyvsp[0].int_ll_ptr);
		    }
		    else if (var && is_grid_t(var)) {
			yyval.boolean = (*DDS_OBJ(arg)).mark(yyvsp[-1].id, true)
			    && process_grid_indices(var, yyvsp[0].int_ll_ptr);
			if (!yyval.boolean) {
			    string msg = "The indices given for `";
			    msg += (string)yyvsp[-1].id + (string)"' are out of range.";
			    ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			    STATUS(arg) = false;
			}
			delete_array_indices(yyvsp[0].int_ll_ptr);
		    }
		    else {
			yyval.boolean = no_such_ident(arg, yyvsp[-1].id, "array or grid");
		    }
		;
    break;}
case 35:
#line 608 "expr.y"
{
		    yyval.int_ll_ptr = make_array_indices(yyvsp[0].int_l_ptr);
		;
    break;}
case 36:
#line 612 "expr.y"
{
		    yyval.int_ll_ptr = append_array_index(yyvsp[-1].int_ll_ptr, yyvsp[0].int_l_ptr);
		;
    break;}
case 37:
#line 618 "expr.y"
{
		    yyval.int_l_ptr = make_array_index(yyvsp[-1].val);
		;
    break;}
case 38:
#line 622 "expr.y"
{
		    yyval.int_l_ptr = make_array_index(yyvsp[-3].val, yyvsp[-1].val);
		;
    break;}
case 39:
#line 626 "expr.y"
{
		    yyval.int_l_ptr = make_array_index(yyvsp[-5].val, yyvsp[-3].val, yyvsp[-1].val);
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
#line 640 "expr.y"


void
exprerror(const string &s)
{ 
    exprerror(s.c_str());
}

void
exprerror(const char *s)
{
    cerr << "Expression parse error: " << s << endl;
}

void
exprerror(const string &s, const string &s2)
{
    exprerror(s.c_str(), s2.c_str());
}

void
exprerror(const char *s, const char *s2)
{
	cerr << "Expression parse error: " << s << ": " << s2 << endl;
}

int
no_such_ident(void *arg, const string &name, const string &word)
{
    return no_such_ident(arg, name.c_str(), word.c_str());
}

int
no_such_ident(void *arg, char *name, char *word)
{
    string msg = "No such " + (string)word + " in dataset.";
    exprerror(msg.c_str(), name);

    msg = "The identifier `" + (string)name + "' is not in the dataset.";
    ERROR_OBJ(arg) = new Error(malformed_expr, msg.c_str());
    STATUS(arg) = false;

    return false;
}

int
no_such_func(void *arg, const string &name)
{
    return no_such_func(arg, name.c_str());
}

int
no_such_func(void *arg, char *name)
{
    exprerror("Not a registered function", name);
    string msg = "The function `" + (string)name 
	+ "' is not defined on this server.";

    ERROR_OBJ(arg) = new Error(malformed_expr, msg.c_str());
    STATUS(arg) = false;

    return false;
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
	return (int_list *)0;

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

int_list *
make_array_index(value &i1, value &i2)
{
    int_list *index = new int_list;

    if (i1.type != dods_int32_c || i2.type != dods_int32_c)
	return (int_list *)0;

    index->append((int)i1.v.i);
    index->append(1);
    index->append((int)i2.v.i);

    DBG(Pix dp;\
	cout << "index: ";\
	for (dp = index->first(); dp; index->next(dp))\
	cout << (*index)(dp) << " ";\
	cout << endl);

    return index;
}

int_list *
make_array_index(value &i1)
{
    int_list *index = new int_list;

    if (i1.type != dods_int32_c)
	return (int_list *)0;

    index->append((int)i1.v.i);
    index->append(1);
    index->append((int)i1.v.i);

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

    assert(index);
    indices->append(index);

    return indices;
}

int_list_list *
append_array_index(int_list_list *indices, int_list *index)
{
    assert(indices);
    assert(index);

    indices->append(index);

    return indices;
}

// Delete an array indices list. 

void
delete_array_indices(int_list_list *indices)
{
    assert(indices);

    for (Pix p = indices->first(); p; indices->next(p)) {
	assert((*indices)(p));
	delete (*indices)(p);
    }

    delete indices;
}

bool
is_array_t(BaseType *variable)
{
    assert(variable);

    if (variable->type() != dods_array_c)
	return false;
    else
	return true;
}

bool
is_grid_t(BaseType *variable)
{
    assert(variable);

    if (variable->type() != dods_grid_c)
	return false;
    else
	return true;
}

bool
process_array_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);
    assert(variable->type() == dods_array_c);
    Array *a = (Array *)variable; // replace with dynamic cast

    DBG(cerr << "Before clear_costraint:" << endl);
    DBG(a->print_decl(cerr, "", true, false, true));

    a->clear_constraint();	// each projection erases the previous one

    DBG(cerr << "After clear_costraint:" << endl);
    DBG(a->print_decl(cerr, "", true, false, true));

    Pix p, r;
    assert(indices);
    for (p = indices->first(), r = a->first_dim(); 
	 p && r; 
	 indices->next(p), a->next_dim(r)) {
	assert((*indices)(p));
	int_list *index = (*indices)(p);

	Pix q = index->first(); 
	assert(q);
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
	
	if (!a->add_constraint(r, start, stride, stop)) {
	    cerr << "Impossible index values in constraint for "
		 << a->name() << "." << endl;
	    status = false;
	    goto exit;
	}

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
    return status;
}

bool
process_grid_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);
    assert(variable->type() == dods_grid_c);
    Grid *g = dynamic_cast<Grid *>(variable);
    if (!g)
	throw Error(unknown_error, "Expected a Grid variable");

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
    assert(indices);
    for (p = indices->first(), r = g->first_map_var(); 
	 p && r; 
	 indices->next(p), g->next_map_var(r)) {
	assert((*indices)(p));
	int_list *index = (*indices)(p);

	Pix q = index->first(); 
	assert(q);
	int start = (*index)(q);

	index->next(q);
	int stride = (*index)(q);
	
	index->next(q);
	int stop = (*index)(q);

	assert(g->map_var(r));
	assert(g->map_var(r)->type() == dods_array_c);
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

	if (!a->add_constraint(a->first_dim(), start, stride, stop)) {
	    cerr << "Impossible index values in constraint for "
		 << a->name() << "." << endl;
	    status = false;
	    goto exit;
	}

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
    return status;
}

// Create a list of r values and add VAL to the list.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
make_rvalue_list(rvalue *rv)
{
    assert(rv);

    rvalue_list *rvals = new rvalue_list;

    return append_rvalue_list(rvals, rv);
}

// Given a rvalue_list pointer RVALS and a value pointer VAL, make a variable
// to hold VAL and append that variable to the list RVALS.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
append_rvalue_list(rvalue_list *rvals, rvalue *rv)
{
    assert(rvals);
    assert(rv);

    rvals->append(rv);

    return rvals;
}

// Given a string which is a URL, dereference it and return the data it
// points to.

static rvalue *
dereference_string(string &s)
{
    unsigned int qpos = s.find('?');
    string url = s.substr(0, qpos);	// strip off CE
    string ce = s.substr(qpos+1);	// yes, get the CE

    // I don't think that the `false' is really necessary, but g++ seems to
    // want it. jhrg 2/10/97
    Connect c = Connect(url, false); // make the virtual connection

    // the initial URL must be a complete reference to data; thus no
    // additional CE is needed. 
    DDS *d = c.request_data(ce, false, false); 

    // By definition, the DDS `D' can have only one variable, so make sure
    // that is true.
    if (d->num_var() != 1) {
	cerr << 
	    "Too many variables in URL; use only single variable projections"
	     << endl;
	return 0;
    }

    // OK, we're here. The first_var() must be the only var, return it bound
    // up in an rvalue struct. NB: the *object* must be copied since the one
    // within DDS `D' will be deleted by D's dtor.
    BaseType *btp = d->var(d->first_var())->ptr_duplicate();
    rvalue *rv = new rvalue(btp);

    delete d;

    return rv;
}

rvalue *
dereference_url(value &val)
{
    if (val.type != dods_str_c)
	return 0;

    return dereference_string(*val.v.s);
}

// Given a rvalue, get the BaseType that encapsulates its value, make sure it
// is a string and, if all that works, dereference it.

rvalue *
dereference_variable(rvalue *rv, DDS &dds)
{
    assert(rv);
    // the value will be read over the net
    BaseType *btp = rv->bvalue("dummy", dds); 
    if (btp->type() != dods_str_c && btp->type() != dods_url_c) {
	cerr << "Variable: " << btp->name() 
	    << " must be either a string or a url" 
	    << endl;
	return 0;
    }

    string s;
    string  *sp = &s;
    btp->buf2val((void **)&sp);
    
    return dereference_string(s);
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

proj_func
get_proj_function(const DDS &table, const char *name)
{
    proj_func f;

    if (table.find_function(name, &f))
	return f;
    else
	return 0;
}
