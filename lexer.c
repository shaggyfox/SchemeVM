#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
enum {
  INIT,
  IN_SYMBOL,
  IN_STRING,
  IN_NUMBER,
  IN_ESC,
};

#define IS_NUMBER(c) strchr("0123456789", c)
#define IS_SPECIAL(c) strchr(",'()'", c)
#define IS_WHITESPACE(c) strchr(" \t\n\r", c)

int read_token(int fd, char *buff, int len)
{
#define add_to_buff(c) if (len > 1){ *(buff++) = c;}
  int state = INIT;
  static char c = '\0';
  static int read_again = 0;
  int ret = T_ERROR;
  int esc = 0;

  for (;;) {
    if (!read_again) {
      if (1 != read(fd, &c, 1)) {
        *buff = '\0';
        return ret;
      }
    } else {
      read_again = 0;
    }
    switch (state) {
      case INIT:
        if (c == '\\') {
          return T_ERROR;
        } else if (c == '"') {
          state = IN_STRING;
          ret = T_STRING;
        } else if (IS_WHITESPACE(c)) {
          /* do nothing */
        } else if (IS_NUMBER(c)) {
          state = IN_NUMBER;
          add_to_buff(c);
          ret = T_NUMBER;
        } else if (IS_SPECIAL(c)) {
          add_to_buff(c);
          *buff = '\0';
          return T_SPECIAL;
        } else {
          state = IN_SYMBOL;
          add_to_buff(c);
          ret = T_SYMBOL;
        }
        break;
      case IN_STRING:
        if (c == '"') {
          if (esc) {
            add_to_buff(c);
          } else {
            *buff = '\0';
            return ret;
          }
        } else if (c == '\\') {
          if (esc) {
            add_to_buff(c);
          } else {
            esc = 1;
            continue;
          }
        } else {
          add_to_buff(c);
        }
        break;
      case IN_SYMBOL:
        if (c == '\\') {
          return T_ERROR;
        } else if (IS_WHITESPACE(c) || IS_SPECIAL(c)) {
          read_again = 1;
          *buff = '\0';
          return ret;
        } else {
          add_to_buff(c);
        }
        break;
      case IN_NUMBER:
        if (c == '\\') {
          return T_ERROR;
        } else if (IS_SPECIAL(c) || IS_WHITESPACE(c)) {
          read_again = 1;
          *buff = '\0';
          return ret;
        } else if(IS_NUMBER(c)) {
          add_to_buff(c);
        } else {
          /* error */
          return T_ERROR;
        }
        break;
    }
    esc = 0;
  }
}

#if 0
int main() {
  int c;
  char buff[1024];
  while (T_ERROR != (c = read_token(0, buff, sizeof(buff)))) {
    switch (c) {
      case T_STRING:
        printf("string:%s\n", buff);
        break;
      case T_NUMBER:
        printf("number:%s\n", buff);
        break;
      case T_SYMBOL:
        printf("symbol:%s\n", buff);
        break;
      case T_SPECIAL:
        printf("special:%s\n", buff);
        break;
    }
  }
}
#endif
