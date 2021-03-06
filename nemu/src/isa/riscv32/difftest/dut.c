#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  //printf(FMT_WORD " " FMT_WORD "\n", cpu.pc, pc);
  //if (cpu.pc != pc) return false;
  for(int i = 0; i < 32; i++){
    //printf("%d\n" FMT_WORD " " FMT_WORD "\n", i, gpr(i), ref_r->gpr[i]._32);
    if (cpu.gpr[i]._32 != ref_r->gpr[i]._32) return false;
  }
  return true;
}

void isa_difftest_attach() {
}
