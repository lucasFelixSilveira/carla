#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "../utils/vector.h"
#include "../utils/strings.h"
#include "../utils/symbols.h"
#include "parser.h"
#include "types.h"
#include "llvm.h"
#include "std.h"

/** 
  TODO: For loops, while loops, INTER operator e WALRUS (not :=) operator
  TODO: Auto-cast do resultado de uma expressão para o tipo correspondente ao da variável na qual é receptora
*/

int 
exponence(int x, int y)
{
  int result = 1;

  for(int i = 0; i < y; i++)
    {
      result *= x;
    }

  return result;
}

#define GET(vector, index) ((PNode*)vector->items)[index]
#define GET_T(T, vector, index) ((T*)vector->items)[index]
#define GETNP(T, vector, index) ((T*)vector.items)[index]
#define COMPOP(buffer, operator) strcmp (buffer, operator) == 0
#define STATIC_TYPE_LENGTH 4

#define BEGIN_SWITCH(str)    { char *_switch_str = str;
#define CASE(val)            if( strcmp (_switch_str, (val)) == 0 ) {
#define BREAK_CASE(val)      } else if( strcmp (_switch_str, (val)) == 0 ) {
#define DEFAULT              } else {
#define END_SWITCH           }
#define BREAK                }

#define COMP_PRECOMPILATION(output, comp) llvm_alloca_t (output, "bool"); \
                                          llvm_store_l (output, 8, comp ? 1 : 0, (var - 1)); \
                                          llvm_load_number (output, 8, (var-1));

#define MATH_PRECOMPILATION(output, expr) llvm_alloca_t (output, "int64"); \
                                          llvm_store_l (output, 64, expr, (var - 1)); \
                                          llvm_load_number (output, 64, (var-1));

unsigned int var = 0;
unsigned int label_id = 0;
int complement = 0;
int tab = 0;
int calli = 0;
int debugid = 0;

ExprCache cache[1024];
struct ARG {
  unsigned int llvm;
  char *arg_type;
  int index;
} args[128];
int alen = 0;
int clen = 0;

int
comp_get(Vector *vec, char *content) {

  for(
    int i = 0; 
    i < vec->length; 
    i++
  ) {
      Complement comp = GET_T(Complement, vec, i);
      if( strcmp (comp.string, content) == 0 )
        return comp.id;
    }
    
  return -1;
}

void
genT(char **tabs)
{
  if( tab == 0 )
    (*tabs)[0] = 0x0;
  else if( tab >= 1 )
    sprintf (*tabs, "  ");
}

Fn
find_fn(PNode call, Vector *vars) 
{
  int result = 0;
  if( call.type == NODE_INTERNAL )
    return std_fn (call);

  Variable var;
  VECTOR_CONTAINS(Variable, vars, id, call.data.super, &result);
  if( result )
    {
      VECTOR_FIND(Variable, vars, id, call.data.super, &var);

      if( var.v_type != Function )
        {
          printf ("%s IT IS NOT A FUNCTION!", var.id);
          exit(0);
        }

      return (Fn) {
        .id = call.data.super,
        .type = var.type,
        .lib = "SUPER",
        .args = "" 
      };
    } 
  else 
    {
      printf ("%s NOT FOUND!", call.data.super);
      exit(0);
    }
}

/*
  LLVM util functions
*/

int
llvm_sizeof (char *type)
{
  if( type[0] == 'i' || type[0] == 'u' )
    {
      char *bits = substr (type, 1, strlen (type) - 1);
      int bytes = atoi (bits) / 8;
      return bytes;
    }

  if( strcmp (type, "ptr") == 0 ) 
    return 8;
  
  return 0;
}

char *
llvm_type (char *type) 
{
  if( strcmp (type, "llvm<bool>") == 0 )
    return "i1";

  const char ptrsufix = '*';
  if( type[strlen (type) - 1] == ptrsufix )
    return "ptr";

  const char arrayprefix = '[';
  if( type[0] == arrayprefix )
    return "ptr";

  char *__type = (char*)malloc (128);
  char *prefix = "int";

  integer_by_bits: {
    size_t len = strlen (prefix);
    for( char i = 0; i < len; i++ )
      {
        if( prefix[i] != type[i] )
          break;
        if( i == (len - 1) )
          {
            char *bits = substr (type, len, strlen (type) - (len - 1));
            char *result = (char*)malloc (strlen (type));
            sprintf (result, "i%s", bits);
            free (bits);
            free (__type);
            return result;
          }
      }
    if( prefix[0] == 'u' )
      goto __next_step_type;
  }
  
  prefix = "uint";
  goto integer_by_bits;
  __next_step_type: {}

  if( strcmp (type, "void") == 0 )
    {
      memcpy (__type, type, strlen (type));
      __type[4] = 0;
      return __type;
    }
  
  char *matrix[STATIC_TYPE_LENGTH][16] = {
    { "ascii", "i8" },
    { "bool", "i8" },
    { "char", "i8" },
    { "byte",  "i8" }
  };
  
  for( int i = 0; i < STATIC_TYPE_LENGTH; i++ )
    {
      if( strcmp (matrix[i][0], type) != 0 )
        continue;
      
      memcpy (__type, matrix[i][1], strlen (matrix[i][1]));
      __type[strlen (matrix[i][1])] = 0;
      return __type;
    }

  return strdup ("void");
}

void 
llvm_load_number(FILE *output, int bits, unsigned int to_load) 
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  fprintf (output, "%s%c%d = load i%d, ptr %c%d, align %d\n", 
          tabs, '%',
          var++,
          bits, '%',
          to_load,
          (bits / 8)
  );
  free (tabs);
}

void
llvm_alloca(FILE *output, PNode node)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  char *type = llvm_type (node.data.definition.type);
  fprintf (output, "%s%c%d = alloca %s, align %d\n", 
          tabs, '%',
          var++,
          type,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_alloca_t(FILE *output, char *__type)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  char *type = llvm_type (__type);
  fprintf (output, "%s%c%d = alloca %s, align %d\n", 
          tabs, '%',
          var++,
          type,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

char *
backPtr(char *type) 
{
  if( isType (type) )
    return type;

  if( type[0] == '[' ) 
    {
      uint16_t len = 0;
      while(type[len++] != ']');
      return substr (type, len, (strlen (type) - len));
    }

  if( type[strlen (type) - 1] == '*' ) 
    {
      return substr (type, 0, (strlen (type) - 1));
    }

  return "void";
}

void
llvm_store(FILE *output, PNode node, int __src_var__, int __dst_var__)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  char *type = llvm_type (node.data.definition.type);
  fprintf (output, "%sstore %s %c%d, ptr %c%d, align %d\n", 
          tabs, 
          type, '%',
          __src_var__, '%',
          __dst_var__,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_store_l(FILE *output, int bits, int original, uint32_t dist)
{
  int number = original;
  if( original > (exponence(2, bits) / 2) - 1 )
    number = -(((exponence(2, bits) / 2)) - (number - ((exponence(2, bits) / 2))));

  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  fprintf (output, "%sstore i%d %d, ptr %c%d, align %d\n", 
          tabs, 
          bits,
          number, '%',
          dist,
          (bits / 8)
  );
  free (tabs);
}

Variable
llvm_get(Vector *vec, char *id)
{
  for(
    int i = 0; 
    i < vec->length; 
    i++
  ) {
      Variable var = ((Variable*)vec->items)[i];
      if( strcmp (var.id, id) == 0 )
        return var;
    }
  
  printf ("%s NOT FOUND!", id);
  exit (0);
}

uint32_t
llvm_get_i(Vector *vec, char *id)
{
  for(
    int i = 0; 
    i < vec->length; 
    i++
  ) {
      Variable var = ((Variable*)vec->items)[i];
      if( strcmp (var.id, id) == 0 )
        return i;
    }
  
  printf ("%s NOT FOUND!", id);
  exit (0);
}

Variable
llvm_get_f(Vector *vec, unsigned int id)
{
  for(
    int i = 0; 
    i < vec->length; 
    i++
  ) {
      Variable var = ((Variable*)vec->items)[i];
      if( var.llvm == id )
        return var;
    }
  
  printf ("%d NOT FOUND!", id);
  exit (0);
}

void
llvm_resize(FILE *output, unsigned int collect, char *type, Variable changable)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  
  fprintf (output, "%s%c%d = ", 
          tabs, '%',
          var++ 
  );
  free (tabs);

  if( llvm_sizeof (llvm_type (type)) > llvm_sizeof (llvm_type (changable.type)) )
    fprintf (output, "sext ");
  else 
    fprintf (output, "trunc ");
  
  fprintf (output, "%s %c%d to %s\n", 
          llvm_type (changable.type), '%',
          collect,
          llvm_type (type)
  );

}

void
llvm_operation(FILE *output, char *operator, unsigned int left, unsigned int right)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);

  fprintf (output, "%s%c%d = ",
          tabs, '%',
          var++
  );

  BEGIN_SWITCH(operator)
    CASE("+") fprintf (output, "add nsw ");
    BREAK_CASE("-") fprintf (output, "sub nsw ");
    BREAK_CASE("*") fprintf (output, "mul nsw ");

    BREAK_CASE("%") fprintf (output, "srem ");
    BREAK_CASE("/") fprintf (output, "sdiv ");
    BREAK
  END_SWITCH

  fprintf (output, "i64 %c%d, %c%d\n", '%',
          left, '%',
          right
  );
}

void 
llvm_label(FILE *output, char *name) 
{
  fprintf (output, ".l%d.carla.%s:\n", 
          label_id++,
          name
  );
}

void 
llvm_label_id(FILE *output, unsigned int id, char *name) 
{
  fprintf (output, ".l%d.carla.%s:\n", 
          id,
          name
  );
}

void 
llvm_br(FILE *output, unsigned int id, char *name) 
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  fprintf (output, "%sbr label %c.l%d.carla.%s\n",
          tabs, '%', 
          id,
          name 
  );
  free (tabs);
}

void 
llvm_icmpbr(FILE *output, unsigned int icmp, unsigned int general_id, char *true_name, char *false_name) 
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  fprintf (output, "%sbr i1 %c%d, label %c.l%d.carla.%s, label %c.l%d.carla.%s\n",
          tabs, '%', 
          icmp, '%',
          general_id,
          true_name, '%',
          general_id,
          false_name 
  );
  free (tabs);
}

void
llvm_resize_bits(FILE *output, unsigned int collect, char *type, char *change)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  
  fprintf (output, "%s%c%d = ", 
          tabs, '%',
          var++ 
  );
  free (tabs);

  if( 
    ( llvm_sizeof (llvm_type (type)) > llvm_sizeof (llvm_type (change)) )
    || strcmp (change, "llvm<bool>") == 0
  )
    fprintf (output, "sext ");
  else 
    fprintf (output, "trunc ");
  
  fprintf (output, "%s %c%d to %s\n", 
          llvm_type (change), '%',
          collect,
          llvm_type (type)
  );

}

void
llvm_comp_bits(FILE *output, char bits, char *operator, unsigned int left, unsigned int right, char resize)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);

  fprintf (output, "%s%c%d = icmp ",
          tabs, '%',
          var++
  );

  BEGIN_SWITCH(operator)
    CASE("==") fprintf (output, "eq ");
    BREAK_CASE("!=") fprintf (output, "ne ");
    BREAK_CASE(">=") fprintf (output, "sge ");
    BREAK_CASE("<=") fprintf (output, "sle ");
    BREAK_CASE("<") fprintf (output, "slt ");
    BREAK_CASE(">") fprintf (output, "sgt ");
    BREAK
  END_SWITCH

  fprintf (output, "i%d %c%d, %c%d\n", 
          bits,'%',
          left, '%',
          right
  );

  if( resize ) 
    llvm_resize_bits (output, (var-1), "int8", "llvm<bool>");
}

void
llvm_comp(FILE *output, char *operator, unsigned int left, unsigned int right)
{ llvm_comp_bits (output, 64, operator, left, right, 1); }

void 
llvm_load(FILE *output, Variable toLoad)
{
  char *tabs = (char*)malloc (1024);
  char *type = llvm_type (toLoad.type);
  genT (&tabs);
  fprintf (output, "%s%c%d = load %s, ptr %c%d, align %d\n", 
          tabs, '%',
          var++,
          type, '%',
          toLoad.llvm,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_getelementptr(FILE *output, ExprCache vec) 
{
  char *tabs = (char*)malloc (1024);
  char *type = llvm_type (vec.info.access.type);
  genT (&tabs);
  fprintf (output, "%s%c%d = getelementptr inbounds %s, %s %c%d, i64 %c%d\n", 
          tabs, '%',
          var,
          type, 
          llvm_type (vec.info.access.type), '%',
          vec.info.access.load_vec, '%',
          (var-1)
  );
  var++;
  free (type);
  free (tabs);
}

void
llGenerate(FILE *output, Vector *pTree)
{

  Vector vars        = vector_init (sizeof (Variable));
  Vector scopes      = vector_init (sizeof (ScopeType));
  Vector retStack    = vector_init (sizeof (RetStack));
  Vector complements = vector_init (sizeof (Complement));

  int i = 0;
  for(; i < pTree->length; i++ )
    {
      PNode branch = GET(pTree, i);

      goto jmp_expr;
      wait_expr: 
        {
          while(1) 
            {
              PNode next = GET(pTree, i + 1);
              
              switch (next.type)
                {
                  case NODE_ACCESS:
                    {
                      i++;
                      Variable access = llvm_get (&vars, next.data.value);

                      llvm_load (output, access);
                      cache[clen++] = (ExprCache) {
                        .type = ACCESS_EXPR,
                        .info = {
                          .access = {
                            .need_load = 1,
                            .load_vec  = (var-1),
                            .type      = access.type,
                            .id        = access.id
                          }
                        }
                      };

                    } break;

                  case NODE_INTERNAL:
                  case NODE_INTERNAL_SUPER:
                    {
                      i++;
                      fprintf (output, "; [CARLA DEBUG]: Function added to stack: %s.\n", find_fn (next, &vars).id);
                      cache[clen++] = (ExprCache) {
                        .type = FUNCTION_CALL,
                        .info = {
                          .fn_call = find_fn (next, &vars)
                        }
                      };
                      calli++;
                    } break;

                  case NODE_OPERATION:
                    {
                      i++;

                      Token left_data = next.data.operation.left;
                      Token operator_data = next.data.operation.operation;
                      Token right_data = next.data.operation.right;
                      char left_int = left_data.type == Integer;
                      char right_int = right_data.type == Integer;
                      char left_identifier = left_data.type == Identifier;
                      char right_identifier = right_data.type == Identifier;


                      if( left_int && right_int ) 
                        {
                          int left = atoi (left_data.buffer);
                          int right = atoi (right_data.buffer);

                          int bytes = 0;

                          BEGIN_SWITCH(operator_data.buffer)
                            CASE("==") bytes = 1; COMP_PRECOMPILATION(output, left == right);
                            BREAK_CASE("!=") bytes = 1; COMP_PRECOMPILATION(output, left != right);
                            BREAK_CASE(">=") bytes = 1; COMP_PRECOMPILATION(output, left >= right);
                            BREAK_CASE("<=") bytes = 1; COMP_PRECOMPILATION(output, left <= right);
                            
                            /* MATH precompilation */
                            BREAK_CASE("+") bytes = 8; MATH_PRECOMPILATION(output, left + right);
                            BREAK_CASE("-") bytes = 8; MATH_PRECOMPILATION(output, left - right);
                            BREAK_CASE("/") bytes = 8; MATH_PRECOMPILATION(output, left / right);
                            BREAK_CASE("*") bytes = 8; MATH_PRECOMPILATION(output, left * right);
                            BREAK_CASE("%") bytes = 8; MATH_PRECOMPILATION(output, left % right);
                            BREAK
                          END_SWITCH

                          fprintf (output, "; [CARLA DEBUG]: Pre-compiled obvious operation\n");
                       
                          if( clen > 0 )
                            {
                              ExprCache resolve = cache[clen - 1];

                              char *type = (resolve.type == RETURN_KEY) ? 
                                resolve.info.node.data.definition.type 
                                : (resolve.type == VAR_DECLARATION) ?
                                  resolve.info.var.node.data.definition.type
                                  : "int64";

                              char *buffer = (char*)malloc (16);
                              sprintf (buffer, "int%d", bytes * 8);

                              if( llvm_sizeof (llvm_type (type)) != bytes ) 
                                llvm_resize_bits (output, (var-1), type, buffer);

                              free (buffer);

                              if( resolve.type == FUNCTION_CALL ) 
                                {
                                  args[alen++] = (struct ARG) {
                                    .llvm = (var - 1),
                                    .arg_type = type,
                                    .index = calli
                                  };
                                }
                            }
                        }
                      else
                        {
                          if( left_identifier ) 
                            {
                              Variable id = llvm_get (&vars, left_data.buffer);
                              llvm_load (output, id);
                              if( llvm_sizeof (id.type) != 8 ) 
                                llvm_resize_bits (output, (var-1), "int64", id.type);
                            }
                          else if( left_int )
                            {
                              int left = atoi (left_data.buffer);
                              llvm_alloca_t (output, "int64");
                              llvm_store_l (output, 64, left, (var-1));
                              llvm_load_number (output, 64, (var-1));
                            } 
                          
                          unsigned int left_var = (var-1);
                            
                          if( right_identifier ) 
                            {
                              Variable id = llvm_get (&vars, right_data.buffer);
                              llvm_load (output, id);
                              if( llvm_sizeof (id.type) != 8 ) 
                                llvm_resize_bits (output, (var-1), "int64", id.type);
                            }
                          else if( right_int )
                            {
                              int right = atoi (right_data.buffer);
                              llvm_alloca_t (output, "int64");
                              llvm_store_l (output, 64, right, (var-1));
                              llvm_load_number (output, 64, (var-1));
                            } 

                          unsigned int right_var = (var-1);
                          
                         switch(operator_data.type)
                          { case MathOP: 
                              {
                                llvm_alloca_t (output, "int64");
                                llvm_operation (output, operator_data.buffer, left_var, right_var);
                                llvm_store (output, (PNode) {
                                  .data.definition.type = "int64"
                                } , (var - 1), (var - 2));
                                llvm_load_number (output, 64, (var-2));
                              } break;

                            case ComparationOP: 
                              {
                                llvm_alloca_t (output, "bool");
                                llvm_comp (output, operator_data.buffer, left_var, right_var);
                                llvm_store (output, (PNode) {
                                  .data.definition.type = "bool"
                                } , (var - 1), (var - 3));
                                llvm_load_number (output, 8, (var-3));
                              } break;
                            default: break;
                          }

                          ExprCache resolve = cache[clen - 1];
                          if( resolve.type == VAR_DECLARATION )
                            {
                              llvm_store (output, (PNode) {
                                .data.definition.type = resolve.info.var.node.data.definition.type
                              } , (var - 1), resolve.info.var.llvm);
                              break;

                              if( GET(pTree, i).type == NODE_EEXPR  ) 
                                clen--;
                            }

                          if( resolve.type == FUNCTION_CALL )
                            {
                              args[alen++] = (struct ARG) {
                                .llvm = (var - 1),
                                .arg_type = (operator_data.type == MathOP) ? "int64" : "bool",
                                .index = calli
                              };
                              break;
                            }
                        }

                    } break;

                  case NODE_SINGLE:
                    {
                      i++;
                      ExprCache resolve = cache[--clen];
                      switch(next.data.single.type)
                        {
                          case NODE_NUMBER: 
                            {
                              switch (resolve.type)
                                {
                                  case RETURN_KEY:
                                    {
                                      llvm_alloca_t (output, resolve.info.node.data.definition.type);
                                      llvm_store_l (output, llvm_sizeof (llvm_type (resolve.info.node.data.definition.type)) * 8, (int)next.data.single.data.number, (var - 1));
                                      llvm_load (output, (Variable) { .llvm = (var-1), .type = resolve.info.node.data.definition.type });
                                    } break;
                                
                                  case VAR_DECLARATION: 
                                    {
                                      llvm_store_l (output, llvm_sizeof (llvm_type (resolve.info.var.node.data.definition.type)) * 8, (int)next.data.single.data.number, resolve.info.var.llvm);
                                    } break;

                                  case ACCESS_EXPR:
                                    {
                                      llvm_alloca_t (output, "int64");
                                      llvm_store_l (output, 64, (int)next.data.single.data.number, (var - 1));
                                      llvm_load (output, (Variable) { .llvm = (var-1), .type = "int64"});
                                    } break;

                                  default: 
                                    {
                                      llvm_alloca_t (output, "int64");
                                      llvm_store_l (output, 64, (int)next.data.single.data.number, (var - 1));
                                    } break; 

                                }
                            } break;
                          case NODE_TEXT: {
                            switch (resolve.type)
                              {
                                case FUNCTION_CALL:
                                  llvm_alloca_t (output, "[]byte");
                                  break;
                                default: break;
                              }

                            int already_exist = comp_get (&complements, next.data.single.data.value);
                            if( already_exist == -1  )
                              {
                                vector_push (&complements, ((void*)(&(Complement) {
                                  .id = complement,
                                  .string = strdup (next.data.single.data.value),
                                  .type = 0
                                })));
                                already_exist = (complement++);
                              }
                            
                            char *tabs = (char*)malloc (1024);
                            genT (&tabs);

                            fprintf (output, "%s%c%d = getelementptr inbounds [%d x i8], ptr @.carla.static.str.%d, i32 0, i32 0\n", 
                                    tabs, '%',
                                    var++,
                                    ((int)strlen (next.data.single.data.value) + 1),
                                    already_exist
                            );

                            free (tabs);

                            if( resolve.type != RETURN_KEY )
                              {
                                llvm_store (output, (PNode) {
                                  .data.definition.type = "[]byte"
                                } , (var - 1), (var - 2));
                              }

                          } break;
                          case NODE_ID:
                            {
                              Variable id = llvm_get (&vars, next.data.value);
                              llvm_load (output, id);
                              
                              switch (resolve.type)
                                {
                                  case VAR_DECLARATION:
                                  case ACCESS_EXPR:
                                  case RETURN_KEY:
                                    {
                                      char *type = (resolve.type == RETURN_KEY) ? 
                                        resolve.info.node.data.definition.type 
                                        : (resolve.type == VAR_DECLARATION) ?
                                          resolve.info.var.node.data.definition.type
                                          : "int64";

                                      char isInteger = types_int (type) && types_int (id.type);
                                      if(! isInteger )
                                        break;
                                      
                                      if( llvm_sizeof (llvm_type (type)) != llvm_sizeof (llvm_type (id.type)) )
                                          llvm_resize (output, (var - 1), type, id);

                                      switch (resolve.type)
                                        {
                                          case VAR_DECLARATION:
                                            clen++; break;
                                          default: break;
                                        }
                                    } break;
                                    
                                  default: break;
                                }
                            } break;
                          default: break;
                        }

                      switch (resolve.type)
                        {
                          case ACCESS_EXPR:
                            {
                              llvm_getelementptr (output, resolve);
                              llvm_load (output, (Variable) { .llvm = (var-1), .type = backPtr (resolve.info.access.type) });
                            
                              if( clen > 0 && cache[clen - 1].type == FUNCTION_CALL )
                                {
                                  args[alen++] = (struct ARG) {
                                    .llvm = (var - 1),
                                    .arg_type = backPtr (resolve.info.access.type),
                                    .index = calli
                                  };
                                }
                            } break;

                          case FUNCTION_CALL:
                            {
                              if( next.data.single.type == NODE_NUMBER )
                                {
                                  llvm_load (output, (Variable) { .llvm = (var-1), .type = "int64" });
                                  fprintf (output, "; [CARLA DEBUG]: The argument is a number\n");
                                }

                              if( next.data.single.type == NODE_TEXT )
                                {
                                  llvm_load (output, (Variable) { .llvm = (var-2), .type = "[]byte" });
                                  fprintf (output, "; [CARLA DEBUG]: The argument is a text\n");
                                }

                              args[alen++] = (struct ARG) {
                                .llvm = (var - 1),
                                .arg_type = 
                                  (next.data.single.type == NODE_NUMBER) ? 
                                    "int64" :
                                    (next.data.single.type == NODE_TEXT) ?
                                      "[]byte" :
                                      llvm_get (&vars, next.data.single.data.value).type,
                                .index = calli
                              };

                              clen++;
                            } break;

                          case RETURN_KEY:
                            { clen++; } break;

                          case VAR_DECLARATION:
                            { 
                              if( next.type == NODE_OPERATION ) {
                                int bits = 8;
                                if( next.data.operation.operation.type != ComparationOP )
                                  bits = 64;

                                llvm_store_l (output, bits, (var-1), (var-3));
                              }
                            } break;

                          default: break;
                        }

                    } break;

                  case NODE_CLOSE: 
                    {
                      i++;
                      ExprCache resolve = cache[--clen];
                      if( resolve.type != FUNCTION_CALL )
                        goto __llvm_gen_error__;
                      
                      char *tabs = (char*)malloc (1024);
                      genT (&tabs);
                      if( strcmp (resolve.info.fn_call.type, "void") != 0 )
                        {
                          fprintf (output, "%s%c%d = ", 
                                  tabs, '%',
                                  var++
                          );
                        } 
                      else 
                        fprintf (output, "%s", tabs);
                      free (tabs);

                      if( strcmp (resolve.info.fn_call.lib, "SUPER") == 0 )
                        {
                          fprintf (output, "call %s @%s(",
                                  llvm_type (resolve.info.fn_call.type), 
                                  resolve.info.fn_call.id
                          );
                        }
                      else 
                        {
                          fprintf (output, "call %s @%s.%s(",
                                  llvm_type (resolve.info.fn_call.type), 
                                  resolve.info.fn_call.lib,
                                  resolve.info.fn_call.id
                          );
                        }

                      int j = 0;
                      int cpy_alen = alen;
                      for(; j < cpy_alen && args[j].index == calli; j++ )
                        {
                          fprintf (output, "%s%s %c%d", 
                                  (j == 0) ? "" : ", ",
                                  llvm_type (args[j].arg_type), '%',
                                  args[j].llvm
                          );
                          alen--;
                        }
                      

                      calli--;
                      fprintf (output, ")\n");
                    
                      if( clen > 0 && cache[clen - 1].type == FUNCTION_CALL )
                        {
                          char *type = llvm_type (resolve.info.fn_call.type);
                          args[alen++] = (struct ARG) {
                            .llvm = (var - 1),
                            .arg_type = resolve.info.fn_call.type,
                            .index = calli
                          };
                        }

                      fprintf (output, "; [CARLA DEBUG]: %d items remaining in the stack.\n", alen);
                    } break;
                  
                  default: 
                    goto finish;
                }

              continue;
              finish: { break; }
            }
          
            if( clen > 0 ) 
              {
                ExprCache resolve = cache[clen - 1];
                switch(resolve.type) 
                  {
                    case RETURN_KEY: 
                      {
                        clen--;

                        char *tabs = (char*)malloc (1024);
                        genT (&tabs);
                        fprintf (output, "%sret %s %c%d\n",
                                tabs,
                                llvm_type (resolve.info.node.data.definition.type), '%',
                                (var - 1)
                        );
                        free (tabs);
                        continue;
                      }

                    case VAR_DECLARATION:
                      {
                        clen--;

                        llvm_store (output, resolve.info.var.node, (var - 1), resolve.info.var.llvm);
                      } break;

                    default: break;
                  }
              }
          continue;
        }
      jmp_expr: {}

      switch(branch.type)
        {
          /* type id (; | =) */
          case NODE_DEFINITION: 
            {

              if( GET(pTree, i+1).type == NODE_LAMBDA && tab == 0 )
                {
                  i += 2;

                  tab++;
                  char *type = llvm_type (branch.data.definition.type);
                  char *tabs = (char*)malloc (1024);
                  genT (&tabs);
                  fprintf (output, "\n%sdefine %s @%s(",
                          tabs,
                          type,
                          branch.data.definition.id 
                  );
                  free (tabs);

                  vector_push (&vars, ((void*)&(Variable) {
                    .v_type = Function, 
                    .type   = branch.data.definition.type,
                    .llvm   = 0,
                    .tab    = tab,
                    .id     = branch.data.definition.id
                  }));

                  vector_push (&retStack, ((void*)&(RetStack) {
                    .type = branch.data.definition.type
                  }));

                  int j = 0;
                  PNode arg;
                  if( GET(pTree, i).type == NODE_CLOSE && GET(pTree, i+1).type == NODE_BEGIN )
                    {
                      i += 1;
                      goto only_open;
                    }

                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    {
                      switch(arg.type)
                        {
                          case NODE_DEFINITION:
                            {
                              char *prefix = (char*)malloc (3); 
                              if( j > 0 ) 
                                sprintf (prefix, ", ");
                              else memset (prefix, 0, 3);
                              char *type = llvm_type (arg.data.definition.type);
                              fprintf (output, "%s%s %c%d",
                                      prefix,
                                      type, '%',
                                      var++
                              );
                              free (prefix);
                            } break;
                          
                          default: {
                            goto __llvm_gen_error__;
                          } break;
                        }
                    }

                  only_open: {}
                  int q = j;

                  fprintf (output, ") {\n");
                  if( strcmp (branch.data.definition.id, "main") == 0 )
                    {
                      fprintf (output, "entry:\n");
                    }
                  var++;
                  tab++;

                  j = 0;
                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    {
                      vector_push (&vars, ((void*)&(Variable) {
                        .v_type = Normal, 
                        .type   = arg.data.definition.type,
                        .llvm   = var,
                        .tab    = tab,
                        .id     = arg.data.definition.id
                      }));
                      llvm_alloca (output, arg);
                    }

                  j = 0;
                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    llvm_store (output, arg, var - (q * 2) - 1 + j, var - q + j);

                  free (type);
                  i += q;

                  ScopeType stype = (ScopeType) {
                    .type = Lambda
                  };
                  vector_push (&scopes, ((void*)&stype));

                  continue;
                }

              vector_push (&vars, ((void*)&(Variable) {
                .v_type = Normal, 
                .type   = branch.data.definition.type,
                .llvm   = var,
                .tab    = tab,
                .id     = branch.data.definition.id
              }));

              llvm_alloca (output, branch);
              if(! branch.data.definition.hopeful )
                continue;

              cache[clen++] = (ExprCache) {
                .type = VAR_DECLARATION,
                .info = {
                  .var = {
                    .llvm = (var - 1),
                    .node = branch
                  }
                }
              };
              
              goto wait_expr;
              
            } break;

          case NODE_END:
            {
              
              int index = scopes.length - 1;
              ScopeType scope = GETNP(ScopeType, scopes, index);
              
              tab -= (
                  scope.type    == For 
                  || scope.type == For_complex
                ) ? 0 : 1;
              char *tabs = (char*)malloc (1024);
              genT (&tabs);
              if( tab == 0 )
                var = 0;

              switch(scope.type)
                {
                  case Lambda:
                    {
                      fprintf (output, "}\n");
                    } break;
                  case For:
                    {
                      fprintf (output, "; [CARLA DEBUG]: End of ID iteration %d\n", scope.label_id);
                      
                      llvm_alloca_t (output, "int64");
                      llvm_store_l (output, 64, 1, (var-1));
                      llvm_load_number (output, 64, (var-1));
                      unsigned int right_llvm = (var-1);

                      Variable left = llvm_get_f (&vars, scope.operation.left);
                      fprintf (output, "; [CARLA DEBUG]: Found variable: %s\n", left.id);
                      int bytes = llvm_sizeof (llvm_type (left.type));
                      llvm_load_number (output, bytes * 8, scope.operation.left);
                      if( bytes != 8 )
                        llvm_resize_bits (output, (var-1), "int64", left.type); 

                      llvm_operation (output, scope.operation.operator, (var-1), right_llvm);
                      
                      if( bytes != 8 )
                        llvm_resize_bits (output, (var-1), left.type, "int64"); 
                     
                      llvm_store (output, (PNode) {
                        .data.definition.type = left.type
                      } , (var - 1), scope.operation.left);
                      llvm_br (output, scope.label_id, "for_begin");
                      llvm_label_id (output, scope.label_id, "for_end");
                      var++;

                      tab--;
                    } break; 
                  case For_complex:
                    {
                      
                      fprintf (output, "; [CARLA DEBUG]: End of ID iteration %d\n", scope.label_id);

                      char bits = llvm_sizeof (llvm_type (scope.complex.type)) * 8;

                      llvm_alloca_t (output, "int64");
                      llvm_store_l (output, 64, 1, (var-1));
                      llvm_load_number (output, 64, (var-1));
                      unsigned int one = (var-1);

                      llvm_load_number (output, bits, scope.complex.left);
                      if( bits != 64 )
                        llvm_resize_bits (output, (var-1), "int64", scope.complex.type); 

                      unsigned int iter = (var-1);  

                      llvm_icmpbr (output, scope.complex.comp, scope.label_id, "for_decrement", "for_increment");

                      llvm_label_id (output, scope.label_id, "for_decrement");
                      llvm_operation (output, "-", iter, one);
                     
                      if ( bits != 64 )  
                        llvm_resize_bits (output, (var-1), scope.complex.type, "int64"); 
                      
                      llvm_store (output, (PNode) {
                        .data.definition.type = scope.complex.type
                      } , (var-1), scope.complex.left);           
                      
                      llvm_br (output, scope.label_id, "for_begin");

                      llvm_label_id (output, scope.label_id, "for_increment");
                      llvm_operation (output, "+", iter, one);

                      if ( bits != 64 )  
                        llvm_resize_bits (output, (var-1), scope.complex.type, "int64"); 
                      
                      llvm_store (output, (PNode) {
                        .data.definition.type = scope.complex.type
                      } , (var-1), scope.complex.left);           
                      
                      llvm_br (output, scope.label_id, "for_begin");
                      llvm_label_id (output, scope.label_id, "for_end");
                      var++;

                      tab--;
                    } break;  
                  default: break;
                }              
              
              vector_remove (&scopes, index);
              
              int last = (vars.length - 1);
              Variable __var;
              while( last > 0 && (__var = GETNP(Variable, vars, last)).v_type == Normal && __var.tab > tab )
                vector_remove (&vars, last--);

              free (tabs);
            } break;

          case NODE_RET:
            {
              PNode node = {
                .data = {
                  .definition = {
                    .type = GETNP(RetStack, retStack, (retStack.length - 1)).type
                  }
                }
              };

              cache[clen++] = (ExprCache) {
                .type = RETURN_KEY,
                .info = {
                  .node = node
                }
              };

              goto wait_expr;
            
            } break;

          case NODE_FOR:
            {
              PNode definition = GET(pTree, i + 1);
              if( definition.type == NODE_DEFINITION && definition.data.definition.iter ) 
                {
                  fprintf (output, "; [CARLA DEBUG]: For iteration\n");
    
                  PNode iterator = GET(pTree, i + 2);
                  PNode open = GET(pTree, i + 3);
                  if( 
                    (
                      (
                        iterator.type == NODE_OPERATION 
                        && iterator.data.operation.operation.type == Iter 
                      ) || (
                        iterator.type == NODE_SINGLE
                        && iterator.data.single.type == NODE_ID
                      )
                    )
                    && open.type == NODE_BEGIN 
                  ) {
                      i += 3;
                      
                      llvm_alloca (output, definition);
                      unsigned int iteration = (var-1);
                      vector_push (&vars, ((void*)&(Variable) {
                        .v_type = Normal, 
                        .type   = definition.data.definition.type,
                        .llvm   = iteration,
                        .tab    = ++tab,
                        .id     = definition.data.definition.id
                      }));

                      switch(iterator.type)
                        { case NODE_OPERATION: 
                            {

                              Token left = iterator.data.operation.left;
                              Token right = iterator.data.operation.right;

                              int left_isint = left.type == Integer; 
                              int left_identifier = left.type == Identifier; 
                              int right_isint = right.type == Integer;
                              int right_identifier = right.type == Identifier; 

                              char bits = llvm_sizeof (llvm_type (definition.data.definition.type)) * 8;

                              if( left_isint && right_isint && types_int (definition.data.definition.type) )
                                {
                                  int left_v = atoi (left.buffer);
                                  int right_v = atoi (right.buffer);
                                
                                  char *operator = (left_v < right_v) ? "<" : ">";
                                 
                                  llvm_alloca_t (output, definition.data.definition.type);
                                  llvm_store_l (output, bits, right_v, (var-1));
                                  llvm_load_number (output, bits, (var-1));
                                  unsigned int right_llvm = (var-1);
                                  
                                  llvm_store_l (output, bits, left_v, iteration);
                                  llvm_br (output, label_id, "for_begin");
                                  
                                  llvm_label (output, "for_begin");
                                  var++;

                                  llvm_load_number (output, bits, iteration);
                                  llvm_comp_bits (output, bits, operator, (var-1), right_llvm, 0);
                                  llvm_icmpbr (output, (var-1), (label_id - 1), "for_body", "for_end");

                                  llvm_label_id (output, (label_id - 1), "for_body");
                                  var++;

                                  ScopeType stype = (ScopeType) {
                                    .type = For,
                                    .label_id = (label_id - 1),
                                    .operation.left = iteration,
                                    .operation.operator = (left_v < right_v) ? "+" : "-"
                                  }; 

                                  vector_push (&scopes, ((void*)&stype));
                                } 
                              else 
                                {
                                  if( left_identifier ) 
                                    {
                                      Variable id = llvm_get (&vars, left.buffer);
                                      llvm_load (output, id);
                                      if( llvm_sizeof (llvm_type (id.type)) * 8 != bits ) 
                                        llvm_resize_bits (output, (var-1), definition.data.definition.type, id.type);
                                    }
                                  else if( left_isint )
                                    {
                                      int _left = atoi (left.buffer);
                                      llvm_alloca_t (output, definition.data.definition.type);
                                      llvm_store_l (output, bits, _left, (var-1));
                                      llvm_load_number (output, bits, (var-1));
                                    } 
                                  
                                  unsigned int left_var = (var-1);
                                    
                                  if( right_identifier ) 
                                    {
                                      Variable id = llvm_get (&vars, right.buffer);
                                      llvm_load (output, id);
                                      if( llvm_sizeof (llvm_type (id.type)) * 8 != bits ) 
                                        llvm_resize_bits (output, (var-1), definition.data.definition.type, id.type);
                                    }
                                  else if( right_isint )
                                    {
                                      int _right = atoi (right.buffer);
                                      llvm_alloca_t (output, definition.data.definition.type);
                                      llvm_store_l (output, bits, _right, (var-1));
                                      llvm_load_number (output, bits, (var-1));
                                    } 

                                  unsigned int right_var = (var-1);

                                  llvm_comp_bits (output, bits, ">", left_var, right_var, 0); 
                                  unsigned int comp = (var-1);

                                  llvm_store (output, (PNode) {
                                    .data.definition.type = definition.data.definition.type
                                  }, left_var, iteration);

                                  llvm_br (output, label_id, "for_begin");
                                  llvm_label (output, "for_begin");
                                  var++;

                                  unsigned int begin = (label_id - 1);

                                  llvm_load_number (output, bits, iteration);
                                  unsigned int iter = (var-1);

                                  llvm_icmpbr (output, comp, begin, "for_check.grant", "for_check.less"); 
                                  
                                  /* left > right = LEFT IS GRANT THAN RIGHT*/
                                  llvm_label_id (output, begin, "for_check.grant");

                                  llvm_comp_bits (output, bits, ">", iter, right_var, 0);
                                  llvm_icmpbr (output, (var-1), begin, "for_body", "for_end");

                                  /* !(left > right) = LEFT IS LESS THAN RIGHT*/
                                  llvm_label_id (output, begin, "for_check.less");

                                  llvm_comp_bits (output, bits, "<", iter, right_var, 0);
                                  llvm_icmpbr (output, (var-1), begin, "for_body", "for_end");

                                  llvm_label_id (output, begin, "for_body");
                                  var++;

                                  ScopeType stype = (ScopeType) {
                                    .type = For_complex,
                                    .label_id = begin,
                                    .complex.left = iteration,
                                    .complex.comp = comp,
                                    .complex.type = definition.data.definition.type
                                  }; 

                                  vector_push (&scopes, ((void*)&stype));
                                }
                            } break;
                          default: break;
                        }
                    }
                }
            } break;

          case NODE_INTERNAL:
          case NODE_INTERNAL_SUPER:
            { 
              i--;
              goto wait_expr;
            }
          default: break;
        }
    }

  goto __ignore_error;
  __llvm_gen_error__: {
    printf ("Fail to generate the LLVM code\n");
  }
  __ignore_error: {}


  int j = 0;
  for(; j < vars.length; j++ )
    {
      free (GETNP(Variable, vars, j).type);
      free (GETNP(Variable, vars, j).id);
    }
  vector_free (&vars);
  j = 0;
  fprintf (output, "\n");
  for(; j < complements.length; j++ )
    {
      const int t = GETNP(Complement, complements, j).type;
      const int string = 0;
      fprintf (output, "@.carla.static.%s.%d = ",
              (t == string) ? "str" : "unknown",
              j
      );

      switch (t)
        { case 0:
            {
              const char *content = GETNP(Complement, complements, j).string; 
              fprintf (output, "private constant [%d x i8] c\"%s\\00\", align 1\n",
                      ((int)strlen (content) + 1),
                      content
              );
            } break;
          default: break;
        }

      free (GETNP(Complement, complements, j).string);
    }
  vector_free (&scopes);
  
}