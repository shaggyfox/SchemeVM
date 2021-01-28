#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

#define ALIGN_TO(len, type) \
    (len % sizeof(type) ? len + sizeof(type) - len % sizeof(type) : len)

static struct memcell_s *glob_memory = NULL;
static struct memcell_s *glob_memory_last = NULL;
static uint32_t glob_memory_size = 0;

void memcell_init(uint32_t size)
{
  size = ALIGN_TO(size, sizeof(struct memcell_s));
  glob_memory=calloc(1, size);
  glob_memory[0].next = size / sizeof(struct memcell_s) - 1;
  glob_memory[glob_memory[0].next].in_use = 255; /* end of memory */
  glob_memory_last = glob_memory;
}

void memcell_cleanup(void)
{
  free(glob_memory);
  glob_memory = NULL;
  glob_memory_last = NULL;
}

void *memcell_alloc(int type, uint32_t len)
{
  uint8_t next = ALIGN_TO(len, sizeof(struct memcell_s)) / sizeof(struct memcell_s);
  struct memcell_s *ret = NULL;

  for (int cnt = 0; cnt < 2; ++cnt) {
    if (glob_memory_last->in_use == 255) {
      /* rewind */
      glob_memory_last = glob_memory;
      continue;
    }
    if (!glob_memory_last->in_use && glob_memory_last->next >= next) {
      ret = glob_memory_last;
      break;
    }
    glob_memory_last += glob_memory_last->next;
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
