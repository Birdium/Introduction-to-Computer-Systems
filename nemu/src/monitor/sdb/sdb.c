#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/vaddr.h>
#include <memory/paddr.h>
#include <memory/host.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_d(char *args){
  if (args == NULL){
		printf("Please type in syntax.\n");
		return 1;
  }
	bool success = 1;
	uint32_t N = expr(args, &success);
	if (!success) {
		printf("Invalid expression.\n");
		return 1;
	}
  WP *p = find_wp(N);
  if (p){
    free_wp(p);
    printf("Deleted watchpoint NO.%d\n", N);
  } else{
    printf("Unavailable watchpoint NO.%d.\n", N);
    return 1;
  }
  return 0;
}

static int cmd_w(char *args){
  if (args == NULL){
		printf("Please type in syntax.\n");
		return 1;
  }
	bool success = 1;
	uint32_t N = expr(args, &success);
	if (!success) {
		printf("Invalid expression.\n");
		return 1;
	}
	WP *p = new_wp();
  p->expr = args;
  p->val = N;
  printf("Set watchpoint NO. %d, %s.\n", p->NO, p->expr);
	return 0;
}

static int cmd_p(char *args){
	if (args == NULL){
		printf("Please type in syntax.\n");
		return 1;
	}
	bool success = 1;
	uint32_t N = expr(args, &success);
	if (!success) {
		printf("Invalid expression.\n");
		return 1;
	}
	printf("0x%x %u\n", N, N);
	return 0;
}
// scan memory
static int cmd_x(char *args){
	if (args == NULL){
		printf("Please type in syntax.\n");
		return 1;
	}
	char *arg = strtok(args, " ");
	char *endptr;
	uint32_t N = strtoul(args, &endptr, 10);
	if (endptr < arg + strlen(arg)){
		printf("Invalid number.\n");
		return 1;
	}

	args = args + strlen(arg) + 1;
  	bool success = 1;
  	uint32_t addr = expr(args, &success);
  	if (!success) {
  	  printf("Invalid expression.\n");
  	  return 1;
  	}  
	for(uint32_t i = 0; i < N; i++){
		if (i > 0 && addr + i == 0) break;
		if (i > 0) printf(" ");
		printf("%02x", vaddr_read(addr + i, 1));
	}	
	printf("\n");
	return 0 ;
}

// print info
static int cmd_info(char *args){
	if (args == NULL || strlen(args) > 1) {
		printf("Invalid syntax.\n");
		return 1;
	}
	switch (args[0]){
		case 'r' : isa_reg_display(); break;
		default : {
			printf("Invalid syntax.\n");
			return 1;
		}		
	}
	return 0;
}
	
// Step into
static int cmd_si(char *args) {
	if (args == NULL){
  		cpu_exec(1);
  	} 
  	else {
  	  	char *arg = strtok(args, " ");
		char *arg_end = arg + strlen(arg);
		char *endptr = NULL;
		uint64_t n = strtoull(arg, &endptr, 10);
	  	if (endptr < arg_end){
			printf("Invalid number \"%s\".\n", arg);
  			return 1;
		}
		cpu_exec(n);

  	}
  	return 0;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step into the execution of the program", cmd_si},
  { "info", "Print the status of register of watched point", cmd_info},
  { "x", "Scanning the memory", cmd_x},
  { "p", "Calculating expression", cmd_p}, 
  { "w", "Set new watchpoints", cmd_w},
  { "d", "Delete watchpoints", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
