#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

uint32_t NDL_GetTicks() {
  struct timeval now;
  // printf("in navy:%x\n", sizeof(now.tv_sec));
  gettimeofday(&now, NULL);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int f_event = open("/dev/events", 0, 0);
  int ret = read(f_event, buf, len);
  if (ret) return 1;
  else return 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  int f_dispinfo = open("/proc/dispinfo", 0, 0);
  // assert(f_dispinfo);
  char buf[64];
  read(f_dispinfo, buf, sizeof(buf));
  sscanf(buf, "WIDTH:%d\nHEIGHT:%d\n", &screen_w, &screen_h);
  if (*w == 0 && *h == 0) {
    *w = screen_w, *h = screen_h;
  }
  canvas_w = *w, canvas_h = *h;
  canvas_x = (screen_w - canvas_w) / 2;
  canvas_y = (screen_h - canvas_h) / 2;
  close(f_dispinfo);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int f_fb = open("/dev/fb", 0, 0);
  int w_num = w < canvas_w - x ? w : canvas_w - x;
  int h_num = h < canvas_h - y ? h : canvas_h - y;
  // printf("%d %d %d %d\n", w, w_num, h, h_num);
  for(int i = 0; i < h_num; i++) {
    int offset = 4 * (canvas_x + x + (canvas_y + y + i) * screen_w);
    lseek(f_fb, offset, SEEK_SET);
    printf("%d %d %d %d\n", w, w_num, h, h_num);
    write(f_fb, pixels + i * w, w_num * 4);
    // while(1);
  }
  close(f_fb);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
