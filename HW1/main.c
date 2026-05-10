#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define _POSIX_C_SOURCE 199309L


//----- time now in milliseconds ----
double now_ms() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1.0e6;
}



//--------------- Test Harness -----------------------

int main(void) {
  srand(1234);
  double start, end, elapsed;

  start = now_ms();

  // Your kernel goes here ...

  end = now_ms();
  elapsed = end - start;
  printf("%20.1f", elapsed);

  return 0;
}
