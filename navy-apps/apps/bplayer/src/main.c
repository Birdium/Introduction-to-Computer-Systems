#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vorbis.h>
#include <fixedptc.h>

#define MUSIC_PATH "/share/music/Do.ogg"
#define SAMPLES 4096
#define MAX_VOLUME 128

stb_vorbis *v = NULL;
stb_vorbis_info info = {};
int16_t *stream_save = NULL;

int main(int argc, char *argv[]) {
  FILE *fp = fopen(MUSIC_PATH, "r");
  assert(fp);
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  void *buf = malloc(size);
  assert(size);
  fseek(fp, 0, SEEK_SET);
  int ret = fread(buf, size, 1, fp);
  assert(ret == 1);
  fclose(fp);

  int error;
  v = stb_vorbis_open_memory(buf, size, &error, NULL);
  printf("%d\n", error);
  assert(v);
  info = stb_vorbis_get_info(v);

  return 0;
}
