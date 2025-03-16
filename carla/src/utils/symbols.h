#ifndef SYMBOLS_H
# define SYMBOLS_H
# define KEYWORDS_LENGTH 12
# define MACROS_LENGTH 1
# define TYPES_LENGTH 21
# define INTEGER_TYPES_LENGTH 14
# define TOTAL KEYWORDS_LENGTH + MACROS_LENGTH + TYPES_LENGTH 

  typedef struct { char len; char *val; } Key;
  extern const Key staticSymbols[]; 

  char inStaticSymbols (char buffer[], int x, int y);

  /* REGEX matches */
  char isIdentifier (char buffer[]);
  char isKeyword    (char buffer[]);
  char isNumeric    (char buffer[]);
  char isMacro      (char buffer[]);
  char isType       (char buffer[]);

#endif