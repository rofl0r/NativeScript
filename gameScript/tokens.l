%{
#include <string>
#include "ast.h"
#include "parser.h"

extern int yyparse();

#define DEBUG 
#define DEBUG_BCK fprintf(stdout, "tokenized: %s\n", yytext)
%}

%option noyywrap

%%

[ \t\n]                 ;
[a-zA-Z_][a-zA-Z0-9_]*  DEBUG; yylval.string = new std::string(yytext, yyleng); return TIDENTIFIER;
[0-9]+(\.[0-9]+)?       DEBUG; yylval.num = atof(yytext); return TNUMBER;
"("                     DEBUG; return TLPAREN;
")"                     DEBUG; return TRPAREN;
"+"                     DEBUG; return '+';
"-"                     DEBUG; return '-';
"*"                     DEBUG; return '*';
.                       printf("Unknown token!\n"); yyterminate();

%%

namespace gs {
	void scan_string(const char* str)
	{
		yypush_buffer_state(YY_CURRENT_BUFFER);
		YY_BUFFER_STATE bs = yy_scan_string(str);
		yyparse();
		yy_delete_buffer(bs);
		yypop_buffer_state();
		yyrestart(yyin);
	}
}
