#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
void check_expr();

#define EXPR_TEST



int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef EXPR_TEST
  check_expr();
#else

#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

#endif

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
