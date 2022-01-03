#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <sys/time.h>

// const char *regs[] = {
//   "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
//   "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
//   "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
//   "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
// };

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {

    Event ev = {0};
    switch (c->mcause) {
      case 0xb :
          // printf("0x%x\n", c->gpr[17]);
        switch (c->GPR1) {
          case 0xffffffff : ev.event = EVENT_YIELD; break;
          default: ev.event = EVENT_SYSCALL; break;
        }
        break;
      case 0x80000007: ev.event = EVENT_IRQ_TIMER; break;
      default: ev.event = EVENT_ERROR; break;
    }
    c = user_handler(ev, c);
    assert(c != NULL);
    // printf("%x\n", *(uint32_t*)0x824fa014);
    c->mepc += 4;

    // if(c->GPR1 == 19 && tm){
    //   printf("%d:%d\n", (int)tm->tv_sec, (int)tm->tv_usec);
    // }

  }

	// for(int i = 0; i < 32; i++){
	// 	printf("%-16s 0x%-16x\n", regs[i], c->gpr[i]);
	// }
  // printf("0x%-16x0x%-16x0x%-16x\n", c->mcause, c->mstatus, c->mepc);
  // assert(c->pdir);
  __am_switch(c);
  assert(0);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  int mstatus_init = 0x1800;
  asm volatile("csrw mtvec, %0; csrw mstatus, %1" : : "r"(__am_asm_trap), "r"(mstatus_init));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *cp = kstack.end - sizeof(Context) - 64; // WTF is this ??? 
  cp->pdir = NULL;
  cp->mstatus = 0x1808;
  cp->mepc = (uintptr_t)entry;
  cp->GPRx = (uintptr_t)arg;
  return cp;
}

void yield() {
  // printf("%x\n", *(uint32_t*)0x824fa014);
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
