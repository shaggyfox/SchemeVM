#include <memory.h>

struct number_s *number(int nr);
struct symbol_s *mk_symbol(char *data);
struct memcell_s *mk_buildin(int cmd);

struct memcell_s *parser(int in_fd);
void memcell_print(struct memcell_s *cell);

