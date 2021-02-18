#include <memory.h>

struct number_s *number(int nr);

struct memcell_s *parser(int in_fd);
void memcell_print(struct memcell_s *cell);

void cleanup_symbols(void);
