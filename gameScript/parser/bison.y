%{
    #include "../ast.h"
	#include <vector>

    extern int yylex();
	namespace gs {
		std::vector<gs::NFunction*> *astRootBison; /* the top level root node of our final AST */
		extern int source_line_number;
	}

    void yyerror(const char *s) { printf("Parsing error on line %d: %s\n", gs::source_line_number, s); }
	#define BINARY_OP_ACTION(z,a,b,c) z = new gs::NBinaryOperator(*a, b, *c)
%}

/* Represents the many different ways we can access our data */
%union {
    gs::NExpression *expr;
	gs::NFunction* func;
	std::vector<gs::NFunction*> *funcList;
	std::vector<gs::NExpression*> *exprList;
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
%token '(' ')' '{' '}'
%token TIF TELSE TFOR TRETURN TDECLARE
%token '+' '-' '*' '<' TEQUAL
%token ',' '=' ';'

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <funcList> funcList
%type <exprList> exprList
%type <exprList> stmtList
%type <funcArgs> funcArgs
%type <expr> expr
%type <func> func

/* Operator precedence for mathematical operators */
%left '='
%left '<' TEQUAL
%left '+' '-'
%left '*'

%start program

%%

program : funcList { gs::astRootBison = $1; }

funcList : func { $$ = new std::vector<gs::NFunction*>(); $$->push_back($<func>1); }
		 | funcList func { $1->push_back($<func>2); }
		 ;

func : TIDENTIFIER '(' funcArgs ')' '{' stmtList TRETURN expr ';' '}' { $$ = new gs::NFunction(*$1, *$3, $8, $6); }
	 | TDECLARE TIDENTIFIER '(' funcArgs ')' {$$ = new gs::NFunction(*$2, *$4); }
	 ;

funcArgs : /*blank*/  { $$ = new std::vector<std::string*>(); }
		 | TIDENTIFIER { $$ = new std::vector<std::string*>(); $$->push_back($1); }
		 | funcArgs ',' TIDENTIFIER { $1->push_back($3); }
		 ;

exprList : /*blank*/  { $$ = new std::vector<gs::NExpression*>(); }
          | expr { $$ = new std::vector<gs::NExpression*>(); $$->push_back($1); }
          | exprList ',' expr  { $1->push_back($3); }
          ;

stmtList : /*blank*/  { $$ = new std::vector<gs::NExpression*>(); }
          | expr ';' { $$ = new std::vector<gs::NExpression*>(); $$->push_back($1); }
          | stmtList expr ';'  { $1->push_back($2); }
          ;
    
/* The binary operators must be explicitly listed to make the precedence work */
expr : TIDENTIFIER { $$ = new gs::NVariable(*$1); delete $1; }
	 | TIDENTIFIER '=' expr { $$ = new gs::NAssignment(*$1,*$3); delete $1;}
	 | expr '+' expr { BINARY_OP_ACTION($$,$1,'+',$3); }
	 | expr '-' expr { BINARY_OP_ACTION($$,$1,'-',$3); }
	 | expr '*' expr { BINARY_OP_ACTION($$,$1,'*',$3); }
	 | expr '<' expr { BINARY_OP_ACTION($$,$1,'<',$3); }
	 | expr TEQUAL expr { BINARY_OP_ACTION($$,$1,TEQUAL,$3); }
     | TNUMBER {$$ = new gs::NNumber($1);}
     | '(' expr ')' { $$ = $2; }
	 | TIF '(' expr ')' expr TELSE expr { $$ = new gs::NCondition(*$3,*$5,*$7); }
	 | TFOR '(' TIDENTIFIER '=' expr ',' expr ',' expr ')' expr { $$ = new gs::NForloop(*$3,*$5,*$7,*$9,*$11); }
	 | TIDENTIFIER '(' exprList ')' { $$ = new gs::NFunctionCall(*$1,*$3); }
     ;
		 
%%

