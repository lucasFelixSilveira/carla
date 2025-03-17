#ifndef ERRORS_H
# define ERRORS_H
# include <stdio.h>
# include "../frontend/lexer.h"

  typedef enum {
    SyntaxTypeError,
    InvalidAccess,
    UnrecognizedSymbol
  } CarlaErrors;

  typedef enum {
    ChangePrivacity,
  } CarlaTips;

  void errors_common(FILE *logs, TokenLocation local, CarlaErrors error, char *buffer, char *msg); 
  void errors_tip(FILE *logs, TokenLocation local, CarlaErrors error, char *buffer, char *msg, char *can, CarlaTips tip); 
  void errors_uses(FILE *logs, TokenLocation usesLocal, TokenLocation local, CarlaErrors error, char *uses, char *buffer, char *msg, char *can, CarlaTips tip); 
  #endif