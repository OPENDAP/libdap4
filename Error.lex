
/* 
   (c) COPYRIGHT URI/MIT 1994-1996
   Please read the full copyright statement in the file COPYRIGH.  

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

/* 
 * $Log: Error.lex,v $
 * Revision 1.4  1998/08/06 16:14:47  jimg
 * Fixed the cvs log. It cannot have the $ Log $ and the comment thing on the
 * same line...
 *
 * Revision 1.3  1996/10/28 23:46:55  jimg
 * Added define of YY_NO_UNPUT to suppress creation of that function.
 *
 * Revision 1.2  1996/08/13 18:18:17  jimg
 * Added __unused__ to rcsid definition.
 * Added include of assert.h.
 *
 * Revision 1.1  1996/05/31 23:18:16  jimg
 * Added.
 *
 */

%{

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: Error.lex,v 1.4 1998/08/06 16:14:47 jimg Exp $"};

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
    
%x quote
%x comment

INT	[0-9]+

ERROR	error|Error|ERROR
CODE	code|Code|CODE
MSG	message|Message|MESSAGE
PTYPE	program_type|ProgramType|PROGRAM_TYPE|Program_Type
PROGRAM	program|Program|PROGRAM

NEVER   [^a-zA-Z0-9_/.+\-{}:;,]

%%


{ERROR}	    	    	store_string(); return ERROR;

{CODE}                  store_string(); return CODE;
{MSG}			store_string(); return MSG;
{PTYPE}			store_string(); return PTYPE;
{PROGRAM}		store_string(); return PROGRAM;

{INT}			store_integer(); return INT;

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
			  return STR;
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

int 
yywrap(void)
{
    return 1;
}

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
