/*Driver for required Test IF_F*/

int f();

int main(){
	return f()!=2; /* function under test, f, should return 2. So if it does, return 0, else 1*/
}
