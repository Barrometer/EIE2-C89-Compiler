#ifndef ast_statements_hpp
#define ast_statements_hpp

#include "ast_expressions.hpp"
#include <vector>
#include <iostream> 

class Statement : public Node {
	//Once we go over how to separate Expression Statement and ReturnStatement it should be good
};

typedef const Statement *StatementPtr;

class ExpressionStatement : public Statement {

	protected:
		ExpressionPtr expr;
	public:	
		ExpressionStatement(ExpressionPtr _expr) : expr(_expr){} // constructor
//need to add evaluater
		virtual void print(std::ostream &dst) const override {
		expr->print(dst);
		dst<<";"<<std::endl;
	}
	

//TODO add functionality to this class
};

//TODO add more statements, eg Return statement, if statement
class ReturnStatement : public Statement { // added 28/02/18. I think this is the correct format?
	protected:
		ExpressionPtr ret;
	public:
		ReturnStatement(ExpressionPtr _ret) : ret(_ret){}
		ExpressionPtr getReturnExpression() const{return ret;} 
		//print, evaluate, translate, compile, etc
		virtual void print(std::ostream &dst) const override{
			dst<<"return ";
			ret->print(dst);
			dst<<";";
			dst<<std::endl;
		}
};

//TODO make Compound Statement and Statement List work

class StatementList ;
typedef const StatementList *StatementListPtr;

class StatementList : public Statement
{ //lol, rewrite
	protected: 
		StatementPtr current;
		StatementPtr next;
	public:
		//constructor with no next list
		StatementList(StatementPtr _current) :current(_current),next(NULL){ // wip constructor
			std::cerr<<"In constructor for StatementList with no next Statement List"<<std::endl;		 
		}
		//constructor with next list
		StatementList(StatementPtr _current, StatementPtr _next) :current(_current),next(_next){
			std::cerr<<"In constructor for StatementList with next Statement List"<<std::endl;
		}
	//will have printer, translator, etc
	//will simply call the function of those beneath
	
	virtual void print(std::ostream &dst) const override {
		std::cerr<<"Print on statement list got called"<<std::endl;
		current->print(dst);
		if(next!=NULL){
			next->print(dst);
		}
		
		std::cerr<<"Print on statement list successfully finished"<<std::endl;
	}
};



class CompoundStatement : public Node{ 

	protected:
		//TODO DeclListPtr dref
		StatementPtr sref;
		
	public:
		CompoundStatement(StatementPtr _sref) : sref(_sref){std::cerr<<"In constructor for CompoundStatement with no decl list"<<std::endl;}		
		virtual void print(std::ostream &dst) const override {
			//dref->print(dst);
			sref->print(dst);
		}

};

typedef const CompoundStatement *CompStatementPtr;


#endif
