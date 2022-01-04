#include <isa.h>
#include "../local-include/rtl.h"

#define IRQ_TIMER 0x80000007

#define MIE (1 << 3)
#define MPIE (1 << 7)


word_t isa_raise_intr(Decode *s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  // Log("intr before: %x", *mstatus);
  rtl_li(s, mstatus, (*mstatus & ~(MIE | MPIE)) | ((*mstatus & MIE) << 4)); 
  // Log("intr after:  %x", *mstatus);
  rtl_li(s, mepc, epc);
  rtl_li(s, mcause, NO);
  rtl_jr(s, mtvec);
  #ifdef CONFIG_ETRACE
  log_write("Exception NO:%d in " FMT_WORD ", jump to "FMT_WORD"\n", NO, epc, *mtvec);
  #endif
  // printf("Intr, pc:%x\n", cpu.pc);
  
  return *mtvec;
}

word_t isa_query_intr() {
  if (cpu.INTR == true && *mstatus & MIE) {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
