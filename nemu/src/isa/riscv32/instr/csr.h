def_EHelper(csrrw) {
    Log("mscratch before: %x", *mscratch);
    Log("Opr before: %x, %x, %x", *ddest, *dsrc1, *dsrc2);
    rtl_mv(s, s0, dsrc1);
    rtl_mv(s, ddest, dsrc2);
    rtl_mv(s, dsrc2, s0);
    Log("mscratch after: %x", *mscratch);
    Log("Opr after: %x, %x, %x", *ddest, *dsrc1, *dsrc2);
}

def_EHelper(csrrwi) {
    Log("mscratch before: %x", *mscratch);
    Log("Opr before: %x, %x, %x", *ddest, id_src1->imm, *dsrc2);
    rtl_mv(s, ddest, dsrc2);
    rtl_li(s, dsrc2, id_src1->imm);
    Log("mscratch after: %x", *mscratch);
    Log("Opr after: %x, %x, %x", *ddest, id_src1->imm, *dsrc2);
}

def_EHelper(csrrs) {
    Log("mscratch before: %x", *mscratch);
    Log("Opr before: %x, %x, %x", *ddest, id_src1->imm, *dsrc2);
    rtl_mv(s, s0, dsrc1);
    rtl_mv(s, ddest, dsrc2);
    rtl_or(s, dsrc2, dsrc2, s0);
    Log("mscratch after: %x", *mscratch);
    Log("Opr after: %x, %x, %x", *ddest, id_src1->imm, *dsrc2);
}

def_EHelper(csrrsi) {
    rtl_mv(s, ddest, dsrc2);
    rtl_ori(s, dsrc2, dsrc2, id_src1->imm);
}

def_EHelper(csrrc) {
    Log("mscratch before: %x", *mscratch);
    Log("Opr before: %x, %x, %x", *ddest, id_src1->imm, *dsrc2);
    rtl_neg(s, s0, dsrc1);
    rtl_mv(s, ddest, dsrc2);
    rtl_and(s, dsrc2, dsrc2, s0);
    Log("mscratch after: %x", *mscratch);
    Log("Opr after: %x, %x, %x", *ddest, id_src1->imm, *dsrc2);
}

def_EHelper(csrrci) {
    rtl_mv(s, ddest, dsrc2);
    rtl_andi(s, dsrc2, dsrc2, ~id_src1->imm);
}

def_EHelper(ecall) {
    isa_raise_intr(s, 11, cpu.pc);
}

#define MIE (1 << 3)
#define MPIE (1 << 7)

def_EHelper(mret) {
    // Log("mret before: %x", *mstatus);
    *mstatus = ((*mstatus | MPIE) & (~MIE)) | ((*mstatus & MPIE) >> 4);
    // Log("mret after:  %x", *mstatus);
    // Log("%x", cpu.gpr[10]._32);
    // Log("%x", *mepc);
    rtl_jr(s, mepc); // + 4 or not
}