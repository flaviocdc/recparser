int func() {
  int a;

  a = 1;
  a = 2;
  
  a = a;
  a = func();
  a = foo(a,b);

  return a;
}
