#include <am.h>
#include <nemu.h>
//#include <stdio.h>

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t lo = inl(RTC_ADDR + 0);
  uint32_t hi = inl(RTC_ADDR + 4);
  //if (lo == 0 ) printf("%u %u\n", hi, lo);
  uptime->us = (((uint64_t)hi * 1000000) + lo); 
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
