#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

void itoa(int val, char* dest, int base){
  char index[]="0123456789ABCDEF";
  char *dp = dest;
  unsigned uval;
  bool flag = 0;
  if (val < 0){
    uval = (unsigned) val; flag = 1;
  }
  else uval = val;
  while(uval){
    *dp = index[uval % base];
    uval /= base; ++dp; 
  }
  if (flag) {*dp = '-'; dp++;}
  *dp = '\0'; dp--;
  while(dest < dp){
    char tmp = *dest; *dest = *dp; *dp = tmp;
    dp--; dest++;
  }
}

void uitoa(unsigned uval, char* dest, int base){
  char index[]="0123456789ABCDEF";
  char *dp = dest;
  while(uval){
    *dp = index[uval % base];
    uval /= base; ++dp; 
  }
  *dp = '\0'; dp--;
  while(dest < dp){
    char tmp = *dest; *dest = *dp; *dp = tmp;
    dp--; dest++;
  }
}
// void *malloc(size_t size) {
//   panic("Not implemented");
// }

void free(void *ptr) {
}

#endif
