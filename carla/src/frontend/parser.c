#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../utils/vector.h"
#include "../utils/symbols.h"
#include "../utils/carla.h"
#include "parser.h"
#include "lexer.h"

#define GET(vector, index) ((Token*)vector->items)[index]

#define BEGIN_SWITCH(str)    { char *_switch_str = str;
#define CASE(val)            if( strcmp (_switch_str, (val)) == 0 ) {
#define BREAK_CASE(val)      } else if( strcmp (_switch_str, (val)) == 0 ) {
#define DEFAULT              } else {
#define END_SWITCH           }
#define BREAK                }

void 
trim_non_alphanumeric(char *str) 
{
  int i, j = 0;
  
  for(
    i = 0;
    str[i] != '\0'; 
    i++
  ) {
      if (isalnum((unsigned char) str[i])) 
        str[j++] = str[i];
    }
  str[j] = '\0'; 
}

char 
isStruct(char *buffer, Vector *strucies) 
{
  char result = 0;
  VECTOR_CONTAINS (CacheStructs, strucies, identifier, buffer, &result);

  return result;
}

char 
isVarType(char *buffer, Vector *variables) 
{
  char result = 0;
  VECTOR_CONTAINS (CacheStructs, variables, identifier, buffer, &result);

  return result;
}

void 
tGenerate(Vector *tree, Vector *tks, Vector *libs) 
{
  Vector structs = vector_init (sizeof (CacheStructs));
  Vector struct_vars = vector_init (sizeof (CacheStructs));
  char *struct_type_name = NULL;
  char lambda = 0;
  char impl = 0;
  char is_bound = 0;
  char elements = 0;
  int i = 0;
  for(; i < tks->length; i++ ) 
    {
      Token first = GET(tks, i);
      switch(first.type)
        {
          case Macro:
            {
              BEGIN_SWITCH (first.buffer)
                CASE("#include")
                  Token quoute = GET(tks, ++i);
                  if( quoute.type == Text )
                    {
                      int result;
                      VECTOR_CONTAINS(Library, libs, lib, quoute.buffer, &result);
                      if( result ) 
                        continue;

                      char *lib = strdup (quoute.buffer);
                      trim_non_alphanumeric (lib);

                      vector_push (libs, ((void*)&(Library) {
                        .lib = lib,
                        .fn  = NULL,
                        .is  = 0 
                      }));
                      
                    }
                  else
                    goto __cancel_parse__;
                BREAK
              END_SWITCH
            } break;

          case Type:
          case Integer:
          case Text:
          case Keyword:
          case Identifier:
            {

              if( first.type == Keyword && strcmp (first.buffer, "bound") == 0 )
                {
                  is_bound = 1;
                  break;
                }

              if( first.type == Keyword && strcmp (first.buffer, "return") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_RET,
                    .data.number = 0
                  }));
                  break;
                }
              
              if( first.type == Keyword && strcmp (first.buffer, "for") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_FOR,
                    .data.number = 0
                  }));
                  break;
                }

              if( first.type == Keyword && strcmp (first.buffer, "if") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_IF,
                    .data.number = 0
                  }));
                  break;
                }

              if( first.type == Keyword && strcmp (first.buffer, "else") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_ELSE,
                    .data.number = 0
                  }));
                  break;
                }

              if( first.type == Keyword && strcmp (first.buffer, "struct") == 0 )
                {
                  if( struct_type_name != NULL )
                    goto __cancel_parse__;

                  i++;
                  printf ("struct");
                  char *id = GET(tks, i++).buffer;
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_STRUCT,
                    .data.value = id
                  }));

                  vector_push (&structs, ((void*)&(CacheStructs) {
                    .identifier = id
                  }));

                  struct_type_name = id;

                  continue;
                }
              
              if( first.type == Identifier && strcmp (first.buffer, "self") == 0 && lambda )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_DEFINITION,
                    .data = {
                      .definition = {
                        .type     = strdup (struct_type_name),
                        .id       = "self",
                        .is_bound = 0,
                        .hopeful  = 0
                      }
                    }
                  }));

                  struct_type_name = NULL;
                  break;
                }

              if( first.type == Identifier && strcmp (GET(tks, i + 1).buffer, ".") == 0 ) 
                {
                  i += 1;
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_FIELD_ACCESS,
                    .data.value = first.buffer
                  }));
                 
                  continue;
                }

              if( first.type == Identifier && strcmp (GET(tks, i + 1).buffer, "=") == 0 ) 
                {
                  i += 1;
                  
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_ASSIGNMENT,
                    .data.value = first.buffer
                  }));
                 
                  continue;
                }
              
              if( first.type == Type || isType (first.buffer) || (isStruct(first.buffer, &structs) && strcmp (GET(tks, i + 1).buffer, "::") != 0 ) )
                {
                  i++;
                  Token id = GET(tks, i);
                  if( id.type == Identifier ) 
                    {
                      i++;
                      char *final = GET(tks, i).buffer;
                      if(
                          strcmp (final, "=")    == 0 ||
                          strcmp (final, ";")    == 0 ||
                          strcmp (final, ":")    == 0 ||
                          strcmp (final, "end")  == 0 ||
                          (
                            (strcmp (final, ",") == 0 || strcmp (final, ")") == 0) 
                            && lambda
                          ) 
                        ) {
                            vector_push (tree, ((void*)&(PNode) {
                              .posD = id.local,
                              .type = NODE_DEFINITION,
                              .data = {
                                .definition = {
                                  .is_bound = is_bound,
                                  .hopeful  = strcmp (final, "=") == 0,
                                  .iter     = strcmp (final, ":") == 0,
                                  .type     = first.buffer,
                                  .id       = id.buffer
                                }
                              }
                            }));

                            vector_push (&struct_vars, ((void*)&(CacheStructs) {
                              .identifier = id.buffer,
                              ._type = first.buffer
                            }));
                            
                            is_bound = 0;
                          }
                      else
                        goto __cancel_parse__;
                    }
                  else
                    {
                      vector_push (tree, ((void*)&(PNode) {
                        .type = NODE_TYPE,
                        .data.value = first.buffer
                      }));
                    }
                  break;
                }

              Token next = GET(tks, i + 1);
              Token after = GET(tks, i + 2);
              Token parenthesis = GET(tks, i + 3);
              Token method = GET(tks, i + 4);
              if(
                (
                  first.type == Identifier 
                  || (
                    first.type == Keyword
                    && strcmp (first.buffer, "super") == 0
                  ) 
                ) 
                && (
                  next.type     == Quad 
                  || next.type  == MethodAccess 
                ) 
                && after.type == Identifier 
                && ((
                  parenthesis.type == Unknown 
                  && parenthesis.buffer[0] == '('
                ) || parenthesis.type == MethodAccess)
              ) {
                  i += 3;

                  if( strcmp (first.buffer, "super") == 0 )
                    {
                      vector_push (tree, ((void*)&(PNode) {
                        .posD = after.local,
                        .type = NODE_INTERNAL_SUPER,
                        .data.super = after.buffer
                      }));
                      break;
                    }

                  if( isVarType (first.buffer, &struct_vars) )
                    {
                      CacheStructs str;
                      VECTOR_FIND(CacheStructs, &struct_vars, identifier, first.buffer, &str);

                      vector_push (tree, ((void*)&(PNode) {
                        .posD = after.local,
                        .type = NODE_INTERNAL_STRUCT,
                        .data.internal_struct = {
                          .need_pass_the_instance = 1,
                          .instance_id = first.buffer,
                          .__struct    = str._type,
                          .fn          = after.buffer,
                        }
                      }));

                      break;
                    }

                  if( isStruct (first.buffer, &structs) )
                    {
                      char *instance = NULL;
                      char need_self = 0;
                      char *direct_method_or_class_method = after.buffer;
                      if( parenthesis.type == MethodAccess )
                        { 
                          instance = direct_method_or_class_method;
                          need_self = 1;
                          direct_method_or_class_method = method.buffer;
                        }

                      vector_push (tree, ((void*)&(PNode) {
                        .posD = after.local,
                        .type = NODE_INTERNAL_STRUCT,
                        .data.internal_struct = {
                          .need_pass_the_instance = need_self,
                          .instance_id = instance,
                          .__struct    = first.buffer,
                          .fn          = direct_method_or_class_method,
                        }
                      }));
                      
                      break;
                    }

                  int result = 0;
                  char *module = (char*)malloc (128);
                  sprintf (module, "std%s", first.buffer);
                  VECTOR_CONTAINS(Library, libs, lib, module, &result);

                  if( result )
                    {
                      Library library;
                      VECTOR_FIND(Library, libs, lib, module, &library);
                      VECTOR_CONTAINS(Library, libs, fn, after.buffer, &result);

                      if(! result )
                        {
                          vector_push (libs, ((void*)&(Library) {
                            .fn  = strdup (after.buffer),
                            .lib = library.lib,
                            .is  = 1
                          }));
                        }

                      vector_push (tree, ((void*)&(PNode) {
                        .posD = after.local,
                        .type = NODE_INTERNAL,
                        .data = {
                          .internal   = {
                            .function = after.buffer,
                            .lib      = module
                          }
                        }
                      }));

                      continue;
                    } 
                  else
                    {
                      free (module);
                      goto __cancel_parse__;
                    }
                }

              // EXPRESSIONS
              if( 
                  ( first.type    == Integer || first.type == Identifier || first.type == Text    ) 
                  && ( next.type  == MathOP  || next.type  == Semi       || next.type  == Unknown || next.type == Iter || next.type == BitsOP || next.type == ComparationOP || next.type == Keyword )
              ) {
                  if(
                      ( next.type == Semi ) 
                      || ( next.type == Keyword ) 
                      || (
                          next.type == Unknown 
                          && (
                            next.buffer[0] == ']' 
                            || next.buffer[0] == '}'
                            || next.buffer[0] == ')'
                            || next.buffer[0] == ','
                          )
                         )
                    ) { 
                        if( first.type == Identifier ) 
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_SINGLE,
                            .data = {
                              .single = {
                                .type = NODE_ID,
                                .data.value = first.buffer
                              }
                            }
                          }));
                        else
                        if( first.type == Text ) 
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_SINGLE,
                            .data = {
                              .single = {
                                .type = NODE_TEXT,
                                .data.value = first.buffer
                              }
                            }
                          }));
                        else
                        if( first.type == Integer ) 
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_SINGLE,
                            .data = {
                              .single = {
                                .type = NODE_NUMBER,
                                .data.number = atoi (first.buffer)
                              }
                            }
                          }));

                        if( next.type == Semi )
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_EEXPR,
                            .data.number = 0
                          }));

                        break;
                      }
                    else 
                      if( 
                           ( first.type == Integer || first.type == Identifier )
                        && ( next.type  == MathOP  || next.type  == BitsOP || next.type == ComparationOP || next.type == Iter )
                        && ( first.type == Integer || first.type == Identifier )
                      ) 
                        {
                          i += 2;
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_OPERATION,
                            .data = {
                              .operation   = {
                                .left      = first,
                                .operation = next,
                                .right     = after, 
                              }
                            }
                          }));
                        }

                }

              Token can  = GET(tks, i + 2);
              if( next.type == Unknown && next.buffer[0] == '[' 
                  && (
                    can.type == Integer || can.type == Identifier
                  ) 
                ) {
                    vector_push (tree, ((void*)&(PNode) {
                      .type = NODE_ACCESS,
                      .data.value = first.buffer
                    }));
                    break;
                  }
            } break;

          case Semi:
            {
              vector_push (tree, ((void*)&(PNode) {
                .type = NODE_EEXPR,
                .data.number = 0
              }));
            }

          case Unknown:
            {
              if( strcmp (first.buffer, "}") == 0 )
                {
                  char is_impl_close = strcmp (GET(tks, i + 1).buffer, "impl") == 0;
                  char begin = 0;

                  vector_push (tree, ((void*)&(PNode) {
                    .data.number = 0,
                    .type = (is_impl_close) 
                              ? NODE_END_IMPLEMENT 
                              : NODE_END,
                  }));

                }

              if( strcmp (first.buffer, ")") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_CLOSE,
                    .data.number = 0
                  }));
                }

              if( strcmp (first.buffer, "{") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_BEGIN,
                    .data.number = 0
                  }));
                  lambda = 0;
                }
                
              if( strcmp (first.buffer, "(") == 0 )
                {
                  int j = i + 1;
                  while( strcmp (GET(tks, j++).buffer, ")") != 0 );
                  if( strcmp (GET(tks, j).buffer, "{") == 0 || (
                    strcmp (GET(tks, j).buffer, "our") == 0 
                    && strcmp (GET(tks, j + 1).buffer, "{") == 0
                  ))
                    {

                      vector_push (tree, ((void*)&(PNode) {
                        .type = NODE_LAMBDA,
                        .data.our = (strcmp (GET(tks, j).buffer, "our") == 0)
                      }));
                      lambda = 1;
                      break;
                    } 
                }
            }
          case EnumType:
            {
              if( GET(tks, i + 1).buffer[0] == '=' && GET(tks, i + 2).buffer[0] == '{' )
                {
                  i += 2;
                  char *integer = first.sub;
                  char *definition = first.def;

                  Vector fields = vector_init (sizeof (EnumFieldName));
                  while(1) 
                    {
                      Token identifier = GET(tks, i + 1);
                      Token comma = GET(tks, i + 2);
                      i += 2;
                      char byte = identifier.buffer[0];
                      if( identifier.type == Identifier && byte >= 0x41 && byte <= 0x5a ) 
                        vector_push(&fields, ((void*)&(EnumFieldName) {
                          .identifier = identifier.buffer
                        }));
                      
                      char ch = comma.buffer[0];
                      if( ch == '}' ) 
                        break;
                      
                      if ( ch != ',' )
                        goto __cancel_parse__;
                        
                    }

                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_ENUM,
                    .data = {
                      .enumerator = {
                        .ctx.integer_t  = integer,
                        .ctx.definition = definition,
                        .pFields        = &fields
                      }
                    }
                  }));
                  
                  continue;
                }
            } break;
          default: break;
        }
    }
  goto skip;
  __cancel_parse__: {
    printf ("\nPARSE ERROR: Fail! Check your code\n");
  }
  skip: {}
}