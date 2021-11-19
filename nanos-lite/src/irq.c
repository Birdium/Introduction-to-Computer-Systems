#include <common.h>
#include <sys/time.h>

extern struct timeval *tm;

void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD : 
      printf("Yielded...\n");
      #ifdef __ISA_RISCV32__
      c->mepc += 4;
      #endif
      break;
    case EVENT_SYSCALL :
      do_syscall(c);
      #ifdef __ISA_RISCV32__
      c->mepc += 4;
      #endif
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  if(c->GPR1 == 19 && tm){
    printf("%d\n", (int)tm->tv_usec);
    printf("Addr :%p\n", tm);
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
