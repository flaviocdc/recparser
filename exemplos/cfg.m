int foo(int a, int b) {
  return a + b;
}

int func() {
  int a, b, c;

  a = 1;
  a = 2;
  
  a = a;
  a = func();
  a = foo(a,b);
  a = 1 + 2;
  a = 1 + a + c;

  return a;
}

int func2() {
  int a, b, c;

  b = 1 + 2;

  a = 1 + foo(1, 1+2);
  
  c = -(1+a);

  return a;
}
