/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SCAN_ATTR = 258,
     SCAN_WORD = 259,
     SCAN_ALIAS = 260,
     SCAN_BYTE = 261,
     SCAN_INT16 = 262,
     SCAN_UINT16 = 263,
     SCAN_INT32 = 264,
     SCAN_UINT32 = 265,
     SCAN_FLOAT32 = 266,
     SCAN_FLOAT64 = 267,
     SCAN_STRING = 268,
     SCAN_URL = 269
   };
#endif
#define SCAN_ATTR 258
#define SCAN_WORD 259
#define SCAN_ALIAS 260
#define SCAN_BYTE 261
#define SCAN_INT16 262
#define SCAN_UINT16 263
#define SCAN_INT32 264
#define SCAN_UINT32 265
#define SCAN_FLOAT32 266
#define SCAN_FLOAT64 267
#define SCAN_STRING 268
#define SCAN_URL 269




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE daslval;



