#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  // char *empty[] =  {NULL };
  printf("%d\n", *args);
  int argc = *args;
  char **argv = (char**)(args + 1);
  void **envp = (char**)(args + argc + 2);
  assert(0);
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
