#ifndef ast_hpp
#define ast_hpp


#include "context.hpp" // needs to be on top
#include "AST/ast_node.hpp"
#include "AST/ast_expressions.hpp"
#include "AST/ast_statements.hpp"
#include "AST/ast_declarations.hpp"
#include "AST/ast_primitives.hpp"
#include "AST/ast_varb_declarations.hpp"
#include "AST/ast_program.hpp"



extern const Node *parseAST(const char* location);

#endif
