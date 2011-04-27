int umparam(int a) {
  foo();
}

char doisparam(int a, char b) {
  foo(a,b);
}

char varios(int a, int b, char c, char d) {
  foo(a,b, bar(c,d));
}
