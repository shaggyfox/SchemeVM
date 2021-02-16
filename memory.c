#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

#define ALIGN_TO(len, type) \
    (len % sizeof(type) ? len + sizeof(type) - len % sizeof(type) : len)

struct cons_s *CONS_3p(void* car, void *cdr, struct memcell_pool_s *pool) {
  struct cons_s *ret = memcell_alloc_p2(TYPE_CONS, sizeof(*ret), pool, car, cdr);
  ret->car = car;
  ret->cdr = cdr;
  return ret;
}

struct memcell_pool_s *memcell_init(uint32_t size)
{
  struct memcell_pool_s *ret = calloc(1, sizeof(*ret));
  size = ALIGN_TO(size, struct memcell_s);
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

void memcell_garbage_collect(struct memcell_pool_s *pool, void *param_a, void *param_b)
{
  if (!pool->reclaim_memory) {
    /* no reclaim no gc ;) */
    return;
  }
  struct memcell_s *cell = pool->pool;
  while (cell->in_use != 255) {
    cell->in_use = 0;
    cell = &cell[cell->next];
  }
  memcell_unfree_r(param_a);
  memcell_unfree_r(param_b);
  (*pool->reclaim_memory)();
}

#include <assert.h>
void *memcell_alloc_p2(int type, uint32_t len, struct memcell_pool_s *pool, void *a, void *b)
{
  uint8_t next = ALIGN_TO(len, struct memcell_s) / sizeof(struct memcell_s);
  struct memcell_s *ret = NULL;

  for (int cnt = 0; cnt < 2; ++cnt) {
    while (!ret) {
      if (pool->last->in_use == 255) {
        /* rewind */
        pool->last = pool->pool;
        printf("rewind\n");
        memcell_garbage_collect(pool, a, b);
        break;
      }
      if (!pool->last->in_use && pool->last->next >= next) {
        ret = pool->last;
        break;
      }
      pool->last += pool->last->next;
    }
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
  assert(ret);
  if (!ret) {
    printf("out of memory");
    exit(1);
  }
  return ret;
}
void *memcell_alloc(int type, uint32_t len, struct memcell_pool_s *pool)
{
  return memcell_alloc_p2(type, len, pool, NULL, NULL);
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

void memcell_unfree_r(void *in)
{
  if (!in) {
    /* */
  } else {
    struct memcell_s *cell = in;
    if (cell->type == TYPE_CONS) {
      struct cons_s *cons = (void*)cell;
      memcell_unfree_r(cons->car);
      memcell_unfree_r(cons->cdr);
    }
    memcell_unfree(in);
  }
}

void memcell_set_gc(struct memcell_pool_s *pool, void (*gc)(void))
{
  pool->reclaim_memory = gc;
}

