#include "globals.h"
struct memcell_pool_s *static_pool = (void*)0;
struct memcell_pool_s *dynamic_pool = (void*)0;

void init_globals(void) {
  static_pool = memcell_init(1024);
  dynamic_pool = memcell_init(1024);
}

void cleanup_globals(void) {
  memcell_cleanup(static_pool);
  memcell_cleanup(dynamic_pool);
}
