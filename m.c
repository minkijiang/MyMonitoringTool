#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <time.h>

int main() {


  int c = log10(4);

  clock_t start_time = clock();
  while ((clock() - start_time) * 1000 / CLOCKS_PER_SEC < 50000);

  return 0;
}

//gcc --std=c99 m.c -lrt -lm 