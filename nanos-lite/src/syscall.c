#include <common.h>
#include "syscall.h"

enum {
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

static uintptr_t sys_write(int fd, char *buf, size_t count){
  // printf("0x%x\n", count);
  uintptr_t ret = 0;
  if (fd == 1 || fd == 2){
    //printf("%d\n", count);
    for(size_t i = 0; i < count; i++){
      putch(*(buf + i));
      ret++;
    }
  }
  return ret;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case SYS_yield: yield(); c->GPRx = 0; break;
    case SYS_exit: halt(a[1]); break;
    case SYS_write: c->GPRx = sys_write((int)a[1], (void*)a[2], 13); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
