%{
    #include "ast.h"
	#include <vector>
    std::vector<gs::NFunction*> *astRoot; /* the top level root node of our final AST */

    extern int yylex();

    void yyerror(const char *s) { printf("Parsing error: %s\n", s); }
	#define BINARY_OP_ACTION(z,a,b,c) z = new gs::NBinaryOperator(*a, b, *c)
%}

/* Represents the many different ways we can access our data */
%union {
    gs::NExpression *expr;
	gs::NFunction *func;
	std::vector<gs::NFunction*> *funcList;
	std::vector<std::string*> *funcArgs;
    std::string *string;
	double num;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER 
%token <num> TNUMBER
%token TLPAREN TRPAREN
%token '+' '-' '*'

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <funcList> funcList
%type <funcArgs> funcArgs
%type <expr> expr
%type <func> func

/* Operator precedence for mathematical operators */
%left '+' '-'
%right '*'

%start program

%%

program : funcList { astRoot = $1; }

funcList : func { $$ = new std::vector<gs::NFunction*>(); $$->push_back($<func>1); }
		 | funcList func { $1->push_back($<func>2); }
		 ;

func : TIDENTIFIER TLPAREN funcArgs TRPAREN expr  { $$ = new gs::NFunction(*$1, *$3, *$5); }
	 ;

funcArgs : TIDENTIFIER { $$ = new std::vector<std::string*>(); $$->push_back($1); }
		 | funcArgs TIDENTIFIER { $1->push_back($2); }
		 ;
    
/* The binary operators must be explicitly listed to make the precedence work */
expr : TIDENTIFIER { $$ = new gs::NIdentifier(*$1); delete $1; }
	 | expr '+' expr { BINARY_OP_ACTION($$,$1,'+',$3); }
	 | expr '-' expr { BINARY_OP_ACTION($$,$1,'-',$3); }
	 | expr '*' expr { BINARY_OP_ACTION($$,$1,'*',$3); }
     | TNUMBER {$$ = new gs::NNumber($1);}
     | TLPAREN expr TRPAREN { $$ = $2; }
     ;
		 
%%

