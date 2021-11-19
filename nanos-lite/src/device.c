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
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;
  sprintf(buf, "%s %s\0", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
  return strlen(buf);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T g_config = io_read(AM_GPU_CONFIG);
  sprintf(buf, "WIDTH:%d\nHEIGHT:%d\n\0", g_config.width, g_config.height);
  // printf("%d %d\n", g_config.width, g_config.height);
  return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

int sys_gettimeofday(uint32_t *tv, void *tz) {
  uint64_t am_uptime = io_read(AM_TIMER_UPTIME).us;
  *tv = am_uptime / 1000000;
  *(tv + 1) = am_uptime % 1000000;
  // printf("%p of sec, %p of usec\n", &tv->tv_sec, &tv->tv_usec);
  // printf("in nanos:%x\n", sizeof(struct timeval));
  // assert(tv->tv_usec == 0);
  // printf("%d:%d\n", (int)tv->tv_sec, (int)tv->tv_usec);
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
