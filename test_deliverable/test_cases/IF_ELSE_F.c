/*Required Test, meets IF_ELSE_F*/

int f(){
	if(5!=5){ /*ie false*/
		return 5; 
	}
	else{
		return 9; /* does this*/
	}
	return 11; /* probably never see this one tbh. Its for manual checking, not the automated testbench*/
}
