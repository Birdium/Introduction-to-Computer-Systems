#include <common.h>
#include <fs.h>
#include <sys/time.h>
#include "syscall.h"

#define CONFIG_STRACE

static uintptr_t sys_brk(void *addr){
  // void *new_pbrk = addr;
  return 0;
}

// size_t sys_write(int fd, char *buf, size_t count){
//   // printf("0x%x\n", count);
//   uintptr_t ret = 0;
//   if (fd == 1 || fd == 2){
//     //printf("%d\n", count);
//     for(size_t i = 0; i < count; i++){
//       putch(*(buf + i));
//       ret++;
//     }
//   } else {
//     ret = fs_write(fd, buf, count);
//   }
//   return ret;
// }

int sys_gettimeofday(struct timeval *tv, struct timezone *tz);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  #ifdef CONFIG_STRACE
    Log("syscall ID = %d, args = 0x%x, 0x%x, 0x%x", a[0],a[1],a[2],a[3]);
  #endif
  switch (a[0]) {
    case SYS_yield: yield(); c->GPRx = 0; break;
    case SYS_exit: halt(a[1]); break;
    case SYS_open: c->GPRx = fs_open((const char*)a[1], a[2], a[3]); break; 
    case SYS_read: c->GPRx = fs_read(a[1], (void*)a[2], a[3]); break;
    case SYS_write: c->GPRx = fs_write(a[1], (void*)a[2], a[3]); break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_brk: c->GPRx = sys_brk((void*)a[1]); break;
    case SYS_gettimeofday: c->GPRx = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
