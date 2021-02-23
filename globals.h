#ifndef GLOBALS_H
#define GLOBALS_H
#include "memory.h"

struct cons_s *symbol_list;
struct cons_s *parser_stack;
struct cons_s *source;

void init_globals(void);
void cleanup_globals(void);
struct memcell_pool_s *memory_pool;

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

#define CONS(in_car, in_cdr) CONS_3p(in_car, in_cdr, memory_pool)
#endif

