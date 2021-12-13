#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  // char *empty[] =  {NULL };
  int argc = *args;
  char *argv = args + 1;
  char *envp = args + argc + 2;
  environ = (char**)envp;
  printf("%p\n", environ);
  exit(main(argc, (char**)argv, (char**)envp));
  assert(0);
}
