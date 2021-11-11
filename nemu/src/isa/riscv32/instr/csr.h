def_EHelper(csrrw) {
    rtl_mv(s, ddest, dsrc2);
    rtl_mv(s, dsrc2, dsrc1);
}

def_EHelper(csrrwi) {
    rtl_mv(s, ddest, dsrc2);
    rtl_li(s, dsrc2, id_src1->imm);
}

def_EHelper(csrrs) {
    rtl_mv(s, ddest, dsrc2);
    rtl_or(s, dsrc2, dsrc2, dsrc1);
}

def_EHelper(csrrsi) {
    rtl_mv(s, ddest, dsrc2);
    rtl_ori(s, dsrc2, dsrc2, id_src1->imm);
}

def_EHelper(csrrc) {
    rtl_mv(s, ddest, dsrc2);
    rtl_neg(s, s0, dsrc1);
    rtl_and(s, dsrc2, dsrc2, s0);
}

def_EHelper(csrrci) {
    rtl_mv(s, ddest, dsrc2);
    rtl_andi(s, dsrc2, dsrc2, ~id_src1->imm);
}