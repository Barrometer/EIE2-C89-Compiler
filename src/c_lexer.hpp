#ifndef c_lexer.hpp
#define c_lexer.hpp


#include <string>

enum TokenType{
    None		= 0, // This indicates there are no more tokens
    Int			= 1, // token code. Techically a constant 
    Char		= 2, //Also constant
	Float		= 3, //Also constant
    Enumerator	= 4, //A type of constant. No clue what it is though
    Operator	= 5,
    Keyword		= 6, // ie stuff like for, if etc. May later be split into additonal cases
    Identifier	= 7, // ie variable names
	Punctuator	= 8,
	StringLiteral //implicityl 9
	
	
	
    
    // THIS IS A WIP list, will need to edit
    
    
};

union TokenValue{
    double numberValue;
    std::string *wordValue;
};

// This is a global variable used to move the
// attribute value from the lexer back to the
// main program.
// By convention it is called yylval, as that is
// the name that will be automatially generated
// by Bison (see next lab).
extern TokenValue yylval;

// This is the lexer function defined by flex. Each
// time it is called, a token type value will be
// returned.
extern int yylex();

#endif
