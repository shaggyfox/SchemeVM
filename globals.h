#ifndef GLOBALS_H
#define GLOBALS_H
#include "memory.h"
void init_globals(void);
void cleanup_globals(void);
struct memcell_pool_s *static_pool;
struct memcell_pool_s *dynamic_pool;
#endif

