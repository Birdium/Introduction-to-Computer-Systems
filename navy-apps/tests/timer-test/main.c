#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

int main() {

  struct timeval start;
  gettimeofday(&start, NULL);  
  int half_sec = 0;
  while (1) {
    struct timeval now;
    gettimeofday(&now, NULL);
    if (((now.tv_sec - start.tv_sec) * 1000000 + now.tv_usec - start.tv_usec) / 500000 > half_sec){
        printf("%d / 2 seconds).\n", half_sec);
        half_sec ++;
    } 
  }
  return 0;
}