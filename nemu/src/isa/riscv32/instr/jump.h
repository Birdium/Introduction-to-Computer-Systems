def_EHelper(jal){
  rtl_j(s, s->pc + id_src1->imm);
  printf("0x%08x\n", s->pc + id_src1->imm);
  rtl_li(s, ddest, s->pc + 4);
  printf("0x%08x\n", s->pc + 4);
}