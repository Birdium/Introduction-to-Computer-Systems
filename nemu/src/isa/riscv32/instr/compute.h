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

def_EHelper(slti){
  rtl_slti(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sltiu){
  rtl_sltiu(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli){
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai){
  rtl_srai(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(add){
  //printf("0x%08x + 0x%08x\n", *dsrc1, *dsrc2);
  rtl_add(s, ddest, dsrc1, dsrc2);
  //printf("= 0x%08x\n", *ddest);
}

def_EHelper(sub){
  rtl_sub(s, ddest, dsrc1, dsrc2);
}