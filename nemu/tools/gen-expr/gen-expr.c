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
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static uint32_t choose(uint32_t x){
  return rand() % x;
}

static int pos_buf = 0;

static void gen_num(){
  char str[12];
  sprintf(str, "%d", rand());
  int len = strlen(str);
  for(int i = 0; i < len; i++){
    buf[pos_buf++] = str[i];
    if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
  }
}

static void gen(char s){
  buf[pos_buf++] = s;
  if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
}

static int op_list[4] = {'+', '-', '*', '/'};

static void gen_space(){
  int n = choose(4);
  for(int i = 0; i < n; i++){
    buf[pos_buf++] = ' ';
    if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
  }
}

static void gen_op(){
  buf[pos_buf++] = op_list[choose(4)];
  if (pos_buf == BUFF_SIZE - 1){pos_buf = -1; return;}
}

static void gen_rand_expr() {
  gen_space();
  switch (pseudo_random()){
    case 0 : gen_num(); break;
    case 1 : gen('('); gen_rand_expr(); gen(')'); break;
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

    int ret = system("gcc -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}