#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>
enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

#define SBUF_NUM CONFIG_SB_SIZE / sizeof(uint8_t)

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

static int tail = 0, cnt = 0;

static void audio_play(void *userdata, uint8_t *stream, int len) {
  int nread = len;
  volatile int cnt = audio_base[reg_count];
  if (cnt < len) nread = cnt;
  for(int i = 0; i < len; i++){
    stream[i] = sbuf[i + tail];
    tail++; if (tail == CONFIG_SB_SIZE) tail = 0;
  }
  cnt -= nread;
  audio_base[reg_count] = cnt;
  if (len > nread) {
    memset(stream + nread, 0, len - nread);
  }
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  switch (offset){
    case 0: case 4: case 8: break;
    case 12:
      audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
      break;
    case 16: ;  
      SDL_AudioSpec s = {};
      s.freq = audio_base[reg_freq];
      s.format = AUDIO_S16SYS;
      s.channels = audio_base[reg_channels];
      s.samples = audio_base[reg_samples];
      s.callback = audio_play;
      s.userdata = NULL;
      int ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
      if (ret == 0) {
        SDL_OpenAudio(&s, NULL);
        SDL_PauseAudio(0);
      }
      break;
    case 20: 
      if (is_write) cnt = audio_base[reg_count];
      else audio_base[reg_count] = cnt;
      break;
    default: assert(0);
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
}
