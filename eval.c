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

#define NEW_STATE(fn, args, value, next) \
  CONS(CONS(number(fn), CONS(args, CONS(value, NULL))), next)

#define JUMP(fn) \
  GET_NUMBER(CAR(CAR(vm_state))) = fn;

#define ARGS() \
  CAR(CDR(CAR(vm_state)))

#define VAL() \
  CAR(CDR(CDR(CAR(vm_state))))

#define CALL(return_fn, call_fn, args) \
  GET_NUMBER(CAR(CAR(vm_state))) = return_fn; \
  vm_state = NEW_STATE(call_fn, args, NULL, vm_state);

#define GET_NUMBER(v) ((struct number_s*)v)->number
#define RET() vm_state = (void*)CDR(vm_state);

struct cons_s *vm_state = NULL;

void my_reclaim_memory(void)
{
  memcell_unfree_r(vm_state);
  memcell_unfree_r(environment);
}

struct memcell_s *eval(struct memcell_s *input){
  void *ret = NULL;
  int i = 0;
  vm_state = NEW_STATE(CMD_PLUS, input, NULL, NULL);
  while(vm_state) {
    switch (GET_NUMBER(CAR(CAR(vm_state)))) {
      case CMD_RESOLV:
        if (ARGS() && ARGS()->type == TYPE_NUMBER) {
          ret = ARGS();
        } else {
          ret = number(0);
        }
        RET();
        break;
      case CMD_RESOLV + 1:
        break;
      case CMD_PLUS:
        VAL() = number(0);
      case CMD_PLUS + 1:
        if (ARGS() && ARGS()->type == TYPE_CONS) {
          if (CAR(ARGS()) && CAR(ARGS())->type != TYPE_NUMBER) {
          CALL(CMD_PLUS + 2, CMD_RESOLV, CAR(ARGS()));
          break;
          }else {
            ret = CAR(ARGS());
          }
          case CMD_PLUS + 2:
          GET_NUMBER(VAL()) += GET_NUMBER(ret);
          ARGS() = CDR(ARGS());
          JUMP(CMD_PLUS + 1);
          break;
        }
        ret = VAL();
        RET();
        break;
    }
  }
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
