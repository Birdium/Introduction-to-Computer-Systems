//test
//#include<unistd.h>

def_EHelper(beq){
  if (*dsrc1 == *dsrc2)
    rtl_j(s, s->pc + id_dest->imm);
}

def_EHelper(bne){
  if (*dsrc1 != *dsrc2)
    rtl_j(s, s->pc + id_dest->imm);
  //printf("0x%08x != 0x%08x\n", id_src1->imm, id_src2->imm);  
  //sleep(2);
}

def_EHelper(blt){
  if ((sword_t)*dsrc1 < (sword_t)*dsrc2)
    rtl_j(s, s->pc + id_dest->imm);
}

def_EHelper(bge){
  if ((sword_t)*dsrc1 >= (sword_t)*dsrc2)
    rtl_j(s, s->pc + id_dest->imm);
}

def_EHelper(bltu){
  if (*dsrc1 < *dsrc2)
    rtl_j(s, s->pc + id_dest->imm);
}

def_EHelper(bgeu){
  if (*dsrc1 >= *dsrc2)
    rtl_j(s, s->pc + id_dest->imm);
}