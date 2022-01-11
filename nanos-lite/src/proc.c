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
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
    // while (1);
  }
}

#define uproc_name_1 "/bin/nterm"
#define uproc_name_2 "/bin/nterm"
#define uproc_name_3 "/bin/nterm"

void init_proc() {
  context_kload(&pcb[0], hello_fun, "114514");
  // context_kload(&pcb[1], hello_fun, "1919810");
  char *argv_1[] = {uproc_name_1, NULL};
  char *envp_1[] = {NULL};
  char *argv_2[] = {uproc_name_2, NULL};
  char *envp_2[] = {NULL};
  char *argv_3[] = {uproc_name_3, "--skip", NULL};
  char *envp_3[] = {NULL};
  // for(int i = 0; i < 4; i++) {
  //   printf("pcb%d : %p %p\n",  i, &pcb[i], (char*)(&pcb[i]) + 8 * PGSIZE);
  // }
  // context_uload(&pcb[0], "/bin/hello", argv, envp);
  context_uload(&pcb[1], uproc_name_1, argv_1, envp_1);
  context_uload(&pcb[2], uproc_name_2, argv_2, envp_2);
  context_uload(&pcb[3], uproc_name_3, argv_3, envp_3);
  switch_boot_pcb();
  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/nterm");

}

static int u_cnt;
int fg_pcb = 1;
#define SWAP_CNT 1000

Context* schedule(Context *prev) {
  current->cp = prev;
  if (current == &pcb[0]) current = &pcb[fg_pcb];
  else {
    u_cnt++;
    if (u_cnt == SWAP_CNT) {
      current = &pcb[0];
      u_cnt = 0;
    }
    else 
      current = &pcb[fg_pcb];
  }
  return current->cp;
}