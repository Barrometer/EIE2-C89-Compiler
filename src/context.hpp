#ifndef context_hpp
#define context_hpp

#include <map>
#include <iostream>

/*header contains declarations for two classes - one handling the context
the other handling register status */


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
		std::map<std::string, std::string>  conReg; // maps a variable name to the register it is stored in currently - may not be stored
		std::map<std::string, int> conOffset; // maps a variable name to its offset from the stack pointer
		std::map<std::string,bool> conGlob; // Maps a variable name to a boolean as to whether the variable is global or local
		int nextOffset; // an incremending counter of where the next variable will live

	public:
		Context(){
			nextOffset =4; // first varb stored at sp+4
		}	
		
		
	
		void growTable(std::string var_id){	// for locals
			conReg[var_id] = "NULL"; // need to use square brackets operator for assignment
											 // by default variables don't live in a register, so
											 // register is called as NULL
			conOffset[var_id] = nextOffset; // as using at would throw an error
			nextOffset=nextOffset+4;
			conGlob[var_id]=false; // this is the function called for local variables, so set to false
		}
		void growGlobals(std::string var_id){// for globals
			conReg[var_id]="NULL";
			conGlob[var_id]=true;
		
		}
		
		bool isGlob(std::string var_id){ // returns whether a variable is global or not
			
			return conGlob[var_id];
		}
		
		int yesGlobals(){ // returns the number of global variables
			return conGlob.size();
		}
		
		void insertGlobals(Context add){ // add global variables to each sub context that needs to know of the globals
			conReg.insert(add.conReg.begin(),add.conReg.end());
			conGlob.insert(add.conGlob.begin(),add.conGlob.end());
		}
		
		

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
		void dumpTable(){ // a debug function
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
		
		void mergeMaps(Context add){ // merge two contexts, which is needed for correct handling of scopes and shadowing
			conReg.insert(add.conReg.begin(),add.conReg.end());
			conOffset.insert(add.conOffset.begin(),add.conOffset.end());
		}
		
		int returnOffset(){ // returns the current offset from the stack pointer
			return nextOffset;
		}
		
		void changeOffset(int x){ // a function that changes the stack pointer of every local variable - used as part of merging contexts
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
	
	
	protected:
		// it doesn't matter what's in a register, it matters if its in usage
		bool register_used[32]; // there are 32 registers.
		
	public:
	
		Registers(){
			for(int i=0; i<32;i++){ // 32 registers, array starts at 0
				register_used[i] = false;
			}
			register_used[0] = true; // reg 0 is always used, and always has value of 0
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
		//return an unused register in valid range If none available, return -1. 
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
