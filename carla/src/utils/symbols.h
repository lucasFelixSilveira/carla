#ifndef SYMBOLS_H
# define SYMBOLS_H
# define KEYWORDS_LENGTH 2
# define TYPES_LENGTH 12

typedef struct { char len; char *val; } Key;
extern const Key staticSymbols[]; 

char inStaticSymbols (char buffer[], int x, int y);

/* REGEX matches */
char isIdentifier (char buffer[]);
char isKeyword (char buffer[]);
char isNumeric (char buffer[]);
char isType (char buffer[]);

#endif