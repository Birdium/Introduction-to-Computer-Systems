#include <proc.h>

#define MAX_NR_PROC 4
void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  // printf("%p\n", &j);
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
    // while (1);
  }
  assert(0);
}

#define uproc_name "/bin/pal"

void init_proc() {
  // context_kload(&pcb[0], hello_fun, "114");
  context_kload(&pcb[0], hello_fun, "114");
  // char *argv[] = {"--skip", NULL};
  // char *envp[] = {"114514", NULL};
  char *argv[] = {uproc_name, NULL};
  char *envp[] = {NULL};
  context_uload(&pcb[1], uproc_name, argv, envp);
  switch_boot_pcb();
  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/nterm");

}

static int u_cnt;
#define SWAP_CNT 30000

Context* schedule(Context *prev) {
  current->cp = prev;
  if (current == &pcb[0]) current = &pcb[1];
  else {
    u_cnt++;
    current = &pcb[1];
    if (u_cnt == SWAP_CNT) {
      current = &pcb[0];
      u_cnt = 0;
    }
  }
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
