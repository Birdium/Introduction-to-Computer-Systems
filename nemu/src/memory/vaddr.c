#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
  paddr_t paddr;
  int mmu_status = isa_mmu_check(addr, len, MEM_TYPE_IFETCH);
  switch (mmu_status){
    case MMU_DIRECT: paddr = addr; break;
    case MMU_TRANSLATE: paddr = isa_mmu_translate(addr, len, MEM_TYPE_IFETCH); break;
    default: assert(0); break;
  }
  return paddr_read(paddr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  paddr_t paddr;
  int mmu_status = isa_mmu_check(addr, len, MEM_TYPE_READ);
  switch (mmu_status){
    case MMU_DIRECT: paddr = addr; break;
    case MMU_TRANSLATE: paddr = isa_mmu_translate(addr, len, MEM_TYPE_READ); break;
    default: assert(0); break;
  }
  return paddr_read(paddr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  paddr_t paddr;
  int mmu_status = isa_mmu_check(addr, len, MEM_TYPE_WRITE);
  switch (mmu_status){
    case MMU_DIRECT: paddr = addr; break;
    case MMU_TRANSLATE: paddr = isa_mmu_translate(addr, len, MEM_TYPE_WRITE); break;
    default: assert(0); break;
  }
  paddr_write(paddr, len, data);
}
