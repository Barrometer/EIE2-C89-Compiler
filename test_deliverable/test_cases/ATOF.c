/*Required Test, meets ATOF*/

double atof(const char* str);

int f(){
	char demo[4] ="100";
	double value = atof(demo);
	return value;
}
