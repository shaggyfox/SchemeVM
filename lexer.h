#ifndef LEXER_H
#define LEXER_H
enum {
  T_ERROR,
  T_SPECIAL,
  T_SYMBOL,
  T_STRING,
  T_NUMBER,
};

int read_token(int fd, char *buff, int len);
#endif
