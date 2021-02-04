#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

#define ALIGN_TO(len, type) \
    (len % sizeof(type) ? len + sizeof(type) - len % sizeof(type) : len)

struct memcell_pool_s *memcell_init(uint32_t size)
{
  struct memcell_pool_s *ret = malloc(sizeof(*ret));
  size = ALIGN_TO(size, sizeof(struct memcell_s));
  ret->pool=calloc(1, size);
  ret->pool[0].next = size / sizeof(struct memcell_s) - 1;
  ret->pool[ret->pool[0].next].in_use = 255; /* end of memory */
  ret->last = ret->pool;
  return ret;
}

void memcell_cleanup(struct memcell_pool_s *pool)
{
  free(pool->pool);
  free(pool);
}

void *memcell_alloc(int type, uint32_t len, struct memcell_pool_s *pool)
{
  uint8_t next = ALIGN_TO(len, sizeof(struct memcell_s)) / sizeof(struct memcell_s);
  struct memcell_s *ret = NULL;

  for (int cnt = 0; cnt < 2; ++cnt) {
    if (pool->last->in_use == 255) {
      /* rewind */
      pool->last = pool->pool;
      continue;
    }
    if (!pool->last->in_use && pool->last->next >= next) {
      ret = pool->last;
      break;
    }
    pool->last += pool->last->next;
  }

  if (ret) {
    if (ret->next > next) {
      /* split */
      struct memcell_s *beyond = ret + next;
      beyond->next = ret->next - next;
      beyond->in_use = 0;
    }
    ret->type = type;
    ret->next = next;
    ret->in_use = 1;
  }
  return ret;
}

void memcell_free(void *in)
{
  struct memcell_s *cell = in;
  cell->in_use = 0;
}

void memcell_unfree(void *in)
{
  struct memcell_s *cell = in;
  cell->in_use = 1;
}
