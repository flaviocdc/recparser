void foo() {
}

int bar(int a, int b) {
  return a+b;
}

int umparam(int a) {
  foo();
}

char doisparam(int a, char b) {
  bar(a,b);
}

char varios(int a, int b, char c, char d) {
  doisparam(a, bar(c,d));
}

int func_com_decl_var(int a, char b) {
  char t1, t2;
  int b1, b2;

  foo();
}

int varargs(int a, ...) {
  foo();
}

int fun_call_exp() {
  int a, x;
  a = umparam(1) + x;

  return x;
}
