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
		virtual void translate(std::ostream &dst, int indent) const override {
			dst<<target;
			dst << " = ( ";
			value->translate(dst,indent);
			dst << " )";
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {	 
			
			/*int tmp = regs.EmptyRegister();
			regs.ReserveRegister(tmp);
			destReg = "$" + std::to_string(tmp);
			value->compile(dst, bindings, regs, destReg,returnLoc);*/
			
			if(bindings.isGlob(target)){
			
				int x =regs.EmptyRegister();
				regs.ReserveRegister(x);
				int y =regs.EmptyRegister();
				regs.ReserveRegister(y);
				
				destReg = "$" + std::to_string(x);
				std::string globReg = "$" + std::to_string(y);
				
				std::cerr<<"storing a global"<<std::endl;
				
				dst<<"lui "<<globReg<<",\%hi("<<target<<")"<<std::endl;
				dst<<"addiu "<<globReg<<", \%lo("<<target<<")"<<std::endl;
				
				value->compile(dst,bindings,regs,destReg,returnLoc);			
				
				dst<<"sw	"<<destReg<<", ("<<globReg<<")"<<std::endl;
				regs.ReleaseRegister(x);
				regs.ReleaseRegister(y);
			}
			
			else{
				int tmp = regs.EmptyRegister();
				regs.ReserveRegister(tmp);
				destReg = "$" + std::to_string(tmp);
				value->compile(dst, bindings, regs, destReg,returnLoc);
				dst<<"sw "<<destReg<<","<<bindings.getOffset(target)<<"($fp)"<<std::endl;
			
				std::cerr<<"By the way, I think that varb "<<target<<" lives at "<<bindings.getOffset(target)<<"and it was stored like a local"<<std::endl;
				regs.ReleaseRegister(tmp);
			}
			destReg = "NULL";
			
			/*		
			dst<<"li $"<<reg.EmptyRegister()<<","<<value<<std::endl;
			*/
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			//target name irrelevant, will be assigned under a name and offset value
			value->explore(declarations,bindings);
		}
};


class FunctionCall : public Expression{
	protected:
		std::string id;
		NodePtr vlist;
	public:
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
			
			
			//store the necassary registers. I need to save 19. 19*4=76
			dst<<"addiu $fp, $fp, -88"<<std::endl;
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


class VarList : public Node{
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

//Start of Arithmetic Operators

class AddOperator : public Operator { 
protected:
	virtual const char *getOpcode() const override { return "+"; }
public:
	AddOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return vl + vr;
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" + ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" + ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
	
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"addu "<<destReg<<","<<destReg<<","<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		//unsure if overwriting something counts, but this will need one
		std::cerr<<"In add, I can terminate here happily. Could have ages ago tbh"<<std::endl;
	}
};

class SubOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "-"; }
public:
	SubOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return vl - vr;
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" - ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" - ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"sub "<<destReg<<","<<destReg<<","<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"In sub, I can terminate here happily. Could have ages ago tbh"<<std::endl;

	}
};

class MulOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "*"; }
public:
	MulOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return vl*vr;
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" * ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" * ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		// we only support 32 bit integers. We can safely discard the upper half registers
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"MULT	"<<destReg<<", "<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
		dst<<"NOP"<<std::endl; //multiplication takes multiple clock cycles?
		dst<<"MFLO	"<<destReg<<std::endl; // the lower half of the result ends up in reg lo
		dst<<"NOP"<<std::endl;
		dst<<"NOP"<<std::endl; // these two also prevent undefined behaviour
		
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class DivOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "/"; }
public:
	DivOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return vl/vr;
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" / ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" / ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		// we only support integers
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"DIV	"<<destReg<<", "<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
		dst<<"NOP"<<std::endl; //division takes multiple clock cycles?
		dst<<"MFLO	"<<destReg<<std::endl; // the lower half of the result ends up in reg lo
		dst<<"NOP"<<std::endl;
		dst<<"NOP"<<std::endl; // these two also prevent undefined behaviour
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

//End of Arithmetic Operators
//Start of Logical Operators

class EqualToOperator : public Operator {	//need to go over how mapping works with booleans
protected:
	virtual const char *getOpcode() const override { return "=="; }
public:
	EqualToOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if(vl == vr){
			return 1;
		}
		else{
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" == ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" == ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		
		int tmp1 = regs.EmptyRegister();
		regs.ReserveRegister(tmp1);
		int tmp2 = regs.EmptyRegister();
		regs.ReserveRegister(tmp2);
		
		std::string Reg1 = "$" + std::to_string(tmp1);
		std::string Reg2 = "$" + std::to_string(tmp2);
		
		left->compile(dst,bindings,regs,destReg,returnLoc);
		right->compile(dst,bindings,regs,Reg1,returnLoc);
		dst<<"slt	"<<Reg2<<", "<<destReg<<", "<<Reg1<<std::endl;
		dst<<"slt	"<<Reg1<<", "<<Reg1<<", "<<destReg<<std::endl;
		
		dst<<"xor "<<destReg<<", "<<Reg1<<", "<<Reg2<<std::endl;
		dst<<"xori "<<destReg<<", "<<destReg<<", 1"<<std::endl;
		regs.ReleaseRegister(tmp2);
		regs.ReleaseRegister(tmp1);

	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class NotEqualOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "!="; }
public:
	NotEqualOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if(vl != vr){
			return 1;
		}
		else{
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" != ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" != ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"sub "<<destReg<<","<<destReg<<","<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class LAndOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "&&"; }
public:
	LAndOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if(vl && vr){
			return 1;
		}
		else{
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" && ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" AND ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		int tmp1 = regs.EmptyRegister();
		regs.ReserveRegister(tmp1);
		int tmp2 = regs.EmptyRegister();
		regs.ReserveRegister(tmp2);
		
		std::string Reg1 = "$" + std::to_string(tmp1);
		std::string Reg2 = "$" + std::to_string(tmp2);
		
		left->compile(dst,bindings,regs,destReg,returnLoc);
		right->compile(dst,bindings,regs,Reg1,returnLoc);
		
		dst<<"sltu	"<<Reg2<<",$0,"<<destReg<<std::endl;
		dst<<"sltu	"<<Reg1<<",$0,"<<Reg1<<std::endl;
		
		dst<<"and "<<destReg<<","<<Reg1<<","<<Reg2<<std::endl;
		regs.ReleaseRegister(tmp2);
		regs.ReleaseRegister(tmp1);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class LOrOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "||"; }
public:
	LOrOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if(vl || vr){
			return 1;
		}
		else{
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" || ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" OR ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		int tmp1 = regs.EmptyRegister();
		regs.ReserveRegister(tmp1);
		int tmp2 = regs.EmptyRegister();
		regs.ReserveRegister(tmp2);
		
		std::string Reg1 = "$" + std::to_string(tmp1);
		std::string Reg2 = "$" + std::to_string(tmp2);
		
		left->compile(dst,bindings,regs,destReg,returnLoc);
		right->compile(dst,bindings,regs,Reg1,returnLoc);
		
		dst<<"sltu	"<<Reg2<<",$0,"<<destReg<<std::endl;
		dst<<"sltu	"<<Reg1<<",$0,"<<Reg1<<std::endl;
		
		dst<<"or "<<destReg<<","<<Reg1<<","<<Reg2<<std::endl;
		regs.ReleaseRegister(tmp2);
		regs.ReleaseRegister(tmp1);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class NotOperator : public Operator {	// a not operator only requires RHS of !
protected:
	virtual const char *getOpcode() const override { return "!"; }
	
public:
	NotOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		//double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if(!vr){
			return 1;
		}
		else{
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" ! ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" NOT ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		right->compile(dst,bindings,regs,destReg,returnLoc);
		dst<<"slt "<<destReg<<", "<<"$0, "<<destReg<<std::endl;
		dst<<"xori "<<destReg<<", "<<destReg<<", 1"<<std::endl;
		
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class GThanOperator : public Operator {	// a not operator only requires RHS of !
protected:
	virtual const char *getOpcode() const override { return "!"; }
public:
	GThanOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override {
		//double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if (!vr) {
			return 1;
		}
		else {
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst << " > ";
		right->print(dst);
		dst << " )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst << "( ";
		left->translate(dst,indent);
		dst << " > ";
		right->translate(dst,indent);
		dst << " )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		int tmp1 = regs.EmptyRegister();
		regs.ReserveRegister(tmp1);

		
		std::string Reg1 = "$" + std::to_string(tmp1);
		
		left->compile(dst,bindings,regs,destReg,returnLoc);
		right->compile(dst,bindings,regs,Reg1,returnLoc);
		dst<<"slt	"<<Reg1<<", "<<Reg1<<", "<<destReg<<std::endl;
		
		dst<<"move "<<destReg<<", "<<Reg1<<std::endl;

		regs.ReleaseRegister(tmp1);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class LThanOperator : public Operator {	// a not operator only requires RHS of !
protected:
	virtual const char *getOpcode() const override { return "!"; }
public:
	LThanOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override {
		//double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if (!vr) {
			return 1;
		}
		else {
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst << " < ";
		right->print(dst);
		dst << " )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst << "( ";
		left->translate(dst,indent);
		dst << " < ";
		right->translate(dst,indent);
		dst << " )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		int tmp1 = regs.EmptyRegister();
		regs.ReserveRegister(tmp1);
		
		std::string Reg1 = "$" + std::to_string(tmp1);
		
		left->compile(dst,bindings,regs,destReg,returnLoc);
		right->compile(dst,bindings,regs,Reg1,returnLoc);
		dst<<"slt	"<<Reg1<<", "<<destReg<<", "<<Reg1<<std::endl;
		
		dst<<"move "<<destReg<<", "<<Reg1<<std::endl;
		regs.ReleaseRegister(tmp1);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class GEThanOperator : public Operator {	// a not operator only requires RHS of !
protected:
	virtual const char *getOpcode() const override { return "!"; }
public:
	GEThanOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override {
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if (vl>=vr) {
			return 1;
		}
		else {
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst << " >= ";
		right->print(dst);
		dst << " )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst << "( ";
		left->translate(dst,indent);
		dst << " >= ";
		right->translate(dst,indent);
		dst << " )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		int tmp1 = regs.EmptyRegister();
		regs.ReserveRegister(tmp1);
		std::string Reg1 = "$" + std::to_string(tmp1);
		left->compile(dst,bindings,regs,destReg,returnLoc);
		right->compile(dst,bindings,regs,Reg1,returnLoc);
		dst<<"slt	"<<Reg1<<", "<<destReg<<", "<<Reg1<<std::endl;
		dst<<"xori "<<Reg1<<", "<<Reg1<<", 0x1"<<std::endl;
		dst<<"andi "<<destReg<<", "<<Reg1<<", 0xff"<<std::endl;
		regs.ReleaseRegister(tmp1);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class LEThanOperator : public Operator {	// a not operator only requires RHS of !
protected:
	virtual const char *getOpcode() const override { return "!"; }
public:
	LEThanOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override {
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		if (vl<=vr) {
			return 1;
		}
		else {
			return 0;
		}
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst << " <= ";
		right->print(dst);
		dst << " )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst << "( ";
		left->translate(dst,indent);
		dst << " <= ";
		right->translate(dst,indent);
		dst << " )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		int tmp1 = regs.EmptyRegister();
		regs.ReserveRegister(tmp1);
		std::string Reg1 = "$" + std::to_string(tmp1);
		left->compile(dst,bindings,regs,destReg,returnLoc);
		right->compile(dst,bindings,regs,Reg1,returnLoc);
		dst<<"slt	"<<Reg1<<", "<<Reg1<<", "<<destReg<<std::endl;
		dst<<"xori "<<Reg1<<", "<<Reg1<<", 0x1"<<std::endl;
		dst<<"andi "<<destReg<<", "<<Reg1<<", 0xff"<<std::endl;
		regs.ReleaseRegister(tmp1);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};
//End of Logical Operators
//Start of Bitwise Operators

class BAndOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "&"; }
public:
	BAndOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return (int)vl & (int)vr;
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" & ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" & ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"and "<<destReg<<","<<destReg<<","<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class BOrOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "|"; }
public:
	BOrOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return (int)vl | (int)vr;
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" | ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" | ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"or "<<destReg<<","<<destReg<<","<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class BNotOperator : public Operator {	//not would only intake RHS
protected:
	virtual const char *getOpcode() const override { return "~"; }
public:
	BNotOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		//double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return ~(int)vr;
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" ~ ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"(~ ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		right->compile(dst,bindings,regs,destReg,returnLoc);
		dst<<"nor "<<destReg<<","<<destReg<<","<<destReg<<std::endl;
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class XorOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "^"; }
public:
	XorOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return (int)vl ^ (int)vr;		//Dont't know bitwise or in C++
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" ^ ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" ^ ";
		right->translate(dst,indent);
		dst<<" )";
	}	
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"xor "<<destReg<<","<<destReg<<","<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class LShiftOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return "<<"; }
public:
	LShiftOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return (int)vl << (int)vr;
	}
	virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" << ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" << ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"sll "<<destReg<<","<<destReg<<","<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

class RShiftOperator : public Operator {
protected:
	virtual const char *getOpcode() const override { return ">>"; }
public:
	RShiftOperator(NodePtr _left, NodePtr _right) : Operator(_left, _right) {}
	virtual double evaluate(const std::map<std::string, double> &bindings) const override{
		double vl = left->evaluate(bindings);
		double vr = right->evaluate(bindings);
		return (int)vl >> (int)vr;
	}virtual void print(std::ostream &dst) const override {
		dst << "( ";
		left->print(dst);
		dst<<" >> ";
		right->print(dst);
		dst<<" )";
	}
	virtual void translate(std::ostream &dst, int indent) const override {
		dst<<"( ";
		left->translate(dst,indent);
		dst<<" >> ";
		right->translate(dst,indent);
		dst<<" )";
	}
	virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
		left->compile(dst,bindings,regs,destReg,returnLoc);
		int tmp = regs.EmptyRegister();
		regs.ReserveRegister(tmp);
		std::string rightReg = "$" + std::to_string(tmp);
		right->compile(dst,bindings,regs,rightReg, returnLoc);
		dst<<"sra "<<destReg<<","<<destReg<<","<<rightReg<<std::endl;
		regs.ReleaseRegister(tmp);
	}
	virtual void explore(int & declarations, Context & bindings) const override{
		std::cerr<<"Not implemented"<<std::endl;
	}
};

#endif
