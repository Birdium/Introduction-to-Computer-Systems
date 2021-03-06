
def_EHelper(add){
  //printf("0x%08x + 0x%08x\n", *dsrc1, *dsrc2);
  rtl_add(s, ddest, dsrc1, dsrc2);
  //printf("= 0x%08x\n", *ddest);
}

def_EHelper(sub){
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll){
  rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt){
  rtl_slt(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu){
  rtl_sltu(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xor){
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl){
  rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra){
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or){
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and){
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul){
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh){
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulhsu){
  rtl_mulsu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulhu){
  rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div){
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu){
  rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem){
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu){
  rtl_divu_r(s, ddest, dsrc1, dsrc2);
}