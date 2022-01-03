#include <isa.h>
#include "../local-include/rtl.h"

#define IRQ_TIMER 0x80000007

#define MIE (1 << 3)
#define MPIE (1 << 7)


word_t isa_raise_intr(Decode *s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  *mstatus = (*mstatus & ~(MIE | MPIE)) | ((*mstatus & MIE) << 4); 
  rtl_li(s, mepc, epc);
  rtl_li(s, mcause, NO);
  rtl_jr(s, mtvec);
  #ifdef CONFIG_ETRACE
  log_write("Exception NO:%d in " FMT_WORD ", jump to "FMT_WORD"\n", NO, epc, *mtvec);
  #endif
  return 0;
}

word_t isa_query_intr() {
  if (cpu.INTR == true && *mstatus & MIE) {
    cpu.INTR = false;
    assert(0);
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
