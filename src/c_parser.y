%code requires{
  #include "ast.hpp"
  #include <string>
  #include <cassert>
  #include <iostream>
  extern FILE *yyin; //hopefully allows reading from not stdin
  extern const Node *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  const Node *node;
  const Expression *expression;
  const Statement *statement;
  const Declaration *declaration;
  int number;
  std::string *string;
}

//Need to put all token types here

%token K_INT K_RETURN  //Keywords. These are the ones needed for my minimal lexer / parser
%token K_IF K_ELSE K_CHAR K_FLOAT K_FOR K_WHILE K_VOID//more keyowords, not needed for minimal parser / lexer
%token O_PLUS O_EQUALS O_MINUS O_ASTR O_DIV //Arithmetic Operators (and pointer I guess). Minimal ones for parser / lexer
%token L_IS_EQUAL L_IS_NOT_EQUAL L_AND L_OR L_NOT L_GTHAN L_LTHAN L_GETHAN L_LETHAN//Logical operators
%token B_AND B_OR B_NOT B_XOR B_LSHIFT B_RSHIFT //Bitwise operators
%token P_LHEADER P_RHEADER P_LSQBRAC P_RSQBRAC P_LCURLBRAC P_RCURLBRAC P_LBRACKET P_RBRACKET // punctuators
%token P_LIST_SEPARATOR P_STATEMENT_LABEL P_STATEMENT_END P_VARIABLE_LENGTH_ARGUMENT_LIST P_INCLUDE P_CHAR_CONST //more punctuators, not sure if needed?
%token T_INT T_IDENTIFIER //Types. Minimal ones for parser / lexer


%type <node> PROGRAM FNC_DEC  COMPOUND_STATEMENT  PARAMETER_LIST PARAMETER VAR_LIST DECL_GLOB
%type <number> T_INT
%type <string> T_IDENTIFIER K_INT K_VOID //K_CHAR K_FLOAT
%type <expression> EXPRESSION  ASSIGNMENT_EXPR CONSTANT  FNC_CALL LEVEL_1 LEVEL_2 LEVEL_3 LEVEL_4 LEVEL_5 LEVEL_6 LEVEL_7 LEVEL_8 LEVEL_9 LEVEL_10 LEVEL_11 LEVEL_12
%type <statement> STATEMENT RETURN_STATEMENT EXPR_STATEMENT STATEMENT_LIST IF_STATEMENT  WHILE_STATEMENT IF_ELSE_STATEMENT SCOPE_STATEMENT
%type <declaration>  DECL_LIST DECL_LOCAL
/*
*/

%start ROOT

%%

ROOT : PROGRAM { g_root = $1; }

 
PROGRAM	: PROGRAM FNC_DEC {$$ = new Program($2,$1);} // oh god, another layer of abstraction	
	|DECL_GLOB PROGRAM {$$ = new Program($2,$1);}
	| FNC_DEC	{$$=$1;}
	|DECL_GLOB {$$=$1;}
	
DECL_GLOB : K_INT T_IDENTIFIER P_STATEMENT_END {$$ = new DeclGlobal(*$1,*$2);}
		| K_INT T_IDENTIFIER O_EQUALS EXPRESSION P_STATEMENT_END {$$ = new DeclGlobal(*$1,*$2,$4);}

//If I was feeling clever, would alter this. Originally we only supported integers, void was added afterwards. There should be another layer of abstraction	
FNC_DEC : K_INT T_IDENTIFIER P_LBRACKET P_RBRACKET P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC {$$ = new FunctionDecl(*$1, *$2, $6);} 
		| K_INT T_IDENTIFIER P_LBRACKET PARAMETER_LIST P_RBRACKET P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC {$$ = new FunctionDecl(*$1, *$2, $7, $4);}
		| K_VOID T_IDENTIFIER P_LBRACKET P_RBRACKET P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC {$$ = new FunctionDecl(*$1, *$2, $6);}
		| K_VOID T_IDENTIFIER P_LBRACKET PARAMETER_LIST P_RBRACKET P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC {$$ = new FunctionDecl(*$1, *$2, $7, $4);}

PARAMETER_LIST : PARAMETER_LIST P_LIST_SEPARATOR PARAMETER {$$ = new ParamList($3,$1);}
	| PARAMETER {$$=$1;}
					
PARAMETER	: K_INT T_IDENTIFIER {$$ = new Param(*$1,*$2);}


CONSTANT : T_INT {$$ = new IntLiteral($1);} 

COMPOUND_STATEMENT : STATEMENT_LIST {$$ = new CompoundStatement($1);} 
		| DECL_LIST {$$ = new CompoundStatement($1);}
		| DECL_LIST STATEMENT_LIST {$$ = new CompoundStatement($2,$1);}
		
DECL_LIST : DECL_LIST DECL_LOCAL {$$ = new DeclList($2,$1);}
		| DECL_LOCAL {$$=$1;}
		
DECL_LOCAL : K_INT T_IDENTIFIER P_STATEMENT_END {$$ = new DeclLocal(*$1,*$2);}
		| K_INT T_IDENTIFIER O_EQUALS EXPRESSION P_STATEMENT_END {$$ = new DeclLocal(*$1,*$2,$4);}
	
//A statement list has a pointer to the current statement, and a pointer to another statement list / node, yay
STATEMENT_LIST : STATEMENT_LIST STATEMENT {$$ = new StatementList($2,$1);}
			|   STATEMENT {$$=$1;}

STATEMENT : RETURN_STATEMENT {$$=$1;}
	| EXPR_STATEMENT {$$=$1;}
	| IF_STATEMENT {$$=$1;}
	| IF_ELSE_STATEMENT {$$=$1;}
	| WHILE_STATEMENT {$$=$1;}
	| SCOPE_STATEMENT {$$=$1;} // new addition
	
SCOPE_STATEMENT : 	P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC {$$ = new ScopeStatement($2);}
			
IF_STATEMENT : K_IF P_LBRACKET EXPRESSION P_RBRACKET P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC {$$ = new IfStatement($3,$6);}
	
IF_ELSE_STATEMENT: K_IF P_LBRACKET EXPRESSION P_RBRACKET P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC K_ELSE P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC {$$= new IfElseStatement($3,$6,$10);}

WHILE_STATEMENT : K_WHILE P_LBRACKET EXPRESSION P_RBRACKET P_LCURLBRAC COMPOUND_STATEMENT P_RCURLBRAC {$$ = new WhileStatement($3, $6);}

RETURN_STATEMENT : K_RETURN EXPRESSION P_STATEMENT_END { $$ = new ReturnStatement($2); }

EXPR_STATEMENT : EXPRESSION P_STATEMENT_END {$$ = new ExpressionStatement($1);}
//Somewhere in here we're going to need to differentiate between special cases and baics
//Everything so far is basic, assignments, 


//where we're going, we don't need comments

LEVEL_12 : LEVEL_12 L_OR LEVEL_11 {$$ = new LOrOperator($1, $3);}
	| LEVEL_11 {$$=$1;}

LEVEL_11 : LEVEL_11 L_AND LEVEL_10 {$$ = new LAndOperator($1, $3);}
	| LEVEL_10 {$$=$1;}

LEVEL_10 :  LEVEL_10 B_OR LEVEL_9 {$$ = new BOrOperator($1, $3);}
	| LEVEL_9 {$$=$1;}

LEVEL_9 : LEVEL_9 B_XOR LEVEL_8 {$$ = new XorOperator($1, $3);}
	| LEVEL_8 {$$=$1;}

LEVEL_8 : LEVEL_8 B_AND LEVEL_7 {$$ = new BAndOperator($1, $3);}
	| LEVEL_7 {$$=$1;}

LEVEL_7 : LEVEL_7 L_IS_EQUAL LEVEL_6 {$$ = new EqualToOperator($1, $3);}
	| LEVEL_7 L_IS_NOT_EQUAL LEVEL_6 {$$ = new NotEqualOperator($1, $3);}
	| LEVEL_6 {$$=$1;}

LEVEL_6 : LEVEL_6 L_GTHAN LEVEL_5 {$$ = new GThanOperator($1, $3);}
	| LEVEL_6 L_LTHAN LEVEL_5 {$$ = new LThanOperator($1, $3);}
	| LEVEL_6 L_GETHAN LEVEL_5 {$$ = new GEThanOperator($1, $3);}
	| LEVEL_6 L_LETHAN LEVEL_5 {$$ = new LEThanOperator($1, $3);}
	| LEVEL_5 {$$=$1;}

LEVEL_5 : LEVEL_5 B_LSHIFT LEVEL_4 {$$ = new LShiftOperator($1, $3);}
	| LEVEL_5 B_RSHIFT LEVEL_4 {$$ = new RShiftOperator($1, $3);}
	| LEVEL_4 {$$=$1;}

LEVEL_4 : LEVEL_4 O_PLUS LEVEL_3 {$$ = new AddOperator($1, $3);}
	| LEVEL_4 O_MINUS LEVEL_3 {$$ = new SubOperator($1, $3);}
	| LEVEL_4 LEVEL_3 {$$= new AddOperator($1, $2);}
	| LEVEL_3 {$$=$1;}

LEVEL_3 : LEVEL_3 O_ASTR LEVEL_2 {$$ = new MulOperator($1, $3);}
	| LEVEL_3 O_DIV LEVEL_2 {$$ = new DivOperator($1, $3);}
	| LEVEL_2 {$$=$1;}

LEVEL_2 : L_NOT LEVEL_1 {$$ = new NotOperator($2, $2);}
	| B_NOT LEVEL_1 {$$ = new BNotOperator($2,$2);}
	| LEVEL_1 {$$=$1;}

LEVEL_1 : CONSTANT {$$=$1;}
	| T_IDENTIFIER {$$ = new Identifier(*$1);}
	| P_LBRACKET EXPRESSION P_RBRACKET {$$ = $2;}
	| FNC_CALL {$$=$1;}



EXPRESSION : ASSIGNMENT_EXPR {$$=$1;}

	| LEVEL_12 {$$=$1;}
	

ASSIGNMENT_EXPR : T_IDENTIFIER O_EQUALS EXPRESSION {$$ = new AssignmentExpression(*$1,$3);}

FNC_CALL : T_IDENTIFIER P_LBRACKET P_RBRACKET {$$ = new FunctionCall(*$1);}
	|  T_IDENTIFIER P_LBRACKET VAR_LIST P_RBRACKET {$$ = new FunctionCall(*$1, $3);}

VAR_LIST : VAR_LIST P_LIST_SEPARATOR T_IDENTIFIER {$$ = new VarList(*$3,$1);}	
	| VAR_LIST P_LIST_SEPARATOR T_INT {$$ = new VarList(std::to_string($3),$1);} // if we supported other types this wouldn't be T_INT
											//maybe condense back into something else, T_INT
											//and T_VAR both as some other layer
											//or just leave it in and forget about it
	| T_IDENTIFIER {$$=new VarList(*$1);}
	| T_INT {$$=new VarList(std::to_string($1));}
	
	


%%
const Node *g_root; // The top of the program is a node. Might be better type?

const Node *parseAST(const char* location) //This function returns the tree
{
	
	yyin = fopen(location,"r");
	g_root=0;
	yyparse();
	return g_root;
}

