int foo(int a, int b) {
  return 1;
}

int inc(int i) {
  return i + 1;
}

int monga() {
  int i;
  i = 0;

  while (i < 10) {
    i = inc(i);
  }

  return i;
}
