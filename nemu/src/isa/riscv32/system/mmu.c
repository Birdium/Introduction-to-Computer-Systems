#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

#define VALID_MASK 0x1
#define OFFSET_MASK 0xfff
#define BASE_ADDR_MASK ~OFFSET_MASK

#define satp (&cpu.csr[4]._32)

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  word_t dir_ndx  = ((uintptr_t)vaddr) >> 22,
        //  offset = ((uintptr_t)vaddr) & 0xfff,
         table_ndx = (((uintptr_t)vaddr) >> 12) & 0x3ff; 

  paddr_t pg_dir_base = *satp & BASE_ADDR_MASK; // 0x3fffff originally, but we have 4GB only, the upper 2bit ignored.

  word_t pg_dir_entry = paddr_read(pg_dir_base + 4 * dir_ndx, 4);
  if ((pg_dir_entry & 0x1) == 0) { // V == 0, Invalid.
    assert(0);
    return MEM_RET_FAIL;
  } 
  paddr_t pg_table_base = pg_dir_entry & BASE_ADDR_MASK;

  word_t pg_table_entry = paddr_read(pg_table_base + 4 * table_ndx, 4);
  if ((pg_dir_entry & 0x1) == 0) {
    assert(0);
    return MEM_RET_FAIL;
  }

  // if (offset + len > PAGE_SIZE) return MEM_RET_CROSS_PAGE; // only need in CISC
  paddr_t pg_paddr = pg_table_entry & BASE_ADDR_MASK;

  return pg_paddr | MEM_RET_OK;
}