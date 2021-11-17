#include <isa.h>
#include "../local-include/rtl.h"

word_t isa_raise_intr(Decode *s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  rtl_li(s, mepc, epc);
  rtl_li(s, mcause, NO);
  rtl_jr(s, mtvec);
  #ifdef CONFIG_ETRACE
  log_write("Exception NO:%d in " FMT_WORD ", jump to "FMT_WORD"\n", NO, epc, *mtvec);
  #endif
  return 0;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
