#ifdef CONFIG_FTRACE
extern void ftrace_call(vaddr_t pc, vaddr_t dest);
extern void ftrace_ret(vaddr_t pc, vaddr_t dest);
#endif


def_EHelper(jal){
#ifdef CONFIG_FTRACE
  ftrace_call(s->pc, s->pc + id_src1->imm);
#endif
  rtl_j(s, s->pc + id_src1->imm);
  rtl_li(s, ddest, s->pc + 4);
}

def_EHelper(jalr){
#ifdef CONFIG_FTRACE
  if (id_src1->imm == 8 && id_src2->imm == 0) 
    ftrace_ret(s->pc, s->pc + *dsrc1 + id_src2->imm);
  else 
    ftrace_call(s->pc, s->pc + *dsrc1 + id_src2->imm);
#endif
  rtl_addi(s, s0, dsrc1, id_src2->imm);
  rtl_jr(s, s0);
  rtl_li(s, ddest, s->pc + 4);
}