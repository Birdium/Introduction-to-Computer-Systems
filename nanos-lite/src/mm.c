#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  pf += PGSIZE * nr_page;
  return pf - PGSIZE * nr_page;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  int nr_page = (n + PGSIZE - 1) / PGSIZE;
  char* ret = new_page(nr_page);
  memset(ret, 0, nr_page * PGSIZE);
  return ret;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

#define OFFSET_MASK 0xfff
#define BASE_ADDR_MASK ~OFFSET_MASK
#define PG_ST(addr) (void*)((uintptr_t)(addr) & BASE_ADDR_MASK)
#define PG_ED(addr) (void*)((uintptr_t)(addr) & BASE_ADDR_MASK + PGSIZE)
#define PROT 0x7

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  void *va = PG_ST(current->max_brk) + PGSIZE;
  printf("va:%d\n", current->max_brk);
  printf("va:%d\n", brk);
  while(va <= PG_ST(brk)) {
    void *pa = new_page(1);
    map(&current->as, va, pa, PROT);
    va += PGSIZE;
  }
  current->max_brk = brk;
  return 0;  
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
