#include <am.h>
#include <nemu.h>
//#include <stdio.h>

#ifdef CONFIG_VGA_SIZE_800x600
#define VGA_W 800
#define VGA_H 600
#else
#define VGA_W 400
#define VGA_H 300
#endif

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
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
    .width = inw(VGACTL_ADDR + 2), .height = inw(VGACTL_ADDR),
    .vmemsz = 0
  };
}

static inline int fb_pos(int x ,int y, int w){
  return x + y * w;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  uint32_t *pxl = (uint32_t *) ctl->pixels;
  for (int i = 0; i < h; i++){
    for (int j = 0; j < w; j++){
      //if (x + j < VGA_W && y + i < VGA_H)
      outl(FB_ADDR + fb_pos(x + j, y + i, VGA_W) * 4, pxl[fb_pos(j, i, w)]);
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
