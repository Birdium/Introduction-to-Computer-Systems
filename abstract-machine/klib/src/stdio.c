#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  vsnprintf(out, INT32_MAX, fmt, ap);
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsprintf(out, fmt, ap);
  va_end(ap);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char *op = out; const char *fp = fmt;
  while(*fp != '\0' && n != 0) {
    if (*fp == '%'){
      ++fp;
      switch (*fp){
      case 'd': 
        {
          int arg = va_arg(ap, int);
          if (arg < 0) {
            *op = '-';
            ++op; --n;
          }
          int tmp = arg; size_t len = 0;  
          while (tmp != 0) {tmp /= 10; ++len;} // pre-calc length
          while (len > n) {arg /= 10; --len;} // cut end
          op += len; n -= len;
          if (len){
            char *ap = op - 1;
            while (arg != 0){
              *ap = arg % 10;
              arg /= 10;
              ap--;
            }
          }
        }
        break;
      case 's': 
        {
          char *arg = va_arg(ap, char*);
          while (*arg != '\0' && n != 0){
            *op = *arg;
            ++op; --n; ++arg;
          }
        }
        break;
      default:
        assert(0);
      }
    }
    else {
      *op = *fp;
      ++op; --n;
    }
    ++fp;
  }
  if (n != 0 && *fp == '\0') *op = '\0';
  return 0;
}

#endif
