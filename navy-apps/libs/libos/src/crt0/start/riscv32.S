.globl  _start
_start:
  move s0, zero
  mv a0, sp
  jal call_main
