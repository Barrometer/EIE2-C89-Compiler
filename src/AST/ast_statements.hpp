#ifndef ast_statements_hpp
#define ast_statements_hpp

#include <iostream> 
#include <vector>
#include <iostream> 
#include <string>


extern int unique_name;

class Statement : public Node {
	//Once we go over how to separate Expression Statement and ReturnStatement it should be good
};

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
		virtual void translate(std::ostream &dst, int indent) const override {
			std::cerr<<"_____stateEXPR1_____"<<std::endl;
			std::cerr<<"Adding indent, indent is currently "<<indent<<std::endl;
			for(int i=0; i<indent;i++){ //Shold make a function / member function, quick hack for now
				dst<<" ";
			}
			expr->translate(dst,indent);
			std::cerr<<"_____stateEXPR2_____"<<std::endl;
			dst<<std::endl;
		}
		/*virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override{
			expr->compile(dst,bindings,regs,destReg,returnLoc);
		}*/
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			
			expr->compile(dst,bindings,regs,destReg,returnLoc);
		}		
		virtual void explore(int & declarations, Context & bindings) const override{
			std::cerr<<"An expression statement can't contain a declaration, stopping"<<std::endl;
		}
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
		virtual void translate(std::ostream &dst, int indent) const override {
			std::cerr<<"_____stateRETURN1_____"<<std::endl;
			std::cerr<<"Adding indent, indent is currently "<<indent<<std::endl;
			for(int i=0; i<indent;i++){//Shold make a function / member function, quick hack for now
				dst<<" ";
			}
			dst<<"return ";
			ret->translate(dst,indent);
			std::cerr<<"_____stateRETURN2_____"<<std::endl;
			dst<<std::endl;
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override{
			std::cerr<<"Returning in compile"<<std::endl;
			destReg="$2";
			regs.ReserveRegister(2);
			ret->compile(dst, bindings, regs, destReg,returnLoc);
			dst<<"j "<<returnLoc<<std::endl;
			dst<<"nop"<<std::endl;
			std::cerr<<"YO"<<std::endl;
			regs.ReleaseRegister(2);
			destReg="NULL";
			dst<<std::endl;
			//destReg will be $2 here
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			std::cerr<<"A return statement can't contain a declaration, stopping"<<std::endl;
		}
};

class StatementList ;


class StatementList : public Statement
{ 
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
		if(next!=NULL){
			next->print(dst);
		}
		current->print(dst);
		std::cerr<<"Print on statement list successfully finished"<<std::endl;
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		std::cerr<<"_____stateLIST1_____"<<std::endl;
		if(next!=NULL){
			next->translate(dst,indent);
		}
		std::cerr<<"_____stateLIST2_____"<<std::endl;
		current->translate(dst,indent);
		std::cerr<<"_____stateLIST3_____"<<std::endl;
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override{
		if(next!=NULL){
			next->compile(dst,bindings,regs,destReg,returnLoc);
		}
		current->compile(dst,bindings,regs,destReg,returnLoc);
	}
	
	virtual void explore(int & declarations, Context & bindings) const override{
		if(next!=NULL){
			next->explore(declarations,bindings);
		}
		current->explore(declarations,bindings);
	}
};

class ScopeStatement : public Statement{

	protected:
		NodePtr body; 	/* a scope statement is just a pointer to a compound statement really. comp statement is a node, so can't directly slot into StatementList without more work
							and I don't want to to run the risk of breaking things at this stage.
							*/
	public:
		ScopeStatement(NodePtr _body) :body(_body){}
		virtual void print(std::ostream &dst) const override {std::cerr<<"Not implemented for ScopeStatement"<<std::endl;}
		virtual void translate(std::ostream &dst, int indent) const override {std::cerr<<"By the spec, Python doesn't need to deal with nested scopes"<<std::endl;}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		
			body->compile(dst,bindings,regs,destReg,returnLoc); // nothing fancy, just compile the compound statement I point to
			
		}
		virtual void explore(int & declarations, Context & bindings) const override{
		
			body->explore(declarations,bindings); // just explore the compount statement
		
		}
};


class IfStatement : public Statement {

	protected:
		ExpressionPtr condition; // the execute condition
		NodePtr body; // actually a compound statement, the body of the if
	public:
		IfStatement(ExpressionPtr _condition, NodePtr _body) : condition(_condition), body(_body) {std::cerr<<"If statement constructor"<<std::endl;}
		virtual void print(std::ostream &dst) const override {//if case exists
				
			dst << "if ( ";		//this won't work until bindings map is made
			condition->print(dst);
			dst << " ) { " << std::endl;
			body->print(dst);
			dst << "}";
			dst << std::endl;
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			for(int i=0; i<indent;i++){ //Shold make a function / member function, quick hack for now
				dst<<" ";
			}		
			dst << "if ";
			condition->translate(dst, indent);
			dst << " :" << std::endl;
			body->translate(dst, indent+4);
			dst << std::endl;
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			int x = unique_name;
			unique_name++;
			std::string if_t = "$if_s"+std::to_string(x); // start of body
			std::string if_f = "$if_f"+std::to_string(x); // end of body
			int y = regs.EmptyRegister(); // number of first free register
			regs.ReserveRegister(y); // mark register as used
			std::string condReg = "$"+std::to_string(y);
			condition->compile(dst,bindings,regs,condReg,returnLoc); // compile the condition, get the result into condReg
			dst<<"bne	$0, "<<condReg<<", "<<if_t<<std::endl; //If cond not zero, branch to body
			regs.ReleaseRegister(y);
			dst<<"nop"<<std::endl;
			dst<<"b	"<<if_f<<std::endl;
			dst<<"nop"<<std::endl;
			dst<<std::endl;
			dst<<if_t<<":"<<std::endl;
			body->compile(dst,bindings,regs,destReg,returnLoc);
			dst<<"b	"<<if_f<<std::endl;
			dst<<"nop"<<std::endl;
			dst<<std::endl;
			dst<<if_f<<":"<<std::endl;
			std::cerr<<"For testing, I left if here"<<std::endl;
		}
	
		virtual void explore(int & declarations, Context & bindings) const override{
			body->explore(declarations,bindings);
		}
};

class IfElseStatement : public Statement{
	
	protected: 	
		ExpressionPtr condition; // the execute condition
		NodePtr body_t; // actually a statement list, the body of the if
		NodePtr body_f;
	public:
		IfElseStatement(ExpressionPtr _condition, NodePtr _t, NodePtr _f) :condition(_condition), body_t(_t), body_f(_f){}
		virtual void print(std::ostream &dst) const override{std::cerr<<"IFELSE not print implemented"<<std::endl;}
		virtual void translate(std::ostream &dst, int indent) const override {
			for(int i=0; i<indent;i++){ //Shold make a function / member function, quick hack for now
				dst<<" ";
			}
			dst << "if ";
			condition->translate(dst, indent);
			dst << " :" << std::endl;
			body_t->translate(dst, indent+4);
			dst << std::endl;
			for(int i=0; i<indent;i++){ //Shold make a function / member function, quick hack for now
				dst<<" ";
			}
			dst << "else :" << std::endl;
			body_f->translate(dst, indent+4);
			dst << std::endl;
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			body_t->explore(declarations,bindings);
			body_f->explore(declarations,bindings);
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			
			int x = unique_name;
			unique_name++;
			std::string if_t = "$if_t"+std::to_string(x); // if true
			std::string if_e = "$else"+std::to_string(x); // else
			std::string if_f = "$if_fin"+std::to_string(x); // finish	
			int y = regs.EmptyRegister(); // number of first free register
			regs.ReserveRegister(y); // mark register as used
			std::string condReg = "$"+std::to_string(y);
			condition->compile(dst,bindings,regs,condReg,returnLoc); // compile the condition, get the result into condReg
			dst<<"bne	$0, "<<condReg<<", "<<if_t<<std::endl; //If cond not zero, branch to body
			regs.ReleaseRegister(y);
			dst<<"nop"<<std::endl;
			dst<<"b	"<<if_e<<std::endl;
			dst<<"nop"<<std::endl;
			dst<<if_t<<":"<<std::endl;
			body_t->compile(dst,bindings,regs,destReg,returnLoc);
			dst<<"b	"<<if_f<<std::endl;
			dst<<"nop"<<std::endl;
			dst<<if_e<<":"<<std::endl;
			body_f->compile(dst,bindings,regs,destReg,returnLoc);
			dst<<"b	"<<if_f<<std::endl;
			dst<<"nop"<<std::endl;
			dst<<if_f<<":"<<std::endl;
			
		}
};



class WhileStatement : public Statement {
	protected:
		ExpressionPtr condition;
		NodePtr body; // actually a statement list, the body of the while
	public:
		WhileStatement(ExpressionPtr _condition, NodePtr _body) : condition(_condition), body(_body) {std::cerr<<"While statement constructor"<<std::endl;}
		virtual void print(std::ostream &dst) const override {//if case exists
			dst << "while (";
			condition->print(dst);
			dst<< ") {" << std::endl;
			body->print(dst);
			dst << "}";
			dst << std::endl;
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			for (int i = 0; i<indent;i++) { //Shold make a function / member function, quick hack for now
				dst << " ";
			}
			dst << "while ";
			condition->translate(dst,indent);
			dst << " :" << std::endl;
			body->translate(dst, indent + 4);
			dst << std::endl;
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		
			int x = unique_name;
			unique_name++;
			std::string cond = "$cond"+std::to_string(x); // if true
			std::string loop = "$body"+std::to_string(x); // else
			std::string end = "$end"+std::to_string(x); // finish	
			int y = regs.EmptyRegister(); // number of first free register
			regs.ReserveRegister(y); // mark register as used
			std::string condReg = "$"+std::to_string(y);
			dst<<cond<<":"<<std::endl;
			condition->compile(dst,bindings,regs,condReg,returnLoc); // compile the condition, get the result into condReg
			dst<<"bne	$0, "<<condReg<<", "<<loop<<std::endl;
			dst<<"nop"<<std::endl;
			dst<<"b "<<end<<std::endl;
			dst<<"nop"<<std::endl;
		
			//body
			dst<<loop<<":"<<std::endl;
			body->compile(dst,bindings,regs,destReg,returnLoc);
			dst<<"b "<<cond<<std::endl;
			dst<<"nop"<<std::endl;
		
			//end
			dst<<end<<":"<<std::endl;
		}
	
		virtual void explore(int & declarations, Context & bindings) const override{
			body->explore(declarations,bindings);
		}
};

#endif
