//This is an unrequired file. This will parse a c file as an ast, and then reprint the AST.
//It exists purely for coding

#include "ast.hpp"
#include <iostream>
#include<cstdlib> //Required for exit
int main(int argc, char *argv[]){
	if(argc<2){
		std::cerr<<"ERROR, expected more arguments"<<std::endl;
		std::exit(1);
	}
	const Node *ast=parseAST(argv[1]); //This function won't really return anything
	//error checking
	std::cerr<<"I parsed the tree"<<std::endl;
	ast->print(std::cout); //Print implemented! Very basic though
	std::cerr<<"I am trying to print"<<std::endl;
    std::cout<<std::endl;
	
	return 0;
}
