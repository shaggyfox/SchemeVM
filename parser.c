#include "lexer.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include <string.h>
#include <search.h>
#include "globals.h"

/* XXX GLOBALS */
struct cons_s *symbol_list = NULL;
struct cons_s *parser_stack = NULL;
struct cons_s *source = NULL;


static struct number_s *parse_number(char *data) {
  struct number_s *ret = memcell_alloc(TYPE_NUMBER, sizeof(*ret), memory_pool);
  ret->number = strtol(data, NULL, 10);
  return ret;
}

struct number_s *number(int nr) {
  struct number_s *ret = memcell_alloc(TYPE_NUMBER, sizeof(*ret), memory_pool);
  ret->number = nr;
  return ret;
}

struct symbol_s *mk_symbol(char *data) {
  struct symbol_s *ret = NULL;
  for (struct cons_s *c = symbol_list; c; c = (void*)CDR(c)) {
    struct symbol_s *e = (void*)CAR(c);
    if (!strcmp(e->symbol, data)) {
      ret = e;
      break;
    }
  }
  if (!ret) {
    ret = memcell_alloc(TYPE_SYMBOL, sizeof(*ret) + strlen(data) + 1, memory_pool);
    strncpy(ret->symbol, data, strlen(data) + 1);
    symbol_list = CONS(ret, symbol_list);
  }
  return ret;
}

struct memcell_s *mk_buildin(int cmd)
{
  struct memcell_s *ret = memcell_alloc(TYPE_BUILDIN, sizeof(*ret), memory_pool);
  ret->cmd = cmd;
  return ret;
}

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
#define add_list_element()              \
  if (pos && !first_in_list) {          \
    pos->cdr = (void*)CONS(NULL, NULL); \
    pos = (void*)pos->cdr;              \
  } else if (!pos){                     \
    source = pos = CONS(NULL, NULL);    \
  }
  source = NULL;
  struct cons_s *pos = NULL;
  char token_value[1024];
  int first_in_list = 0;
  do {
    int token = read_token(in_fd, token_value, sizeof(token_value));
    switch (token) {
      case T_SPECIAL:
        switch (*token_value) {
          case '(':
            add_list_element();
            push((void*)pos);
            pos->car = (void*)CONS(NULL, NULL);
            pos = (void*)pos->car;
            first_in_list = 1;
            break;
          case ')':
            pos = (void*)pop();
            if (first_in_list) {
              pos->car = NULL;
              first_in_list = 0;
            }
            break;
          case '\'':
            break;
          case ',':
            break;
        }
        break;
      case T_NUMBER:
        add_list_element();
        pos->car = (void*)parse_number(token_value);
        first_in_list = 0;
        break;
      case T_STRING:
        break;
      case T_SYMBOL:
        add_list_element();
        pos->car = (void*)mk_symbol(token_value);
        first_in_list = 0;
        break;
    }
  } while (parser_stack);
  if (source) {
    memcell_free(source);
    source = (void*)CAR(source);
    return (void*)source;
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
