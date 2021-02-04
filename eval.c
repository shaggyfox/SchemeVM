#include "memory.h"
#include "parser.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>

struct cons_s *environment = NULL;

void *DEFINE(void *sym, void *value)
{
  if (MEMCELL_TYPE(sym) != TYPE_SYMBOL) {
    printf("error: define first parameter must be symbol\n");
    return NULL;
  }
  environment = CONS(sym, value);
  return value;
}
#define CMD_RESOLV 10
#define CMD_APPLY 20
#define CMD_PLUS  30
#define CMD_MINUS 40

#define PUSH \
  stack = CONS(CONS(args,CONS(value, NULL)),stack)
#define POP \
  args = CAR(CAR(stack)); \
  value =CAR(CDR(CAR(stack)));
//struct cons_s *vm_stack = NULL;
struct memcell_s *eval(struct memcell_s *input){
  struct cons_s *stack = NULL;
  int c = CMD_RESOLV;
  void *ret = NULL;
  struct memcell_s *args = input; /* put on stack later */
  struct memcell_s *value = NULL; /* put on stack later */
  do {
    switch(c) {
      case CMD_RESOLV:
        if(MEMCELL_TYPE(args) == TYPE_NUMBER
           /*|| MEMCELL_TYPE(args) == TYPE_STRING*/) {
          /* numbers resolve to them selfes */
          ret = args;
        } else if(MEMCELL_TYPE(args) == TYPE_CONS) {
          /* cons are executed ... */
        }
        break;
      case CMD_APPLY:

        break;
    }
  }while (stack);
  return ret;
}

int main()
{
  init_globals();
  struct memcell_s *line = parser(0);
  memcell_print(eval(line));
  cleanup_symbols();
  cleanup_globals();
}
