#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k, 
#define NR_KEYS sizeof(keyname) / sizeof(keyname[0])

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  memset(buf, 0, sizeof(buf));
  int ret = NDL_PollEvent(buf, sizeof(buf));
  if (ret == 0 || strlen(buf) <= 3) return 0;
  printf("%s", buf);

  buf[strlen(buf) - 1] = '\0';
  for(int i = 1; i < NR_KEYS; i++) {
    // printf("A: %s\nB: %s\n", buf + 3, keyname[i]);
    if (strcmp(buf + 3, keyname[i]) == 0) {
      ev->key.keysym.sym = i;
      break;
    }
  }
  // printf("%s\n", buf[1]);
  if (buf[1] == 'd') ev->type = SDL_KEYDOWN;
  else if (buf[1] == 'u') ev->type = SDL_KEYUP;
  else assert(0);

  // printf("%d\n", ev->type);

  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  while(SDL_PollEvent(event) == 0);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
