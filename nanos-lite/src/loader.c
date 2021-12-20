#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__) 
# define EXPECT_TYPE EM_RISCV
#elif
# error unsupported ISA __ISA__
#endif

// size_t ramdisk_read(void *buf, size_t offset, size_t len);

#define VALID_MASK 0x1
#define OFFSET_MASK 0xfff
#define BASE_ADDR_MASK ~OFFSET_MASK
#define PROT 0x7

inline uint32_t min(uint32_t x, uint32_t y) {
  return x > y ? x : y;
}

static uintptr_t loader(PCB *pcb, const char *filename)   {
  Elf_Ehdr ehdr;
  Elf_Phdr phdr;
  int fd = fs_open(filename, 0, 0);
  int d = fs_read(fd, &ehdr, sizeof(Elf_Ehdr)); 
  assert(d == sizeof(Elf_Ehdr));
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  // printf("TYPE:%d, expected:%d\n", ehdr.e_machine, EXPECT_TYPE);
  // assert(ehdr.e_machine == EXPECT_TYPE);
  for(int i = 0; i < ehdr.e_phnum; i++){
    d = fs_lseek(fd, ehdr.e_phoff + i*sizeof(Elf_Phdr), SEEK_SET);
    d = fs_read(fd, &phdr, sizeof(Elf_Phdr)); 
    assert(d == sizeof(Elf_Phdr));
    if (phdr.p_type == PT_LOAD) {
      uintptr_t vaddr = phdr.p_vaddr;
      uintptr_t faddr = phdr.p_vaddr + phdr.p_filesz;
      uintptr_t maddr = phdr.p_vaddr + phdr.p_memsz;
      void *paddr = 0;
      fs_lseek(fd, phdr.p_offset, SEEK_SET);
      while(vaddr < faddr) {
        paddr = new_page(1); // paddr : 0x*****000
        int read_len = min(PGSIZE, faddr - vaddr);
        map(&pcb->as, (void *)(vaddr & OFFSET_MASK), paddr, PROT);
        fs_read(fd, paddr + (vaddr & OFFSET_MASK), read_len);
        vaddr += read_len;
      }
      assert(vaddr == faddr);
      if (vaddr & OFFSET_MASK){
        int read_len = min(PGSIZE - (vaddr & OFFSET_MASK), maddr - vaddr);
        memset(paddr + (vaddr & OFFSET_MASK), 0, read_len);
        vaddr += read_len;
      } // in the same page, no need to allocate
      while(vaddr < maddr) {
        paddr = new_page(1); // paddr : 0x*****000
        int read_len = min(PGSIZE, maddr - vaddr);
        map(&pcb->as, (void *)(vaddr & OFFSET_MASK), paddr, PROT);
        memset(paddr + (vaddr & OFFSET_MASK), 0, read_len);
        vaddr += read_len;
      }
      assert(vaddr == maddr);
    }
  }
  fd = fs_close(fd);
  printf("Loading \"%s\" in %x.\n", filename, phdr.p_vaddr);
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  pcb->cp = kcontext(RANGE(pcb->stack, pcb->stack + sizeof(pcb->stack)), entry, arg);
  printf("%p\n", pcb->cp);
  // printf("%p\n", entry);
  // printf("%x %x\n", pcb->stack, pcb->stack + sizeof(pcb->stack));
}

// void protect(AddrSpace *as);

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  // allocate memory
  protect(&pcb->as);
  void *ustack_start = new_page(8);
  Area ustack = {ustack_start, ustack_start + 8 * PGSIZE};
  // pre-process
  int argc = 0, envc = 0, str_len = 0, str_size, init_size = 0;
  printf("Loading file: %s\n", filename);
  // printf("%p\n", envp[0]);
  if (argv)
    while(argv[argc]) {
      str_len += strlen(argv[argc]) + 1;
      argc++;
    }
  // printf("%d %d\n", argc, envc);
  if (envp)
    while(envp[envc]) {
      printf("%s\n", envp[envc]);
      str_len += strlen(envp[envc]) + 1;
      envc++;
    }
  str_size = (str_len + sizeof(uintptr_t) - 1) / sizeof(uintptr_t) * sizeof(uintptr_t);
  init_size = (argc + envc + 3) * sizeof(uintptr_t) + str_size;

  // init
  uintptr_t *init_addr = ustack.end - init_size;
  uintptr_t *ap = init_addr; 
  char *str_addr = ustack.end - str_size;
  char *sp = str_addr;
  *ap++ = argc;
  for (int i = 0; i < argc; i++){
    strcpy(sp, argv[i]);
    *ap++ = (uintptr_t)sp;
    sp += strlen(argv[i]) + 1;
  }
  *ap++ = 0;
  for (int i = 0; i < envc; i++){
    strcpy(sp, envp[i]);
    *ap++ = (uintptr_t)sp;
    sp += strlen(envp[i]) + 1;
  }
  *ap++ = 0;
  // printf("%d\n", *init_addr);

  uintptr_t entry = loader(pcb, filename);
  pcb->cp = ucontext(&pcb->as, RANGE(pcb->stack, pcb->stack + sizeof(pcb->stack)), (void*)entry);
  pcb->cp->GPRx = (uintptr_t)init_addr;
  // printf("%x %x", heap.start, heap.end);
}