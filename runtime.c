#include <stdio.h>

extern int monga();

int main(void) {
  int i = monga();
  printf("monga() = %d\n", i);

  return 0;
}
