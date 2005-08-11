/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse exprparse
#define yylex   exprlex
#define yyerror exprerror
#define yylval  exprlval
#define yychar  exprchar
#define yydebug exprdebug
#define yynerrs exprnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SCAN_STR = 258,
     SCAN_WORD = 259,
     SCAN_EQUAL = 260,
     SCAN_NOT_EQUAL = 261,
     SCAN_GREATER = 262,
     SCAN_GREATER_EQL = 263,
     SCAN_LESS = 264,
     SCAN_LESS_EQL = 265,
     SCAN_REGEXP = 266
   };
#endif
#define SCAN_STR 258
#define SCAN_WORD 259
#define SCAN_EQUAL 260
#define SCAN_NOT_EQUAL 261
#define SCAN_GREATER 262
#define SCAN_GREATER_EQL 263
#define SCAN_LESS 264
#define SCAN_LESS_EQL 265
#define SCAN_REGEXP 266




/* Copy the first part of user declarations.  */
#line 41 "expr.y"


#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <string>

#include "debug.h"
#include "escaping.h"

#include "DDS.h"

#include "Connect.h"

#include "BaseType.h"
#include "Array.h"
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

using std::cerr;
using std::endl;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed into the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro does not explicitly casts OBJ to an
// ERROR *.

#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)

#define YYPARSE_PARAM arg

int exprlex(void);		/* the scanner; see expr.lex */

void exprerror(const char *s);	/* easier to overload than to use stdarg... */
void exprerror(const char *s, const char *s2);
void no_such_func(char *name);
void no_such_ident(char *name, char *word);

void exprerror(const string &s); 
void exprerror(const string &s, const string &s2);
void no_such_func(const string &name);
void no_such_ident(const string &name, const string &word);

int_list *make_array_index(value &i1, value &i2, value &i3);
int_list *make_array_index(value &i1, value &i2);
int_list *make_array_index(value &i1);
int_list_list *make_array_indices(int_list *index);
int_list_list *append_array_index(int_list_list *indices, int_list *index);

void delete_array_indices(int_list_list *indices);
bool bracket_projection(DDS &table, const char *name, 
			int_list_list *indices);

bool process_array_indices(BaseType *variable, int_list_list *indices); 
bool process_grid_indices(BaseType *variable, int_list_list *indices); 
bool process_sequence_indices(BaseType *variable, int_list_list *indices);

bool is_array_t(BaseType *variable);
bool is_grid_t(BaseType *variable);
bool is_sequence_t(BaseType *variable);

rvalue_list *make_rvalue_list(rvalue *rv);
rvalue_list *append_rvalue_list(rvalue_list *rvals, rvalue *rv);

BaseType *make_variable(DDS &table, const value &val);

rvalue *dereference_variable(rvalue *rv, DDS &dds);
rvalue *dereference_url(value &val);

bool_func get_function(const DDS &table, const char *name);
btp_func get_btp_function(const DDS &table, const char *name);
proj_func get_proj_function(const DDS &table, const char *name);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 134 "expr.y"
typedef union YYSTYPE {
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
/* Line 191 of yacc.c.  */
#line 217 "expr.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 229 "expr.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   59

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  22
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  17
/* YYNRULES -- Number of rules. */
#define YYNRULES  40
/* YYNRULES -- Number of states. */
#define YYNSTATES  64

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   266

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    12,     2,
      14,    15,    18,     2,    13,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    21,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    19,     2,    20,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    16,     2,    17,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     6,     7,    11,    15,    17,    21,
      23,    25,    27,    32,    34,    38,    44,    48,    50,    55,
      57,    60,    65,    67,    71,    73,    74,    76,    78,    81,
      83,    86,    90,    96,   104,   106,   108,   110,   112,   114,
     116
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      23,     0,    -1,    -1,    25,    -1,    -1,    12,    24,    28,
      -1,    25,    12,    28,    -1,    26,    -1,    26,    13,    25,
      -1,     4,    -1,    27,    -1,    35,    -1,     4,    14,    33,
      15,    -1,    29,    -1,    28,    12,    29,    -1,    31,    38,
      16,    32,    17,    -1,    31,    38,    31,    -1,    30,    -1,
       4,    14,    33,    15,    -1,    34,    -1,    18,    34,    -1,
       4,    14,    33,    15,    -1,    31,    -1,    32,    13,    31,
      -1,    32,    -1,    -1,     4,    -1,     3,    -1,     4,    36,
      -1,    37,    -1,    36,    37,    -1,    19,     4,    20,    -1,
      19,     4,    21,     4,    20,    -1,    19,     4,    21,     4,
      21,     4,    20,    -1,     5,    -1,     6,    -1,     7,    -1,
       8,    -1,     9,    -1,    10,    -1,    11,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   174,   174,   179,   181,   181,   185,   191,   192,   198,
     210,   214,   220,   240,   241,   247,   256,   267,   273,   286,
     287,   295,   307,   314,   323,   328,   333,   364,   371,   381,
     385,   391,   403,   421,   447,   448,   449,   450,   451,   452,
     453
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SCAN_STR", "SCAN_WORD", "SCAN_EQUAL",
  "SCAN_NOT_EQUAL", "SCAN_GREATER", "SCAN_GREATER_EQL", "SCAN_LESS",
  "SCAN_LESS_EQL", "SCAN_REGEXP", "'&'", "','", "'('", "')'", "'{'", "'}'",
  "'*'", "'['", "']'", "':'", "$accept", "constraint_expr", "@1",
  "projection", "proj_clause", "proj_function", "selection", "clause",
  "bool_function", "r_value", "r_value_list", "arg_list", "id_or_const",
  "array_proj", "array_indices", "array_index", "rel_op", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,    38,    44,    40,    41,   123,   125,    42,    91,
      93,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    22,    23,    23,    24,    23,    23,    25,    25,    26,
      26,    26,    27,    28,    28,    29,    29,    29,    30,    31,
      31,    31,    32,    32,    33,    33,    34,    34,    35,    36,
      36,    37,    37,    37,    38,    38,    38,    38,    38,    38,
      38
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     0,     3,     3,     1,     3,     1,
       1,     1,     4,     1,     3,     5,     3,     1,     4,     1,
       2,     4,     1,     3,     1,     0,     1,     1,     2,     1,
       2,     3,     5,     7,     1,     1,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     9,     4,     0,     3,     7,    10,    11,    25,     0,
      28,    29,     0,     1,     0,     0,    27,    26,     0,    22,
      24,     0,    19,     0,    30,    26,     5,    13,    17,     0,
       6,     8,    25,    26,    20,     0,    12,    31,     0,    25,
       0,    34,    35,    36,    37,    38,    39,    40,     0,     0,
      23,     0,     0,    14,     0,    16,    21,    32,     0,    21,
       0,     0,    15,    33
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     3,    12,     4,     5,     6,    26,    27,    28,    19,
      20,    21,    22,     7,    10,    11,    48
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -21
static const yysigned_char yypact[] =
{
       5,    -4,   -21,    11,    12,    28,   -21,   -21,     2,    10,
      25,   -21,     4,   -21,     4,    38,   -21,    29,    23,   -21,
      32,    31,   -21,    17,   -21,    33,    36,   -21,   -21,    24,
      36,   -21,     2,   -21,   -21,     2,   -21,   -21,    45,     2,
       4,   -21,   -21,   -21,   -21,   -21,   -21,   -21,     0,    35,
     -21,    19,    37,   -21,     2,   -21,   -21,   -21,    47,     1,
       8,    34,   -21,   -21
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -21,   -21,   -21,    40,   -21,   -21,    39,    16,   -21,   -12,
       3,   -20,    41,   -21,   -21,    48,   -21
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -19
static const yysigned_char yytable[] =
{
      29,   -18,    29,    16,    17,    16,    17,    16,    25,     1,
       8,    13,    49,   -18,    23,     9,    54,     2,    18,    52,
      18,    35,    18,    50,    14,    62,    16,    33,    29,    41,
      42,    43,    44,    45,    46,    47,    55,    37,    38,    57,
      58,    15,     1,    32,     9,    35,    36,    39,    40,    51,
      56,    61,    59,    30,    63,    31,    53,    60,    24,    34
};

static const unsigned char yycheck[] =
{
      12,     0,    14,     3,     4,     3,     4,     3,     4,     4,
      14,     0,    32,    12,     4,    19,    16,    12,    18,    39,
      18,    13,    18,    35,    12,    17,     3,     4,    40,     5,
       6,     7,     8,     9,    10,    11,    48,    20,    21,    20,
      21,    13,     4,    14,    19,    13,    15,    14,    12,     4,
      15,     4,    15,    14,    20,    15,    40,    54,    10,    18
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     4,    12,    23,    25,    26,    27,    35,    14,    19,
      36,    37,    24,     0,    12,    13,     3,     4,    18,    31,
      32,    33,    34,     4,    37,     4,    28,    29,    30,    31,
      28,    25,    14,     4,    34,    13,    15,    20,    21,    14,
      12,     5,     6,     7,     8,     9,    10,    11,    38,    33,
      31,     4,    33,    29,    16,    31,    15,    20,    21,    15,
      32,     4,    17,    20
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 174 "expr.y"
    {
		     (*DDS_OBJ(arg)).mark_all(true);
		     yyval.boolean = true;
		 ;}
    break;

  case 4:
#line 181 "expr.y"
    { (*DDS_OBJ(arg)).mark_all(true); ;}
    break;

  case 5:
#line 182 "expr.y"
    { 
		     yyval.boolean = yyvsp[0].boolean;
		 ;}
    break;

  case 6:
#line 186 "expr.y"
    {
		     yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		 ;}
    break;

  case 8:
#line 193 "expr.y"
    {
		    yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		;}
    break;

  case 9:
#line 199 "expr.y"
    { 
		    BaseType *var = (*DDS_OBJ(arg)).var(yyvsp[0].id);
		    if (var) {
			DBG(cerr << "Marking " << yyvsp[0].id << endl);
			yyval.boolean = (*DDS_OBJ(arg)).mark(yyvsp[0].id, true);
			DBG(cerr << "result: " << yyval.boolean << endl);
		    }
		    else {
			no_such_ident(yyvsp[0].id, "identifier");
		    }
		;}
    break;

  case 10:
#line 211 "expr.y"
    {
		    yyval.boolean = yyvsp[0].boolean;
		;}
    break;

  case 11:
#line 215 "expr.y"
    {
		    yyval.boolean = yyvsp[0].boolean;
		;}
    break;

  case 12:
#line 221 "expr.y"
    {
		    proj_func p_f = 0;
		    btp_func f = 0;

		    if ((f = get_btp_function(*(DDS_OBJ(arg)), yyvsp[-3].id))) {
			(*DDS_OBJ(arg)).append_clause(f, yyvsp[-1].r_val_l_ptr);
			yyval.boolean = true;
		    }
		    else if ((p_f = get_proj_function(*(DDS_OBJ(arg)), yyvsp[-3].id))) {
			BaseType **args = build_btp_args(yyvsp[-1].r_val_l_ptr, *(DDS_OBJ(arg)));
			(*p_f)((yyvsp[-1].r_val_l_ptr) ? yyvsp[-1].r_val_l_ptr->size():0, args, *(DDS_OBJ(arg)));
			yyval.boolean = true;
		    }
		    else {
			no_such_func(yyvsp[-3].id);
		    }
		;}
    break;

  case 14:
#line 242 "expr.y"
    {
		    yyval.boolean = yyvsp[-2].boolean && yyvsp[0].boolean;
		;}
    break;

  case 15:
#line 248 "expr.y"
    {
		    if (yyvsp[-4].rval_ptr) {
			(*DDS_OBJ(arg)).append_clause(yyvsp[-3].op, yyvsp[-4].rval_ptr, yyvsp[-1].r_val_l_ptr);
			yyval.boolean = true;
		    }
		    else
			yyval.boolean = false;
		;}
    break;

  case 16:
#line 257 "expr.y"
    {
		    if (yyvsp[-2].rval_ptr) {
			rvalue_list *rv = new rvalue_list;
			rv->push_back(yyvsp[0].rval_ptr);
			(*DDS_OBJ(arg)).append_clause(yyvsp[-1].op, yyvsp[-2].rval_ptr, rv);
			yyval.boolean = true;
		    }
		    else
			yyval.boolean = false;
		;}
    break;

  case 17:
#line 268 "expr.y"
    {
		    yyval.boolean = yyvsp[0].boolean;
		;}
    break;

  case 18:
#line 274 "expr.y"
    {
		   bool_func b_func = get_function((*DDS_OBJ(arg)), yyvsp[-3].id);
		   if (!b_func) {
		       no_such_func(yyvsp[-3].id);
		   }
		   else {
		       (*DDS_OBJ(arg)).append_clause(b_func, yyvsp[-1].r_val_l_ptr);
		       yyval.boolean = true;
		   }
	       ;}
    break;

  case 20:
#line 288 "expr.y"
    {
		    yyval.rval_ptr = dereference_variable(yyvsp[0].rval_ptr, *DDS_OBJ(arg));
		    if (!yyval.rval_ptr) {
			exprerror("Could not dereference the URL", 
				  (yyvsp[0].rval_ptr)->value_name());
		    }
		;}
    break;

  case 21:
#line 296 "expr.y"
    {
		    btp_func func = get_btp_function((*DDS_OBJ(arg)), yyvsp[-3].id);
		    if (func) {
			yyval.rval_ptr = new rvalue(func, yyvsp[-1].r_val_l_ptr);
		    } 
		    else {  		
			no_such_func(yyvsp[-3].id);
		    }
		;}
    break;

  case 22:
#line 308 "expr.y"
    {
		    if (yyvsp[0].rval_ptr)
			yyval.r_val_l_ptr = make_rvalue_list(yyvsp[0].rval_ptr);
		    else
			yyval.r_val_l_ptr = 0;
		;}
    break;

  case 23:
#line 315 "expr.y"
    {
		    if (yyvsp[-2].r_val_l_ptr && yyvsp[0].rval_ptr)
			yyval.r_val_l_ptr = append_rvalue_list(yyvsp[-2].r_val_l_ptr, yyvsp[0].rval_ptr);
		    else
			yyval.r_val_l_ptr = 0;
		;}
    break;

  case 24:
#line 324 "expr.y"
    {  
		  yyval.r_val_l_ptr = yyvsp[0].r_val_l_ptr;
	      ;}
    break;

  case 25:
#line 328 "expr.y"
    { 
		  yyval.r_val_l_ptr = 0; 
	      ;}
    break;

  case 26:
#line 334 "expr.y"
    { 
		    BaseType *btp = (*DDS_OBJ(arg)).var(www2id(string(yyvsp[0].id)));
		    if (!btp) {
			value new_val;
			if (check_int32(yyvsp[0].id)) {
			    new_val.type = dods_int32_c;
			    new_val.v.i = atoi(yyvsp[0].id);
			}
			else if (check_uint32(yyvsp[0].id)) {
			    new_val.type = dods_uint32_c;
			    new_val.v.ui = atoi(yyvsp[0].id);
			}
			else if (check_float64(yyvsp[0].id)) {
			    new_val.type = dods_float64_c;
			    new_val.v.f = atof(yyvsp[0].id);
			}
			else {
			    new_val.type = dods_str_c;
			    new_val.v.s = new string(yyvsp[0].id);
			}
			BaseType *btp = make_variable((*DDS_OBJ(arg)), new_val); 
			// *** test for btp == null
			// delete new_val.v.s; // Str::val2buf copies the value.
			yyval.rval_ptr = new rvalue(btp);
		    }
		    else {
			btp->set_in_selection(true);
			yyval.rval_ptr = new rvalue(btp);
		    }
		;}
    break;

  case 27:
#line 365 "expr.y"
    { 
		    BaseType *btp = make_variable((*DDS_OBJ(arg)), yyvsp[0].val); 
		    yyval.rval_ptr = new rvalue(btp);
		;}
    break;

  case 28:
#line 372 "expr.y"
    {
		  if (!bracket_projection((*DDS_OBJ(arg)), yyvsp[-1].id, yyvsp[0].int_ll_ptr))
		    // no_such_ident throws an exception.
		    no_such_ident(yyvsp[-1].id, "array, grid or sequence");
		  else
		    yyval.boolean = true;
		;}
    break;

  case 29:
#line 382 "expr.y"
    {
		    yyval.int_ll_ptr = make_array_indices(yyvsp[0].int_l_ptr);
		;}
    break;

  case 30:
#line 386 "expr.y"
    {
		    yyval.int_ll_ptr = append_array_index(yyvsp[-1].int_ll_ptr, yyvsp[0].int_l_ptr);
		;}
    break;

  case 31:
#line 392 "expr.y"
    {
		    if (!check_uint32(yyvsp[-1].id)) {
			string msg = "The word `";
			msg += string(yyvsp[-1].id) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    value i;
		    i.type = dods_uint32_c;
		    i.v.i = atoi(yyvsp[-1].id);
		    yyval.int_l_ptr = make_array_index(i);
		;}
    break;

  case 32:
#line 404 "expr.y"
    {
		    if (!check_uint32(yyvsp[-3].id)) {
			string msg = "The word `";
			msg += string(yyvsp[-3].id) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    if (!check_uint32(yyvsp[-1].id)) {
			string msg = "The word `";
			msg += string(yyvsp[-1].id) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    value i,j;
		    i.type = j.type = dods_uint32_c;
		    i.v.i = atoi(yyvsp[-3].id);
		    j.v.i = atoi(yyvsp[-1].id);
		    yyval.int_l_ptr = make_array_index(i, j);
		;}
    break;

  case 33:
#line 422 "expr.y"
    {
		    if (!check_uint32(yyvsp[-5].id)) {
			string msg = "The word `";
			msg += string(yyvsp[-5].id) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    if (!check_uint32(yyvsp[-3].id)) {
			string msg = "The word `";
			msg += string(yyvsp[-3].id) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    if (!check_uint32(yyvsp[-1].id)) {
			string msg = "The word `";
			msg += string(yyvsp[-1].id) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    value i, j, k;
		    i.type = j.type = k.type = dods_uint32_c;
		    i.v.i = atoi(yyvsp[-5].id);
		    j.v.i = atoi(yyvsp[-3].id);
		    k.v.i = atoi(yyvsp[-1].id);
		    yyval.int_l_ptr = make_array_index(i, j, k);
		;}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 1488 "expr.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 456 "expr.y"


// All these error reporting function now throw instnaces of Error. The expr
// parser no longer returns an error code to indicate and error. 2/16/2000
// jhrg.

void
exprerror(const string &s)
{ 
    exprerror(s.c_str());
}

void
exprerror(const char *s)
{
    // cerr << "Expression parse error: " << s << endl;
    string msg = "Constraint expression parse error: " + (string)s;
    throw Error(malformed_expr, msg);
}

void
exprerror(const string &s, const string &s2)
{
    exprerror(s.c_str(), s2.c_str());
}

void
exprerror(const char *s, const char *s2)
{
    string msg = "Constraint expression parse error: " + (string)s + ": " 
	+ (string)s2;
    throw Error(malformed_expr, msg);
}

void
no_such_ident(const string &name, const string &word)
{
    string msg = "No such " + word + " in dataset";
    exprerror(msg.c_str(), name);
}

void
no_such_ident(char *name, char *word)
{
    string msg = "No such " + (string)word + " in dataset";
    exprerror(msg.c_str(), name);
}

void
no_such_func(const string &name)
{
    no_such_func(name.c_str());
}

void
no_such_func(char *name)
{
    exprerror("Not a registered function", name);
}

/* If we're calling this, assume var is not a Sequence. But assume that the
   name contains a dot and it's a separator. Look for the rightmost dot and
   then look to see if the name to the left is a sequence. Return a pointer
   to the sequence if it is otherwise return null. Uses tail-recurrsion to
   'walk' back from right to left looking at each dot. This way the sequence
   will be found even if there are structures between the field and the
   Sequence. */
Sequence *
parent_is_sequence(DDS &table, const char *name)
{
    string n = name;
    string::size_type dotpos = n.find_last_of('.');
    if (dotpos == string::npos)
	return 0;

    string s = n.substr(0, dotpos);
    // If the thing returned by table.var is not a Sequence, this cast
    // will yield null.
    Sequence *seq = dynamic_cast<Sequence*>(table.var(s));
    if (seq)
	return seq;
    else
	return parent_is_sequence(table, s.c_str());
}


bool
bracket_projection(DDS &table, const char *name, int_list_list *indices)
{
    bool status = true;
    BaseType *var = table.var(name);
    Sequence *seq;		// used in last else if clause

    if (!var)
	return false;
	
    if (is_array_t(var)) {
	/* calls to set_send_p should be replaced with
	   calls to DDS::mark so that arrays of Structures,
	   etc. will be processed correctly when individual
	   elements are projected using short names (Whew!)
	   9/1/98 jhrg */
	/* var->set_send_p(true); */
	table.mark(name, true);
	status = process_array_indices(var, indices);
	if (!status) {
	    string msg = "The indices given for `";
	    msg += (string)name + (string)"' are out of range.";
	    throw Error(malformed_expr, msg);
	}
	delete_array_indices(indices);
    }
    else if (is_grid_t(var)) {
	table.mark(name, true);
	/* var->set_send_p(true); */
	status = process_grid_indices(var, indices);
	if (!status) {
	    string msg = "The indices given for `";
	    msg += (string)name + (string)"' are out of range.";
	    throw Error(malformed_expr, msg);
	}
	delete_array_indices(indices);
    }
    else if (is_sequence_t(var)) {
	table.mark(name, true);
	status = process_sequence_indices(var, indices);
	if (!status) {
	    string msg = "The indices given for `";
	    msg += (string)name + (string)"' are out of range.";
	    throw Error(malformed_expr, msg);
	}
	delete_array_indices(indices);
    }
    else if ((seq = parent_is_sequence(table, name))) {
	table.mark(name, true);
	status = process_sequence_indices(seq, indices);
	if (!status) {
	    string msg = "The indices given for `";
	    msg += (string)name + (string)"' are out of range.";
	    throw Error(malformed_expr, msg);
	}
	delete_array_indices(indices);
    }
    else {
	status = false;
    }
  
    return status;
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

    if (i1.type != dods_uint32_c
	|| i2.type != dods_uint32_c
	|| i3.type != dods_uint32_c)
	return (int_list *)0;

    index->push_back((int)i1.v.i);
    index->push_back((int)i2.v.i);
    index->push_back((int)i3.v.i);

    DBG(cout << "index: ";\
	for (int_iter dp = index->begin(); dp != index->end(); dp++)\
	cout << (*dp) << " ";\
	cout << endl);

    return index;
}

int_list *
make_array_index(value &i1, value &i2)
{
    int_list *index = new int_list;

    if (i1.type != dods_uint32_c || i2.type != dods_uint32_c)
	return (int_list *)0;

    index->push_back((int)i1.v.i);
    index->push_back(1);
    index->push_back((int)i2.v.i);

    DBG(cout << "index: ";\
	for (int_citer dp = index->begin(); dp != index->end(); dp++)\
	cout << (*dp) << " ";\
	cout << endl);

    return index;
}

int_list *
make_array_index(value &i1)
{
    int_list *index = new int_list;

    if (i1.type != dods_uint32_c)
	return (int_list *)0;

    index->push_back((int)i1.v.i);
    index->push_back(1);
    index->push_back((int)i1.v.i);

    DBG(cout << "index: ";\
	for (int_citer dp = index->begin(); dp != index->end(); dp++)\
	cout << (*dp) << " ";\
	cout << endl);

    return index;
}

int_list_list *
make_array_indices(int_list *index)
{
    int_list_list *indices = new int_list_list;

    DBG(cout << "index: ";\
	for (int_citer dp = index->begin(); dp != index->end(); dp++)\
	cout << (*dp) << " ";\
	cout << endl);

    assert(index);
    indices->push_back(index);

    return indices;
}

int_list_list *
append_array_index(int_list_list *indices, int_list *index)
{
    assert(indices);
    assert(index);

    indices->push_back(index);

    return indices;
}

// Delete an array indices list. 

void
delete_array_indices(int_list_list *indices)
{
    assert(indices);

    for (int_list_citer i = indices->begin(); i != indices->end(); i++) {
	int_list *il = *i ;
	assert(il);
	delete il;
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
is_sequence_t(BaseType *variable)
{
    assert(variable);

    if (variable->type() != dods_sequence_c)
	return false;
    else
	return true;
}

bool
process_array_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);

    Array *a = dynamic_cast<Array *>(variable); // replace with dynamic cast
    if (!a)
	throw Error(malformed_expr, 
	   string("The constraint expression evaluator expected an array; ")
		    + variable->name() + " is not an array.");
		   
    if (a->dimensions(true) != (unsigned)indices->size())
	throw Error(malformed_expr, 
	   string("Error: The number of dimenstions in the constraint for ")
		    + variable->name() 
		    + " must match the number in the array.");
		   
    DBG(cerr << "Before clear_costraint:" << endl);
    DBG(a->print_decl(stderr, "", true, false, true));

    a->clear_constraint();	// each projection erases the previous one

    DBG(cerr << "After clear_costraint:" << endl);
    DBG(a->print_decl(stderr, "", true, false, true));

    assert(indices);
    int_list_citer p = indices->begin() ;
    Array::Dim_iter r = a->dim_begin() ;
    for (; p != indices->end() && r != a->dim_end(); p++, r++) {
	int_list *index = *p;
	assert(index);

	int_citer q = index->begin(); 
	assert(q!=index->end());
	int start = *q;

	q++;
	int stride = *q;
	
	q++;
	int stop = *q;

	q++;
	if (q != index->end()) {
	    throw Error(malformed_expr,
			string("Too many values in index list for ")
			+ a->name() + ".");
	}

	DBG(cerr << "process_array_indices: Setting constraint on "\
	    << a->name() << "[" << start << ":" << stop << "]" << endl);

	a->add_constraint(r, start, stride, stop);

	DBG(cerr << "Set Constraint: " << a->dimension_size(r, true) << endl);
    }

    DBG(cerr << "After processing loop:" << endl);
    DBG(a->print_decl(stderr, "", true, false, true));

    DBG(cout << "Array Constraint: ";\
	for (Array::Dim_iter dp = a->dim_begin(); dp != a->dim_end(); dp++)\
	    cout << a->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p != indices->end() && r == a->dim_end()) {
	throw Error(malformed_expr,
		    string("Too many indices in constraint for ")
		    + a->name() + ".");
    }

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

    Array *a = dynamic_cast<Array *>(g->array_var());
    if (!a)
	throw InternalErr(__FILE__, __LINE__, "Malformed Grid variable");
    if (a->dimensions(true) != (unsigned)indices->size())
	throw Error(malformed_expr, 
	   string("Error: The number of dimenstions in the constraint for ")
		    + variable->name() 
		    + " must match the number in the grid.");
		   
    // First do the constraints on the ARRAY in the grid.
    process_array_indices(g->array_var(), indices);

    // Now process the maps.
    Grid::Map_iter r = g->map_begin() ;

    // Supress all maps by default.
    for (; r != g->map_end(); r++)
    {
	(*r)->set_send_p(false);
    }

    // Add specified maps to the current projection.
    assert(indices);
    int_list_citer p = indices->begin();
    r = g->map_begin(); 
    for (; p != indices->end() && r != g->map_end(); p++, r++)
    {
	int_list *index = *p;
	assert(index);

	int_citer q = index->begin(); 
	assert(q != index->end());
	int start = *q;

	q++;
	int stride = *q;
	
	q++;
	int stop = *q;

	BaseType *btp = *r;
	assert(btp);
	assert(btp->type() == dods_array_c);
	Array *a = (Array *)btp;
	a->set_send_p(true);
	a->clear_constraint();

	q++;
	if (q!=index->end()) {
	    throw Error(malformed_expr,
			string("Too many values in index list for ")
			+ a->name() + ".");
	}

	DBG(cerr << "process_grid_indices: Setting constraint on "\
	    << a->name() << "[" << start << ":" << stop << "]" << endl);

	Array::Dim_iter si = a->dim_begin() ;
	a->add_constraint(si, start, stride, stop);

	DBG(Array::Dim_iter aiter = a->dim_begin() ; \
	    cerr << "Set Constraint: " \
	    << a->dimension_size(aiter, true) << endl);
    }

    DBG(cout << "Grid Constraint: ";\
	for (Array::Dim_iter dp = ((Array *)g->array_var())->dim_begin();
	     dp != ((Array *)g->array_var())->dim_end(); \
	     dp++)\
	   cout << ((Array *)g->array_var())->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p!=indices->end() && r==g->map_end()) {
	throw Error(malformed_expr,
		    string("Too many indices in constraint for ")
		    + (*r)->name() + ".");
    }

    return status;
}

bool
process_sequence_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);
    assert(variable->type() == dods_sequence_c);
    Sequence *s = dynamic_cast<Sequence *>(variable);
    if (!s)
	throw Error(malformed_expr, "Expected a Sequence variable");

    // Add specified maps to the current projection.
    assert(indices);
    for (int_list_citer p = indices->begin(); p != indices->end(); p++)
    {
	int_list *index = *p;
	assert(index);

	int_citer q = index->begin(); 
	assert(q!=index->end());
	int start = *q;

	q++;
	int stride = *q;
	
	q++;
	int stop = *q;

	q++;
	if (q!=index->end()) {
	  throw Error(malformed_expr, 
		      string("Too many values in index list for ")
		      + s->name() + ".");
	}

	s->set_row_number_constraint(start, stop, stride);
    }

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

    rvals->push_back(rv);

    return rvals;
}

// Given a string which is a URL, dereference it and return the data it
// points to.

static rvalue *
dereference_string(string &s)
{
    // FIX Once Connect/HTTPConnect settle down. ***
    unsigned int qpos = s.find('?');
    string url = s.substr(0, qpos);	// strip off CE
    string ce = s.substr(qpos+1);	// yes, get the CE

    Connect c(url); // make the virtual connection

    // the initial URL must be a complete reference to data; thus no
    // additional CE is needed. 
    BaseTypeFactory *factory = new BaseTypeFactory;
    DataDDS *d = new DataDDS(factory);
    c.request_data(*d, ce); 

    // By definition, the DDS `D' can have only one variable, so make sure
    // that is true.
    if (d->num_var() != 1) {
	delete factory; factory = 0;
	delete d; d = 0;
	throw Error (malformed_expr,
		     string("Too many variables in URL; use only single variable projections"));
    }

    // OK, we're here. The first_var() must be the only var, return it bound
    // up in an rvalue struct. NB: the *object* must be copied since the one
    // within DDS `D' will be deleted by D's dtor.
    BaseType *btp = (*(d->var_begin()))->ptr_duplicate();
    rvalue *rv = new rvalue(btp);

    delete factory; factory = 0;
    delete d; d = 0;

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
	throw Error(malformed_expr, string("The variable `") + btp->name() 
		    + "' must be either a string or a url");
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
#if 0
	var = (BaseType *)NewInt32("dummy");
#endif
	var = table.get_factory()->NewInt32("dummy");
	var->val2buf((void *)&val.v.i);
	break;
      }

      case dods_float64_c: {
#if 0
	var = (BaseType *)NewFloat64("dummy");
#endif
	var = table.get_factory()->NewFloat64("dummy");
	var->val2buf((void *)&val.v.f);
	break;
      }

      case dods_str_c: {
#if 0
	var = (BaseType *)NewStr("dummy");
#endif
	var = table.get_factory()->NewStr("dummy");
	var->val2buf((void *)val.v.s);
	break;
      }

      default:
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

/*
 * $Log: expr.y,v $
 * Revision 1.53  2005/03/30 21:44:44  jimg
 * Now uses the BaseTypeFactory class.
 *
 * Revision 1.52  2005/01/28 21:34:20  jimg
 * Resolved conflicts from rlease-3-4-9 merge. Also minor change to expr.y
 * to support Sequence CEs that use the Array projection notation.
 *
 * Revision 1.51  2005/01/27 23:05:23  jimg
 * Modified the expression processing code in expr.y so that array-style
 * projections on the fields of Sequences work. They should project that
 * field of the Sequence. To test I will merge in new changes on the 3.4
 * branch for the dap only.
 *
 * Revision 1.50  2004/01/21 17:46:03  jimg
 * Removed include of List.h.
 *
 * Revision 1.49  2003/12/08 18:02:31  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.47.2.1  2003/09/06 23:01:46  jimg
 * Now uses the set_in_selection() method to set the in_selection property to
 * true for any variable that is either in the CE selection or is used as a
 * function argument.
 *
 * Revision 1.48  2003/05/23 03:24:57  jimg
 * Changes that add support for the DDX response. I've based this on Nathan
 * Potter's work in the Java DAP software. At this point the code can
 * produce a DDX from a DDS and it can merge attributes from a DAS into a
 * DDS to produce a DDX fully loaded with attributes. Attribute aliases
 * are not supported yet. I've also removed all traces of strstream in
 * favor of stringstream. This code should no longer generate warnings
 * about the use of deprecated headers.
 *
 * Revision 1.47  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.46  2003/02/27 23:42:32  jimg
 * Fixed a call to connect down in the code that handles the '*' operator.
 *
 * Revision 1.45  2003/02/21 00:14:25  jimg
 * Repaired copyright.
 *
 * Revision 1.44.2.1  2003/02/21 00:10:08  jimg
 * Repaired copyright.
 *
 * Revision 1.44  2003/01/23 00:22:25  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.43  2003/01/10 19:46:41  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.39.4.16  2002/12/17 22:35:03  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.39.4.15  2002/11/05 00:53:52  jimg
 * Removed 'identifier.' It was flagged as unneeded by bison.
 *
 * Revision 1.39.4.14  2002/10/28 21:17:44  pwest
 * Converted all return values and method parameters to use non-const iterator.
 * Added operator== and operator!= methods to IteratorAdapter to handle Pix
 * problems.
 *
 * Revision 1.39.4.13  2002/09/05 22:52:55  pwest
 * Replaced the GNU data structures SLList and DLList with the STL container
 * class vector<>. To maintain use of Pix, changed the Pix.h header file to
 * redefine Pix to be an IteratorAdapter. Usage remains the same and all code
 * outside of the DAP should compile and link with no problems. Added methods
 * to the different classes where Pix is used to include methods to use STL
 * iterators. Replaced the use of Pix within the DAP to use iterators instead.
 * Updated comments for documentation, updated the test suites, and added some
 * unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
 *
 * Revision 1.39.4.12  2002/07/06 20:56:06  jimg
 * Somehow the code added on 6.11.2002 that checked to ensure that the number of
 * dimensions in an array's constraint matched the number of dimensions in the
 * array was LOST. Hmmm... I added it back again. Looks like it was a casualty
 * of the libcurl migration.
 *
 * Revision 1.39.4.11  2002/06/18 23:05:33  jimg
 * Updated grammar files when replacing libwww.
 *
 * Revision 1.42  2002/06/03 22:21:16  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.39.4.8  2002/03/01 21:03:08  jimg
 * Significant changes to the var(...) methods. These now take a btp_stack
 * pointer and are used by DDS::mark(...). The exact_match methods have also
 * been updated so that leaf variables which contain dots in their names
 * will be found. Note that constructor variables with dots in their names
 * will break the lookup routines unless the ctor is the last field in the
 * constraint expression. These changes were made to fix bug 330.
 *
 * Revision 1.39.4.7  2002/02/20 19:16:27  jimg
 * Changed the expression parser so that variable names may contain only
 * digits.
 *
 * Revision 1.39.4.6  2001/11/01 00:43:51  jimg
 * Fixes to the scanners and parsers so that dataset variable names may
 * start with digits. I've expanded the set of characters that may appear
 * in a variable name and made it so that all except `#' may appear at
 * the start. Some characters are not allowed in variables that appear in
 * a DDS or CE while they are allowed in the DAS. This makes it possible
 * to define containers with names like `COARDS:long_name.' Putting a colon
 * in a variable name makes the CE parser much more complex. Since the set
 * of characters that people want seems pretty limited (compared to the
 * complete ASCII set) I think this is an OK approach. If we have to open
 * up the expr.lex scanner completely, then we can but not without adding
 * lots of action clauses to teh parser. Note that colon is just an example,
 * there's a host of characters that are used in CEs that are not allowed
 * in IDs.
 *
 * Revision 1.41  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 * Revision 1.39.4.5  2001/09/25 20:24:28  jimg
 * Added some debugging stuff to process_array_indices (and _grid_) to help
 * debug the grid() server function.
 *
 * Revision 1.39.4.4  2001/09/19 21:57:26  jimg
 * Changed no_such_ident(void *, const string &, const string &) so that it
 * calls exprerror(...) directly. The call to it's other overloaded version
 * was not working and resulted in an infinite loop.
 *
 * Revision 1.39.4.3  2001/09/06 22:04:03  jimg
 * Fixed the error message for `No such X in dataset.' I removed an extra
 * colon (Ouch...).
 *
 * Revision 1.40  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.39.4.2  2001/07/28 01:10:42  jimg
 * Some of the numeric type classes did not have copy ctors or operator=.
 * I added those where they were needed.
 * In every place where delete (or delete []) was called, I set the pointer
 * just deleted to zero. Thus if for some reason delete is called again
 * before new memory is allocated there won't be a mysterious crash. This is
 * just good form when using delete.
 * I added calls to www2id and id2www where appropriate. The DAP now handles
 * making sure that names are escaped and unescaped as needed. Connect is
 * set to handle CEs that contain names as they are in the dataset (see the
 * comments/Log there). Servers should not handle escaping or unescaping
 * characters on their own.
 *
 * Revision 1.39.4.1  2001/06/23 00:52:08  jimg
 * Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
 * that they are (more or less) the same in all the scanners. There are
 * one or two characters that differ (for example das.lex allows ( and )
 * in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
 * are essentially the same across the board.
 * Added `#' to the set of characeters allowed in an ID (bug 179).
 *
 * Revision 1.39  2000/09/22 02:17:23  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.38  2000/09/21 16:22:10  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.37  2000/09/14 10:30:20  rmorris
 * Added usage of ends and ostrstream elements in the std namespace for win32.
 *
 * Revision 1.36  2000/09/11 16:17:47  jimg
 * Added Sequence selection using row numbers. This `selection' operation
 * uses the brackets a la arrays and grids.
 *
 * Revision 1.35  2000/07/09 21:43:30  rmorris
 * Mods to increase portability, minimize ifdef's for win32
 *
 * Revision 1.34  2000/06/07 18:07:00  jimg
 * Merged the pc port branch
 *
 * Revision 1.33.14.1  2000/06/02 18:36:39  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.33.8.1  2000/02/17 05:03:17  jimg
 * Added file and line number information to calls to InternalErr.
 * Resolved compile-time problems with read due to a change in its
 * parameter list given that errors are now reported using exceptions.
 *
 * Revision 1.33  1999/07/22 17:11:52  jimg
 * Merged changes from the release-3-0-2 branch
 *
 * Revision 1.32.6.1  1999/06/07 20:03:25  edavis
 * Changed all string class usage of 'data()' to 'c_str()'.
 *
 * Revision 1.32  1999/05/21 17:20:08  jimg
 * Made the parser error messages a bit easier to decode by adding `Expression'
 * to them. Still, these are pretty lame messages...
 *
 * Revision 1.31  1999/05/04 19:47:24  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.30  1999/04/29 02:29:36  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.29  1999/04/22 22:30:52  jimg
 * Uses dynamic_cast
 *
 * Revision 1.28  1998/11/10 00:48:54  jimg
 * Changed no_such_id() to no_such_ident() (the former is used in bastring.h).
 *
 * Revision 1.27  1998/11/05 23:41:20  jimg
 * Made error message for errant CEs involving arrays better.
 * DDS::mark() now used for array variables --- this should fix a potential
 * problem with structures of arrays.
 *
 * Revision 1.26  1998/10/21 16:55:15  jimg
 * Single array element may now be refd as [<int>]. So element seven of the
 * array `a' can be referenced as a[7]. The old syntax, a[7:7], will still
 * work. Projection functions are now supported. Functions listed in the
 * projection part of a CE are evaluated (executed after parsing) as they are
 * found (before the parse of the rest of the projections or the start of the
 * parse of the selections. These functions take the same three arguments as
 * the boll and BaseType * functions (int argc, BaseType *argv[], DDS &dds)
 * but they return void. They can do whatever they like, but the use I
 * foresee is adding new (synthesized - see BaseType.cc/h) variables to the
 * DDS.
 *
 * Revision 1.25  1998/09/17 16:56:50  jimg
 * Made the error messages more verbose (that is, the text in the Error objects
 * sent back to the client).
 * Fixed a bug where non-existent fields could be accessed - with predictably
 * bad results.
 *
 * Revision 1.24.6.2  1999/02/05 09:32:36  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.24.6.1  1999/02/02 21:57:07  jimg
 * String to string version
 *
 * Revision 1.24  1998/03/19 23:22:38  jimg
 * Fixed the error messages so they use `' instead of :
 * Added Error objects for array index errors.
 * Removed old code (that was surrounded by #if 0 ... #endif).
 *
 * Revision 1.23  1998/02/05 20:14:03  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.22  1997/10/09 22:19:31  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.21  1997/10/04 00:33:05  jimg
 * Release 2.14c fixes
 *
 * Revision 1.20.6.1  1997/09/23 15:45:09  jimg
 * Fixed nasty comment bug with CVS 1.9
 *
 * Revision 1.20  1997/02/17 20:27:19  jimg
 * Fixed silly spelling errors.
 *
 * Revision 1.19  1997/02/12 19:46:33  jimg
 * Fixed bad asserts in process_array_indices and process_grid_indices.
 *
 * Revision 1.18  1997/02/10 02:32:46  jimg
 * Added assert statements for pointers
 *
 * Revision 1.17  1996/12/18 18:47:24  jimg
 * Modified the parser so that it returns Error objects for certain types of
 * errors. In order to take advantage of this, callers must examine the
 * returned object and process it as an Error object if status is false.
 *
 * Revision 1.16  1996/11/27 22:40:26  jimg
 * Added DDS as third parameter to function in the CE evaluator
 *
 * Revision 1.15  1996/10/18 16:55:15  jimg
 * Fixed the fix for bison 1.25...
 *
 * Revision 1.14  1996/10/08 17:04:43  jimg
 * Added a fix for Bison 1.25 so that PARSE_PARAM will still work
 *
 * Revision 1.13  1996/08/13 19:00:21  jimg
 * Added not_used to definition of char rcsid[].
 * Switched to the parser_arg object for communication with callers. Removed
 * unused parameters from dereference_{url, variable}, make_rvalue_list and
 * append_rvalue_list.
 *
 * Revision 1.12  1996/06/18 23:54:31  jimg
 * Fixes for Grid constraints. These include not deleting the array indices
 * lists after processing the Array component of a grid (but before processing
 * the Maps...).
 *
 * Revision 1.11  1996/06/11 17:27:11  jimg
 * Moved debug.h in front of all the other DODS includes - this ensures that
 * the debug.h included in this file is the one in effect (as opposed to a copy
 * included by some other include file). We should banish nested includes...
 * Added support for `Grid constraints'. These are like the Array constraints -
 * projections where the start, stop and stride of each dimension may be
 * specified. The new feature required a grammar change (so the parser would
 * accept grids with the array bracket notation) and two new functions:
 * is_grid_t() and process_grid_indices(). The actual projection information is
 * stored in the array members of the Grid.
 *
 * Revision 1.10  1996/05/31 23:31:04  jimg
 * Updated copyright notice.
 *
 * Revision 1.9  1996/05/29 22:08:57  jimg
 * Made changes necessary to support CEs that return the value of a function
 * instead of the value of a variable. This was done so that it would be
 * possible to translate Sequences into Arrays without first reading the
 * entire sequence over the network.
 *
 * Revision 1.8  1996/05/14 15:39:02  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.7  1996/04/05 00:22:21  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.6  1996/03/02 01:17:09  jimg
 * Added support for the complete CE spec.
 *
 * Revision 1.5  1996/02/01 17:43:18  jimg
 * Added support for lists as operands in constraint expressions.
 *
 * Revision 1.4  1995/12/09  01:07:41  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.3  1995/12/06  18:42:44  jimg
 * Added array constraints to the parser.
 * Added functions for the actions of those new rules.
 * Changed/added rule's return types.
 * Changed the types in the %union {}.
 *
 * Revision 1.2  1995/10/23  23:10:38  jimg
 * Added includes for various classes.
 * Aded rules, actions and functions for evaluation of projections.
 * Changed the value of YYSTYPE so that bison's %union feature is used -
 * rules now return several different types.
 *
 * Revision 1.1  1995/10/13  03:04:08  jimg
 * First version. Incorporates Glenn's suggestions. 
 */


