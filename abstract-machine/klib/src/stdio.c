#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char buf[4096];

int printf(const char *fmt, ...) {
  char p_buf[4096];
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(p_buf, fmt, ap);
  va_end(ap);
  char *bp = p_buf;
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
      enum {no_fill, left_fill, right_fill} fill_type = no_fill;
      char fill_ch = ' ';
      size_t width = 0; // precision = 0;
      // flag_characters
      bool flag = 1;
      while (flag){
        switch(*fp){
          case '-': fill_type = right_fill; break;
          case '0': 
            if (fill_type == no_fill) fill_type = left_fill;
            fill_ch = '0';
            break;
          default: flag = 0; break;
        }
        if (flag) ++fp;
      }
      // field width
      flag = 1;
      while (flag){
        switch(*fp){
          case'0': case'1': case'2': case'3': case'4': case'5': case'6': case'7': case'8': case'9':
            width = width * 10 + *fp - '0'; break;
          default: flag = 0; break;
        }
        if (flag) ++fp;
      }
      switch (*fp){
      case 'd': 
        {
          int arg = va_arg(ap, int);
          itoa(arg, buf, 10);
          char *bp = buf;
          size_t buf_len = strlen(buf);
          if (fill_type == left_fill){
            while(buf_len < width && ch_num < n){
              *op = fill_ch;
              ++op; ++ch_num; width--;
            }
          }
          while(*bp != '\0' && ch_num < n){
            *op = *bp;
            ++op; ++ch_num; ++bp;
          }
          if (fill_type == right_fill){
            while(buf_len < width && ch_num < n){
              *op = fill_ch;
              ++op; ++ch_num; width--;
            }
          }
        }
        break;
      case 'u': 
        {
          int arg = va_arg(ap, int);
          uitoa(arg, buf, 10);
          char *bp = buf;
          size_t buf_len = strlen(buf);
          if (fill_type == left_fill){
            while(buf_len < width && ch_num < n){
              *op = fill_ch;
              ++op; ++ch_num; width--;
            }
          }
          while(*bp != '\0' && ch_num < n){
            *op = *bp;
            ++op; ++ch_num; ++bp;
          }
          if (fill_type == right_fill){
            while(buf_len < width && ch_num < n){
              *op = fill_ch;
              ++op; ++ch_num; width--;
            }
          }
        }
        break;
      case 'x': case 'p':
        {
          unsigned arg = va_arg(ap, unsigned);
          uitoa(arg, buf, 16);
          char *bp = buf;
          size_t buf_len = strlen(buf);
          if (fill_type == left_fill){
            while(buf_len < width && ch_num < n){
              *op = fill_ch;
              ++op; ++ch_num; width--;
            }
          }
          while(*bp != '\0' && ch_num < n){
            *op = *bp;
            ++op; ++ch_num; ++bp;
          }
          if (fill_type == right_fill){
            while(buf_len < width && ch_num < n){
              *op = fill_ch;
              ++op; ++ch_num; width--;
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
      case 'c':
        {
          char arg = va_arg(ap, int);
          *op = arg;
          ++op; ++ch_num; ++arg;
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
