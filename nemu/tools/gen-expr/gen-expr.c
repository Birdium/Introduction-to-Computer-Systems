#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
#define BUFF_SIZE 65536
static char buf[BUFF_SIZE] = {};
static char code_buf[BUFF_SIZE + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = (unsigned)%s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
// (unsigned)..


static uint32_t choose(uint32_t x){
  return rand() % x;
}

static int pos_buf = 0;

static void gen_space(){
  int n = choose(4);
  for(int i = 0; i < n; i++){
    buf[pos_buf++] = ' ';
    if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
  }
}

static void gen_num(){
  gen_space();
  char str[12];
  sprintf(str, "%d", rand());
  int len = strlen(str);
  for(int i = 0; i < len; i++){
    buf[pos_buf++] = str[i];
    if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
  }
  buf[pos_buf++] = 'U';
  if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
}

static void gen(char* str){
  gen_space();
  int len = strlen(str);
  for(int i = 0; i < len; i++){
    buf[pos_buf++] = str[i];
    if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
  }
}

static char* op_list[] = {"+", "-", "*", "/", "==", "!=", "&&"};
#define OP_NUM 7

static void gen_op(){
  gen_space();
  char * str = op_list[choose(OP_NUM)];
  for(int i = 0; i < strlen(str); i++){
    buf[pos_buf++] = str[i];
    if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
  }
}

static void gen_rand_expr() {
  gen_space();
  switch (choose(3)){
    case 0 : gen_num(); break;
    case 1 : gen("("); gen_rand_expr(); gen(")"); break;
    default: gen_rand_expr(); gen_op(); gen_rand_expr(); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    pos_buf = -1;
    while(pos_buf==-1) {
      pos_buf = 0;
      gen_rand_expr();
    }
    buf[pos_buf++] = '\0';

    sprintf(code_buf, code_format, buf);
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Werror=div-by-zero -Wno-overflow /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) {i--;continue;}

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}