#include <common.h>
#include <fs.h>
#include <proc.h>
#include <sys/time.h>
#include "syscall.h"

// #define CONFIG_STRACE

// static uintptr_t sys_brk(void *addr){
//   // void *new_pbrk = addr;
//   return 0;
// }
int mm_brk(uintptr_t brk);

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

int sys_gettimeofday(struct timeval *tv, void *tz) {
  uint64_t am_uptime = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = am_uptime / 1000000;
  tv->tv_usec = am_uptime % 1000000;
  // printf("%p of sec, %p of usec\n", &tv->tv_sec, &tv->tv_usec);
  // printf("in nanos:%x\n", sizeof(struct timeval));
  // assert(tv->tv_usec == 0);
  // printf("%d:%d\n", (int)tv->tv_sec, (int)tv->tv_usec);
  return 0;
}

void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
void switch_boot_pcb();

int sys_execve(const char *filename, char *const argv[], char *const envp[]){
  // printf("%p %p\n", argv, envp);
  // printf("%p\n", envp[0]);
  int fd = fs_open(filename, 0, 0);
  if (fd == -1) return -2;
  context_uload(current, filename, argv, envp);
  switch_boot_pcb();
  yield();
  return 0;
}

int sys_exit() {
  // halt(0);
  char *empty[] = {NULL};
  char *argc[] = {"bin/dummy", NULL};
  sys_execve("/bin/dummy", argc, empty);
  return -1;
}

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
    // case SYS_yield: c->GPRx = schedule(); break;
    // case SYS_exit: halt(a[1]); break;
    case SYS_exit: c->GPRx = sys_exit();
    case SYS_open: c->GPRx = fs_open((const char*)a[1], a[2], a[3]); break; 
    case SYS_read: c->GPRx = fs_read(a[1], (void*)a[2], a[3]); break;
    case SYS_write: c->GPRx = fs_write(a[1], (void*)a[2], a[3]); break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_brk: c->GPRx = mm_brk(a[1]); break;
    // case SYS_execve: naive_uload(NULL, (const char*)a[1]); break;
    case SYS_execve: c->GPRx = sys_execve((char *)a[1], (char *const *)a[2], (char *const *)a[3]); break;
    case SYS_gettimeofday: ;
      c->GPRx = sys_gettimeofday((void*)a[1], (void*)a[2]); 
      // printf("%d\n", (int)tm->tv_usec); 
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
