int func() {
  int a;

  a = 1;
  a = 2;
  
  a = a;
  a = func();
  a = foo(a,b);
  a = 1 + 2;
  a = 1 + a + c;

  return a;
}
