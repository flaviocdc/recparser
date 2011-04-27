int umparam(int a) {
  foo();
}

char doisparam(int a, char b) {
  foo(a,b);
}

char varios(int a, int b, char c, char d) {
  foo(a,b, bar(c,d));
}

int func_com_decl_var(int a, char b) {
  char t1, t2;
  int b1, b2;

  foo();
}

int varargs_unico(...) {
  foo();
}

int varargs_ultimo(int a, ...) {
  foo();
}
