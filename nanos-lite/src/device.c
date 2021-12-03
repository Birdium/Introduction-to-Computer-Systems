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
  sprintf(buf, "%s %s\n", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
  return strlen(buf);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T g_config = io_read(AM_GPU_CONFIG);
  sprintf(buf, "WIDTH:%d\nHEIGHT:%d\n\0", g_config.width, g_config.height);
  // printf("%d %d\n", g_config.width, g_config.height);
  return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T g_config = io_read(AM_GPU_CONFIG);
  int w = g_config.width;// h = g_config.height;
  int x = offset / 4 % w, y = offset / 4 / w;
  // printf("offset:0x%x\n", offset);
  io_write(AM_GPU_FBDRAW, x, y, (void*)buf, len / 4, 1, true);
  return len;
}

int sb_write(const void *buf, size_t offset, size_t len) {
  void *bf = (void*)buf;
  Area area = {bf, bf + len};
  io_write(AM_AUDIO_PLAY, area);
  return len;
}

int sbctl_write(const void *buf, size_t offset, size_t len) {
  io_write(AM_AUDIO_CTRL, *((int *) buf), *((int *) buf + 1), *((int *) buf + 2));
  return 0;
}

int sbctl_read(void *buf, size_t offset, size_t len) {
  int buf_size = io_read(AM_AUDIO_CONFIG).bufsize;
  int cnt = io_read(AM_AUDIO_STATUS).count;
  *(int*)buf = buf_size - cnt;
  return buf_size - cnt;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
