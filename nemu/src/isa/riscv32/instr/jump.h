def_EHelper(jal){
  rtl_j(s, id_src1->imm);
  rtl_li(s, ddest, s->pc + 4);
}