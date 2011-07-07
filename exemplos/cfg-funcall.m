int foo() {
  foo();
  bar(1,2+3);
}

int bar(int a, int b) {
  return a+b;
}
