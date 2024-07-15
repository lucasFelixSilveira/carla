#include <stdio.h>

const char *bootstrapping = "../bootstrapping/main.cl";
const char *tests = "../bootstrapping/tests/test.cl";

int
main() 
{
  FILE *main = fopen(tests, "rb");

  printf("Hello, world!");

  fclose(main);
  return 0;
}