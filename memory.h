#ifndef MEMORY_H
#define MEMORY_H
#include <sys/types.h>
#include <inttypes.h>

struct memcell_s {
  uint32_t next;     /* length of data */
  uint8_t type;      /* type of data */
  uint8_t in_use;    /* carbage collection 255 == end of memory*/
  uint8_t unused;    /* cells till next element */
  char data[];       /* cells data */
};

void memcell_init(uint32_t size);
void memcell_cleanup(void);
void *memcell_alloc(int type, uint32_t len);
void memcell_free(void *in);
void memcell_unfree(void *in);
#endif