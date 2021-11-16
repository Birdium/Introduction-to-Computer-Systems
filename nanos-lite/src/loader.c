#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf_hdr;
  // Elf_Phdr pro_hdr;
  int d = ramdisk_read(&elf_hdr, 0, sizeof(elf_hdr));
  assert(d != 0);
  printf("0x%x", *(uint32_t *)elf_hdr.e_ident);
  assert(*(uint32_t *)elf_hdr.e_ident == 0x464c457f);
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

