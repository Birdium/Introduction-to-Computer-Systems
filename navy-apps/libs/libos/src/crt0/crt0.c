#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  // char *empty[] =  {NULL };
  int argc = *args;
  printf("addr:%p\n", &argc);
  char **argv = args + 1;
  char **envp = args + argc + 2;
  // for(int i = 0; i < argc; i++)
  //   printf("%s\n", argv[i]);
  // printf("%d %p %p\n", argc, argv, envp);
  exit(main(argc, (char**)argv, (char**)envp));
  assert(0);
}
