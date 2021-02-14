#include <memory.h>

struct number_s *number(int nr);

struct number_s {
  struct memcell_s cell;
  int64_t number;
};

struct string_s {
  struct memcell_s cell;
  char string[];
};

struct symbol_s {
  struct memcell_s cell;
  char symbol[];
};

struct memcell_s *parser(int in_fd);
void memcell_print(struct memcell_s *cell);

void cleanup_symbols(void);
