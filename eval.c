#include "memory.h"
#include "parser.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>

struct cons_s *environment = NULL;

void *SET_ENV(struct symbol_s *sym, void *value)
{
  if (MEMCELL_TYPE(sym) != TYPE_SYMBOL) {
    printf("error: define first parameter must be symbol\n");
    return NULL;
  }
  environment = CONS(CONS(sym, value), environment);
  return value;
}

void *GET_ENV(void *sym)
{
  void *ret = NULL;
  for(struct cons_s *e = environment; e; e = (void*)CDR(e)) {
    if (sym == (void*)CAR(CAR(e))) {
      ret = CDR(CAR(e));
      break;
    }
  }
  return ret;
}
#define CMD_EVAL 10
#define CMD_APPLY 20
#define CMD_PLUS  30
#define CMD_MINUS 40

#define NEW_STATE(fn, args, value, next) \
  CONS(CONS(number(fn), CONS(args, CONS(value, NULL))), next)

#define JUMP(fn) \
  GET_NUMBER(CAR(CAR(vm_state))) = fn; \
  break;

#define ARGS() \
  CAR(CDR(CAR(vm_state)))

#define VAL() \
  CAR(CDR(CDR(CAR(vm_state))))

#define CALL(return_fn, call_fn, args) \
  GET_NUMBER(CAR(CAR(vm_state))) = return_fn; \
  vm_state = NEW_STATE(call_fn, args, NULL, vm_state); \
  break;

#define GET_NUMBER(v) ((struct number_s*)v)->number
#define RET() vm_state = (void*)CDR(vm_state);break;

struct cons_s *vm_state = NULL;

void my_reclaim_memory(void)
{
  memcell_unfree_r(vm_state);
  memcell_unfree_r(environment);
  memcell_unfree_r(symbol_list);
  memcell_unfree_r(parser_stack);
  memcell_unfree_r(source);
}

struct memcell_s *eval(struct memcell_s *input){
  void *ret = NULL;
  vm_state = NEW_STATE(CMD_EVAL, input, NULL, NULL);
  while(vm_state) {
    switch (GET_NUMBER(CAR(CAR(vm_state)))) {
      case CMD_EVAL:
        if (ARGS() && ARGS()->type == TYPE_CONS) {
          ARGS() = (void*)CONS(CAR(ARGS()), CONS(CDR(ARGS()), NULL));
          JUMP(CMD_APPLY);
        } else if (ARGS() && ARGS()->type == TYPE_SYMBOL) {
          ret = GET_ENV(ARGS());
        } else if (ARGS() && ARGS()->type == TYPE_NUMBER) {
          ret = ARGS();
        } else {
          ret = NULL;
        }
        RET();
      case CMD_APPLY:
        /* eval cmd */
        CALL(CMD_APPLY + 1, CMD_EVAL, CAR(ARGS()));
      case CMD_APPLY + 1:
        if (ret && MEMCELL_TYPE(ret) == TYPE_BUILDIN) {
          ARGS() = CAR(CDR(ARGS()));
          JUMP(((struct memcell_s*)ret)->cmd);
        }
        RET();
      case CMD_PLUS:
        VAL() = number(0);
      case CMD_PLUS + 1:
        if (ARGS() && ARGS()->type == TYPE_CONS) {
          CALL(CMD_PLUS + 2, CMD_EVAL, CAR(ARGS()));
          case CMD_PLUS + 2:
          if (ret && MEMCELL_TYPE(ret) == TYPE_NUMBER) {
            GET_NUMBER(VAL()) += GET_NUMBER(ret);
          }
          ARGS() = CDR(ARGS());
          JUMP(CMD_PLUS + 1);
        }
        ret = VAL();
        RET();
      default:
        printf("error state");
        return NULL;
    }
  }
  return ret;
}

int main()
{
  init_globals();
  SET_ENV(mk_symbol("A"), number(3));
  SET_ENV(mk_symbol("+"), mk_buildin(CMD_PLUS));
  memcell_set_gc(memory_pool, &my_reclaim_memory);
  struct memcell_s *line = parser(0);
  memcell_print(eval(line));
  cleanup_globals();
}
