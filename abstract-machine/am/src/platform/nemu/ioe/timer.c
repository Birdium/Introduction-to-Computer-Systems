#include <am.h>
#include <nemu.h>
#include <stdio.h>

static uint64_t boot_time = 0;

void __am_timer_init() {
  uint32_t lo = inl(RTC_ADDR + 0);
  uint32_t hi = inl(RTC_ADDR + 4);
  boot_time = (((uint64_t)hi << 32) | lo) / 10;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t lo = inl(RTC_ADDR + 0);
  uint32_t hi = inl(RTC_ADDR + 4);
  uptime->us = (((uint64_t)hi << 32) | lo) - boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
