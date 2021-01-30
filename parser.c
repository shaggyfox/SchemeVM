#include "lexer.h"
#include "memory.h"
#include <stdlib.h>

#define TYPE_CONS 1
#define TYPE_SYMBOL 2

struct cons_s {
  struct memcell_s cell;
  struct memcell_s *car;
  struct memcell_s *cdr;
};

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

struct stack_s {
  struct stack_s *next;
  struct memcell_s *v;
} *stack = NULL;

struct cons_s *cons(void* car, void *cdr) {
  struct cons_s *ret = memcell_alloc(TYPE_CONS, sizeof(*ret));
  ret->car = car;
  ret->cdr = cdr;
  return ret;
}
struct number_s *number(char *data) {
  struct number_s *ret = memcell_alloc(T_NUMBER, sizeof(*ret));
  ret->number = strtol(data, NULL, 10);
  return ret;
}


void push(struct memcell_s *v) {
  struct stack_s *n = malloc(sizeof(*n));
  n->v = v;
  n->next = stack;
  stack = n;
}

struct memcell_s *pop(void) {
  struct memcell_s *ret = stack ? stack->v : NULL;
  if (stack) {
    void *fp = stack;
    stack = stack->next;
    free(fp);
  }
  return ret;
}

struct memcell_s *parser(int in_fd)
{
  int open_brackets = 0;
  struct cons_s *ret = NULL;
  struct cons_s **pos = &ret;
  char token_value[1024];
  do {
    int token = read_token(in_fd, token_value, sizeof(token_value));
    if (open_brackets) {
      pos = (void*)&(*pos)->cdr;
    }
    switch (token) {
      case T_SPECIAL:
        switch (*token_value) {
          case '(':
            push((void*)pos);
            pos = (void*)&(*pos)->car;
            ++ open_brackets;
            break;
          case ')':
            if (stack) {
              pos = (void*)pop();
            }
            -- open_brackets;
            break;
          case '\'':
            break;
          case ',':
            break;
        }
        break;
      case T_NUMBER:
        *pos = (void*)cons((void*)number(token_value), NULL);
        break;
      case T_STRING:
        break;
      case T_SYMBOL:
        break;
    }
  } while (open_brackets);
}

int main()
{
  memcell_init(1024);
  parser(0);
}
