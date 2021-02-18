#include "lexer.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include <string.h>
#include <search.h>
#include "globals.h"

/* XXX static pool */
static struct number_s *parse_number(char *data) {
  struct number_s *ret = memcell_alloc(TYPE_NUMBER, sizeof(*ret), static_pool);
  ret->number = strtol(data, NULL, 10);
  return ret;
}

/* XXX dynamic pool */
struct number_s *number(int nr) {
  struct number_s *ret = memcell_alloc(TYPE_NUMBER, sizeof(*ret), dynamic_pool);
  ret->number = nr;
  return ret;
}

void *symbol_tree = NULL;
static int sym_cmp(const void *a, const void *b) {
  const struct symbol_s *sym_a = a;
  const struct symbol_s *sym_b = b;
  return strcmp(sym_a->symbol, sym_b->symbol);
}

void cleanup_symbols(void) {
  while (symbol_tree) {
    void *to_del = *(void**)symbol_tree;
    tdelete(to_del, &symbol_tree, sym_cmp);
    memcell_free(to_del);
  }
}

/* XXX static pool */
static struct symbol_s *symbol(char *data) {
  struct symbol_s *ret = NULL;
  struct symbol_s *new_sym = memcell_alloc(TYPE_SYMBOL, sizeof(*new_sym) + strlen(data) + 1, static_pool);
  memcpy(new_sym->symbol, data, strlen(data) + 1);
  ret = *(void**)tsearch(new_sym, &symbol_tree, sym_cmp);
  if (ret != new_sym) {
    memcell_free(new_sym);
  }
  return ret;
}

struct cons_s *parser_stack = NULL;

void push(struct memcell_s *v) {
  parser_stack = CONS(v, parser_stack);
}

struct memcell_s *pop(void) {
  struct memcell_s *ret = CAR(parser_stack);
  parser_stack = (void*)CDR(parser_stack);
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
            *pos = CONS_3p(NULL, NULL, static_pool);
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
        *pos = (void*)CONS_3p((void*)parse_number(token_value), NULL, static_pool);
        pos = (void*)&(*pos)->cdr;
        break;
      case T_STRING:
        break;
      case T_SYMBOL:
        *pos = (void*)CONS_3p((void*)symbol(token_value), NULL, static_pool);
        pos = (void*)&(*pos)->cdr;
        break;
    }
  } while (parser_stack);
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
