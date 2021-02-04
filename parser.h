#include <memory.h>
#define TYPE_CONS 1
#define TYPE_SYMBOL 2
#define TYPE_NUMBER 3

struct memcell_s *parser(int in_fd);
void memcell_print(struct memcell_s *cell);

#define CAR(v) (((struct cons_s*)v)->car)
#define CDR(v) (((struct cons_s*)v)->cdr)
struct cons_s *CONS(void* car, void *cdr);

void cleanup_symbols(void);
