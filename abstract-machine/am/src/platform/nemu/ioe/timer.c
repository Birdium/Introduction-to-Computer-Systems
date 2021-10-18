#include <am.h>
#include <nemu.h>
#include <stdio.h>

void __am_timer_init() {
  outl(RTC_ADDR + 0, 0);
  outl(RTC_ADDR + 4, 0);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t lo = inl(RTC_ADDR + 0);
  uint64_t hi = inl(RTC_ADDR + 4);
  uptime->us = (hi << 32) + lo;
  printf("%u\n", (uint32_t)uptime->us);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
