#ifndef GLOBALS_H
#define GLOBALS_H
#include "memory.h"
void init_globals(void);
void cleanup_globals(void);
struct memcell_pool_s *static_pool;
struct memcell_pool_s *dynamic_pool;
#define CONS(in_car, in_cdr) CONS_3p(in_car, in_cdr, dynamic_pool)
#endif

