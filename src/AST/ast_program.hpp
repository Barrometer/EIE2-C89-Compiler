#ifndef ast_program_hpp
#define ast_program_hpp


extern int unique_name;

class Program : public Node{ // class that points to one GLB_VAR or FNC_DEC, then another program

	protected:
		NodePtr current; //points to current FunctionDeclaration or GlobalDeclaration
		NodePtr next; //points to next program
		
	public:
		Program(NodePtr _current) : current(_current), next(NULL){} // constructor with no next
		Program(NodePtr _current, NodePtr _next) : current(_current), next(_next){} //full constructor
		
		virtual void print(std::ostream &dst) const override {
			std::cerr<<"Print on Program list got called"<<std::endl;
		
			std::cerr<<"Print on Program list not implemented"<<std::endl;
		}
		
		virtual void translate(std::ostream &dst, int indent) const override {
			std::cerr<<"_____progLIST1_____"<<std::endl;
			if(next!=NULL){
				next->translate(dst,indent);
			}
			std::cerr<<"_____progLIST2_____"<<std::endl;
			current->translate(dst,indent);
			std::cerr<<"_____progLIST3_____"<<std::endl;
		}

		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			if(next!=NULL){
				next->compile(dst,bindings,regs,destReg,returnLoc);
			}		
			current->compile(dst,bindings,regs,destReg,returnLoc);
		}
		virtual void explore(int & declarations, Context & bindings) const override {
			if(next != NULL){
				next->explore(declarations,bindings);
			}
			current->explore(declarations,bindings);
		}
};

#endif
