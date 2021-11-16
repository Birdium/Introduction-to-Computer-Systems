#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

// const char *regs[] = {
//   "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
//   "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
//   "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
//   "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
// };

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {

    Event ev = {0};
    switch (c->mcause) {
      case 0x11 :{
        switch (c->gpr[17]) {
          case -1 : ev.event = EVENT_YIELD; break;
          default: ev.event = EVENT_ERROR; break;
        }
      } 
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

	// for(int i = 0; i < 32; i++){
	// 	printf("%-16s 0x%-16x\n", regs[i], c->gpr[i]);
	// }
  // printf("0x%-16x0x%-16x0x%-16x\n", c->mcause, c->mstatus, c->mepc);

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
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
