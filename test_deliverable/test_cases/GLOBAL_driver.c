/*driver for test case GLOBAL*/

void f();
int test_glob;
int main(){
  f();
  return test_glob!=5;
}
