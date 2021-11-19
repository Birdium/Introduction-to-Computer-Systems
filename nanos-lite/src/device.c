#include <common.h>
#include <sys/time.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  size_t ret = 0;
  for(size_t i = 0; i < len; i++){
    putch(*((char*)buf + i));
    ret++;
  }
  return ret;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  uint64_t am_uptime = io_read(AM_TIMER_UPTIME).us;
  tv->tv_usec = am_uptime / 1000000;
  tv->tv_sec = am_uptime % 1000000;
  // assert(tv->tv_usec == 0);
  // printf("%d:%d\n", (int)tv->tv_sec, (int)tv->tv_usec);
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
