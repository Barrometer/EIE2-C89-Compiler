#ifndef ast_varb_declarations_hpp
#define ast_varb_declarations_hpp

#include <string>
#include <sstream>
#include <iostream> 
#include <vector>

//bloody global variables for handling global variables
extern int myGlobVarbCounter;

extern std::vector<std::string> myGlobVarbContainer;

extern int unique_name;

class Declaration : public Node{
	
};

class DeclLocal : public Declaration{

	protected: 
		std::string type;
		std::string var_id;
		ExpressionPtr value;
	
	public:
		DeclLocal(std::string _type, std::string _var_id) : //constructor with no value assignment
			type(_type),
			var_id(_var_id),
			value(NULL)
			{std::cerr<<"New Declaration with no value assigned"<<std::endl;}
		DeclLocal(std::string _type, std::string _var_id, ExpressionPtr _value) : //constructor with variable assignment
			type(_type),
			var_id(_var_id),
			value(_value)
			{std::cerr<<"New Declaration with value assigned"<<std::endl;}
			
		virtual void print(std::ostream &dst) const override {
			dst<<type;
			dst << " ";
			dst << var_id;
			dst<<" ";
			if(value!=NULL){
				dst<<"= ";
				value->print(dst);
			}
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			for(int i=0; i<indent;i++){//Shold make a function / member function, quick hack for now
				dst<<" ";
			}	
			dst<<var_id;
			dst<<" = ";
			if(value!=NULL){
				value ->translate(dst,indent);
			}
			else{
				dst<<"0";
			}
			dst<<std::endl;
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			//need to specify an open register for the new variable
			//need to use addi or addiu
			//reserve reg
			if(value != NULL){		//value case
				int tmp = regs.EmptyRegister();
				regs.ReserveRegister(tmp);
				destReg = "$"+std::to_string(tmp);
				value->compile(dst, bindings, regs, destReg,returnLoc);
				dst<<std::endl;
				std::cerr<<bindings.getOffset(var_id)<<std::endl;
				dst<<"sw "<<destReg<<","<<bindings.getOffset(var_id)<<"($fp)"<<std::endl;
				
				std::cerr<<"By the way, I think that varb "<<var_id<<" lives at "<<bindings.getOffset(var_id)<<std::endl;
				
				std::cerr<<"Test 1"<<std::endl;
				destReg = "NULL";
				regs.ReleaseRegister(tmp);
			}
			else{

			}
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			declarations++;
			bindings.growTable(var_id);
		}
};


class DeclList : public Declaration{

	protected:
		DeclPtr current; // the current declaration
		DeclPtr next; // the next declaration / declaration list
	
	public:
		//constructor with no next list
		DeclList(DeclPtr _current) : current(_current), next(NULL){
			std::cerr<<"Constructed DeclList with no next Decl"<<std::endl;
		}
		//constructor with next list
		DeclList(DeclPtr _current, DeclPtr _next) : current(_current), next(_next){
			std::cerr<<"Constructed DeclList with next Decl"<<std::endl;
		}
		virtual void print(std::ostream &dst) const override {
			std::cerr<<"Print on decl list got called"<<std::endl;
			if(next!=NULL){
				next->print(dst);
			}
			current->print(dst);
			std::cerr<<"Print on declaration list successfully finished"<<std::endl;
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			std::cerr<<"_____declLIST1_____"<<std::endl;
			if(next!=NULL){
				next->translate(dst,indent);
			}
			std::cerr<<"_____declLIST2_____"<<std::endl;
			current->translate(dst,indent);
			std::cerr<<"_____declLIST3_____"<<std::endl;
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
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

class DeclGlobal : public Node{ 
	protected:
		std::string type;
		std::string var_id;
		ExpressionPtr value;
	public:
		DeclGlobal(std::string _type, std::string _var_id) : 
			type(_type),
			var_id(_var_id),
			value(NULL) {
			
			std::cerr<<"Behold, a global with no initial value. Increment the counter and store the relevant details!"<<std::endl;
			myGlobVarbCounter=true;
			myGlobVarbContainer.push_back(type);
			myGlobVarbContainer.push_back(var_id);
			std::cerr<<"The global constructor hath finished!"<<std::endl;
		}
		DeclGlobal(std::string _type, std::string _var_id, ExpressionPtr _value) :
			type(_type),
			var_id(_var_id),
			value(_value) {
			
			std::cerr<<"Behold, a global with an initial value. Increment the counter and store the relevant details!"<<std::endl;
			myGlobVarbCounter=true;
			myGlobVarbContainer.push_back(type);
			myGlobVarbContainer.push_back(var_id);
			std::cerr<<"The global constructor hath finished!"<<std::endl;
		}
		virtual void print(std::ostream &dst) const override {
			dst<<type;
			dst << " ";
			dst << var_id;
			dst<<" ";
			if(value!=NULL){
				dst<<"= ";
				value->print(dst);
			}
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			for(int i=0; i<indent;i++){//Shold make a function / member function, quick hack for now
				dst<<" ";
			}	
			dst<<var_id;
			dst<<" = ";
			if(value!=NULL){
				value ->translate(dst,indent);
			}
			else{
				dst<<"0";
			}
			dst<<std::endl;
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			
			dst<<".globl "<<var_id<<std::endl;
			dst<<".data "<<std::endl;
			dst<<".align 2"<<std::endl;
			if(value!=NULL){
				dst<<var_id<<":"<<std::endl;
				dst<<".word ";
				value->translate(dst,0);
				dst<<std::endl;
				dst<<".text"<<std::endl;
				dst<<".align 2"<<std::endl;
			}
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			std::cerr<<"Do I make it here?"<<std::endl;
			declarations++;
			
		}
};
class CompoundStatement : public Node{ 

	protected:
		StatementPtr sref;
		DeclPtr dref;
		Context* varb_bindings; // as it currently stands, Compound Statements are our only change of scope. So each compount statement must have a Context.
		int	noDecls; // count how many declarations exist below you
		
	public:
		CompoundStatement(StatementPtr _sref) : sref(_sref), noDecls(0)
		{
			varb_bindings = new Context;
			std::cerr<<"In constructor for CompoundStatement with no decl list"<<std::endl;
			//sref->explore(noDecls,*varb_bindings);

		}	
			
		CompoundStatement(DeclPtr _dref) : dref(_dref), noDecls(0)
		{
			varb_bindings = new Context;
			std::cerr<<"In constructor for CompoundStatement with no statement list"<<std::endl;
			//dref->explore(noDecls,*varb_bindings);

		}
		
		CompoundStatement(StatementPtr _sref,DeclPtr _dref) : sref(_sref),dref(_dref), noDecls(0)
		{
			varb_bindings = new Context;
			std::cerr<<"In constructor for CompoundStatement with both lists"<<std::endl;
			//dref->explore(noDecls,*varb_bindings);
			//sref->explore(noDecls,*varb_bindings);
			
		}	
			
		virtual void print(std::ostream &dst) const override {
			if(dref!=NULL){
				dref->print(dst);
			}
			if(sref!=NULL){
				sref->print(dst);
			}
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			if(myGlobVarbCounter!=0){
				std::cerr<<"There were some global variables to translate"<<std::endl;
				
				for (int i=1; i<myGlobVarbContainer.size();i=i+2){
					for(int i=0; i<indent;i++){//Shold make a function / member function, quick hack for now
						dst<<" ";
					}
					dst<<"global "<<myGlobVarbContainer[i]<<std::endl;
				}
				
			}
			if(dref!=NULL){
				std::cerr<<"_____declCOMP1_____"<<std::endl;
				dref->translate(dst,indent);
				std::cerr<<"_____declCOMP2_____"<<std::endl;
			}
			if(sref!=NULL){
				std::cerr<<"_____declCOMP1_____"<<std::endl;
				sref->translate(dst,indent);
				std::cerr<<"_____declCOMP2_____"<<std::endl;
			}
		}
		//each compile construction will require a context for itself
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			
			if(bindings.yesGlobals() > 0){
				varb_bindings->insertGlobals(bindings);
				std::cerr<<"I recognized some globals and added them"<<std::endl;
			}
			
			std::cerr<<"Testing, dump bindings"<<std::endl;
			varb_bindings->dumpTable();
			//bindings = *varb_bindings;
			if(dref!=NULL){
				dref->compile(dst,*varb_bindings,regs,destReg,returnLoc);
			}
			if(sref!=NULL){
				sref->compile(dst,*varb_bindings,regs,destReg,returnLoc);
			}
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			// here be interesting things
			varb_bindings->changeOffset(bindings.returnOffset());
			varb_bindings->mergeMaps(bindings);
			if(dref!=NULL){
				dref->explore(declarations,*varb_bindings);
			}

			if(sref!=NULL){
				sref->explore(declarations,*varb_bindings);
			}

		}
};

#endif
