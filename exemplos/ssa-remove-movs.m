int foo(int a, int b) {
  return a + b;
}

int main() {

  int a, b, x;

  a = 10;

  a = a + b;
  b = -a;

  foo(a + b, a - b);
  x = foo(10, 5);
  a = x;

  x = foo(10, 5) + 1;

  return a;
}
