int foo() {
  int a;
  int b;
  int c;
  
  a = 1;
  b = 2;
  
  if (a >= b) {
    foo();
  }
  
  c = a && b || 5;
 
  return c * b && a;  
}
