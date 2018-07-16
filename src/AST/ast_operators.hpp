// this header contains definitons for operators

#ifndef ast_operators_hpp
#define ast_operators_hpp

extern int unique_name;

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
