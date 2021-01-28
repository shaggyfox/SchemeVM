#include "memory.h"
#include <string.h>


struct cons_s {
  struct memcell_s memcell;
  char data[100];
};

int main()
{
  memcell_init(1024);
  struct cons_s *cons;
  while ((cons = memcell_alloc(1, sizeof(struct cons_s)))) {
    memcell_free(cons);
  }
  memcell_cleanup();
}
