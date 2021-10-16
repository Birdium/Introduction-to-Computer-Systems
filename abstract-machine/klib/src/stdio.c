#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char buf[10000];

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(buf, fmt, ap);
  va_end(ap);
  char *bp = buf;
  while(*bp != '\0') {
    putch(*bp); ++bp;
  }
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int ret = vsnprintf(out, INT32_MAX, fmt, ap);
  return ret;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  size_t ch_num = 0;
  char *op = out; const char *fp = fmt;
  while(*fp != '\0' && ch_num < n) {
    if (*fp == '%'){
      ++fp;
      switch (*fp){
      case 'd': 
        {
          int arg = va_arg(ap, int);
          if (arg < 0) {
            *op = '-'; // char here...
            ++op; ++ch_num;
          }
          int tmp = arg; size_t len = 0;  
          while (tmp != 0) {tmp /= 10; ++len;} // pre-calc length
          while (len > n) {arg /= 10; --len;} // cut end
          op += len; ch_num += len;
          if (len){
            char *ap = op - 1;
            while (arg != 0){
              *ap = '0' + arg % 10;
              arg /= 10;
              ap--;
            }
          }
        }
        break;
      case 's': 
        {
          char *arg = va_arg(ap, char*);
          while (*arg != '\0' && ch_num < n){
            *op = *arg;
            ++op; ++ch_num; ++arg;
          }
        }
        break;
      default:
        assert(0);
      }
    }
    else {
      *op = *fp;
      ++op; ++ch_num;
    }
    ++fp;
  }
  if (ch_num < n && *fp == '\0') {*op = '\0'; ++ch_num;}
  return ch_num;
}

#endif
