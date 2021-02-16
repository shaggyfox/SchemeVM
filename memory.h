#ifndef MEMORY_H
#define MEMORY_H
#include <sys/types.h>
#include <inttypes.h>

#define TYPE_CONS 1
#define TYPE_SYMBOL 2
#define TYPE_NUMBER 3
#define TYPE_BUILDIN 4

#define CAR(v) (((struct cons_s*)v)->car)
#define CDR(v) (((struct cons_s*)v)->cdr)


struct memcell_s {
  uint32_t next;     /* length of data */
  uint8_t type;      /* type of data */
  uint8_t in_use;    /* carbage collection 255 == end of memory*/
  uint8_t cmd;       /* command or command cells */
};

struct cons_s {
  struct memcell_s cell;
  struct memcell_s *car;
  struct memcell_s *cdr;
};

struct memcell_pool_s {
  struct memcell_s *pool;
  struct memcell_s *last;
  void (*reclaim_memory)(void);
};

struct cons_s *CONS_3p(void* car, void *cdr, struct memcell_pool_s *pool);

#define MEMCELL_TYPE(c) ((struct memcell_s*)c)->type

struct memcell_pool_s *memcell_init(uint32_t size);
void memcell_cleanup(struct memcell_pool_s* pool);
void *memcell_alloc_p2(int type, uint32_t len, struct memcell_pool_s *pool, void *a, void *b);
void *memcell_alloc(int type, uint32_t len, struct memcell_pool_s *pool);
void memcell_free(void *in);
void memcell_unfree(void *in);
void memcell_unfree_r(void *in);
void memcell_set_gc(struct memcell_pool_s *pool, void (*gc)(void));
#endif
