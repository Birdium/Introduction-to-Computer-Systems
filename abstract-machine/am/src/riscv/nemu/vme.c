#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }
  // printf("%x\n", kas.ptr);

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  // printf("updir : %p\n", updir);
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
  // printf("pro ptr %p\n", updir);
  // printf("kas ptr %p\n", kas.ptr);
}

void unprotect(AddrSpace *as) {
  pgfree_usr(as->ptr);
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  } 
}

#define VALID_MASK 0x1
#define OFFSET_MASK 0xfff
#define BASE_ADDR_MASK ~OFFSET_MASK

// #define ENABLE_LOG

#ifdef ENABLE_LOG
#define Log(format, ...) printf(format, ## __VA_ARGS__)
#else 
#define Log(format, ...) while(0)
#endif

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // Log("map: vaddr: %p, paddr: %p\n, dir base: %p\n", va, pa, as->ptr);
  PTE *pg_dir_base = as->ptr;
  int dir_ndx  = ((uintptr_t)va) >> 22,
      // offset = ((uintptr_t)va) & 0xfff,
      table_ndx = (((uintptr_t)va) >> 12) & 0x3ff; 
  // Log("%d %d %p %p\n", dir_ndx, table_ndx, va, pa);
  PTE pg_dir_entry = pg_dir_base[dir_ndx];
  if ((pg_dir_entry & 0x1) == 0) { // V == 0, Invalid.
    // Log("Allocating New Page.\n");
    pg_dir_entry = (PTE)pgalloc_usr(PGSIZE); // pg_dir_entry.base_addr = "new table's base addr"
    pg_dir_base[dir_ndx] = pg_dir_entry | VALID_MASK; 
    // Log("Allocated 0x%08x in 0x%08x\n", va, pg_dir_entry);
  }
  PTE *pg_table_base = (PTE*) (pg_dir_entry & BASE_ADDR_MASK);

  pg_table_base[table_ndx] = (PTE)((uintptr_t)pa & BASE_ADDR_MASK) | VALID_MASK; // pg_table_entry.base_addr = pa; 
  // printf("%x\n",pg_table_base[1]);
  // assert(va == pa); 

}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *cp = kstack.end - sizeof(Context); 
  cp->pdir = as->ptr;
  cp->mstatus = 0x1880;
  cp->mepc = (uintptr_t)entry - sizeof(uintptr_t);
  // printf("mepc : %p\n", cp->mepc);
  // printf("cp : %p\n", cp);
  return cp;
}
