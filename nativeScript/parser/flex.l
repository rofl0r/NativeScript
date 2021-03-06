%{
#include <string>

#include "tokens.h"

extern int yyparse();

#define DEBUG 
#define DEBUG_BCK fprintf(stdout, "tokenized: %s\n", yytext)

namespace ns {
	int source_line_number = 1;
}
%}

%option noyywrap
%option nounistd
%option never-interactive

%%

[ \t]                   ;
\/\/[^\n]*\n			ns::source_line_number++;
[\n]					ns::source_line_number++;
"if"					DEBUG; return TIF;
"else"					DEBUG; return TELSE;
"external"				DEBUG; return TEXTERN;
"for"					DEBUG; return TFOR;
"return"				DEBUG; return TRETURN;
[a-zA-Z_][a-zA-Z0-9_]*  DEBUG; yylval.string = new std::string(yytext, yyleng); return TIDENTIFIER;
[0-9]+(\.[0-9]+)?       DEBUG; yylval.num = atof(yytext); return TNUMBER;
[\(\)\{\}\+\*-<=,;]     DEBUG; return yytext[0];
"=="					DEBUG; return TEQUAL;
.                       printf("Unknown token: \"%s\" on line %d!\n", yytext, ns::source_line_number); yyterminate();

%%

namespace ns {
	void scan_string(const char* str)
	{
		yypush_buffer_state(YY_CURRENT_BUFFER);
		YY_BUFFER_STATE bs = yy_scan_string(str);
		yyparse();
		yy_delete_buffer(bs);
		yypop_buffer_state();
		yyrestart(yyin);
		source_line_number = 1;
	}

	void scan_file(FILE* f) 
	{
		yyin = f;
		do {
			yyparse();
		} while (!feof(yyin));
		source_line_number = 1;
	}
}
