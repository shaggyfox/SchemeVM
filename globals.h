#ifndef GLOBALS_H
#define GLOBALS_H
#include "memory.h"
void init_globals(void);
void cleanup_globals(void);
struct memcell_pool_s *static_pool;
struct memcell_pool_s *dynamic_pool;

struct number_s {
  struct memcell_s cell;
  int64_t number;
};

struct string_s {
  struct memcell_s cell;
  char string[];
};

struct symbol_s {
  struct memcell_s cell;
  char symbol[];
};

#define CONS(in_car, in_cdr) CONS_3p(in_car, in_cdr, dynamic_pool)
#endif

