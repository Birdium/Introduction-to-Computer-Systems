def_EHelper(lui){
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc){
  rtl_li(s, s0, id_src1->imm);
  rtl_add(s, ddest, &cpu.pc, s0);
}

def_EHelper(addi){
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}