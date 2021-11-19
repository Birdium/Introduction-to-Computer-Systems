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
    if (2 * (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 500000 > half_sec){
        printf("%d.%d seconds).\n", half_sec / 2, half_sec % 2 * 5);
        half_sec ++;
    } 
  }
  return 0;
}