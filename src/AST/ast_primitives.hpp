#ifndef ast_primitives_hpp
#define ast_primitives_hpp

extern int unique_name;

class Identifier : public Expression {	//If we can figure out how Variable works then we can tie it in with EqualsOperator so that we know what to return for it
	protected:
		std::string id;
		//int offset;
	public:
   	 	Identifier(const std::string &_id) : id(_id) /*, offset(NULL) */ {}
	    	const std::string getId() const { return id; }
			virtual void print(std::ostream &dst) const override {
			dst<<id;
	    	}
	    	/*virtual double evaluate(const std::map<std::string,double> &bindings) const override{
	       		return bindings.at(id);
	    	}    */
		virtual void translate(std::ostream &dst, int indent) const override {
			std::cerr<<"_____primID1_____"<<std::endl;
			dst<<id;
			std::cerr<<"_____primID2_____"<<std::endl;
		}

		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			//std::cerr<<"Here is error"<<std::endl;
			
			std::cerr<<"By the way, I think that varb "<<id<<" lives at "<<bindings.getOffset(id)<<std::endl;
			if(bindings.isGlob(id)){
				std::cerr<<"I think that the varb was actually a global"<<std::endl;
				/*
				dst<<"lui "<<destReg<<", \%hi("<<id<<")"<<std::endl;
				dst<<"addiu "<<destReg<<". \%lo("<<id<<")"<<std::endl;
				*/
				
				dst<<"lui "<<destReg<<", \%hi("<<id<<")"<<std::endl;
				dst<<"lw "<<destReg<<", \%lo("<<id<<")("<<destReg<<")"<<std::endl;
			}
			else{
				std::cerr<<"I think that the varb was local"<<std::endl;
				dst<<"lw "<<destReg<<", "<<bindings.getOffset(id)<<"($fp)"<<std::endl;
			}
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			std::cerr<<"End of branch"<<std::endl;
		}
};

class IntLiteral : public Expression {
	private:
    		int value;
	public:
	    	IntLiteral(int _value) : value(_value) {}
	    	int getValue() const { return value; }
	    	virtual void print(std::ostream &dst) const override {
	     	   dst<<value;
	    	}
	    	/*virtual double evaluate(const std::map<std::string,double> &bindings) const override {
			return value;
	    	}*/
		virtual void translate(std::ostream &dst, int indent) const override {
			std::cerr<<"_____primINT1_____"<<std::endl;
			dst<<value;
			std::cerr<<"_____primINT2_____"<<std::endl;
		}
		virtual void compile(std::ostream &dst, Context & bindings, Registers & regs, std::string destReg, std::string returnLoc) const override {
			std::cerr<<"IntLiteral"<<std::endl;

			dst<<"li "<<destReg<<", "<<value<<std::endl;;

			
		}
		virtual void explore(int & declarations, Context & bindings) const override{
			std::cerr<<"End of branch"<<std::endl;
		}
};


//NON PRIORITY
/*class StringLiteral : public Expression {
	private:
		std::string str;
	public:
		StringLiteral(std::string _str) : str(_str) {}
		const std::string getStr() const { return str; }
		virtual void print(std::ostream &dst) const override {
			dst << str;
		}
		virtual double evaluate(const std::map<std::string, double> &bindings) const override {
			return str;
		}
};
	
class CharLiteral : public Expression {
	private:
		char character;
	public:
		CharLiteral(char _character) : character(_character) {}
		const char getChar() const { return character; }
		virtual void print(std::ostream &dst) const override {
			dst << character;
		}
		virtual double evaluate(const std::map<std::string, double> &bindings) const override {
			return character;
		}
};

class Float : public Expression { // not priority, handling floats hard
	private:
		float flo;
	public:
		Float(float _flo) : flo(_flo) {}
		const float getFloat() const { return flo; }
		virtual void print(std::ostream &dst) const override {
			dst << flo;
		}
		virtual double evaluate(const std::map<std::string, double> &bindings) const override {
			return flo;
		}
};*/

#endif
