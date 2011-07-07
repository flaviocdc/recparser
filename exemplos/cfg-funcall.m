int bar(int a, int b) {
  return a+b;
}

int foo() {
  foo();
  bar(1,2+3);
}
