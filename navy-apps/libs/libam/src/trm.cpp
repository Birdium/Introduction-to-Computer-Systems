#include <am.h>
#include <klib-macros.h>

#define HEAP_SIZE (128 * 1024 * 1024)
char _heap[HEAP_SIZE / 128];
Area heap = RANGE(_heap, _heap + HEAP_SIZE / 128);


void putch(char ch) {
    putchar(ch);
}

void halt(int code) {
    puts("Should Not Reach Here.");
    while(1);
}
