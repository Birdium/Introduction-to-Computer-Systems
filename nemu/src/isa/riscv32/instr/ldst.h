def_EHelper(lb) {
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(lh) {
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(lw) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
}

def_EHelper(lbu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(lhu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(sb) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 1);
  printf("0x%02x\n", *ddest);
}

def_EHelper(sh) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 2);
  printf("0x%04x\n", *ddest);
}

def_EHelper(sw) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
  printf("0x%08x\n", *ddest);
}
