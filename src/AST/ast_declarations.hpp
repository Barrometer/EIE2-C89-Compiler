#ifndef ast_declarations_hpp
#define ast_declarations_hpp


#include <string>

extern int myGlobVarbCounter;

extern std::vector<std::string> myGlobVarbContainer;

extern int unique_name;

class FunctionDecl : public Node {
	protected:
		std::string ret_type; // string containing what the return type is
		std::string fnc_ID; // string containing the function id
		NodePtr body; // point to the compount Statement containing the body
		NodePtr args; // pointer to a parameter list
		bool isMain; // we need to be able to create a valid main entry point. As such, a boolean tracking if this is the main function
		int myDecls; // we need to record how many variables are declared in this function
	public: 
		//constructor without arguments list
		FunctionDecl(std::string _ret, std::string _ID, NodePtr _body) : 
			ret_type(_ret),
			fnc_ID(_ID),
			body(_body),
			args(NULL)
		
			{
				std::cerr<<"Constructor for func decl"<<std::endl;
				myDecls=0;
				if(fnc_ID=="main"){
					isMain=true;
					std::cerr<<"Just made the main"<<std::endl;
				}
				else{
					isMain=false;
				}
				int explore_v=0;
				Context explore_m; // currently empty
				body->explore(explore_v,explore_m);

				myDecls=explore_v;
				std::cerr<<"I am function "<<fnc_ID<<" I explored myself and found "<<myDecls<<"Decls inside of me"<<std::endl;

			}
			
		//constructor with arguments list
		//going to need to use special case with $4 - $7
		FunctionDecl(std::string _ret, std::string _ID, NodePtr _body, NodePtr _args) : 
			ret_type(_ret),
			fnc_ID(_ID),
			body(_body),
			args(_args)
			
		{
			std::cerr<<"Constructor for func decl with parameters"<<std::endl;
			if(fnc_ID=="main"){
				isMain=true;
				std::cerr<<"Just made the main"<<std::endl;
			}
			else{
				isMain=false;
			}
			int explore_v=0;
			
			
			
			Context explore_m; // currently empty
			body->explore(explore_v,explore_m);
			myDecls=explore_v;
			std::cerr<<"I am function "<<fnc_ID<<" I explored myself and found "<<myDecls<<"Decls inside of me"<<std::endl;
			
		}
		
		virtual void print(std::ostream &dst) const override {
			std::cerr<<"I am trying to print the func declr"<<std::endl;
			dst<<ret_type;
			dst<<" ";
			dst<<fnc_ID;
			dst<<" ( ";
			dst<<" ) ";
			dst<<std::endl;
			dst<<"{"<<std::endl;
			body->print(dst);
			dst<<"}";
		}

		virtual void translate(std::ostream &dst, int indent) const override {
		
			std::cerr<<"_____dec1_____"<<std::endl;
			dst<<"def "<<fnc_ID<<"(";
			std::cerr<<"_____dec2_____"<<std::endl;
			if(args!=NULL){
				args->translate(dst, indent);
			}
			dst<<"):"<<std::endl;
			std::cerr<<"_____dec3_____"<<std::endl;
			body->translate(dst,indent+4);
			std::cerr<<"_____dec4_____"<<std::endl;
			
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		
		
			if(myGlobVarbCounter!=0){
				std::cerr<<"There are some global variables to consider."<<std::endl;
				for(int i=1; i<myGlobVarbContainer.size();i=i+2){
					std::cerr<<"Found a global called "<<myGlobVarbContainer[i]<<std::endl;
					bindings.growGlobals(myGlobVarbContainer[i]);			
							
				}
			
			}
			bindings.dumpTable();
		
			dst<<"	.globl	"<<fnc_ID<<std::endl;
			dst<<"	.ent	"<<fnc_ID<<std::endl;
			dst<<fnc_ID<<":"<<std::endl;
			
			std::string returnLable = "$returnLable" + std::to_string(unique_name);
			unique_name++;
			
			std::cerr<<"DEBUG, myDECLS is "<<myDecls<<std::endl;
			int stackAllocate = 8 + 4*myDecls;//dynamically work out how much stack to allocate
			dst<<"addiu $sp,$sp,-"<<stackAllocate <<std::endl; //allocate stack
			dst<<"sw $fp,"<<(stackAllocate-4)<<"($sp)"<<std::endl; //the location of the old frame pointer is 4 less the top of the stack
			dst<<"move	$fp,$sp"<<std::endl;			
			if(args!=NULL){
				args->compile(dst, bindings, regs,destReg,returnLable);
			}
			body->compile(dst, bindings, regs,destReg,returnLable);
			dst<<returnLable<<":"<<std::endl;
			dst<<"move $sp,$fp" << std::endl; // boiler plate
			dst<<"lw $fp,"<<(stackAllocate-4)<<"($sp)"<<std::endl; //the location of the old frame pointer is 4 less the top of the stack
			dst<<"addiu $sp,$sp,"<<stackAllocate <<std::endl; // restore stack pointer
			dst<<"j $31"<<std::endl;
			dst<<"nop"<<std::endl;			
			dst<<"	.end	"<<fnc_ID<<std::endl;			
		}	//may be an idea to make sure stuff can point to parent
			//or at least the capability to count up a glob var
		virtual void explore(int & declarations, Context & bindings) const  override{
			if(args!=NULL){
				args->explore(declarations,bindings);
			}
			body->explore(declarations,bindings);
			
			
		}
};

class Param : public Node{

	protected:
		std::string type;
		std::string id;
		//ExpresstionPtr value; // technically this is valid eg int f(int x=2){return x;} is a valid function, returning 2 or the input. Not going to be supported.
	public:
		Param (std::string _type, std::string _id) : type(_type), id(_id){} //constructor
		virtual void print(std::ostream &dst) const override {
			
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			dst<<id;
				
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			std::cerr<<"Parameter Compilation not implemented"<<std::endl;
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			std::cerr<<"Parameter Exploration unimplemented"<<std::endl;
		}
};

class ParamList : public Node{ // list of function paramaters

	protected:
		NodePtr current; // pointer to current Parameter, of type node
		NodePtr next; // pointer to next Parameter list, of type node too
	public:
		ParamList(NodePtr _current) : current(_current), next(NULL){} // constructor with no next
		ParamList(NodePtr _current, NodePtr _next) : current(_current), next(_next){} // constructor with next
		
		virtual void print(std::ostream &dst) const override {
			
		}
		
		virtual void translate(std::ostream &dst, int indent) const override {
			std::cerr<<"_____paramLIST1_____"<<std::endl;
			if(next!=NULL){				
				next->translate(dst,indent);
				dst<<", ";
			}
			std::cerr<<"_____paramLIST2_____"<<std::endl;
			current->translate(dst,indent);
			std::cerr<<"_____paramLIST3_____"<<std::endl;
				
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			std::cerr<<"Parameter List Compilation unimplemented"<<std::endl;
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			std::cerr<<"Parameter List Exploration unimplemented"<<std::endl;
		}
};



#endif

