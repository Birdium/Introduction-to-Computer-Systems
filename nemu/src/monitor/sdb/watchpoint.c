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
    if (wp->next) wp->next->prev = wp->prev;
  } else{
    if (wp->next) wp->next->prev = NULL;
    head = wp->next;
  }
  wp->prev = NULL;
  wp->next = free_;
  if (free_) free_->prev = wp;
  free_ = wp;
}

void wp_display(){
  WP* p = head;
  while(p){
    printf("NO.%3d : %10u : %s\n", p->NO, p->val, p->expr);
    p = p->next;
  }
}

WP* find_wp(int N){
  WP* p = head;
  while(p){
    if (p->NO == N) return p;
    p=p->next;
  }
  return NULL;
}

WP* wp_head(){
  return head;
}

bool wp_check(){
  bool ret_value = 1;
  WP* p = wp_head();
  while(p){
    bool suc = 1;
    uint32_t expr_val = expr(p->expr, &suc);
    if (!suc) assert(0);
    if (expr_val != p->val){
      printf("Triggered watchpoint NO.%d, %u -> %u\n", p->NO, p->val, expr_val);
      p->val = expr_val;
      ret_value = 0;
    }
    p = p->next;
  }
  return ret_value;
}
/* TODO: Implement the functionality of watchpoint */

