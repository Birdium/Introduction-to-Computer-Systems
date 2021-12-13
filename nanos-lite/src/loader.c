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
      fs_lseek(fd, phdr.p_offset, SEEK_SET);
      fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
      memset((void *)phdr.p_vaddr + phdr.p_filesz, 0, phdr.p_memsz - phdr.p_filesz);
    }
  }
  fd = fs_close(fd);
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

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  uintptr_t entry = loader(pcb, filename);
  void *ustack_end = heap.end - sizeof(uintptr_t);
  // pre-process
  int argc = 0, envc = 0, str_len = 0, str_size, init_size = 0;
  while(argv[argc]) {
    str_len += strlen(argv[argc]) + 1;
    argc++;
  }
  assert(0);
  // printf("%d %d\n", argc, envc);
  while(envp[envc]) {
    str_len += strlen(envp[envc]) + 1;
    envc++;
  }
  str_size = (str_len + sizeof(uintptr_t) - 1) / sizeof(uintptr_t) * sizeof(uintptr_t);
  init_size = (argc + envc + 3) * sizeof(uintptr_t) + str_size;

  // init
  uintptr_t *init_addr = ustack_end - init_size;
  uintptr_t *ap = init_addr; 
  char *str_addr = ustack_end - str_size;
  char *sp = str_addr;
  int i = 0;
  *ap = argc;
  while(argv[i]) {
    *ap++ = (uintptr_t)argv[i];
    strcpy(sp, argv[i]);
    sp += strlen(argv[i]) + 1;
  }
  ap++;
  while(envp[i]) {
    *ap++ = (uintptr_t)envp[i];
    strcpy(sp, envp[i]);
    sp += strlen(envp[i]) + 1;
  }

  pcb->cp = ucontext(&pcb->as, RANGE(pcb->stack, pcb->stack + sizeof(pcb->stack)), (void*)entry);
  pcb->cp->GPRx = (uintptr_t)init_addr;
  // printf("%x %x", heap.start, heap.end);
}