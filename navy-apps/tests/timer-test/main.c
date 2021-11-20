#include <stdio.h>
#include <assert.h>

int main() {
  NDL_Init(0);
  int start = NDL_GetTicks();
  int half_sec = 1;
  while (1) {
    int now = NDL_GetTicks();
    // printf("%d:%d\n", (int)now.tv_sec, (int)now.tv_usec);
    if ((now - start) / 500 > half_sec){
        printf("%d / 2 seconds).\n", half_sec);
        half_sec ++;
    } 
  }
  return 0;
}