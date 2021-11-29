#include <am.h>
#include <klib-macros.h>
#include <sys/time.h>

void __am_timer_init();
void __am_gpu_init();
// void __am_audio_init();
void __am_input_keybrd(AM_INPUT_KEYBRD_T *);
void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);
void __am_gpu_config(AM_GPU_CONFIG_T *);
void __am_gpu_status(AM_GPU_STATUS_T *);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *);
// void __am_audio_config(AM_AUDIO_CONFIG_T *);
// void __am_audio_ctrl(AM_AUDIO_CTRL_T *);
// void __am_audio_status(AM_AUDIO_STATUS_T *);
// void __am_audio_play(AM_AUDIO_PLAY_T *);
// void __am_disk_config(AM_DISK_CONFIG_T *cfg);
// void __am_disk_status(AM_DISK_STATUS_T *stat);
// void __am_disk_blkio(AM_DISK_BLKIO_T *io);

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }
static void __am_uart_config(AM_UART_CONFIG_T *cfg)   { cfg->present = false; }
static void __am_net_config (AM_NET_CONFIG_T *cfg)    { cfg->present = false; }

typedef void (*handler_t)(void *buf);
static void *lut[128] = {
  [AM_TIMER_CONFIG] = __am_timer_config,
  [AM_TIMER_RTC   ] = __am_timer_rtc,
  [AM_TIMER_UPTIME] = __am_timer_uptime,
  [AM_INPUT_CONFIG] = __am_input_config,
  [AM_INPUT_KEYBRD] = __am_input_keybrd,
  [AM_GPU_CONFIG  ] = __am_gpu_config,
  [AM_GPU_FBDRAW  ] = __am_gpu_fbdraw,
  [AM_GPU_STATUS  ] = __am_gpu_status,
  // [AM_UART_CONFIG ] = __am_uart_config,
  // [AM_AUDIO_CONFIG] = __am_audio_config,
  // [AM_AUDIO_CTRL  ] = __am_audio_ctrl,
  // [AM_AUDIO_STATUS] = __am_audio_status,
  // [AM_AUDIO_PLAY  ] = __am_audio_play,
  // [AM_DISK_CONFIG ] = __am_disk_config,
  // [AM_DISK_STATUS ] = __am_disk_status,
  // [AM_DISK_BLKIO  ] = __am_disk_blkio,
  // [AM_NET_CONFIG  ] = __am_net_config,
};

static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i]) lut[i] = fail;
  __am_gpu_init();
  __am_timer_init();
  // __am_audio_init();
  return true;
}

void ioe_read (int reg, void *buf) { ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }

// timer

static struct timeval boot_time = {};

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  struct timeval now;
  gettimeofday(&now, NULL);
  long seconds = now.tv_sec - boot_time.tv_sec;
  long useconds = now.tv_usec - boot_time.tv_usec;
  uptime->us = seconds * 1000000 + (useconds + 500);
}

void __am_timer_init() {
  gettimeofday(&boot_time, NULL);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}

// keyboard

#define _KEYS(_) \
  _(ESCAPE) _(F1) _(F2) _(F3) _(F4) _(F5) _(F6) _(F7) _(F8) _(F9) _(F10) _(F11) _(F12) \
  _(GRAVE) _(1) _(2) _(3) _(4) _(5) _(6) _(7) _(8) _(9) _(0) _(MINUS) _(EQUALS) _(BACKSPACE) \
  _(TAB) _(Q) _(W) _(E) _(R) _(T) _(Y) _(U) _(I) _(O) _(P) _(LEFTBRACKET) _(RIGHTBRACKET) _(BACKSLASH) \
  _(CAPSLOCK) _(A) _(S) _(D) _(F) _(G) _(H) _(J) _(K) _(L) _(SEMICOLON) _(APOSTROPHE) _(RETURN) \
  _(LSHIFT) _(Z) _(X) _(C) _(V) _(B) _(N) _(M) _(COMMA) _(PERIOD) _(SLASH) _(RSHIFT) \
  _(LCTRL) _(APPLICATION) _(LALT) _(SPACE) _(RALT) _(RCTRL) \
  _(UP) _(DOWN) _(LEFT) _(RIGHT) _(INSERT) _(DELETE) _(HOME) _(END) _(PAGEUP) _(PAGEDOWN)

#define keyname(k) #k, 
#define NR_KEYS sizeof(keyname) / sizeof(keyname[0])

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  // int k = inl(KBD_ADDR);
  // kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  // kbd->keycode = k & ~KEYDOWN_MASK;
  char buf[64];
  memset(buf, 0, sizeof(buf));

  int fd = open("/dev/events", 0, 0);
  read(fd, buf, sizeof(buf));
  buf[strlen(buf) - 1] = '\0';
  int key_ndx = 1;
  for(; key_ndx < NR_KEYS; key_ndx++) {
    if (strcmp(buf + 3, keyname[key_ndx]) == 0) {
      kbd->keycode = key_ndx;
      break;
    }
  }
  printf("%s", buf);
  kbd->keydown = (buf[1] == 'd') ? 1 : 0;
  close(fd);
}

// gpu

static int w = 0, h = 0;
void __am_gpu_init() {
  NDL_Init(0);
  NDL_OpenCanvas(&w, &h);
  // int i;
  // int w = VGA_W;
  // int h = VGA_H;
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i ++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  uint32_t *pxl = (uint32_t *) ctl->pixels;
  NDL_DrawRect(pxl, x, y, w, h);
  // while (1) printf("1\n");
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
