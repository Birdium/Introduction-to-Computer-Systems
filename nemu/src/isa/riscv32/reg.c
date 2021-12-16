#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
const char *regs_csr[] = {
	"mcause", "mstatus", "mepc", "mtvec", "satp"
};

#define GPR_NUM sizeof(regs) / sizeof(char *)
#define CSR_NUM sizeof(regs_csr) / sizeof(char *)

void isa_reg_display() {
	int i;
	for(i = 0; i < 32; i++){
		printf("%-16s0x%-16x%-16u\n", regs[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
	}
	printf("%-16s0x%-16x%-16u\n", "pc", cpu.pc, cpu.pc);
	for(i = 0; i < 4; i++){
		printf("%-16s0x%-16x%-16u\n", regs_csr[i], cpu.csr[i]._32, cpu.csr[i]._32);
	}
}

word_t isa_reg_str2val(const char *s, bool *success) {
	if (strcmp(s, "pc") == 0){
		*success = true;
		return cpu.pc;
	}
	for(int i = 0; i < GPR_NUM; i++){
		if (strcmp(s, regs[i]) == 0){
			*success = true;
			return cpu.gpr[i]._32;
		}	
	}
	for(int i = 0; i < CSR_NUM; i++){
		if (strcmp(s, regs_csr[i]) == 0){
			*success = true;
			return cpu.csr[i]._32;
		}	
	}
	*success = false;
	return 0;
}
