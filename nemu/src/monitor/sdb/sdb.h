#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  struct watchpoint *prev;
  char * expr;
  uint32_t val;
  /* TODO: Add more members if necessary */

} WP;

WP *new_wp();
void free_wp(WP*);
WP* find_wp(int);

WP* wp_head();

void wp_display();

#endif
