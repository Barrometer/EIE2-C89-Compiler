/*Driver for required Test IF_T*/

int f();

int main(){
	return f()!=12; /* function under test, f, should return 12. So if it does, return 0, else 1*/
}
