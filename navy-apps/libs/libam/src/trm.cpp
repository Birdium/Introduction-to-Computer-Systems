#include <am.h>

Area heap;

void putch(char ch) {
    putchar(ch);
}

void halt(int code) {
    puts("Should Not Reach Here.");
    while(1);
}
