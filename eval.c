#include "memory.h"
#include "parser.h"
#include "globals.h"
#include <string.h>


struct cons_s {
  struct memcell_s memcell;
  char data[100];
};

struct memcell_s *eval(struct memcell_s *in){
  if (!in) {
    return NULL;
  }
  switch(in->type) {
    case TYPE_NUMBER:
      return in;
  }
  return NULL;
}

int main()
{
  init_globals();
  struct memcell_s *line = parser(0);
  //memcell_print(eval(line));
  memcell_print(line);
  cleanup_globals();
}
