def_EHelper(lui){
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc){
  rtl_addi(s, ddest, &cpu.pc, id_src1->imm);
  //printf("0x%08x\n", *ddest);
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

def_EHelper(xori){
  rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(ori){
  rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi){
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slli){
  rtl_slli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli){
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai){
  rtl_srai(s, ddest, dsrc1, id_src2->imm);
}
