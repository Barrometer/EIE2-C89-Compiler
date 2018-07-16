/*driver for test cast number 7, self recursion*/

int f(int x);

int main(){

	return f(3)!=5; /* recursive call of x should always return 5. If so return a zero, else a one*/
}
