
/* 
   (c) COPYRIGHT URI/MIT 1994-1996,1998,1999
   Please read the full copyright statement in the file COPYRIGHT.

   Authors:
        jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
*/

/*
  Scanner for the Error object. It recognizes the five keywords in the
  persistent representation of the Error object plus some syntactic sugar
  (`=', `{', ...). The object's persistant representation uses a keyword =
  value notation, where the values are quoted strings or integers.

  The scanner is not reentrant, but can share name spaces with other
  scanners. It must be processed by GNU's flex scanner generator.
*/

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: Error.lex,v 1.8 2001/08/24 17:46:22 jimg Exp $"};

#include <string.h>
#include <assert.h>

#define YY_NO_UNPUT
#define YY_DECL int Errorlex YY_PROTO(( void ))

#include "Error.tab.h"

int error_line_num = 1;
static int start_line;		/* used in quote and comment error handlers */

void store_integer();
void store_string();

%}
    
%option noyywrap
%x quote
%x comment

SCAN_INT		[0-9]+

SCAN_ERROR		error|Error|ERROR
SCAN_CODE		code|Code|CODE
SCAN_MSG		message|Message|MESSAGE
SCAN_PTYPE		program_type|ProgramType|PROGRAM_TYPE|Program_Type
SCAN_PROGRAM	program|Program|PROGRAM

NEVER   [^a-zA-Z0-9_/.+\-{}:;,]

%%


{SCAN_ERROR}	store_string(); return SCAN_ERROR;

{SCAN_CODE}				store_string(); return SCAN_CODE;
{SCAN_MSG}				store_string(); return SCAN_MSG;
{SCAN_PTYPE}			store_string(); return SCAN_PTYPE;
{SCAN_PROGRAM}			store_string(); return SCAN_PROGRAM;

{SCAN_INT}				store_integer(); return SCAN_INT;

"{" 	    	    	return (int)*yytext;
"}" 	    	    	return (int)*yytext;
";" 	    	    	return (int)*yytext;
"="			return (int)*yytext;

[ \t]+
\n	    	    	++error_line_num;
<INITIAL><<EOF>>    	yy_init = 1; error_line_num = 1; yyterminate();

"#"	    	    	BEGIN(comment);
<comment>[^\n]*
<comment>\n		++error_line_num; BEGIN(INITIAL);
<comment><<EOF>>        yy_init = 1; error_line_num = 1; yyterminate();

\"			BEGIN(quote); start_line = error_line_num; yymore();
<quote>[^"\n\\]*	yymore();
<quote>[^"\n\\]*\n	yymore(); ++error_line_num;
<quote>\\.		yymore();
<quote>\"		{ 
    			  BEGIN(INITIAL); 
			  store_string();
			  return SCAN_STR;
                        }
<quote><<EOF>>		{
                          char msg[256];
			  sprintf(msg,
				  "Unterminated quote (starts on line %d)\n",
				  start_line);
			  YY_FATAL_ERROR(msg);
                        }

{NEVER}                 {
                          if (yytext) {	/* suppress msgs about `' chars */
                            fprintf(stderr, "Character `%c' is not", *yytext);
                            fprintf(stderr, " allowed (except within");
			    fprintf(stderr, " quotes) and has been ignored\n");
			  }
			}
%%

void
store_integer()
{
    Errorlval.integer = atoi(yytext);
}

void
store_string()
{
    Errorlval.string = yytext;
}

/* 
 * $Log: Error.lex,v $
 * Revision 1.8  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.7.4.1  2001/08/16 17:26:19  edavis
 * Use "%option noyywrap" instead of defining yywrap() to return 1.
 *
 * Revision 1.7  2000/09/22 02:17:20  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.6  2000/06/07 18:06:58  jimg
 * Merged the pc port branch
 *
 * Revision 1.5.20.1  2000/06/02 18:21:27  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.5  1999/04/29 02:29:29  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.4.2.1  1999/02/05 09:32:34  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined math code
 *
 * Revision 1.4  1998/08/06 16:14:47  jimg
 * Fixed the cvs log. It cannot have the $ Log $ and the comment thing on the
 * same line...
 *
 * Revision 1.3  1996/10/28 23:46:55  jimg
 * Added define of YY_NO_UNPUT to suppress creation of that function.
 *
 * Revision 1.2  1996/08/13 18:18:17  jimg
 * Added not_used to rcsid definition.
 * Added include of assert.h.
 *
 * Revision 1.1  1996/05/31 23:18:16  jimg
 * Added.
 */
