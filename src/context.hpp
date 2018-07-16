#ifndef context_hpp
#define context_hpp

#include <map>
#include <iostream>


class Context{ // contains a map, key is string, stored is string. Maps variables to values
	/*
		now creates two maps, both have the same set of var_id keys

				reg_stored_in
		x	|	NULL
		y	|	$8

				offset
		x	|	4
		y	|	8

	*/
	
	protected:
		//in retrospect, structs
		std::map<std::string, std::string>  conReg;
		std::map<std::string, int> conOffset;
		std::map<std::string,bool> conGlob;
		int nextOffset;

	public:
		Context(){
			nextOffset =4; // first varb stored at sp+4
		}	
		
		/*
		//assuming array of all of the different variables exists
		
		std::string[declarations] var_ids;

		Context tmp();

		for(int i=0; i<=declarations; i++){
			tmp.updateKey(var_ids[i]);
		}

		*/
	
		void growTable(std::string var_id){	
			conReg[var_id] = "NULL"; // need to use square brackets operator for assignment
			conOffset[var_id] = nextOffset; // as using at would throw an error
			nextOffset=nextOffset+4;
			conGlob[var_id]=false;
		}
		void growGlobals(std::string var_id){
			conReg[var_id]="NULL";
			conGlob[var_id]=true;
		
		}
		
		bool isGlob(std::string var_id){
			
			return conGlob[var_id];
		}
		
		int yesGlobals(){
			return conGlob.size();
		}
		
		void insertGlobals(Context add){
			conReg.insert(add.conReg.begin(),add.conReg.end());
			conGlob.insert(add.conGlob.begin(),add.conGlob.end());
		}
		//really annoying but there's a bug that says you can't just assign a NULL space to a string allocated mem space, so I'm changing it to LITERAL NULL, a STRING LITERAL,

		//DO NOT FORGET ABOUT THE STRING LITERAL HERE
		//ALSO IT ONLY AFFECTS CONREG
		
		//the above two functions should work, though it ain't pretty		

		std::string getReg(std::string var_id){	//returns reg_stored_in for a given key
			std::string tmp = conReg.at(var_id); // this works assuming the variable exists.
			return tmp;
		}
		int getOffset(std::string var_id){ // returns SP offset for a given key
			int offset = conOffset[var_id];
			return offset;
		}
		void updateConReg(std::string var_id, std::string newReg){ // update reg stored in
			conReg.at(var_id) = newReg;	
		}
		void updateConOffset(std::string var_id, int offset){ // update SP offset
			conOffset.at(var_id) = offset;
		}
		void dumpTable(){
			std::cerr<<"Dumping map for testing"<<std::endl;
			for(std::map<std::string,std::string>::iterator pos = conReg.begin(); pos!= conReg.end(); ++pos){
				std::cerr<< pos->first<<" "<<pos->second<<std::endl;
					
				
			}
			for(std::map<std::string,int>::iterator pos = conOffset.begin(); pos!= conOffset.end(); ++pos){
				std::cerr<< pos->first<<" "<<pos->second<<std::endl;
					
				
			}
			for(std::map<std::string,bool>::iterator pos = conGlob.begin(); pos!= conGlob.end(); ++pos){
				std::cerr<< pos->first<<" "<<pos->second<<std::endl;
					
				
			}
		
		}
		
		void mergeMaps(Context add){
			conReg.insert(add.conReg.begin(),add.conReg.end());
			conOffset.insert(add.conOffset.begin(),add.conOffset.end());
		}
		
		int returnOffset(){
			return nextOffset;
		}
		
		void changeOffset(int x){
			for(std::map<std::string,int>::iterator pos = conOffset.begin(); pos!= conOffset.end(); ++pos){
				pos->second+=x-4;
				
			}
			nextOffset+=x-4;
		}
};

class Registers{ // contains useful info about registers
	/*Register conventions
	$0 - always 0
	$1 - used by assembler?
	$2,3 - return values ($2 by default, $3 if more than 32 bits)
	$4-7 - argument registers, contain first four arg values in function call. These are also by convention stored at the top of the stack / bottom of last frame?
	$8-15,24,25 - temp registers
	$16-23 - saved registers
	$26-27 - Kernel reserved, don't use
	$28 - global pointers? Refrence says to ignore
	$29 - stack pointer
	$30 - frame pointer
	$31 - return address of subroutine call
	
	*/
	
	//genuinely unsure which we can use
	protected:
		// it doesn't matter what's in a register, it matters if its in usage
		bool register_used[32]; // there are 32 registers.
		
	public:
		Registers(){
			for(int i=0; i<32;i++){ // 32 registers, array starts at 0
				register_used[i] = false;
			}
			register_used[0] = true; // reg 0 is always used
		}
		bool ReserveRegister(int x){ // mark a register as used
			if(register_used[x]){ // ie if the register *is* used
				return false; // this indicates I could not reserve register x
			}
			register_used[x] = true; // its now in use
			return true; // indicates success
		}
		void ReleaseRegister(int x){
			if(x!=0){ // can't unfree register 0
				register_used[x]=false;
			}
		}
		int EmptyRegister(){
		//return an unused register in valid range(figure that out). If none available, return -1 I guess
			for(int i= 8;i<=25;i++){
				if(!register_used[i]){
					return i;
				}
			}
			
			//if no free register found
			return -1;
		}
};

#endif
