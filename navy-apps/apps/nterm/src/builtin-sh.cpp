#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  setenv("PATH", "/bin", 0);

  size_t len = strlen(cmd);
  char buf[128];
  memcpy(buf, cmd, len);
  buf[len-1] = '\0';
  
  char *argv[16];
  argv[0] = strtok(buf, " ");
  printf("%s\n", argv[0]);

  char *token = argv[0]; 

  int i = 1;
  while(token != NULL) {
    token = strtok(NULL, " ");
    argv[i] = token;
    // printf("%s\n", token);
    // printf("%d\n", i);
    i++;
  }

  execvp(argv[0], argv);
  // char *echo = "echo";
  // if (strcmp(token, "echo") == 0){
  //   token = strtok(NULL, " ");
  //   if (token) sh_printf("%s", token);
  //   while (token = strtok(NULL, " "))
  //     sh_printf(" %s", token);
  //   sh_printf("\n");
  // }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
