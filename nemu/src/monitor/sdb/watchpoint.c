#include "sdb.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    if (i) wp_pool[i].prev = &wp_pool[i - 1];
  }
  wp_pool[0].prev = NULL;
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(){
  WP* p = free_;
  assert(p);
  if (p->next) p->next->prev = NULL;
  free_ = p->next;
  p->next = head;
  if (head) head->prev = p;
  head = p;
  return p;
}

void free_wp(WP* wp){
  if (wp->prev){
    wp->prev->next = wp->next;
    wp->next->prev = wp->prev;
  } else{
    wp->next->prev = NULL;
    head = wp->next;
  }
  wp->prev = NULL;
  wp->next = free_;
  if (free_) free_->prev = wp;
  free_ = wp;
}

WP* find_wp(int N){
  WP* p = head;
  while(p){
    if (p->NO == N) return p;
    p=p->next;
  }
  return NULL;
}

/* TODO: Implement the functionality of watchpoint */

