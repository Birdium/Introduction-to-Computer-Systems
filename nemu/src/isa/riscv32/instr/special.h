def_EHelper(inv) {
  rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0);
}

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0); // gpr(10) is $a0
}

def_EHelper(ecall) {
  rtl_mv(s, mepc, &cpu.pc);
  rtl_li(s, mcause, 16);
  //printf("%x\n", *mtvec);
  rtl_jr(s, mtvec);
}