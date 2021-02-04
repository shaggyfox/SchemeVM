#include <memory.h>
#define TYPE_CONS 1
#define TYPE_SYMBOL 2
#define TYPE_NUMBER 3

struct memcell_s *parser(int in_fd);
void memcell_print(struct memcell_s *cell);

