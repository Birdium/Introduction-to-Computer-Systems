#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_DEC = 10,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},         // minus
  {"\\*", '*'},         // multiply
  {"\\/", '/'},         // divide
  {"[0-9]+", TK_DEC},   // decimal number
  {"\\(", '('},         // left bracket
  {"\\)", ')'},         // right bracket
  {"==", TK_EQ},        // equal
  
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

#define TOKEN_SIZE 32768

static Token tokens[TOKEN_SIZE] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

bool is_op(int pos){
  switch (tokens[pos].type){
    case '+' : case '-' : case '*' : case '/' : case TK_EQ :
      return 1;
      break;
    default : return 0;
  }
  return 0;
}

int find_op(int p, int q){
  int cnt = 0, pre_level = 0, pos = -1;
  for(int i = p; i < q; i++){
    switch (tokens[i].type){
    case '(' : 
      cnt++;
      break;
    case ')' :
      cnt--;
      break;
    case '+' : case '-' :
      if (cnt == 0 && pre_level <= 4 && i != p && !is_op(i-1)){
        pre_level = 4; pos = i;
        //Log("\"%c\" is a main operator in pos:%d.", tokens[i].type, i);
      }
      break;
    case '*' : case '/' :
      if (cnt == 0 && pre_level <= 3){
        pre_level = 3; pos = i;
      }
      break;
    default:
      break;
    }
  } 
  return pos;
}

int check_parentheses(int p, int q){
  // check invalid brackets
  int cnt = 0;
  for(int i = p ; i <= q; i++){
    if (tokens[i].type == '(') cnt++;
    else if (tokens[i].type == ')') cnt--;
    if (cnt < 0) return 2;
  }
  if (cnt != 0) return 2;

  // check if brackets are paired on p, q

  if (tokens[p].type != '(' || tokens[q].type != ')') return 1;
  for(int i = p + 1; i < q; i++){
    if (tokens[i].type == '(') cnt++;
    else if (tokens[i].type == ')') cnt--;
    if (cnt < 0) return 1;
  }
  return 0;
}

word_t eval(int p, int q, bool *success){
  if (p > q){
    *success = false;
    return 0;
  } else if(p == q){
    if (tokens[p].type == 10){
      word_t ans = strtoul(tokens[p].str, NULL, 10);
      return ans;
    } else{
      *success = false;
      return 0;
    }
  } 
  else 
    switch (check_parentheses(p, q)){
      case 0 : return eval(p+1, q-1, success);
      case 1 : ;
        int op = find_op(p, q);
        if (op == -1){
          if (tokens[p].type == '-') return -eval(p+1, q, success);
          else {
            *success = false; 
            return 0;
            break;
          }
        }
        Log("\"%c\" is a main operator in pos:%d.", tokens[op].type, op);
        word_t val1 = eval(p, op - 1, success);
        word_t val2 = eval(op + 1, q, success);
        if (!success) return 0;
        switch (tokens[op].type){
          case '+' : return val1 + val2;
          case '-' : return val1 - val2;
          case '*' : return val1 * val2;
          case '/' : 
            if (val2 == 0){
              printf("Floating point exception.\n");
              *success = false;
              return 0;
            }
            else return val1 / val2;
          // to be added...
          default:assert(0);
        }
        break; 
      case 2 :
        *success = false; 
        return 0;
        break;
    }
  return 0;
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (nr_token == TOKEN_SIZE){
          printf("too long expression.\n");
          return false;
        }
        switch (rules[i].token_type) {
          case TK_NOTYPE : break;
          case TK_DEC : 
            tokens[nr_token].type = rules[i].token_type;
            if (substr_len < 32) strncpy(tokens[nr_token].str, substr_start, substr_len);
            else {
              printf("too large int : %s\n", substr_start);
              return false;
            }
            nr_token++;
            break;
          default: 
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  word_t ans = eval(0, nr_token - 1, success);
  return ans;
}
