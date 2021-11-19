#include <stdio.h>
#include <assert.h>
#include "../../libs/libndl/include/NDL.h"
uint32_t NDL_GetTicks();

int main() {
  NDL_Init(0);
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