#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "vector.h"
#include "parser.h"
#include "symbols.h"

typedef enum {
  List,
  Num,
  String,
  Char
} Basic;

typedef enum {
  Scope_if,
  Scope_else,
  Scope_for_iter,
  Scope_for_revese_iter,
  Scope_for_reverse_comparator,
  Scope_for_comparator,
  Scope_while,
  Scope_fun
} scope_t;

typedef struct {
  scope_t type;
  int label;
  int var_id;
  int to_c;
} Scopes;

typedef struct {
  DMemory def;
  int level;
  int llvm;
} Variable;

int stackpos  = 0;
int varspos   = 0;
int types_len = 0;
int fun_len   = 0;
char *stacktype[1024];
Variable variables[2048];
char **types[1024];
char *functions[4096];
int scope = 1;
int label = 0;
int arr_id = 0;

char opcodes[4][3] = {
  "sub", "add", "mul", "div"
};

Scopes scopes[2048]; // { ID, Respective }
size_t scopes_position = 0;

char 
is_numeric(char *buff) 
{
  for (
    int i = 0;
    i < strlen (buff);
    i++
  ) {
      if(! isdigit (buff[i]) ) 
        /*->*/ return 0;
    }
  return 1;
}

char** 
str_split(char* a_str, const char a_delim)
{
  char** result    = 0;
  size_t count     = 0;
  char* tmp        = a_str;
  char* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  while (*tmp)
    {
      if (a_delim == *tmp)
        {
          count++;
          last_comma = tmp;
        }
      tmp++;
    }

  count += last_comma < (a_str + strlen (a_str) - 1);
  count++;

  result = malloc (sizeof(char*) * count);

  if (result)
    {
      size_t idx  = 0;
      char* token = strtok (a_str, delim);

      while (token)
        {
          assert (idx < count);
          *(result + idx++) = strdup (token);
          token = strtok (0, delim);
        }
      assert (idx == count - 1);
      *(result + idx) = 0;
    }

  return result;
}

int
isFunction (char *f_) 
{
  for(
    int i = 0;
    i < fun_len;
    i++
  ) {
      if( strcmp (functions[i], f_) == 0 )
        /*->*/ return 1;
    }
  
  return 0;
}

int
llvm_sizeof (char *type)
{
  if( strcmp (type, "i8") == 0 || strcmp (type, "u8") == 0 )
    return 1;
  if( strcmp (type, "i16") == 0 || strcmp (type, "u16") == 0 )
    return 2;
  if( strcmp (type, "i32") == 0 || strcmp (type, "u32") == 0 )
    return 4;
  if( strcmp (type, "i64") == 0 || strcmp (type, "u64") == 0 || strcmp (type, "ptr") == 0 )
    return 8;
  if( strcmp (type, "i128") == 0 || strcmp (type, "u128") == 0 )
    return 16;
  
  for(
    int i = 0; 
    i < types_len;
    i++
  ) {
      char **__type = types[i]; 
      
      if( strcmp (__type[0], type) == 0 && '*' == __type[1][strlen (__type[1]) - 1] )
        return 8;

      free (__type);
    }
  return 0;
}

char * 
concat(const char* str1, const char* str2) 
{
  size_t len1 = strlen (str1);
  size_t len2 = strlen (str2);
  size_t len = len1 + len2 + 1; 

  char* result = (char*)malloc (len * sizeof(char));

  strcpy (result, str1);
  strcat (result, str2);

  return result;
}


char *
substr(int pos, int len, int total, char string[])
{

  char *substring = (char*)malloc (total);

  int i = 0;
  while (i < len) 
    {
      substring[i] = string[pos + i - 1];
      i++;
    }

  substring[i] = '\0';
  return substring;
}

void 
fheader(FILE *file, char *new_line) 
{
  fseek (file, 0, SEEK_SET);
  fseek (file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek (file, 0, SEEK_SET);

  char *original_content = malloc(file_size + 1);
  fread (original_content, 1, file_size, file);
  original_content[file_size] = '\0';

  fseek (file, 0, SEEK_SET);

  size_t new_line_length = strlen(new_line);
  size_t total_content_length = new_line_length + file_size + 2;

  fprintf (file, "%s\n%s", new_line, original_content);

  free (original_content);

  fseek (file, 0, SEEK_END);
}

char *
back_pointer (char *type)
{
  for(
    int i = 0; 
    i < types_len;
    i++
  ) {
      char **__type = types[i]; 
      
      if( strcmp (__type[0], type) == 0 && '*' == __type[1][strlen (__type[1]) - 1] )
        return substr (1, strlen (__type[1]) - 1, strlen (__type[1]), __type[1]);

      free (__type);
    }
}

void 
llGenerate (FILE *output, char *directory, Vector *pTree) 
{
  int var = 1;

  for(
    int i = 0;
    i < pTree->length; 
    i++
  ) {
      PNode branch = ((PNode*)pTree->items)[i];
      if( 
        branch.type == Definition
      ) {
          if( branch.saves.definition.key_type )
            {
              PNode t = ((PNode*)pTree->items)[i+1];
              if( t.type == ArrayType )
                {
                  char *op1 = (char*)malloc (64);
                  char *op2 = (char*)malloc (64);

                  sprintf (op1, "%s*", t.saves.definition.array.type);
                  sprintf (op2, "[%s x %s]", t.saves.definition.array.size, t.saves.definition.array.type);

                  fprintf (output, "%c%s = type %s\n", '%',
                          branch.saves.definition.id,
                          strcmp (t.saves.definition.array.size, "undefined") == 0 ? op1 : op2
                  );

                  types[types_len][0] = branch.saves.definition.id;
                  types[types_len++][1] = strcmp (t.saves.definition.array.size, "undefined") == 0 ? op1 : op2;

                  if( strcmp (t.saves.definition.array.size, "undefined") == 0 )
                    { free (op2); }
                  else 
                    { free (op1); }
                }
              else {
                fprintf (output, "\n\ndo nothing\n\n");
              }

              continue;
            }

          if( 
            branch.saves.definition.hopeful 
            && ((PNode*)pTree->items)[i + 1].type == Lambda 
          ) {
              char *type = branch.saves.definition.type;
              char *id = branch.saves.definition.id;
              
              char *keyOfDef = i == 0 ? "define" 
                             : ((PNode*)pTree->items)[i - 1].type == Magic ? strcmp (((PNode*)pTree->items)[i - 1].saves.magic, "extern") == 0 ? "declare" 
                                                                           : "define"                                                          : "define";

              stacktype[stackpos++] = strdup (type);
              fprintf (output, "%s %s @%s(", 
                      keyOfDef,
                      branch.saves.definition.type, 
                      branch.saves.definition.id
              );

              functions[fun_len++] = branch.saves.definition.id;
              variables[varspos++] = (Variable) {
                .level = scope, 
                .def = branch.saves.definition,
                .llvm = 0
              };

              scopes[scopes_position++] = (Scopes) {
                .type = Scope_fun,
                .label = 0,
                .var_id = 0
              };

              char *prefix = "";
              int j;
              int old = var;
              i += 2;
              for(
                j = i;
                j < pTree->length;
                j++ 
              ) {
                  PNode branch = ((PNode*)pTree->items)[j];
                  if( 
                    branch.type == Begin
                    && strcmp (keyOfDef, "define") == 0 
                  ) {
                      fprintf (output, ") {\n");
                      int x = i;
                      if( x < j ) { var++; } 
                      for(
                        ;x < j;
                        x++
                      ) {
                          PNode branch = ((PNode*)pTree->items)[x];
                          if( branch.type != Definition ) 
                            /*->*/ break;

                          char *t = strcmp (branch.saves.definition.type, "ptr") == 0 ? ( strcmp (branch.saves.definition.array.size, "undefined") == 0 ? concat(isType (branch.saves.definition.array.type) ? branch.saves.definition.array.type : concat("%", branch.saves.definition.array.type), "*")
                                                                                                                                                        : concat("[", concat(branch.saves.definition.array.size, concat(" x ", isType(branch.saves.definition.array.type) ? concat(branch.saves.definition.array.type, "]") : concat("%", concat(branch.saves.definition.array.type, "]"))))) )
                                                                                      : isType(branch.saves.definition.type) ? branch.saves.definition.type 
                                                                                                                             : concat("%", branch.saves.definition.type);

                          fprintf (output, "%c%d = alloca %s, align %d\n", '%', 
                                  var, 
                                  t,
                                  llvm_sizeof (branch.saves.definition.type)
                          );

                          fprintf (output, "store %s %c%d, ptr %c%d, align %d\n",
                                  t, '%',
                                  old + (x - i), '%',
                                  var,
                                  llvm_sizeof (branch.saves.definition.type)
                          );

                          variables[varspos++] = (Variable) {
                            .level = scope, 
                            .def = branch.saves.definition,
                            .llvm = var++
                          };
                        }
                      break;
                    }

                  if( 
                    (branch.type != LiteralType && branch.type != ArrayType)
                    && strcmp (keyOfDef, "declare") == 0 
                  ) {
                      fprintf(output, ") nounwind\n");
                      break;
                    }

                  if( 
                    (branch.type == LiteralType || branch.type == ArrayType)
                    && strcmp (keyOfDef, "declare") == 0 
                  ) {
                      if( strlen (prefix) == 1 )
                        {
                          prefix = (char*)malloc (3);
                          sprintf(prefix, ", ");
                        }

                      if( branch.type == LiteralType && isType (branch.saves.lit) )
                        {

                          fprintf (output, "%s%s",
                                  prefix,
                                  branch.saves.lit
                          );

                        }     
                      else 
                      if( branch.type == ArrayType )
                        {

                          if( strcmp (branch.saves.array.size, "undefined") == 0 )
                            {
                              fprintf (output, "%s%c%s*",
                                      prefix, '%',
                                      branch.saves.array.type
                              );
                            }
                          else
                            {
                              fprintf (output, "%s[%s x %c%s]",
                                      prefix, 
                                      branch.saves.array.size, '%',
                                      branch.saves.array.type
                              );
                            }

                        }        
                      else
                        {

                          fprintf (output, "%s%c%s",
                                  prefix, '%',
                                  branch.saves.lit
                          );

                        }

                      if( strlen (prefix) == 0 )
                        /*->*/ prefix = ".";

                    }

                  if( 
                    branch.type == Definition 
                    && branch.saves.definition.arg 
                    && strcmp (keyOfDef, "define") == 0 
                  ) {
                      if( strlen (prefix) == 1 )
                        {
                          prefix = (char*)malloc (3);
                          sprintf(prefix, ", ");
                        }

                      if( isType (branch.saves.definition.type) )
                        {

                          fprintf (output, "%s%s noundef %c%d",
                                  prefix,
                                  branch.saves.definition.type, '%', 
                                  var++
                          );

                        }     
                      else 
                      if( strcmp (branch.saves.definition.type, "ptr") == 0 )
                        {

                          if( strcmp (branch.saves.definition.array.size, "undefined") == 0 )
                            {
                              fprintf (output, "%s%c%s* noundef %c%d",
                                      prefix, '%',
                                      branch.saves.definition.array.type, '%', 
                                      var++
                              );
                            }
                          else
                            {
                              fprintf (output, "%s[%s x %c%s] noundef %c%d",
                                      prefix, 
                                      branch.saves.array.size, '%',
                                      branch.saves.definition.array.type, '%', 
                                      var++
                              );
                            }

                        }        
                      else
                        {

                          fprintf (output, "%s%c%s noundef %c%d",
                                  prefix, '%',
                                  branch.saves.definition.type, '%', 
                                  var++
                          );

                        }

                      if( strlen (prefix) == 0 )
                        /*->*/ prefix = ".";

                    }
                }

              i = j;
              scope++;
            }
          else
            {

              if( isType (branch.saves.definition.type) )
                {
                  fprintf (output, "%c%d = alloca %s, align %d\n", '%', 
                          var,
                          branch.saves.definition.type,
                          llvm_sizeof (branch.saves.definition.type)
                  );
                }
              else 
                {
                  fprintf (output, "%c%d = alloca %c%s, align %d\n", '%', 
                          var, '%',
                          branch.saves.definition.type,
                          llvm_sizeof (branch.saves.definition.type)
                  );
                }
              
              variables[varspos++] = (Variable) {
                .level = scope, 
                .def = branch.saves.definition,
                .llvm = var++
              };

              PNode next = ((PNode*)pTree->items)[i + 1];
              if( 
                next.type == ArrayType &&
                strcmp (next.saves.array.size, "undefined") != 0
              ) {
                  char *array = next.saves.array.type;
                  char *index = next.saves.array.size;
                  
                  for(
                    int i = 0;
                    i < varspos;
                    i++
                  ) {
                      if( variables[i].level > scope )
                        /*->*/ break;

                      if( strcmp (variables[i].def.id, array) != 0 )
                        /*->*/ continue; 
                    
                      char *prefix = malloc(sizeof(char) * 16);
                      prefix[0] = '\0';
                      if(! isType (variables[i].def.array.type) )
                        /*->*/ prefix = "%";

                      char *op1 = (char*)malloc (64);
                      char *op2 = (char*)malloc (64);
                      char *op3 = (char*)malloc (64);

                      sprintf (op1, "%s%s", prefix, variables[i].def.array.type);
                      sprintf (op2, "%s%s*", prefix, variables[i].def.array.type);
                      sprintf (op3, "[%s x %s%s]", variables[i].def.array.size, prefix, variables[i].def.array.type);

                      fprintf (output, "%c%d = load %s, ptr %c%d, align 8\n", '%',
                              var,
                              strcmp (variables[i].def.array.size, "undefined") == 0 ? strdup (op2) : strdup (op3), '%',
                              variables[i].llvm
                      );

                      if( is_numeric (index) ) 
                        {
                          fprintf (output, "%c%d = getelementptr inbounds %s, %s %c%d, i64 %s\n", '%',
                                  var+1,
                                  op1, 
                                  strcmp (variables[i].def.array.size, "undefined") == 0 ? strdup (op2) : strdup (op3), '%',
                                  var,
                                  index
                          );

                          fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                                  var + 2,
                                  isType (variables[i].def.array.type) ? variables[i].def.array.type : concat("%", variables[i].def.array.type), '%',
                                  var + 1,
                                  llvm_sizeof (variables[i].def.array.type)
                          );

                          free (op1);
                          free (op2);
                          free (op3);

                          fprintf (output, "store %s%s %c%d, ptr %c%d, align %d\n", 
                                  prefix,
                                  variables[i].def.array.type, '%',
                                  var + 2, '%',
                                  (var - 1),
                                  llvm_sizeof (branch.saves.definition.type)        
                          );

                          var+=3;
                        }
                      else 
                        {
                          for(
                            int x = 0;
                            x < varspos;
                            x++
                          ) {
                              if( variables[x].level > scope )
                                /*->*/ break;

                              if( strcmp (variables[x].def.id, index) != 0 )
                                /*->*/ continue; 
                              
                              fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                                      (var + 1),
                                      variables[x].def.type, '%',
                                      variables[x].llvm,
                                      llvm_sizeof (variables[x].def.type)
                              );

                              fprintf (output, "%c%d = getelementptr inbounds %s, %s %c%d, %s %c%d\n", '%',
                                      var+2,
                                      op1, 
                                      strcmp (variables[i].def.array.size, "undefined") == 0 ? strdup (op2) : strdup (op3), '%',
                                      var, 
                                      variables[x].def.type, '%',
                                      var+1
                              );
                            }

                          fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                                  var + 3,
                                  isType (variables[i].def.array.type) ? variables[i].def.array.type : concat("%", variables[i].def.array.type), '%',
                                  var + 2,
                                  llvm_sizeof (variables[i].def.array.type)
                          );

                          free (op1);
                          free (op2);
                          free (op3);

                          fprintf (output, "store %s%s %c%d, ptr %c%d, align %d\n", 
                                  prefix,
                                  variables[i].def.array.type, '%',
                                  var + 3, '%',
                                  (var - 1),
                                  llvm_sizeof (branch.saves.definition.type)        
                          );

                          var+=4;
                        }
                    }

                  free (array);
                  free (index);
                }
              else if( 
                next.type == Normal 
                && next.saves.token.type == Integer 
              ) {
                  fprintf (output, "store %s %s, ptr %c%d, align %d\n", 
                          branch.saves.definition.type,
                          next.saves.token.buffer, '%',
                          (var - 1),
                          llvm_sizeof (branch.saves.definition.type)        
                  );
                }
              if( // Alocar a string
                next.type == Normal 
                && next.saves.token.type == Literal 
              ) {
                  const char *predef = "$arr_9999999999 = comdat any\n@arr_9999999999 = linkonce_odr dso_local unnamed_addr constant [999999 x i8] c\"\\00\", comdat, align 1\n";
                  char *content = (char*)malloc (strlen (predef) + strlen (next.saves.token.buffer));
                  
                  sprintf (content, "$arr_%d = comdat any\n@arr_%d = linkonce_odr dso_local unnamed_addr constant [%d x i8] c\"%s\\00\", comdat, align 1",
                          arr_id,
                          arr_id,
                          (int) next.saves.token.real,
                          strdup (next.saves.token.buffer)
                  );
                  fheader (output, content);

                  fprintf (output, "store i8* @arr_%d, ptr %c%d, align %d\n", 
                          arr_id++, '%',
                          (var - 1),
                          llvm_sizeof (branch.saves.definition.type)        
                  );
                }
              else
              if( 
                next.type == Normal 
                && next.saves.token.type == Identifier 
              ) {
                for(
                  int i = 0;
                  i < varspos;
                  i++
                ) {
                    if( variables[i].level > scope )
                      /*->*/ break;

                    if( strcmp (variables[i].def.id, next.saves.token.buffer) != 0 )
                      /*->*/ continue; 

                    int size_var = llvm_sizeof (variables[i].def.type);
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var,
                            variables[i].def.type, '%',
                            variables[i].llvm,
                            size_var    
                    );

                    int cast = 0;
                    if( size_var > llvm_sizeof (branch.saves.definition.type) ) 
                      {
                        cast = 1;
                        fprintf (output, "%c%d = trunc %s %c%d to %s\n", '%',
                                var + 1,
                                variables[i].def.type, '%',
                                var,
                                branch.saves.definition.type    
                        );
                      }
                    else 
                    if( size_var < llvm_sizeof (branch.saves.definition.type) ) 
                      {
                        cast = 1;
                        fprintf (output, "%c%d = sext %s %c%d to %s\n", '%',
                                var + 1,
                                variables[i].def.type, '%',
                                var,
                                branch.saves.definition.type    
                        );
                      }

                      fprintf (output, "store %s %c%d, ptr %c%d, align %d\n", 
                              branch.saves.definition.type, '%',
                              var + cast, '%',
                              var - 1,
                              llvm_sizeof (branch.saves.definition.type)        
                      );

                    var = var + 1 + cast;
                  }
                }
              i++;
            }
        }
      else
      if( 
        branch.type == Magic 
        && strcmp (branch.saves.magic, "return") == 0
      ) {
          
          PNode next = ((PNode*)pTree->items)[i + 1];

          if( next.type == Normal && next.saves.token.type == Integer ) 
            {
              fprintf (output, "ret %s %s\n", 
                      stacktype[--stackpos], 
                      next.saves.token.buffer
              );
            }
          else
          if( next.type == Normal && next.saves.token.type == Identifier ) 
            {
              for(
                int i = 0;
                i < varspos;
                i++
              ) {
                  if( variables[i].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[i].def.id, next.saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  int size_var = llvm_sizeof (variables[i].def.type);
                  int size_last = llvm_sizeof (stacktype[--stackpos]);

                  if( size_var == size_last ) {
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var, 
                            stacktype[stackpos], '%',
                            variables[i].llvm,
                            size_last
                    );

                    fprintf (output, "ret %s %c%d\n", 
                            stacktype[stackpos], '%', 
                            var++
                    );
                  } else {
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var, 
                            variables[i].def.type, '%', 
                            variables[i].llvm, 
                            size_var
                    );

                    fprintf (output, "%c%d = %s %s %c%d to %s\n", '%',
                            var + 1, 
                            ( size_var > size_last ? "trunc" : "sext"),
                            variables[i].def.type, '%',
                            var, 
                            stacktype[stackpos]
                    );

                    fprintf (output, "ret %s %c%d\n", 
                            stacktype[stackpos], '%',
                            var + 1
                    );
                    var += 2;
                  }
                  i++;
                }
            }

          i++;
          stacktype[stackpos] = NULL;
        }
      else
      if( 
        branch.type == Magic 
        && strcmp (branch.saves.magic, "if") == 0
      ) {
          PNode next = ((PNode*)pTree->items)[i + 1];
          for(
            int x = 0;
            x < varspos;
            x++
          ) {
              if( variables[x].level > scope )
                /*->*/ break;

              if( strcmp (variables[x].def.id, next.saves.token.buffer) != 0 )
                /*->*/ continue; 

              PNode pOperator = ((PNode*)pTree->items)[i + 2];
              char *operator;

              
              char *content = pOperator.saves.token.buffer; 
              if( strcmp (content, "==") == 0 ){
                operator = "eq";
              } 
              else
              if( strcmp (content, ">=") == 0 ){
                operator = "sge";
              } 
              else
              if( strcmp (content, ">") == 0 ){
                operator = "sgt";
              } 
              else
              if( strcmp (content, "<=") == 0 ){
                operator = "sle";
              } 
              else
               if( strcmp (content, "<") == 0 ){
                operator = "slt";
              } 
              else
              if( strcmp (content, "!=") == 0 ){
                operator = "ne";
              } 

              fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%', 
                      var, 
                      variables[x].def.type, '%',
                      variables[x].llvm,
                      llvm_sizeof (variables[x].def.type)
              );

              

              PNode toCompare = ((PNode*)pTree->items)[i + 3];
              if( toCompare.type == Normal && toCompare.saves.token.type == Identifier ) 
                {
                  for(
                    int j = 0;
                    j < varspos;
                    j++
                  ) {
                      if( variables[j].level > scope )
                        /*->*/ break;

                      if( strcmp (variables[j].def.id, toCompare.saves.token.buffer) != 0 )
                        /*->*/ continue; 

                      if( variables[j].def.type[0] != 'i' ) 
                        /*->*/ break;
                    
                      int size = llvm_sizeof (variables[j].def.type);
                      int start = llvm_sizeof (variables[x].def.type);
                      
                      fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%', 
                              (var + 1),
                              variables[j].def.type, '%',
                              variables[j].llvm,
                              size
                      );

                      var += 2;

                      if( size > x ) 
                        {
                          fprintf (output, "%c%d = sext %s %c%d to %s\n", '%', 
                            var,
                            variables[x].def.type, '%',
                            (var - 2),
                            variables[j].def.type
                          );
                          var++;

                          fprintf (output, "%c%d = icmp %s %s %c%d, %c%d\n", '%', 
                                  (var + 1),
                                  operator, 
                                  variables[j].def.type, '%',
                                  (var - 2), '%',
                                  (var - 1)
                          );
                        }
                      else if( size < start )
                        {
                          fprintf (output, "%c%d = sext %s %c%d to %s\n", '%', 
                                  var,
                                  variables[j].def.type, '%',
                                  (var - 1),
                                  variables[x].def.type
                          );
                          var++;

                          fprintf (output, "%c%d = icmp %s %s %c%d, %c%d\n", '%', 
                                  (var + 1),
                                  operator, 
                                  variables[x].def.type, '%',
                                  (var - 3), '%',
                                  (var - 1)
                          );
                        }

                      fprintf (output, "br i1 %c%d, label %cL%d, label %cE%d\n", '%',
                              (var + 1), '%',
                              label, '%',
                              label
                      );

                      fprintf (output, "\nL%d:\n",
                              label 
                      );

                      scopes[scopes_position++] = (Scopes) {
                        .type = Scope_if,
                        .label = label,
                        .var_id = 0
                      };
                      
                      i += 4;
                      var += 2;
                      label++;
                      scope++;

                    }
                }
              else
              if( toCompare.type == Normal && toCompare.saves.token.type == Integer ) 
                {
                  fprintf (output, "%c%d = icmp %s %s %c%d, %s\n", '%', 
                          (var + 1),
                          operator, 
                          variables[x].def.type, '%',
                          var,
                          toCompare.saves.token.buffer
                  );

                  fprintf (output, "br i1 %c%d, label %cL%d, label %cE%d\n", '%',
                          (var + 1), '%',
                          label, '%',
                          label
                  );

                  fprintf (output, "\nL%d:\n",
                          label 
                  );

                  scopes[scopes_position++] = (Scopes) {
                    .type = Scope_if,
                    .label = label,
                    .var_id = 0
                  };

                  i += 4;
                  var += 3;
                  label++;
                  scope++;
                }
              
             
            }
        }
      else
      if( 
        branch.type == Magic 
        && strcmp (branch.saves.magic, "while") == 0
      ) {
          PNode next = ((PNode*)pTree->items)[i + 1];
          for(
            int x = 0;
            x < varspos;
            x++
          ) {
              if( variables[x].level > scope )
                /*->*/ break;

              if( strcmp (variables[x].def.id, next.saves.token.buffer) != 0 )
                /*->*/ continue; 

              PNode pOperator = ((PNode*)pTree->items)[i + 2];
              char *operator;

              
              char *content = pOperator.saves.token.buffer; 
              if( strcmp (content, "==") == 0 ){
                operator = "eq";
              } 
              else
              if( strcmp (content, ">=") == 0 ){
                operator = "sge";
              } 
              else
              if( strcmp (content, ">") == 0 ){
                operator = "sgt";
              } 
              else
              if( strcmp (content, "<=") == 0 ){
                operator = "sle";
              } 
              else
               if( strcmp (content, "<") == 0 ){
                operator = "slt";
              } 
              else
              if( strcmp (content, "!=") == 0 ){
                operator = "ne";
              } 

              fprintf (output, "br label %cE%d\n\nE%d:\n%c%d = load %s, ptr %c%d, align %d\n", '%',
                      label, 
                      label, '%',
                      var, 
                      variables[x].def.type, '%',
                      variables[x].llvm,
                      llvm_sizeof (variables[x].def.type)
              );

              

              PNode toCompare = ((PNode*)pTree->items)[i + 3];
              if( toCompare.type == Normal && toCompare.saves.token.type == Identifier ) 
                {
                  for(
                    int j = 0;
                    j < varspos;
                    j++
                  ) {
                      if( variables[j].level > scope )
                        /*->*/ break;

                      if( strcmp (variables[j].def.id, toCompare.saves.token.buffer) != 0 )
                        /*->*/ continue; 

                      if( variables[j].def.type[0] != 'i' ) 
                        /*->*/ break;
                    
                      int size = llvm_sizeof (variables[j].def.type);
                      int start = llvm_sizeof (variables[x].def.type);
                      
                      fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%', 
                              (var + 1),
                              variables[j].def.type, '%',
                              variables[j].llvm,
                              size
                      );

                      var += 2;

                      if( size > x ) 
                        {
                          fprintf (output, "%c%d = sext %s %c%d to %s\n", '%', 
                            var,
                            variables[x].def.type, '%',
                            (var - 2),
                            variables[j].def.type
                          );
                          var++;

                          fprintf (output, "%c%d = icmp %s %s %c%d, %c%d\n", '%', 
                                  (var + 1),
                                  operator, 
                                  variables[j].def.type, '%',
                                  (var - 2), '%',
                                  (var - 1)
                          );
                        }
                      else if( size < start )
                        {
                          fprintf (output, "%c%d = sext %s %c%d to %s\n", '%', 
                            var,
                            variables[j].def.type, '%',
                            (var - 1),
                            variables[x].def.type
                          );
                          var++;

                          fprintf (output, "%c%d = icmp %s %s %c%d, %c%d\n", '%', 
                                  (var + 1),
                                  operator, 
                                  variables[x].def.type, '%',
                                  (var - 3), '%',
                                  (var - 1)
                          );
                        }

                      fprintf (output, "br i1 %c%d, label %cL%d, label %cC%d\n", '%',
                              (var + 1), '%',
                              label, '%',
                              label
                      );

                      fprintf (output, "\nL%d:\n",
                              label 
                      );

                      scopes[scopes_position++] = (Scopes) {
                        .type = Scope_while,
                        .label = label,
                        .var_id = 0
                      };
                      
                      i += 4;
                      var += 2;
                      label++;
                      scope++;

                    }
                }
              else
              if( toCompare.type == Normal && toCompare.saves.token.type == Integer ) 
                {
                  fprintf (output, "%c%d = icmp %s %s %c%d, %s\n", '%', 
                          (var + 1),
                          operator, 
                          variables[x].def.type, '%',
                          var,
                          toCompare.saves.token.buffer
                  );

                  fprintf (output, "br i1 %c%d, label %cL%d, label %cC%d\n", '%',
                          (var + 1), '%',
                          label, '%',
                          label
                  );

                  fprintf (output, "\nL%d:\n",
                          label 
                  );

                  scopes[scopes_position++] = (Scopes) {
                    .type = Scope_while,
                    .label = label,
                    .var_id = 0
                  };

                  i += 4;
                  var += 3;
                  label++;
                  scope++;
                }
              
              
            }
        }
      else
      if( 
        branch.type == Magic 
        && strcmp (branch.saves.magic, "for") == 0
      ) {
          PNode next = ((PNode*)pTree->items)[i + 1];

          if( 
            next.type == Normal 
            && next.saves.token.type == Identifier 
            && ((PNode*)pTree->items)[i + 2].type == Normal 
            && ((PNode*)pTree->items)[i + 2].saves.token.type == Iter
            && ((PNode*)pTree->items)[i + 3].type == Normal 
            && ((PNode*)pTree->items)[i + 3].saves.token.type == Integer
          ) {
              int id = 0;
              Variable __var;

              for(
                int x = 0;
                x < varspos;
                x++
              ) {
                  if( variables[x].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[x].def.id, next.saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  __var = variables[x];
                  id = __var.llvm;
                  
                }

              var++;

              if( id == 0 ) {
                variables[varspos] = (Variable) {
                  .level = scope, 
                  .def = (DMemory) {
                    .arg = 0,
                    .key_type = 0,
                    .id = next.saves.token.buffer,
                    .array = (AMemory) {
                      .size = "0",
                      .type = "Unknown"
                    },
                    .type = "i64",
                    .hopeful = 0
                  },
                  .llvm = var++
                };
                __var = variables[varspos++];
              }

              fprintf (output, "%c%d = alloca i64, align 8\nstore i64 0, ptr %c%d, align 8\nbr label %cE%d\n\nE%d:\n", '%',
                      (var - 1), '%',
                      (var - 1), '%',
                      label, 
                      label
              );

              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      var, '%',
                      (var - 1)
              );
              var++;

              fprintf (output, "%c%d = icmp slt i64 %c%d, %s\n", '%',
                      var, '%',
                      (var - 1),
                      ((PNode*)pTree->items)[i + 3].saves.token.buffer
              );

              fprintf (output, "br i1 %c%d, label %cL%d, label %cC%d\n\nL%d:\n", '%',
                      var, '%',
                      label, '%',
                      label, 
                      label
              );

              var++;
              scope++;
              
              scopes[scopes_position++] = (Scopes) {
                .type = Scope_for_iter,
                .var_id = (varspos - 1),
                .label = label
              };
            }
          else 
          if( 
            next.type == Normal 
            && next.saves.token.type == Integer 
            && ((PNode*)pTree->items)[i + 2].type == Normal 
            && ((PNode*)pTree->items)[i + 2].saves.token.type == Iter
            && ((PNode*)pTree->items)[i + 3].type == Normal 
            && ((PNode*)pTree->items)[i + 3].saves.token.type == Identifier
          ) {
              int id = 0;
              Variable __var;

              for(
                int x = 0;
                x < varspos;
                x++
              ) {
                  if( variables[x].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[x].def.id, next.saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  __var = variables[x];
                  id = __var.llvm;
                  
                }

              if( id == 0 ) {
                variables[varspos] = (Variable) {
                  .level = scope, 
                  .def = (DMemory) {
                    .arg = 0,
                    .key_type = 0,
                    .id = ((PNode*)pTree->items)[i + 3].saves.token.buffer,
                    .array = (AMemory) {
                      .size = "0",
                      .type = "Unknown"
                    },
                    .type = "i64",
                    .hopeful = 0
                  },
                  .llvm = var++
                };
                __var = variables[varspos++];
              }

              fprintf (output, "%c%d = alloca i64, align 8\nstore i64 %s, ptr %c%d, align 8\nbr label %cE%d\n\nE%d:\n", '%',
                      (var - 1),
                      ((PNode*)pTree->items)[i + 1].saves.token.buffer, '%',
                      (var - 1), '%',
                      label, 
                      label
              );

              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      var, '%',
                      (var - 1)
              );
              var++;

              fprintf (output, "%c%d = icmp sgt i64 %c%d, 0\n", '%',
                      var, '%',
                      (var - 1)
              );

              fprintf (output, "br i1 %c%d, label %cL%d, label %cC%d\n\nL%d:\n", '%',
                      var, '%',
                      label, '%',
                      label, 
                      label
              );

              var++;
              scope++;
              
              scopes[scopes_position++] = (Scopes) {
                .type = Scope_for_revese_iter,
                .var_id = (varspos - 1),
                .label = label
              };
            }
          else 
          if( 
            next.type == Normal 
            && next.saves.token.type == Identifier 
            && ((PNode*)pTree->items)[i + 2].type == Normal 
            && ((PNode*)pTree->items)[i + 2].saves.token.type == Iter
            && ((PNode*)pTree->items)[i + 3].type == Normal 
            && ((PNode*)pTree->items)[i + 3].saves.token.type == Identifier
          ) {
              int __id01 = 0;
              int __id02 = 0;
              Variable first;
              Variable second;

              for(
                int x = 0;
                x < varspos;
                x++
              ) {
                  if( variables[x].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[x].def.id, next.saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  first = variables[x];
                  __id01 = first.llvm;
                }

              for(
                int x = 0;
                x < varspos;
                x++
              ) {
                  if( variables[x].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[x].def.id, ((PNode*)pTree->items)[i + 3].saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  second = variables[x];
                  __id02 = second.llvm;
                }

              if( (__id01 + __id02) == 0 || ( __id01 != 0 && __id02 != 0 ))
                {
                  printf ("Compilation failed");
                  exit (1);
                }

              Variable always_defined = (__id01 ? first : second);
              int bytes = llvm_sizeof (always_defined.def.type);

              fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                      var++,
                      always_defined.def.type, '%',
                      always_defined.llvm,
                      bytes
              );

              if( bytes > 8 ) 
                {
                  fprintf (output, "%c%d = trunc %s %c%d to i64\n", '%',
                          var,
                          always_defined.def.type, '%',
                          (var - 1)
                  );
                }
              else
              if( bytes < 8 ) 
                {
                  fprintf (output, "%c%d = sext %s %c%d to i64\n", '%',
                          var,
                          always_defined.def.type, '%',
                          (var - 1)
                  );
                }
              var++;

              if( __id01 )
                {
                  fprintf (output, "%c%d = alloca i64, align 8\nstore i64 %c%d, ptr %c%d, align 8\nbr label %cE%d\n\nE%d:\n", '%',
                          var, '%',
                          (var - 1), '%',
                          var, '%',
                          label, 
                          label
                  );
                }
              else
                {
                  fprintf (output, "%c%d = alloca i64, align 8\nstore i64 0, ptr %c%d, align 8\nbr label %cE%d\n\nE%d:\n", '%',
                          var, '%',
                          var, '%',
                          label, 
                          label
                  );
                }

              variables[varspos++] = (Variable) {
                .level = scope, 
                .def = (DMemory) {
                  .arg = 0,
                  .key_type = 0,
                  .id = (__id01 ? ((PNode*)pTree->items)[i + 3] : next).saves.token.buffer,
                  .array = (AMemory) {
                    .size = "0",
                    .type = "Unknown"
                  },
                  .type = "i64",
                  .hopeful = 0
                },
                .llvm = var
              };
              var++;

              fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                      var++,
                      always_defined.def.type, '%',
                      always_defined.llvm,
                      bytes
              );

              if( bytes > 8 ) 
                {
                  fprintf (output, "%c%d = trunc %s %c%d to i64\n", '%',
                          var,
                          always_defined.def.type, '%',
                          (var - 1)
                  );
                }
              else
              if( bytes < 8 ) 
                {
                  fprintf (output, "%c%d = sext %s %c%d to i64\n", '%',
                          var,
                          always_defined.def.type, '%',
                          (var - 1)
                  );
                }
              var++;
              

              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      (var + 1), '%',
                      (var - 3)
              );
              var += 2;

              if( __id01 )
                {
                  fprintf (output, "%c%d = icmp %s i64 %c%d, 0\n", '%',
                          var,
                          (__id01 ? "sgt" : "sgt"), '%',
                          (var - 1)
                  );
                }
              else 
                {
                  fprintf (output, "%c%d = icmp %s i64 %c%d, %c%d\n", '%',
                          var,
                          (__id01 ? "sgt" : "sgt"), '%',
                          (var - 3), '%',
                          (var - 1)
                  );
                }

              fprintf (output, "br i1 %c%d, label %cL%d, label %cC%d\n\nL%d:\n", '%',
                      var++, '%',
                      label, '%',
                      label, 
                      label
              );

              scope++;

              scopes[scopes_position++] = (Scopes) {
                .type = (__id01 ? Scope_for_revese_iter : Scope_for_iter),
                .var_id = (varspos - 1),
                .label = label
              };
            }
          else
          if( 
            next.type == Normal 
            && next.saves.token.type == Identifier 
            && ((PNode*)pTree->items)[i + 2].type == Normal 
            && ((PNode*)pTree->items)[i + 2].saves.token.type == Unknown
            && strcmp (((PNode*)pTree->items)[i + 2].saves.token.buffer, ":") == 0
          ) {
              if(
                  ((PNode*)pTree->items)[i + 3].type == Normal 
                  && ((PNode*)pTree->items)[i + 3].saves.token.type == Integer 
                  && ((PNode*)pTree->items)[i + 4].type == Normal 
                  && ((PNode*)pTree->items)[i + 4].saves.token.type == Iter
                  && ((PNode*)pTree->items)[i + 5].type == Normal 
                  && ((PNode*)pTree->items)[i + 5].saves.token.type == Integer  
                ) {
                    const char *first_buff = ((PNode*)pTree->items)[i + 3].saves.token.buffer;
                    int x = atoi (first_buff);
                    const char *second_buff = ((PNode*)pTree->items)[i + 5].saves.token.buffer;
                    int y = atoi (second_buff);

                    fprintf (output, "%c%d = alloca i64, align 8\nstore i64 %d, ptr %c%d, align 8\nbr label %cE%d\n\nE%d:\n", '%',
                            var, 
                            x, '%',
                            var, '%',
                            label, 
                            label
                    );

                    int __id = var;

                    variables[varspos++] = (Variable) {
                      .level = scope, 
                      .def = (DMemory) {
                        .arg = 0,
                        .key_type = 0,
                        .id = next.saves.token.buffer,
                        .array = (AMemory) {
                          .size = "0",
                          .type = "Unknown"
                        },
                        .type = "i64",
                        .hopeful = 0
                      },
                      .llvm = var++
                    };

                    scopes[scopes_position++] = (Scopes) {
                      .type = ((x > y) ? Scope_for_revese_iter : Scope_for_iter),
                      .var_id = (varspos - 1),
                      .label = label
                    };
                    
                    scope++;

                    fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                            var, '%',
                            __id
                    );

                    fprintf (output, "%c%d = icmp %s i64 %c%d, %d\n", '%',
                            (var + 1),
                            ((x > y) ? "sgt" : "slt"), '%',
                            var,
                            y
                    );

                    fprintf (output, "br i1 %c%d, label %cL%d, label %cC%d\n\nL%d:\n", '%',
                            (var + 1), '%',
                            label, '%',
                            label,
                            label
                    );

                    var += 2;

                  }
                else
                if( 
                  ((PNode*)pTree->items)[i + 3].type == Normal 
                  && ((PNode*)pTree->items)[i + 3].saves.token.type == Identifier 
                  && ((PNode*)pTree->items)[i + 4].type == Normal 
                  && ((PNode*)pTree->items)[i + 4].saves.token.type == Iter
                  && ((PNode*)pTree->items)[i + 5].type == Normal 
                  && ((PNode*)pTree->items)[i + 5].saves.token.type == Integer  
                ) {
                    int x;
                    int icmp_id;
                    int __id;
                    Variable address;

                    for(
                      int z = 0;
                      z < varspos;
                      z++
                    ) {
                        if( variables[z].level > scope )
                          /*->*/ break;

                        if( strcmp (variables[z].def.id, ((PNode*)pTree->items)[i + 3].saves.token.buffer) != 0 )
                          /*->*/ continue; 

                        address = variables[z];
                        x = address.llvm;
                      }

                    const char *second_buff = ((PNode*)pTree->items)[i + 5].saves.token.buffer;
                    int y = atoi (second_buff);

                    int bytes = llvm_sizeof (address.def.type);
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var++, 
                            address.def.type, '%',
                            x,
                            bytes
                    );

                    if( bytes > 8 ) 
                      {
                        fprintf (output, "%c%d = trunc %s %c%d to i64\n", '%', 
                                var, 
                                address.def.type, '%',
                                (var - 1)
                        );
                        var++;
                      }
                    else
                    if( bytes < 8 )
                      {
                        fprintf (output, "%c%d = sext %s %c%d to i64\n", '%', 
                                var, 
                                address.def.type, '%',
                                (var - 1)
                        );
                        var++;
                      }

                    variables[varspos++] = (Variable) {
                      .level = scope, 
                      .def = (DMemory) {
                        .arg = 0,
                        .key_type = 0,
                        .id = next.saves.token.buffer,
                        .array = (AMemory) {
                          .size = "0",
                          .type = "Unknown"
                        },
                        .type = "i64",
                        .hopeful = 0
                      },
                      .llvm = var
                    };

                    __id = var;
                    fprintf (output, "%c%d = alloca i64, align 8\n%c%d = alloca i8, align 1\nstore i64 %c%d, ptr %c%d, align 8\n\n", '%',
                            var, '%', 
                            (var + 1), '%', 
                            (var - 1), '%',
                            var
                    );
                    var += 2;


                    fprintf (output, "%c%d = icmp sgt i64 %c%d, %d\n", '%',
                            var, '%',
                            (var - 3),
                            y
                    );

                    icmp_id = var;

                    fprintf (output, "br i1 %c%d, label %cESB%d, label %cEAD%d\n\nESB%d:\n", '%',
                                      var, '%',
                                      label, '%',
                                      label,
                                      label
                    );
                    var++;

                    fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n%c%d = icmp sgt i64 %c%d, %d\nbr i1 %c%d, label %cL%d, label %cC%d\n\nEAD%d:\n", '%',
                            var, '%',
                            __id, '%',
                            (var + 1), '%',
                            var, 
                            y, '%',
                            (var + 1), '%',
                            label, '%',
                            label,
                            label
                    );

                    var += 2;

                    fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n%c%d = icmp slt i64 %c%d, %d\nbr i1 %c%d, label %cL%d, label %cC%d\n\nL%d:\n", '%',
                            var, '%',
                            __id, '%',
                            (var + 1), '%',
                            var, 
                            y, '%',
                            (var + 1), '%',
                            label, '%',
                            label,
                            label
                    );

                    var += 2;

                    var++;

                    scopes[scopes_position++] = (Scopes) {
                      .type = Scope_for_comparator,
                      .var_id = (varspos - 1),
                      .label = label,
                      .to_c = icmp_id
                    };
                    
                    label++;
                  }
                else
                if( 
                  ((PNode*)pTree->items)[i + 3].type == Normal 
                  && ((PNode*)pTree->items)[i + 3].saves.token.type == Integer 
                  && ((PNode*)pTree->items)[i + 4].type == Normal 
                  && ((PNode*)pTree->items)[i + 4].saves.token.type == Iter
                  && ((PNode*)pTree->items)[i + 5].type == Normal 
                  && ((PNode*)pTree->items)[i + 5].saves.token.type == Identifier  
                ) {
                    int x;
                    int icmp_id;
                    int __id;
                    Variable address;

                    for(
                      int z = 0;
                      z < varspos;
                      z++
                    ) {
                        if( variables[z].level > scope )
                          /*->*/ break;

                        if( strcmp (variables[z].def.id, ((PNode*)pTree->items)[i + 5].saves.token.buffer) != 0 )
                          /*->*/ continue; 

                        address = variables[z];
                        x = address.llvm;
                      }

                    const char *first_buff = ((PNode*)pTree->items)[i + 3].saves.token.buffer;
                    int y = atoi (first_buff);

                    int bytes = llvm_sizeof (address.def.type);
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var++, 
                            address.def.type, '%',
                            x,
                            bytes
                    );

                    if( bytes > 8 ) 
                      {
                        fprintf (output, "%c%d = trunc %s %c%d to i64\n", '%', 
                                var, 
                                address.def.type, '%',
                                (var - 1)
                        );
                        var++;
                      }
                    else
                    if( bytes < 8 )
                      {
                        fprintf (output, "%c%d = sext %s %c%d to i64\n", '%', 
                                var, 
                                address.def.type, '%',
                                (var - 1)
                        );
                        var++;
                      }

                    variables[varspos++] = (Variable) {
                      .level = scope, 
                      .def = (DMemory) {
                        .arg = 0,
                        .key_type = 0,
                        .id = next.saves.token.buffer,
                        .array = (AMemory) {
                          .size = "0",
                          .type = "Unknown"
                        },
                        .type = "i64",
                        .hopeful = 0
                      },
                      .llvm = var
                    };

                    __id = var;
                    fprintf (output, "%c%d = alloca i64, align 8\nstore i64 %d, ptr %c%d, align 8\n\n", '%',
                            var,
                            y, '%',
                            var
                    );
                    var += 1;


                    fprintf (output, "%c%d = icmp slt i64 %c%d, %d\n", '%',
                            var, '%',
                            (var - 2),
                            y
                    );

                    icmp_id = var;

                    fprintf (output, "br i1 %c%d, label %cEAD%d, label %cESB%d\n\nESB%d:\n", '%',
                                      var, '%',
                                      label, '%',
                                      label,
                                      label
                    );
                    var++;

                    fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n%c%d = icmp slt i64 %c%d, %c%d\nbr i1 %c%d, label %cL%d, label %cC%d\n\nEAD%d:\n", '%',
                            var, '%',
                            __id, '%',
                            (var + 1), '%',
                            var, '%',
                            (var - 3), '%',
                            (var + 1), '%',
                            label, '%',
                            label,
                            label
                    );

                    var += 2;

                    fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n%c%d = icmp sgt i64 %c%d, %c%d\nbr i1 %c%d, label %cL%d, label %cC%d\n\nL%d:\n", '%',
                            var, '%',
                            __id, '%',
                            (var + 1), '%',
                            var, '%', 
                            (var - 5), '%',
                            (var + 1), '%',
                            label, '%',
                            label,
                            label
                    );

                    var += 2;

                    var++;

                    scopes[scopes_position++] = (Scopes) {
                      .type = Scope_for_reverse_comparator,
                      .var_id = (varspos - 1),
                      .label = label,
                      .to_c = icmp_id
                    };
                    
                    label++;
                  }
            }
        }
      /*
        identifier operator(=) number
      */
      else
        if( 
          branch.type == Normal 
          && branch.saves.token.type == Identifier 
          && isFunction (branch.saves.token.buffer)
        ) {
             for(
                int x = 0;
                x < varspos;
                x++
              ) {
                  if( variables[x].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[x].def.id, branch.saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  int *variables_id = (int*)malloc (sizeof (int) * 128);

                  for(
                    int j = 1;
                    1;
                    j++
                  ) {
                      if(
                        ((PNode*)pTree->items)[i + j].type == Magic
                        && ((PNode*)pTree->items)[i + j].saves.token.type != Identifier  
                      ) /*->*/ break; 

                      for(
                        int v = 0;
                        v < varspos;
                        v++
                      ) {
                          if( variables[v].level > scope )
                            /*->*/ break;

                          if( strcmp (variables[v].def.id, ((PNode*)pTree->items)[i + j].saves.token.buffer) != 0 )
                            /*->*/ continue;

                          fprintf (output, "%c%d = load ", '%',
                                  var
                          );

                          if( isType (variables[v].def.type) )
                            {
                              fprintf (output, "%s, ptr", variables[v].def.type);
                            }
                          else if( strcmp (variables[v].def.type, "ptr") != 0 )
                            {
                              fprintf (output, "%c%s, ptr", '%', variables[v].def.type);
                            }

                          fprintf (output, " %c%d, align %d\n", '%', 
                                  variables[v].llvm, 
                                  llvm_sizeof (variables[v].def.type)
                          );

                          variables_id[j-1] = var++; 
                        }
                    }
                  
                  fprintf (output, "call %s @%s(",
                          variables[x].def.type,
                          variables[x].def.id
                  );

                  int tosum = 0;

                  for(
                    int j = 1;
                    1;
                    j++
                  ) {

                      if(
                        ((PNode*)pTree->items)[i + j].type == Magic
                        && ((PNode*)pTree->items)[i + j].saves.token.type != Identifier  
                      ) /*->*/ break;

                      for(
                        int v = 0;
                        v < varspos;
                        v++
                      ) {
                          if( variables[v].level > scope )
                            /*->*/ break;

                          if( strcmp (variables[v].def.id, ((PNode*)pTree->items)[i + j].saves.token.buffer) != 0 )
                            /*->*/ continue;

                          if( j != 1 ) {
                            fprintf (output, ", ");
                          }

                          if( isType (variables[v].def.type) )
                            {
                              fprintf (output, "%s", variables[v].def.type);
                            }
                          else if( strcmp (variables[v].def.type, "ptr") != 0 )
                            {
                              fprintf (output, "%c%s", '%', variables[v].def.type);
                            }

                          fprintf (output, " %c%d", '%',
                                  variables_id[j-1]
                          );

                          tosum++;

                        }
                          
                    }

                  free (variables_id);
                  i += tosum;

                  fprintf (output, ")\n");
                  var++;
                }
          }
      else
      if( 
        branch.type == Normal 
        && branch.saves.token.type == Identifier 
        && ((PNode*)pTree->items)[i + 1].type == Normal
        && (
          strcmp (((PNode*)pTree->items)[i + 1].saves.token.buffer, "=") == 0
          || (
            ((PNode*)pTree->items)[i + 1].saves.token.type >= OP_CONSTANT
            && ((PNode*)pTree->items)[i + 2].saves.token.type == Integer
          )
        )
      ) {
          i++;

          if( strcmp (((PNode*)pTree->items)[i].saves.token.buffer, "=") == 0 )
            {
              expr_t expr_type = ENone;
              int x = (i + 1);
              for(
                ;1;
                x++
              ) {
                  PNode value = ((PNode*)pTree->items)[x];

                  if( value.type == Normal && expr_type == ENone ) 
                    {
                      expr_type = ELit;
                      continue;
                    }
                  else 
                    { break; }

                }
              
              for(
                    int y = 0;
                    y < varspos;
                    y++
                  ) {
                      if( variables[y].level > scope )
                        /*->*/ break;

                      if( strcmp (variables[y].def.id, branch.saves.token.buffer) != 0 )
                        /*->*/ continue; 
                        
                      switch(expr_type)
                        {
                          case ELit: 
                            {
                              fprintf (output, "store %s %s, ptr %c%d, align %d\n", 
                                      variables[y].def.type,
                                      ((PNode*)pTree->items)[++i].saves.token.buffer, '%',
                                      variables[y].llvm,
                                      llvm_sizeof (variables[y].def.type)
                              );

                              break;
                            }
                          default:
                            {
                              printf ("Ocorred a problem");
                              exit (0);
                              break;
                            }
                        }
                  }
            }
          else
            {
              PNode operator = ((PNode*)pTree->items)[i];
              char *opcode;

              switch (operator.saves.token.type - Sub)
                {
                  case 0: {
                    opcode = "sub";
                  } break;
                  case 1: {
                    opcode = "add";
                  } break;
                  case 2: {
                    opcode = "mul";
                  } break;
                  case 3: {
                    opcode = "div";
                  } break;
                }

              for(
                int y = 0;
                y < varspos;
                y++
              ) {
                  if( variables[y].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[y].def.id, branch.saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                          var++, 
                          variables[y].def.type, '%',
                          variables[y].llvm,
                          llvm_sizeof (variables[y].def.type)
                  );

                  char changed = 0;
                  if( llvm_sizeof (variables[y].def.type) < 4 ) 
                    {
                      changed++;


                      fprintf (output, "%c%d = sext %s %c%d to i32\n", '%',
                              var,
                              variables[y].def.type, '%',
                              (var - 1)
                      );
                    }
                  if( llvm_sizeof (variables[y].def.type) > 4 ) 
                    {
                      changed++;


                      fprintf (output, "%c%d = trunc %s %c%d to i32\n", '%',
                              var,
                              variables[y].def.type, '%',
                              (var - 1)
                      );
                    }

                  fprintf (output, "%c%d = %s nsw i32 %c%d, %s\n", '%', 
                          (var + changed),
                          opcode, '%',
                          (var + changed) - 1,
                          ((PNode*)pTree->items)[++i].saves.token.buffer
                  ); 

                  if( changed ) 
                    {
                      fprintf (output, "%c%d = %s i32 %c%d to %s\n", '%',
                              (var + 2), 
                              (llvm_sizeof (variables[y].def.type) > 4 ? "sext" : "trunc"), '%', 
                              (var + changed),
                              variables[y].def.type
                      );
                    }

                  fprintf (output, "store %s %c%d, ptr %c%d, align %d\n", 
                          variables[y].def.type, '%',
                          (var + changed + 1 + (changed ? 0 : -1)), '%',
                          variables[y].llvm,
                          llvm_sizeof (variables[y].def.type)
                  );

                  var += 2 + changed;
                }
            }
        }
      else
      if( branch.type == End )
        { 
          Scopes scope_info = scopes[(scopes_position--) - 1];
          if( scope_info.type == Scope_if ) 
            {
              PNode next = ((PNode*)pTree->items)[++i];
              if( next.type == Magic && strcmp (next.saves.magic, "else") == 0 )
                {
                  fprintf (output, "br label %cC%d\n", '%',
                          label
                  );

                  scopes[scopes_position++] = (Scopes) {
                    .type = Scope_else,
                    .label = label++,
                    .var_id = 0
                  };

                  var++;
                  i++;
                }
              else {
                fprintf (output, "br label %cE%d\n", '%',
                        scope_info.label
                );
                scope--;
                i--; 
                var+=2;
              }

              fprintf (output, "\nE%d:\n",
                      scope_info.label
              );
                          
            }
          else 
          if( scope_info.type == Scope_else ) 
            {
              fprintf (output, "br label %cC%d\n\nC%d:\n", '%',
                scope_info.label,
                scope_info.label
              );

              scope--;
            }
          else
          if( scope_info.type == Scope_while ) 
            {
              fprintf (output, "br label %cE%d\n\nC%d:\n", '%',
                scope_info.label,
                scope_info.label
              );

              scope--;
            }
          if( scope_info.type == Scope_for_iter ) 
            {
              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      var++, '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "%c%d = add nsw i64 %c%d, 1\n", '%',
                      var, '%',
                      (var - 1)
              );

              var++;
              fprintf (output, "store i64 %c%d, ptr %c%d, align 8\n", '%',
                      (var - 1), '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "br label %cE%d\n\nC%d:\n", '%',
                scope_info.label,
                scope_info.label
              );

              scope--;
            }
          else
          if( scope_info.type == Scope_for_revese_iter ) 
            {
              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      var++, '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "%c%d = sub nsw i64 %c%d, 1\n", '%',
                      var, '%',
                      (var - 1)
              );

              var++;
              fprintf (output, "store i64 %c%d, ptr %c%d, align 8\n", '%',
                      (var - 1), '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "br label %cE%d\n\nC%d:\n", '%',
                      scope_info.label,
                      scope_info.label
              );

              scope--;
            }
          else
          if( scope_info.type == Scope_for_comparator ) 
            {
              
              fprintf (output, "\nbr i1 %c%d, label %cCSB%d, label %cCAD%d\n", '%',
                      scope_info.to_c, '%',
                      scope_info.label, '%',
                      scope_info.label
              );

              fprintf (output, "\nCSB%d:\n", scope_info.label);

              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      var++, '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "%c%d = sub nsw i64 %c%d, 1\n", '%',
                      var, '%',
                      (var - 1)
              );

              var++;
              fprintf (output, "store i64 %c%d, ptr %c%d, align 8\n", '%',
                      (var - 1), '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "br label %cESB%d\n", '%', scope_info.label);

              fprintf (output, "\nCAD%d:\n", scope_info.label);

              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      var++, '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "%c%d = add nsw i64 %c%d, 1\n", '%',
                      var, '%',
                      (var - 1)
              );

              var++;
              fprintf (output, "store i64 %c%d, ptr %c%d, align 8\n", '%',
                      (var - 1), '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "br label %cEAD%d\n\nC%d:\n", '%', scope_info.label, scope_info.label);

              scope--;
            }
          else
          if( scope_info.type == Scope_for_reverse_comparator ) 
            {
              
              fprintf (output, "\nbr i1 %c%d, label %cCAD%d, label %cCSB%d\n", '%',
                      scope_info.to_c, '%',
                      scope_info.label, '%',
                      scope_info.label
              );

              fprintf (output, "\nCSB%d:\n", scope_info.label);

              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      var++, '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "%c%d = add nsw i64 %c%d, 1\n", '%',
                      var, '%',
                      (var - 1)
              );

              var++;
              fprintf (output, "store i64 %c%d, ptr %c%d, align 8\n", '%',
                      (var - 1), '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "br label %cESB%d\n", '%', scope_info.label);

              fprintf (output, "\nCAD%d:\n", scope_info.label);

              fprintf (output, "%c%d = load i64, ptr %c%d, align 8\n", '%',
                      var++, '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "%c%d = sub nsw i64 %c%d, 1\n", '%',
                      var, '%',
                      (var - 1)
              );

              var++;
              fprintf (output, "store i64 %c%d, ptr %c%d, align 8\n", '%',
                      (var - 1), '%',
                      variables[scope_info.var_id].llvm
              );

              fprintf (output, "br label %cEAD%d\n\nC%d:\n", '%', scope_info.label, scope_info.label);

              scope--;
            }
          else  
          if( scope_info.type == Scope_fun ) 
            {
              fprintf (output, "}\n");
            }

          for(
            int i = varspos;
            i > 0;
            i--
          ) {
              if( variables[i].level > scope ) 
                {
                  var--;
                  varspos--;
                }

              if( variables[i].level < scope )
                /*->*/ continue;
            }
        }
    }
}
