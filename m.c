#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define _POSIX_C_SOURCE 199309L

#include <time.h>

int main() {


  int c = log10(4);

  int nano = 900000000;
  int sec = 1;

  struct timespec sleep_time;
  sleep_time.tv_sec = sec;  
  sleep_time.tv_nsec = nano; 

  nanosleep(&sleep_time, NULL);

  return 0;
}

//gcc --std=c99 m.c -lrt -lm 