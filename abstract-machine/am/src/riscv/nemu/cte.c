#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <sys/time.h>

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

static Context* (*user_handler)(Event, Context*) = NULL;

void prt_() {
  printf("a\n");
}

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0xb :
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
    c->mepc += 4;

  }

  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  int mstatus_init = 0x1800;
  uintptr_t mscratch_init = 0;
  asm volatile("csrw mtvec, %0; csrw mstatus, %1; csrw mscratch, %2" : : "r"(__am_asm_trap), "r"(mstatus_init), "r"(mscratch_init));

  // register event handler
  user_handler = handler;

  return true;
}

#define KERNEL 0

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *cp = kstack.end - sizeof(Context); // WTF is this ??? 
  cp->pdir = NULL;
  cp->mstatus = 0x1880;
  cp->mepc = (uintptr_t)entry - sizeof(uintptr_t);
  cp->GPRx = (uintptr_t)arg;
  cp->np = KERNEL;
  cp->gpr[2] = (uintptr_t)kstack.end;
  return cp;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
