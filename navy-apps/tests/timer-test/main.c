#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

uint32_t NDL_GetTicks();
struct timeval start;
struct timeval now;

int main() {

  uint32_t start = NDL_GetTicks();
  int half_sec = 0;
  while (1) {
    uint32_t now = NDL_GetTicks();
    // printf("%d:%d\n", (int)now.tv_sec, (int)now.tv_usec);
    if ((now - start) / 500 > half_sec){
        printf("%d / 2 seconds).\n", half_sec);
        half_sec ++;
    } 
  }
  return 0;
}