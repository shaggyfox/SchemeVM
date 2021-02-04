#ifndef MEMORY_H
#define MEMORY_H
#include <sys/types.h>
#include <inttypes.h>

struct memcell_s {
  uint32_t next;     /* length of data */
  uint8_t type;      /* type of data */
  uint8_t in_use;    /* carbage collection 255 == end of memory*/
  uint8_t cmd;       /* command or command cells */
  char data[];       /* cells data */
};

struct memcell_pool_s {
  struct memcell_s *pool;
  struct memcell_s *last;
};

#define MEMCELL_TYPE(c) ((struct memcell_s*)c)->type

struct memcell_pool_s *memcell_init(uint32_t size);
void memcell_cleanup(struct memcell_pool_s* pool);
void *memcell_alloc(int type, uint32_t len, struct memcell_pool_s *pool);
void memcell_free(void *in);
void memcell_unfree(void *in);
#endif
