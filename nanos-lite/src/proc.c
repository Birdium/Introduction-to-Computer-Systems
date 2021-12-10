#include <proc.h>

#define MAX_NR_PROC 4
void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
    // while (1);
  }
}

void init_proc() {
  printf("%p %p", pcb[0].stack, pcb[1].stack);
  context_kload(&pcb[0], hello_fun, "114");
  context_uload(&pcb[1], "/bin/pal");
  switch_boot_pcb();
  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/nterm");

}

Context* schedule(Context *prev) {
  current->cp = prev;
  // current = &pcb[0];
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
