#include "globals.h"
struct memcell_pool_s *memory_pool = (void*)0;

void init_globals(void) {
  memory_pool = memcell_init(1024);
}

void cleanup_globals(void) {
  memcell_cleanup(memory_pool);
}
