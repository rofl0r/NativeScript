%{
	#include <vector>
    #include "../ast.h"

    extern int yylex();
	namespace ns {
		std::vector<ns::NFunction*> *astRootBison; /* the top level root node of our final AST */
		extern int source_line_number;
	}

    void yyerror(const char *s) { printf("Parsing error on line %d: %s\n", ns::source_line_number, s); }
	#define NS_BINARY_OP_ACTION(z,a,b,c) z = new ns::NBinaryOperator(*a, b, *c)
%}

/* Represents the many different ways we can access our data */
%union {
    ns::NExpression *expr;
	ns::NFunction* func;
	std::vector<ns::NFunction*> *funcList;
	std::vector<ns::NExpression*> *exprList;
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
%token TIF TELSE TFOR TRETURN TEXTERN
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

program : funcList { ns::astRootBison = $1; }

funcList : func { $$ = new std::vector<ns::NFunction*>(); $$->push_back($<func>1); }
		 | funcList func { $1->push_back($<func>2); }
		 ;

func : TIDENTIFIER '(' funcArgs ')' '{' stmtList TRETURN expr ';' '}' { $$ = new ns::NFunction(*$1, *$3, $8, $6); }
	 | TEXTERN TIDENTIFIER '(' funcArgs ')' {$$ = new ns::NFunction(*$2, *$4); }
	 ;

funcArgs : /*blank*/  { $$ = new std::vector<std::string*>(); }
		 | TIDENTIFIER { $$ = new std::vector<std::string*>(); $$->push_back($1); }
		 | funcArgs ',' TIDENTIFIER { $1->push_back($3); }
		 ;

exprList : /*blank*/  { $$ = new std::vector<ns::NExpression*>(); }
          | expr { $$ = new std::vector<ns::NExpression*>(); $$->push_back($1); }
          | exprList ',' expr  { $1->push_back($3); }
          ;

stmtList : /*blank*/  { $$ = new std::vector<ns::NExpression*>(); }
          | expr ';' { $$ = new std::vector<ns::NExpression*>(); $$->push_back($1); }
          | stmtList expr ';'  { $1->push_back($2); }
          ;
    
/* The binary operators must be explicitly listed to make the precedence work */
expr : TIDENTIFIER { $$ = new ns::NVariable(*$1); delete $1; }
	 | TIDENTIFIER '=' expr { $$ = new ns::NAssignment(*$1,*$3); delete $1;}
	 | expr '+' expr { NS_BINARY_OP_ACTION($$,$1,'+',$3); }
	 | expr '-' expr { NS_BINARY_OP_ACTION($$,$1,'-',$3); }
	 | expr '*' expr { NS_BINARY_OP_ACTION($$,$1,'*',$3); }
	 | expr '<' expr { NS_BINARY_OP_ACTION($$,$1,'<',$3); }
	 | expr TEQUAL expr { NS_BINARY_OP_ACTION($$,$1,TEQUAL,$3); }
     | TNUMBER {$$ = new ns::NNumber($1);}
     | '(' expr ')' { $$ = $2; }
	 | TIF '(' expr ')' expr TELSE expr { $$ = new ns::NCondition(*$3,*$5,*$7); }
	 | TFOR '(' TIDENTIFIER '=' expr ',' expr ',' expr ')' expr { $$ = new ns::NForloop(*$3,*$5,*$7,*$9,*$11); }
	 | TIDENTIFIER '(' exprList ')' { $$ = new ns::NFunctionCall(*$1,*$3); }
     ;
		 
%%

