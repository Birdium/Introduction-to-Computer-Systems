#include <am.h>
#include <nemu.h>
//#include <SDL2/SDL.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

static int head = 0, tail = 0;
static int bufnum = 0;

// static void audio_play(void *userdata, uint8_t *stream, int len) {
//   int nread = len;
//   volatile int cnt = inl(AUDIO_COUNT_ADDR);
//   if (cnt < len) nread = cnt;
//   uint8_t *buf  = AUDIO_SBUF_ADDR;
//   for(int i = 0; i < len; i++){
//     assert(head != tail);
//     stream[i] = inb(buf + tail);
//     tail++; if (tail == bufnum) tail = 0;
//   }
//   cnt -= nread;
//   outl(AUDIO_COUNT_ADDR, cnt);
//   if (len > nread) {
//     memset(stream + nread, 0, len - nread);
//   }
// }

static void audio_write(uint8_t *buf, int len) {
  uint8_t *sbuf = (uint8_t *) AUDIO_SBUF_ADDR;
  for (int i = 0; i < len; i++) {
    assert(head != tail - 1);
    outb((uint32_t)sbuf + i, buf[i + head]);
    head++; if (head >= bufnum) head = 0;
  }
  outl(AUDIO_COUNT_ADDR, inl(AUDIO_COUNT_ADDR) + len);
}

void __am_audio_init() {
  outl(AUDIO_COUNT_ADDR, 0);
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
  bufnum = cfg->bufsize / sizeof(uint8_t);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
  outl(AUDIO_INIT_ADDR, 1);

  // SDL_AudioSpec s = {};
  // s.freq = ctrl->freq;
  // s.format = AUDIO_S16SYS;
  // s.channels = ctrl->channels;
  // s.samples = ctrl->samples;
  // s.callback = audio_play;
  // s.userdata = NULL;

  // int ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
  // if (ret == 0) {
  //   SDL_OpenAudio(&s, NULL);
  //   SDL_PauseAudio(0);
  // }
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  int len = ctl->buf.end - ctl->buf.start;
  audio_write(ctl->buf.start, len);
}
