def_EHelper(beq){
  if (id_src1->imm == id_src2->imm)
  rtl_j(s, s->pc + id_dest->imm);
}

def_EHelper(bne){
  if (id_src1->imm != id_src2->imm)
  rtl_j(s, s->pc + id_dest->imm);
}