#include <common.h>
#include "syscall.h"

enum {
    SYS_EXIT = 0,
    SYS_YIELD = 1
} syscall_type;

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case SYS_YIELD: yield(); c->GPRx = 0; break;
    case SYS_EXIT: halt(a[0]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
