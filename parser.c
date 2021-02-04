#include "lexer.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include <string.h>
#include <search.h>
#include "globals.h"

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
  struct cons_s *ret = memcell_alloc(TYPE_CONS, sizeof(*ret), dynamic_pool);
  ret->car = car;
  ret->cdr = cdr;
  return ret;
}
struct number_s *number(char *data) {
  struct number_s *ret = memcell_alloc(TYPE_NUMBER, sizeof(*ret), dynamic_pool);
  ret->number = strtol(data, NULL, 10);
  return ret;
}

void *symbol_tree = NULL;
static int sym_cmp(const void *a, const void *b) {
  const struct symbol_s *sym_a = a;
  const struct symbol_s *sym_b = b;
  return strcmp(sym_a->symbol, sym_b->symbol);
}

struct symbol_s *symbol(char *data) {
  struct symbol_s *ret = NULL;
  struct symbol_s *new_sym = memcell_alloc(TYPE_SYMBOL, sizeof(*new_sym) + strlen(data) + 1, static_pool);
  memcpy(new_sym->symbol, data, strlen(data) + 1);
  ret = *(void**)tsearch(new_sym, &symbol_tree, sym_cmp);
  if (ret != new_sym) {
    memcell_free(new_sym);
  }
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
  struct cons_s *ret = NULL;
  struct cons_s **pos = &ret;
  char token_value[1024];
  do {
    int token = read_token(in_fd, token_value, sizeof(token_value));
    switch (token) {
      case T_SPECIAL:
        switch (*token_value) {
          case '(':
            *pos = cons(NULL, NULL);
            push((void*)&(*pos)->cdr);
            pos = (void*)&(*pos)->car;
            *pos = NULL;
            break;
          case ')':
            pos = (void*)pop();
            break;
          case '\'':
            break;
          case ',':
            break;
        }
        break;
      case T_NUMBER:
        *pos = (void*)cons((void*)number(token_value), NULL);
        pos = (void*)&(*pos)->cdr;
        break;
      case T_STRING:
        break;
      case T_SYMBOL:
        *pos = (void*)cons((void*)symbol(token_value), NULL);
        pos = (void*)&(*pos)->cdr;
        break;
    }
  } while (stack);
  if (ret) {
    memcell_free(ret);
    return ret->car;
  }
  return NULL;
}

void memcell_print(struct memcell_s *cell) {
  struct cons_s *c;
  struct number_s *number;
  struct symbol_s *symbol;
  int in_list = 0;
  if (!cell) {
    printf("()");
    return;
  }
  switch(cell->type) {
    case TYPE_CONS:
      while (cell) {
        c = (void*)cell;
        if (in_list == 0) {
          printf("(");
          in_list = 1;
        }
        memcell_print(c->car);
        cell = c->cdr;
        if (!cell) {
          printf(")");
        }
     }
     break;
   case TYPE_NUMBER:
     number = (void*)cell;
     printf("%li ", number->number);
     break;
   case TYPE_SYMBOL:
     symbol = (void*)cell;
     printf("%s ", symbol->symbol);
     break;
  }
}

/*
int main()
{
  memcell_init(1024);
  print_cons(parser(0));
  memcell_cleanup();
}
*/
