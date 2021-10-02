def_EHelper(jal){
  rtl_j(s, s->pc + id_src1->imm);
  rtl_li(s, ddest, s->pc + 4);
}

def_EHelper(jalr){
  rtl_addi(s, s0, dsrc1, id_src2->imm);
  rtl_jr(s, s0);
  rtl_li(s, ddest, s->pc + 4);
}