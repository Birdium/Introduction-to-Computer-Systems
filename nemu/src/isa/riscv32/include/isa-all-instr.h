#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(auipc) f(jal) f(jalr) \
f(beq) f(bne) f(blt) f(bge) f(lb) f(lh) f(lw) f(sw) \
f(addi) f(slti) f(sltiu) f(srli) f(srai) f(add) f(sub) f(srl) f(sra)\
f(inv) f(nemu_trap)

def_all_EXEC_ID();
