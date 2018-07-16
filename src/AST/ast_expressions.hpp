// this header contains definitons for most things relating to expressions

#ifndef ast_expressions_hpp
#define ast_expressions_hpp

extern int unique_name;

class Expression : public Node {

};



class AssignmentExpression : public Expression{ // ie for EXPRESSION = EXPRESSION	
	protected:
		std::string target; // has a left expressions
		ExpressionPtr value; // to be assigned to right expression
		
	public:
		AssignmentExpression(std::string _target, ExpressionPtr _value) : target(_target), value(_value){}
		
		const Expression *getvalue() const;
		
		virtual void print(std::ostream &dst) const override {
			dst<<target;
			dst << " = ";
			value->print(dst);
		}
		
		virtual void translate(std::ostream &dst, int indent) const override { // for python translation requirement
			dst<<target;
			dst << " = ( ";
			value->translate(dst,indent);
			dst << " )";
		}
		
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {	 
			
			
			if(bindings.isGlob(target)){ // ie when assigning value to a global variable
			
				int x =regs.EmptyRegister(); // obtain one empty register
				regs.ReserveRegister(x); // reserve it. 
				int y =regs.EmptyRegister(); // and another
				regs.ReserveRegister(y);
				
				// need two registers temporatily - one for storing result, one for storing operand
				
				destReg = "$" + std::to_string(x);
				std::string globReg = "$" + std::to_string(y);
				
				std::cerr<<"storing a global"<<std::endl;
				
				// some boiler plate to allow global assignment to work
				dst<<"lui "<<globReg<<",\%hi("<<target<<")"<<std::endl;
				dst<<"addiu "<<globReg<<", \%lo("<<target<<")"<<std::endl;
				
				/* recursively call compile on the value expression.
					consider the following; x = a + b;
					to assign correctly, must work out value of a+b
				*/
				value->compile(dst,bindings,regs,destReg,returnLoc);			
				
				dst<<"sw	"<<destReg<<", ("<<globReg<<")"<<std::endl;
				
				// now mark registers as unused
				regs.ReleaseRegister(x);
				regs.ReleaseRegister(y);
			}
			
			else{ // the case for assigning to a local variable. Largely the same as above
				int tmp = regs.EmptyRegister();
				regs.ReserveRegister(tmp);
				destReg = "$" + std::to_string(tmp);
				value->compile(dst, bindings, regs, destReg,returnLoc);
				dst<<"sw "<<destReg<<","<<bindings.getOffset(target)<<"($fp)"<<std::endl;
			
				std::cerr<<"By the way, I think that varb "<<target<<" lives at "<<bindings.getOffset(target)<<"and it was stored like a local"<<std::endl; // legacy debug
				regs.ReleaseRegister(tmp);
			}
			destReg = "NULL";
			
			
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			//target name irrelevant, will be assigned under a name and offset value
			value->explore(declarations,bindings);
		}
};


class FunctionCall : public Expression{
	protected:
		std::string id; // functions have an id
		NodePtr vlist; // and a list of variables they take as input, not every function has this
	public:
		// two constructors for two cases
		FunctionCall(std::string _id) : id(_id), vlist(NULL) {}
		FunctionCall(std::string _id, NodePtr _vlist) : id(_id), vlist(_vlist) {}
		
		virtual void print(std::ostream &dst) const override {
			std::cerr<<"Not implemented"<<std::endl;
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			dst<<id<<" ( ";
			if(vlist != NULL){
				vlist->translate(dst, indent);
			}
			dst<<" )";
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			// currently only works on functions that do not take inputs
			
			//store the necassary registers. I need to save 19. 19*4=76
			dst<<"addiu $fp, $fp, -88"<<std::endl; // 88 on 76 as additional space needed by convention
			int offset =8;
			for(int i = 8;i<=25;i++){
				dst<<"sw $"<<i<<", "<<offset<<"($fp)"<<std::endl;
				offset=offset+4;
			}
			dst<<"sw $31, 4($fp)"<<std::endl;
			//call function
			
			dst<<"jal "<<id<<std::endl;
			dst<<"nop"<<std::endl;
			
			
			//recover stuff
			
			dst<<"lw $31, 4($fp)"<<std::endl;
			offset =8;
			for(int i = 8;i<=25;i++){
				
				dst<<"lw $"<<i<<", "<<offset<<"($fp)"<<std::endl;
				offset=offset+4;
			}
			
			//put function output (reg2) into destReg
			
			dst<<"move "<<destReg<<",$2"<<std::endl;
			//restore fp
			dst<<"addiu $fp, $fp, 88"<<std::endl;
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			
			vlist->explore(declarations,bindings);
			std::cerr<<"An function call can't contain a declaration, stopping"<<std::endl;
		}
};


class VarList : public Node{ // a list of variables, as part of a function call
	protected:
		std::string current;
		NodePtr next;
	public:
		VarList(std::string _current) : current(_current), next(NULL){}
		VarList(std::string _current, NodePtr _next) : current(_current), next(_next){}
		virtual void print(std::ostream &dst) const override {
			std::cerr<<"Not implemented"<<std::endl;
		}
		virtual void translate(std::ostream &dst, int indent) const override {
			if(next!=NULL){				
				next->translate(dst,indent);
				dst<<", ";
			}
			dst<<current;
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			std::cerr<<"Varlist compilation not implemented"<<std::endl;
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			if(next!=NULL){
				next->explore(declarations,bindings);
			}
			else{std::cerr<<"Have reached the end of a VarList, stopping"<<std::endl;}
		}
};

class Operator : public Expression {
protected:
	NodePtr left;
	NodePtr right;
	
public:
	Operator(NodePtr _left, NodePtr _right) : left(_left), right(_right){}
	virtual const char *getOpcode() const = 0;
	NodePtr getLeft() const { return left; }
	NodePtr getRight() const { return right; }
};
// all implementation of operators moved to ast_operators.hpp

#endif
